/*
 *  Revision 20160712, Kai Peter
 *  - switched to 'buffer'
 *  Revision 20160509, Kai Peter
 *  - changed return type of main to int
 *  - added 'sig.h'
 */
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "datetime.h"
#include "wait.h"
#include "fd.h"
#include "fmt.h"
#include "strerr.h"
#include "buffer.h"
#include "sig.h"

#define FATL "predate: fatal: "

static char *montab[12] = {
"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

char num[FMT_ULONG];
char outbuf[1024];

int main(int argc,char **argv)
{
  time_t now;
  struct tm *tm;
  struct datetime dt;
  datetime_sec utc;
  datetime_sec local;
  int minutes;
  int pi[2];
  buffer ss;
  int wstat;
  int pid;

  sig_pipeignore();

  if (!argv[1])
    strerr_die1x(100,"predate: usage: predate child");

  if (pipe(pi) == -1)
    strerr_die2sys(111,FATL,"unable to create pipe: ");

  switch(pid = fork()) {
    case -1:
      strerr_die2sys(111,FATL,"unable to fork: ");
    case 0:
      close(pi[1]);
      if (fd_move(0,pi[0]) == -1)
        strerr_die2sys(111,FATL,"unable to set up fds: ");
      sig_pipedefault();
      execvp(argv[1],argv + 1);
      strerr_die4sys(111,FATL,"unable to run ",argv[1],": ");
  }
  close(pi[0]);
  buffer_init(&ss,write,pi[1],outbuf,sizeof(outbuf));

  time(&now);

  tm = gmtime(&now);
  dt.year = tm->tm_year;
  dt.mon = tm->tm_mon;
  dt.mday = tm->tm_mday;
  dt.hour = tm->tm_hour;
  dt.min = tm->tm_min;
  dt.sec = tm->tm_sec;
  utc = datetime_untai(&dt); /* utc == now, if gmtime ignores leap seconds */

  tm = localtime(&now);
  dt.year = tm->tm_year;
  dt.mon = tm->tm_mon;
  dt.mday = tm->tm_mday;
  dt.hour = tm->tm_hour;
  dt.min = tm->tm_min;
  dt.sec = tm->tm_sec;
  local = datetime_untai(&dt);

  buffer_puts(&ss,"Date: ");
  buffer_put(&ss,num,fmt_uint(num,dt.mday));
  buffer_puts(&ss," ");
  buffer_puts(&ss,montab[dt.mon]);
  buffer_puts(&ss," ");
  buffer_put(&ss,num,fmt_uint(num,dt.year + 1900));
  buffer_puts(&ss," ");
  buffer_put(&ss,num,fmt_uint0(num,dt.hour,2));
  buffer_puts(&ss,":");
  buffer_put(&ss,num,fmt_uint0(num,dt.min,2));
  buffer_puts(&ss,":");
  buffer_put(&ss,num,fmt_uint0(num,dt.sec,2));


  if (local < utc) {
    minutes = (utc - local + 30) / 60;
    buffer_puts(&ss," -");
    buffer_put(&ss,num,fmt_uint0(num,minutes / 60,2));
    buffer_put(&ss,num,fmt_uint0(num,minutes % 60,2));
  }
  else {
    minutes = (local - utc + 30) / 60;
    buffer_puts(&ss," +");
    buffer_put(&ss,num,fmt_uint0(num,minutes / 60,2));
    buffer_put(&ss,num,fmt_uint0(num,minutes % 60,2));
  }

  buffer_puts(&ss,"\n");
  buffer_copy(&ss,buffer_0);
  buffer_flush(&ss);
  close(pi[1]);

  if (wait_pid(&wstat,pid) == -1)
    strerr_die2sys(111,FATL,"wait failed: ");
  if (wait_crashed(wstat))
    strerr_die2x(111,FATL,"child crashed");
  _exit(wait_exitcode(wstat));
  return(0);  /* never reached */
}
