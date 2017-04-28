/*
 *  Revision 20160711, Kai Peter
 *  - switched to 'buffer', <unistd.h>
 *  Revision 20160503, Kai Peter
 *  - changed return type of main to int
 */
#include <unistd.h>
#include "sig.h"
#include "env.h"
#include "error.h"
#include "wait.h"
#include "seek.h"
#include "qmail.h"
#include "strerr.h"
#include "buffer.h"
#include "fmt.h"

#define FATL "condredirect: fatal: "

struct qmail qqt;

ssize_t mywrite(int fd,char *buf,int len)
{
  qmail_put(&qqt,buf,len);
  return len;
}

char inbuf[BUFFER_INSIZE];
char outbuf[1];
buffer ssin = BUFFER_INIT(read,0,inbuf,sizeof inbuf);
buffer ssout = BUFFER_INIT(mywrite,-1,outbuf,sizeof outbuf);

char num[FMT_ULONG];

int main(int argc,char **argv)
{
  char *sender;
  char *dtline;
  int pid;
  int wstat;
  char *qqx;

  if (!argv[1] || !argv[2])
    strerr_die1x(100,"condredirect: usage: condredirect newaddress program [ arg ... ]");

  pid = fork();
  if (pid == -1)
    strerr_die2sys(111,FATL,"unable to fork: ");
  if (pid == 0) {
    execvp(argv[2],argv + 2);
    if (error_temp(errno)) _exit(111);
    _exit(100);
  }
  if (wait_pid(&wstat,pid) == -1)
    strerr_die2x(111,FATL,"wait failed");
  if (wait_crashed(wstat))
    strerr_die2x(111,FATL,"child crashed");
  switch(wait_exitcode(wstat)) {
    case 0: break;
    case 111: strerr_die2x(111,FATL,"temporary child error");
    default: _exit(0);
  }

  if (seek_begin(0) == -1)
    strerr_die2sys(111,FATL,"unable to rewind: ");
  sig_pipeignore();

  sender = env_get("SENDER");
  if (!sender) strerr_die2x(100,FATL,"SENDER not set");
  dtline = env_get("DTLINE");
  if (!dtline) strerr_die2x(100,FATL,"DTLINE not set");

  if (qmail_open(&qqt) == -1)
    strerr_die2sys(111,FATL,"unable to fork: ");
  qmail_puts(&qqt,dtline);
  if (buffer_copy(&ssout,&ssin) != 0)
    strerr_die2sys(111,FATL,"unable to read message: ");
  buffer_flush(&ssout);

  num[fmt_ulong(num,qmail_qp(&qqt))] = 0;

  qmail_from(&qqt,sender);
  qmail_to(&qqt,argv[1]);
  qqx = qmail_close(&qqt);
  if (*qqx) strerr_die2x(*qqx == 'D' ? 100 : 111,FATL,qqx + 1);
  strerr_die2x(99,"condredirect: qp ",num);
  return(0);  /* never reached */
}
