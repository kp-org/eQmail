/*
 *  Revision 20180413, Kai Peter
 *  - switched to buffer
 *  Revision 20171023, Kai Peter
 *  - moved folder 'users' to 'var/users'
 *  Revision 20170316, Kai Peter
 *  - added <sys/types.h>, <unistd.h>, 'byte.h', 'open.h'
 */
#include <unistd.h>
#include "fd.h"
#include "wait.h"
#include "prot.h"
#include "buffer.h"
#include "stralloc.h"
#include "scan.h"
#include "errmsg.h"
#include "cdbread.h"
#include "case.h"
#include "readclose.h"
#include "auto_qmail.h"
#include "auto_uids.h"
#include "qlx.h"
#include "byte.h"
#include "open.h"

char *aliasempty;

static struct cdb c;

void initialize(int argc,char **argv) {
  aliasempty = argv[1];
  if (!aliasempty) _exit(100);
}

int truncreport = 3000;

void report(buffer *b,int wstat,char *s,int len)
{
  int i;
  if (wait_crashed(wstat))
  { buffer_puts(b,"Zqmail-local crashed.\n"); return; }
  switch(wait_exitcode(wstat))
  {
   case QLX_CDB:
     buffer_puts(b,"ZTrouble reading var/users/cdb in qmail-lspawn.\n"); return;
   case QLX_NOMEM:
     buffer_puts(b,"ZOut of memory in qmail-lspawn.\n"); return;
   case QLX_SYS:
     buffer_puts(b,"ZTemporary failure in qmail-lspawn.\n"); return;
   case QLX_NOALIAS:
     buffer_puts(b,"ZUnable to find alias user!\n"); return;
   case QLX_ROOT:
     buffer_puts(b,"ZNot allowed to perform deliveries as root.\n"); return;
   case QLX_USAGE:
     buffer_puts(b,"ZInternal qmail-lspawn bug.\n"); return;
   case QLX_NFS:
     buffer_puts(b,"ZNFS failure in qmail-local.\n"); return;
   case QLX_EXECHARD:
     buffer_puts(b,"DUnable to run qmail-local.\n"); return;
   case QLX_EXECSOFT:
     buffer_puts(b,"ZUnable to run qmail-local.\n"); return;
   case QLX_EXECPW:
     buffer_puts(b,"ZUnable to run qmail-getpw.\n"); return;
   case 111: case 71: case 74: case 75:
     buffer_put(b,"Z",1); break;
   case 0:
     buffer_put(b,"K",1); break;
   case 100:
   default:
     buffer_put(b,"D",1); break;
  }

  for (i = 0;i < len;++i) if (!s[i]) break;
  buffer_put(b,s,i);
}

stralloc lower = {0};
stralloc nughde = {0};
stralloc wildchars = {0};

void nughde_get(char *local)
{
  char *(args[3]);
  int pi[2];
  int gpwpid;
  int gpwstat;
  int r;
  int fd;
  int flagwild;

  if (!stralloc_copys(&lower,"!")) _exit(QLX_NOMEM);
  if (!stralloc_cats(&lower,local)) _exit(QLX_NOMEM);
  if (!stralloc_0(&lower)) _exit(QLX_NOMEM);
  case_lowerb(lower.s,lower.len);

  if (!stralloc_copys(&nughde,"")) _exit(QLX_NOMEM);

  fd = open_read("var/users/cdb");
  if (fd == -1)
    if (errno != ENOENT)
      _exit(QLX_CDB);

  if (fd != -1)
  {
    unsigned int i;

    cdb_init(&c,fd);
    r = cdb_find(&c,"",0);
    if (r != 1) _exit(QLX_CDB);
      if (!stralloc_ready(&wildchars,cdb_datalen(&c))) _exit(QLX_NOMEM);
        wildchars.len = cdb_datalen(&c);
    if (cdb_read(&c,wildchars.s,wildchars.len,cdb_datapos(&c)) == -1) _exit(QLX_CDB);

    i = lower.len;
    flagwild = 0;

    do
    {
      /* i > 0 */
      if (!flagwild || (i == 1) || (byte_chr(wildchars.s,wildchars.len,lower.s[i - 1]) < wildchars.len))
      {
        r = cdb_find(&c,lower.s,i);
        if (r == -1) _exit(QLX_CDB);
        if (r == 1)
        {
          if (!stralloc_ready(&nughde,cdb_datalen(&c))) _exit(QLX_NOMEM);
          nughde.len = cdb_datalen(&c);
          if (cdb_read(&c,nughde.s,nughde.len,cdb_datapos(&c)) == -1) _exit(QLX_CDB);
          if (flagwild)
            if (!stralloc_cats(&nughde,local + i - 1)) _exit(QLX_NOMEM);
          if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);
          close(fd);
          return;
        }
      }
      --i;
      flagwild = 1;
    }
    while (i);

    close(fd);
  }

  if (pipe(pi) == -1) _exit(QLX_SYS);
  args[0] = "bin/qmail-getpw";
  args[1] = local;
  args[2] = 0;
  switch(gpwpid = fork())
  {
    case -1:
      _exit(QLX_SYS);
    case 0:
      if (prot_gid(auto_gidn) == -1) _exit(QLX_USAGE);
      if (prot_uid(auto_uidp) == -1) _exit(QLX_USAGE);
      close(pi[0]);
      if (fd_move(1,pi[1]) == -1) _exit(QLX_SYS);
      execv(*args,args);
      _exit(QLX_EXECPW);
  }
  close(pi[1]);

  if (readclose(pi[0],&nughde,128) == -1) _exit(QLX_SYS);

  if (wait_pid(&gpwstat,gpwpid) != -1)
  {
    if (wait_crashed(gpwstat)) _exit(QLX_SYS);
    if (wait_exitcode(gpwstat) != 0) _exit(wait_exitcode(gpwstat));
  }
}

int spawn(int fdmess,int fdout,char *s,char *r,int at)
{
  int f;

  if (!(f = fork()))
  {
    char *(args[11]);
    unsigned long u;
    int n;
    int uid;
    int gid;
    char *x;
    unsigned int xlen;

    r[at] = 0;
    if (!r[0]) _exit(0); /* <> */

    if (chdir(auto_qmail) == -1) _exit(QLX_USAGE);

    nughde_get(r);

    x = nughde.s;
    xlen = nughde.len;

    args[0] = "bin/qmail-local";
    args[1] = "--";
    args[2] = x;
    n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE); x += n; xlen -= n;

    scan_ulong(x,&u);
    uid = u;
    n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE); x += n; xlen -= n;

    scan_ulong(x,&u);
    gid = u;
    n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE); x += n; xlen -= n;

    args[3] = x;
    n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE); x += n; xlen -= n;

    args[4] = r;
    args[5] = x;
    n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE); x += n; xlen -= n;

    args[6] = x;
    n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE); x += n; xlen -= n;

    args[7] = r + at + 1;
    args[8] = s;
    args[9] = aliasempty;
    args[10] = 0;

    if (fd_move(0,fdmess) == -1) _exit(QLX_SYS);
    if (fd_move(1,fdout) == -1) _exit(QLX_SYS);
    if (fd_copy(2,1) == -1) _exit(QLX_SYS);
    if (prot_gid(gid) == -1) _exit(QLX_USAGE);
    if (prot_uid(uid) == -1) _exit(QLX_USAGE);
    if (!getuid()) _exit(QLX_ROOT);

    execv(*args,args);
    if (errno) _exit(QLX_EXECSOFT);
    _exit(QLX_EXECHARD);
  }
  return f;
}
