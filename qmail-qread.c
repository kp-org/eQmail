/*
 *  Revision 20180417, Kai Peter
 *  - switched to 'buffer', 'errmsg'
 *  - changed 'queue' dir
 *  - removed 'die_nomem()'
 *  Revision 20160907, Kai Peter
 *  - changed return type of main to int
 *  - added parentheses to while loop
 *  - included <unistd.h>
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "buffer.h"
#include "errmsg.h"
#include "fmt.h"
#include "str.h"
#include "getln.h"
#include "fmtqfn.h"
#include "readsubdir.h"
#include "auto_qmail.h"
#include "open.h"
#include "datetime.h"
#include "date822fmt.h"

#define WHO "qmail-qread"

readsubdir rs;

void die(int n) { buffer_flush(buffer_1); _exit(n); }

void warn(char *s1,char *s2)
{
  char *x;
//  x = error_str(errno);
  x = errstr(errno);
  buffer_puts(buffer_1,s1);
  buffer_puts(buffer_1,s2);
  buffer_puts(buffer_1,": ");
  buffer_puts(buffer_1,x);
  buffer_puts(buffer_1,"\n");
}

void die_chdir() { warn("fatal: unable to chdir",""); die(111); }
void die_opendir(char *fn) { warn("fatal: unable to opendir ",fn); die(111); }

void err(unsigned long id)
{
 char foo[FMT_ULONG];
 foo[fmt_ulong(foo,id)] = 0;
 warn("warning: trouble with #",foo);
}

char fnmess[FMTQFN];
char fninfo[FMTQFN];
char fnlocal[FMTQFN];
char fnremote[FMTQFN];
char fnbounce[FMTQFN];

char inbuf[1024];
stralloc sender = {0};

unsigned long id;
datetime_sec qtime;
int flagbounce;
unsigned long size;

unsigned int fmtstats(char *s)
{
  struct datetime dt;
  unsigned int len;
  unsigned int i;

  len = 0;
  datetime_tai(&dt,qtime);
  i = date822fmt(s,&dt) - 7/*XXX*/; len += i; if (s) s += i;
  i = fmt_str(s," GMT  #"); len += i; if (s) s += i;
  i = fmt_ulong(s,id); len += i; if (s) s += i;
  i = fmt_str(s,"  "); len += i; if (s) s += i;
  i = fmt_ulong(s,size); len += i; if (s) s += i;
  i = fmt_str(s,"  <"); len += i; if (s) s += i;
  i = fmt_str(s,sender.s + 1); len += i; if (s) s += i;
  i = fmt_str(s,"> "); len += i; if (s) s += i;
  if (flagbounce)
  {
    i = fmt_str(s," bouncing"); len += i; if (s) s += i;
  }

  return len;
}

stralloc stats = {0};

void out(char *s,unsigned int n)
{
  while (n > 0)
  {
    buffer_put(buffer_1,((*s >= 32) && (*s <= 126)) ? s : "_",1);
    --n;
    ++s;
  }
}
void outs(char *s) { out(s,str_len(s)); }
void outok(char *s) { buffer_puts(buffer_1,s); }

void putstats()
{
  if (!stralloc_ready(&stats,fmtstats(FMT_LEN))) errmem;
  stats.len = fmtstats(stats.s);
  out(stats.s,stats.len);
  outok("\n");
}

stralloc line = {0};

int main()
{
  int channel;
  int match;
  struct stat st;
  int fd;
  buffer b;
  int x;

  if (chdir(auto_qmail) == -1) die_chdir();
  if (chdir("var/queue") == -1) die_chdir();
  readsubdir_init(&rs,"info",die_opendir);

  while ((x = readsubdir_next(&rs,&id)))
    if (x > 0)
    {
      fmtqfn(fnmess,"mess/",id,1);
      fmtqfn(fninfo,"info/",id,1);
      fmtqfn(fnlocal,"local/",id,1);
      fmtqfn(fnremote,"remote/",id,1);
      fmtqfn(fnbounce,"bounce/",id,0);

      if (stat(fnmess,&st) == -1) { err(id); continue; }
      size = st.st_size;
      flagbounce = !stat(fnbounce,&st);

      fd = open_read(fninfo);
      if (fd == -1) { err(id); continue; }
      buffer_init(&b,read,fd,inbuf,sizeof(inbuf));
      if (getln(&b,&sender,&match,0) == -1) errmem;
      if (fstat(fd,&st) == -1) { close(fd); err(id); continue; }
      close(fd);
      qtime = st.st_mtime;

      putstats();

      for (channel = 0;channel < 2;++channel)
      {
        fd = open_read(channel ? fnremote : fnlocal);
        if (fd == -1)
        {
          if (errno != error_noent)
            err(id);
        }
        else
        {
          for (;;)
          {
          if (getln(&b,&line,&match,0) == -1) errmem;
          if (!match) break;
          switch(line.s[0])
            {
            case 'D':
              outok("  done");
            case 'T':
              outok(channel ? "\tremote\t" : "\tlocal\t");
              outs(line.s + 1);
              outok("\n");
              break;
            }
          }
          close(fd);
        }
      }
    }

  die(0);
  return(0);  /* never reached */
}
