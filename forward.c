/*
 *  Revistion 20170317, Kai Peter
 *  - switched to 'buffer'
 *  - changed return type of main to int
 */
#include "sig.h"
#include <unistd.h>
#include "env.h"
#include "qmail.h"
#include "strerr.h"
#include "buffer.h"
#include "fmt.h"

#define FATL "forward: fatal: "

void die_nomem() { strerr_die2x(111,FATL,"out of memory"); }

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
  char *qqx;

  sig_pipeignore();

  sender = env_get("NEWSENDER");
  if (!sender)
    strerr_die2x(100,FATL,"NEWSENDER not set");
  dtline = env_get("DTLINE");
  if (!dtline)
    strerr_die2x(100,FATL,"DTLINE not set");

  if (qmail_open(&qqt) == -1)
    strerr_die2sys(111,FATL,"unable to fork: ");
  qmail_puts(&qqt,dtline);
  if (buffer_copy(&ssout,&ssin) != 0)
    strerr_die2sys(111,FATL,"unable to read message: ");
  buffer_flush(&ssout);

  num[fmt_ulong(num,qmail_qp(&qqt))] = 0;

  qmail_from(&qqt,sender);
  while (*++argv) qmail_to(&qqt,*argv);
  qqx = qmail_close(&qqt);
  if (*qqx) strerr_die2x(*qqx == 'D' ? 100 : 111,FATL,qqx + 1);
  strerr_die2x(0,"forward: qp ",num);
  return(0);  /* never reached */
}
