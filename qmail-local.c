/*
 *  Revision 20180409, Kai Peter
 *  - changed buffer variable names, code refactoring (partial)
 *  - replaced 'temp_nomem()' by 'errmem'
 *  Revision 20180123, Kai Peter
 *  - prevented 'misleading-indentation' warning
 *  Revision 20160709, Kai Peter
 *  - switched to 'buffer'
 *  Revision 20160503, Kai Peter
 *  - added explicit braces to inline if-else block (line 321)
 *  - changed return type of main to int
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "buffer.h"
#include "sig.h"
#include "env.h"
#include "byte.h"
#include "open.h"
#include "wait.h"
#include "lock.h"
#include "seek.h"
#include "getln.h"
#include "strerr.h"
#include "getoptb.h"
#include "alloc.h"
#include "errmsg.h"
#include "stralloc.h"
#include "fmt.h"
#include "str.h"
#include "now.h"
#include "case.h"
#include "quote.h"
#include "qmail.h"
#include "slurpclose.h"
#include "myctime.h"
#include "gfrom.h"
#include "auto_patrn.h"

#define WHO "qmail-send"

void usage() { strerr_die1x(100,"qmail-local: usage: qmail-local [ -nN ] user homedir local dash ext domain sender aliasempty"); }

//void temp_nomem() { strerr_die1x(111,"Out of memory. (#4.3.0)"); }
void temp_rewind() { strerr_die1x(111,"Unable to rewind message. (#4.3.0)"); }
void temp_childcrashed() { strerr_die1x(111,"Aack, child crashed. (#4.3.0)"); }
void temp_fork() { strerr_die3x(111,"Unable to fork: ",error_str(errno),". (#4.3.0)"); }
void temp_read() { strerr_die3x(111,"Unable to read message: ",error_str(errno),". (#4.3.0)"); }
void temp_slowlock()
{ strerr_die1x(111,"File has been locked for 30 seconds straight. (#4.3.0)"); }
void temp_qmail(fn) char *fn;
{ strerr_die5x(111,"Unable to open ",fn,": ",error_str(errno),". (#4.3.0)"); }

int flagdoit;
int flag99;

char *user;
char *homedir;
char *local;
char *dash;
char *ext;
char *host;
char *sender;
char *aliasempty;

stralloc safeext = {0};
stralloc ufline = {0};
stralloc rpline = {0};
stralloc envrecip = {0};
stralloc dtline = {0};
stralloc qme = {0};
stralloc ueo = {0};
stralloc cmds = {0};
stralloc messline = {0};
stralloc foo = {0};

char buf[1024];
char outbuf[1024];

/* child process */

char fntmptph[80 + FMT_ULONG * 2];
char fnnewtph[80 + FMT_ULONG * 2];
void tryunlinktmp() { unlink(fntmptph); }
void sigalrm() { tryunlinktmp(); _exit(3); }

void maildir_child(dir)
char *dir;
{
 unsigned long pid;
 unsigned long time;
 char host[64];
 char *s;
 int loop;
 struct stat st;
 int fd;
 buffer b;
 buffer bout; // = BUFFER_INIT(write,1,outbuf,sizeof outbuf);

 sig_alarmcatch(sigalrm);
 if (chdir(dir) == -1) { if (error_temp(errno)) _exit(1); _exit(2); }
 pid = getpid();
 host[0] = 0;
 gethostname(host,sizeof(host));
 for (loop = 0;;++loop)
  {
   time = now();
   s = fntmptph;
   s += fmt_str(s,"tmp/");
   s += fmt_ulong(s,time); *s++ = '.';
   s += fmt_ulong(s,pid); *s++ = '.';
   s += fmt_strn(s,host,sizeof(host)); *s++ = 0;
   if (stat(fntmptph,&st) == -1) if (errno == error_noent) break;
   /* really should never get to this point */
   if (loop == 2) _exit(1);
   sleep(2);
  }
 str_copy(fnnewtph,fntmptph);
 byte_copy(fnnewtph,3,"new");

 alarm(86400);
 fd = open_excl(fntmptph);
 if (fd == -1) _exit(1);

 buffer_init(&b,read,0,buf,sizeof(buf));
 buffer_init(&bout,write,fd,outbuf,sizeof(outbuf));
 if (buffer_put(&bout,rpline.s,rpline.len) == -1) goto fail;
 if (buffer_put(&bout,dtline.s,dtline.len) == -1) goto fail;

 switch(buffer_copy(&bout,&b))
  {
   case -2: tryunlinktmp(); _exit(4);
   case -3: goto fail;
  }

 if (buffer_flush(&bout) == -1) goto fail;
 if (fsync(fd) == -1) goto fail;
 if (close(fd) == -1) goto fail; /* NFS dorks */

 if (link(fntmptph,fnnewtph) == -1) goto fail;
  /* if it was error_exist, almost certainly successful; i hate NFS */
 tryunlinktmp(); _exit(0);

 fail: tryunlinktmp(); _exit(1);
}

/* end child process */

void maildir(char *fn) {
  int child;
  int wstat;

  if (seek_begin(0) == -1) temp_rewind();

  switch(child = fork()) {
  case -1:
    temp_fork();
  case 0:
    maildir_child(fn);
    _exit(111);
  }

  wait_pid(&wstat,child);
  if (wait_crashed(wstat))
    temp_childcrashed();
  switch(wait_exitcode(wstat)) {
   case 0: break;
   case 2: strerr_die1x(111,"Unable to chdir to maildir. (#4.2.1)");
   case 3: strerr_die1x(111,"Timeout on maildir delivery. (#4.3.0)");
   case 4: strerr_die1x(111,"Unable to read message. (#4.3.0)");
   default: strerr_die1x(111,"Temporary error on maildir delivery. (#4.3.0)");
  }
}

void mailfile(char *fn)
{
  int fd;
  buffer b;
  buffer bout;
  int match;
  seek_pos pos;
  int flaglocked;

  if (seek_begin(0) == -1) temp_rewind();

  fd = open_append(fn);
  if (fd == -1)
    strerr_die5x(111,"Unable to open ",fn,": ",error_str(errno),". (#4.2.1)");

  sig_alarmcatch(temp_slowlock);
  alarm(30);
  flaglocked = (lock_ex(fd) != -1);
  alarm(0);
  sig_alarmdefault();

  seek_end(fd);
  pos = seek_cur(fd);

  buffer_init(&b,read,0,buf,sizeof(buf));
  buffer_init(&bout,write,fd,outbuf,sizeof(outbuf));
  if (buffer_put(&bout,ufline.s,ufline.len)) goto writeerrs;
  if (buffer_put(&bout,rpline.s,rpline.len)) goto writeerrs;
  if (buffer_put(&bout,dtline.s,dtline.len)) goto writeerrs;
  for (;;)
  {
    if (getln(&b,&messline,&match,'\n') != 0) 
    {
      strerr_warn3("Unable to read message: ",error_str(errno),". (#4.3.0)",0);
      if (flaglocked) { seek_trunc(fd,pos); } close(fd);
      _exit(111);
    }
    if (!match && !messline.len) break;
    if (gfrom(messline.s,messline.len))
      if (buffer_put(&bout,">",1)) goto writeerrs;
    if (buffer_put(&bout,messline.s,messline.len)) goto writeerrs;
    if (!match)
    {
      if (buffer_puts(&bout,"\n")) goto writeerrs;
      break;
    }
  }
  if (buffer_puts(&bout,"\n")) goto writeerrs;
  if (buffer_flush(&bout)) goto writeerrs;
  if (fsync(fd) == -1) goto writeerrs;
  close(fd);
  return;

  writeerrs:
  strerr_warn5("Unable to write ",fn,": ",error_str(errno),". (#4.3.0)",0);
  if (flaglocked) seek_trunc(fd,pos);
  close(fd);
  _exit(111);
}

void mailprogram(char *prog) {
  int child;
  char *(args[4]);
  int wstat;

 if (seek_begin(0) == -1) temp_rewind();

 switch(child = fork())
  {
   case -1:
     temp_fork();
   case 0:
     args[0] = "/bin/sh"; args[1] = "-c"; args[2] = prog; args[3] = 0;
     sig_pipedefault();
     execv(*args,args);
     strerr_die3x(111,"Unable to run /bin/sh: ",error_str(errno),". (#4.3.0)");
  }

 wait_pid(&wstat,child);
 if (wait_crashed(wstat))
   temp_childcrashed();
 switch(wait_exitcode(wstat))
  {
   case 100:
   case 64: case 65: case 70: case 76: case 77: case 78: case 112: _exit(100);
   case 0: break;
   case 99: flag99 = 1; break;
   default: _exit(111);
  }
}

unsigned long mailforward_qp = 0;

void mailforward(char **recips) {
  struct qmail qqt;
  char *qqx;
  buffer b;
  int match;

  if (seek_begin(0) == -1) temp_rewind();
  buffer_init(&b,read,0,buf,sizeof(buf));

  if (qmail_open(&qqt) == -1) temp_fork();
  mailforward_qp = qmail_qp(&qqt);
  qmail_put(&qqt,dtline.s,dtline.len);
  do
  {
    if (getln(&b,&messline,&match,'\n') != 0) { qmail_fail(&qqt); break; }
    qmail_put(&qqt,messline.s,messline.len);
  }
  while (match);
  qmail_from(&qqt,ueo.s);
  while (*recips) qmail_to(&qqt,*recips++);
  qqx = qmail_close(&qqt);
  if (!*qqx) return;
  strerr_die3x(*qqx == 'D' ? 100 : 111,"Unable to forward message: ",qqx + 1,".");
}

void bouncexf() {
  int match;
  buffer b;

  if (seek_begin(0) == -1) temp_rewind();
  buffer_init(&b,read,0,buf,sizeof(buf));
  for (;;) {
    if (getln(&b,&messline,&match,'\n') != 0) temp_read();
    if (!match) break;
    if (messline.len <= 1)
      break;
    if (messline.len == dtline.len)
      if (!str_diffn(messline.s,dtline.s,dtline.len))
        strerr_die1x(100,"This message is looping: it already has my Delivered-To line. (#5.4.6)");
  }
}

void checkhome()
{
 struct stat st;

 if (stat(".",&st) == -1)
   strerr_die3x(111,"Unable to stat home directory: ",error_str(errno),". (#4.3.0)");
 if (st.st_mode & auto_patrn)
   strerr_die1x(111,"Uh-oh: home directory is writable. (#4.7.0)");
 if (st.st_mode & 01000)
  {
   if (flagdoit)
     strerr_die1x(111,"Home directory is sticky: user is editing his .qmail file. (#4.2.1)");
   else
     strerr_warn1("Warning: home directory is sticky.",0);
  }
}

int qmeox(dashowner)
char *dashowner;
{
 struct stat st;

 if (!stralloc_copys(&qme,".qmail")) errmem;
 if (!stralloc_cats(&qme,dash)) errmem;
 if (!stralloc_cat(&qme,&safeext)) errmem;
 if (!stralloc_cats(&qme,dashowner)) errmem;
 if (!stralloc_0(&qme)) errmem;
 if (stat(qme.s,&st) == -1)
  {
   if (error_temp(errno)) temp_qmail(qme.s);
   return -1;
  }
 return 0;
}

int qmeexists(fd,cutable)
int *fd;
int *cutable;
{
  struct stat st;

  if (!stralloc_0(&qme)) errmem;

  *fd = open_read(qme.s);
  if (*fd == -1) {
    if (error_temp(errno)) temp_qmail(qme.s);
    if (errno == error_perm) temp_qmail(qme.s);
    if (errno == error_acces) temp_qmail(qme.s);
    return 0;
  }

  if (fstat(*fd,&st) == -1) temp_qmail(qme.s);
  if ((st.st_mode & S_IFMT) == S_IFREG) {
    if (st.st_mode & auto_patrn)
      strerr_die1x(111,"Uh-oh: .qmail file is writable. (#4.7.0)");
    *cutable = !!(st.st_mode & 0100);
    return 1;
  }
  close(*fd);
  return 0;
}

/* "" "": "" */
/* "-/" "": "-/" "-/default" */
/* "-/" "a": "-/a" "-/default" */
/* "-/" "a-": "-/a-" "-/a-default" "-/default" */
/* "-/" "a-b": "-/a-b" "-/a-default" "-/default" */
/* "-/" "a-b-": "-/a-b-" "-/a-b-default" "-/a-default" "-/default" */
/* "-/" "a-b-c": "-/a-b-c" "-/a-b-default" "-/a-default" "-/default" */

void qmesearch(fd,cutable)
int *fd;
int *cutable;
{
  int i;

  if (!stralloc_copys(&qme,".qmail")) errmem;
  if (!stralloc_cats(&qme,dash)) errmem;
  if (!stralloc_cat(&qme,&safeext)) errmem;
  if (qmeexists(fd,cutable)) {
    if (safeext.len >= 7) {
      i = safeext.len - 7;
      if (!byte_diff("default",7,safeext.s + i))
	if (i <= str_len(ext)) /* paranoia */
	  if (!env_put("DEFAULT",ext + i)) errmem;
    }
    return;
  }

  for (i = safeext.len;i >= 0;--i)
    if (!i || (safeext.s[i - 1] == '-')) {
      if (!stralloc_copys(&qme,".qmail")) errmem;
      if (!stralloc_cats(&qme,dash)) errmem;
      if (!stralloc_catb(&qme,safeext.s,i)) errmem;
      if (!stralloc_cats(&qme,"default")) errmem;
      if (qmeexists(fd,cutable)) {
	if (i <= str_len(ext)) /* paranoia */
	  if (!env_put("DEFAULT",ext + i)) errmem;
        return;
      }
    }

  *fd = -1;
}

unsigned long count_file = 0;
unsigned long count_forward = 0;
unsigned long count_program = 0;
char count_buf[FMT_ULONG];

void count_print() {
// Kai: subfdoutsmall uses 256b of buffer size, buffer_1 8092b (default)
// substdio_puts(subfdoutsmall,"did ");
  buffer_puts(buffer_1,"did ");
  buffer_put(buffer_1,count_buf,fmt_ulong(count_buf,count_file));
  buffer_puts(buffer_1,"+");
  buffer_put(buffer_1,count_buf,fmt_ulong(count_buf,count_forward));
  buffer_puts(buffer_1,"+");
  buffer_put(buffer_1,count_buf,fmt_ulong(count_buf,count_program));
  buffer_puts(buffer_1,"\n");
  if (mailforward_qp) {
    buffer_puts(buffer_1,"qp ");
    buffer_put(buffer_1,count_buf,fmt_ulong(count_buf,mailforward_qp));
    buffer_puts(buffer_1,"\n");
  }
  buffer_flush(buffer_1);
}

void sayit(char *type, char *cmd, int len) {
  buffer_puts(buffer_1,type);
  buffer_put(buffer_1,cmd,len);
  buffer_putsflush(buffer_1,"\n");
}

int main(int argc,char **argv)
{
 int opt;
 int i;
 int j;
 int k;
 int fd;
 int numforward;
 char **recips;
 datetime_sec starttime;
 int flagforwardonly;
 char *x;

 umask(077);
 sig_pipeignore();

 if (!env_init()) errmem;

 flagdoit = 1;
 while ((opt = getopt(argc,argv,"nN")) != opteof)
   switch(opt)
    {
     case 'n': flagdoit = 0; break;
     case 'N': flagdoit = 1; break;
     default:
       usage();
    }
 argc -= optind;
 argv += optind;

 if (!(user = *argv++)) usage();
 if (!(homedir = *argv++)) usage();
 if (!(local = *argv++)) usage();
 if (!(dash = *argv++)) usage();
 if (!(ext = *argv++)) usage();
 if (!(host = *argv++)) usage();
 if (!(sender = *argv++)) usage();
 if (!(aliasempty = *argv++)) usage();
 if (*argv) usage();

 if (homedir[0] != '/') usage();
 if (chdir(homedir) == -1)
   strerr_die5x(111,"Unable to switch to ",homedir,": ",error_str(errno),". (#4.3.0)");
 checkhome();

 if (!env_put("HOST",host)) errmem;
 if (!env_put("HOME",homedir)) errmem;
 if (!env_put("USER",user)) errmem;
 if (!env_put("LOCAL",local)) errmem;

 if (!stralloc_copys(&envrecip,local)) errmem;
 if (!stralloc_cats(&envrecip,"@")) errmem;
 if (!stralloc_cats(&envrecip,host)) errmem;

 if (!stralloc_copy(&foo,&envrecip)) errmem;
 if (!stralloc_0(&foo)) errmem;
 if (!env_put("RECIPIENT",foo.s)) errmem;

 if (!stralloc_copys(&dtline,"Delivered-To: ")) errmem;
 if (!stralloc_cat(&dtline,&envrecip)) errmem;
 for (i = 0;i < dtline.len;++i) if (dtline.s[i] == '\n') dtline.s[i] = '_';
 if (!stralloc_cats(&dtline,"\n")) errmem;

 if (!stralloc_copy(&foo,&dtline)) errmem;
 if (!stralloc_0(&foo)) errmem;
 if (!env_put("DTLINE",foo.s)) errmem;

 if (flagdoit)
   bouncexf();

 if (!env_put("SENDER",sender)) errmem;

 if (!quote2(&foo,sender)) errmem;
 if (!stralloc_copys(&rpline,"Return-Path: <")) errmem;
 if (!stralloc_cat(&rpline,&foo)) errmem;
 for (i = 0;i < rpline.len;++i) if (rpline.s[i] == '\n') rpline.s[i] = '_';
 if (!stralloc_cats(&rpline,">\n")) errmem;

 if (!stralloc_copy(&foo,&rpline)) errmem;
 if (!stralloc_0(&foo)) errmem;
 if (!env_put("RPLINE",foo.s)) errmem;

 if (!stralloc_copys(&ufline,"From ")) errmem;
 if (*sender)
  {
   int len; int i; char ch;

   len = str_len(sender);
   if (!stralloc_readyplus(&ufline,len)) errmem;
   for (i = 0;i < len;++i)
    {
     ch = sender[i];
     if ((ch == ' ') || (ch == '\t') || (ch == '\n')) ch = '-';
     ufline.s[ufline.len + i] = ch;
    }
   ufline.len += len;
  }
 else
   if (!stralloc_cats(&ufline,"MAILER-DAEMON")) errmem;
 if (!stralloc_cats(&ufline," ")) errmem;
 starttime = now();
 if (!stralloc_cats(&ufline,myctime(starttime))) errmem;

 if (!stralloc_copy(&foo,&ufline)) errmem;
 if (!stralloc_0(&foo)) errmem;
 if (!env_put("UFLINE",foo.s)) errmem;

 x = ext;
 if (!env_put("EXT",x)) errmem;
 x += str_chr(x,'-'); if (*x) ++x;
 if (!env_put("EXT2",x)) errmem;
 x += str_chr(x,'-'); if (*x) ++x;
 if (!env_put("EXT3",x)) errmem;
 x += str_chr(x,'-'); if (*x) ++x;
 if (!env_put("EXT4",x)) errmem;

 if (!stralloc_copys(&safeext,ext)) errmem;
 case_lowerb(safeext.s,safeext.len);
 for (i = 0;i < safeext.len;++i)
   if (safeext.s[i] == '.')
     safeext.s[i] = ':';

 i = str_len(host);
 i = byte_rchr(host,i,'.');
 if (!stralloc_copyb(&foo,host,i)) errmem;
 if (!stralloc_0(&foo)) errmem;
 if (!env_put("HOST2",foo.s)) errmem;
 i = byte_rchr(host,i,'.');
 if (!stralloc_copyb(&foo,host,i)) errmem;
 if (!stralloc_0(&foo)) errmem;
 if (!env_put("HOST3",foo.s)) errmem;
 i = byte_rchr(host,i,'.');
 if (!stralloc_copyb(&foo,host,i)) errmem;
 if (!stralloc_0(&foo)) errmem;
 if (!env_put("HOST4",foo.s)) errmem;

 flagforwardonly = 0;
 qmesearch(&fd,&flagforwardonly);
 if (fd == -1)
   if (*dash)
     strerr_die1x(100,"Sorry, no mailbox here by that name. (#5.1.1)");

 if (!stralloc_copys(&ueo,sender)) errmem;
 if (str_diff(sender,""))
   if (str_diff(sender,"#@[]"))
     if (qmeox("-owner") == 0)
      {
       if (qmeox("-owner-default") == 0)
	{
         if (!stralloc_copys(&ueo,local)) errmem;
         if (!stralloc_cats(&ueo,"-owner-@")) errmem;
         if (!stralloc_cats(&ueo,host)) errmem;
         if (!stralloc_cats(&ueo,"-@[]")) errmem;
	}
       else
	{
         if (!stralloc_copys(&ueo,local)) errmem;
         if (!stralloc_cats(&ueo,"-owner@")) errmem;
         if (!stralloc_cats(&ueo,host)) errmem;
	}
      }
 if (!stralloc_0(&ueo)) errmem;
 if (!env_put("NEWSENDER",ueo.s)) errmem;

 if (!stralloc_ready(&cmds,0)) errmem;
 cmds.len = 0;
 if (fd != -1)
   if (slurpclose(fd,&cmds,256) == -1) errmem;

 if (!cmds.len)
  {
   if (!stralloc_copys(&cmds,aliasempty)) errmem;
   flagforwardonly = 0;
  }
 if (!cmds.len || (cmds.s[cmds.len - 1] != '\n'))
   if (!stralloc_cats(&cmds,"\n")) errmem;

 numforward = 0;
 i = 0;
 for (j = 0;j < cmds.len;++j)
   if (cmds.s[j] == '\n')
    {
     switch(cmds.s[i]) { case '#': case '.': case '/': case '|': break;
       default: ++numforward; }
     i = j + 1;
    }

 recips = (char **) alloc((numforward + 1) * sizeof(char *));
 if (!recips) errmem;
 numforward = 0;

 flag99 = 0;

 i = 0;
 for (j = 0;j < cmds.len;++j)
   if (cmds.s[j] == '\n')
    {
     cmds.s[j] = 0;
     k = j;
     while ((k > i) && ((cmds.s[k - 1] == ' ') || (cmds.s[k - 1] == '\t')))
       cmds.s[--k] = 0;
     switch(cmds.s[i]) {
       case 0: /* k == i */
         if (i) break;
//         strerr_die1x(111,"Uh-oh: first line of .qmail file is blank. (#4.2.1)");
         errint(EHARD,"first line of .qmail file is blank. (#4.2.1)");
       case '#': break;
       case '.':
       case '/':
	 ++count_file;
	 if (flagforwardonly) strerr_die1x(111,"Uh-oh: .qmail has file delivery but has x bit set. (#4.7.0)");
	 if (cmds.s[k - 1] == '/')
           if (flagdoit) maildir(cmds.s + i);
           else sayit("maildir ",cmds.s + i,k - i);
	 else
           if (flagdoit) mailfile(cmds.s + i);
           else sayit("mbox ",cmds.s + i,k - i);
         break;
       case '|':
	 ++count_program;
	 if (flagforwardonly) strerr_die1x(111,"Uh-oh: .qmail has prog delivery but has x bit set. (#4.7.0)");
         if (flagdoit) mailprogram(cmds.s + i + 1);
         else sayit("program ",cmds.s + i + 1,k - i - 1);
         break;
       case '+':
	     if (str_equal(cmds.s + i + 1,"list"))
	       flagforwardonly = 1;
	     break;
       case '&':
         ++i;
       default:
	     ++count_forward;
         if (flagdoit) recips[numforward++] = cmds.s + i;
         else sayit("forward ",cmds.s + i,k - i);
         break;
     }  /* --> end 'switch(cmd.s[i])' */
     i = j + 1;
     if (flag99) break;
  }  /* --> end 'if (cmd.s[j] == '\n')' */

  if (numforward) if (flagdoit) {
    recips[numforward] = 0;
    mailforward(recips);
  }

  count_print();
  _exit(0);
}
