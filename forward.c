/*
 *  Revision 20170502, Kai Peter
 *  - switched to 'errmsg', removed 'strerr'
 *  Revision 20170317, Kai Peter
 *  - switched to 'buffer'
 *  - changed return type of main to int
 */
#include <unistd.h>
#include "sig.h"
#include "env.h"
#include "qmail.h"
#include "errmsg.h"
#include "buffer.h"
#include "fmt.h"

#define WHO "forward"

struct qmail qqt;

ssize_t mywrite(int fd,char *buf,int len) {
  qmail_put(&qqt,buf,len);
  return len;
}

char inbuf[BUFFER_INSIZE];
char outbuf[1];
buffer bin = BUFFER_INIT(read,0,inbuf,sizeof inbuf);
buffer bout = BUFFER_INIT(mywrite,-1,outbuf,sizeof outbuf);

char strnum[FMT_ULONG];

int main(int argc,char **argv)
{
  char *sender;
  char *dtline;
  char *qqx;

  sig_pipeignore();

  sender = env_get("NEWSENDER");
  if (!sender)
    err_tmp_plus(ESOFT,"NEWSENDER not set");
  dtline = env_get("DTLINE");
  if (!dtline)
    err_tmp_plus(ESOFT,"DTLINE not set");

  if (qmail_open(&qqt) == -1)
    err_sys_plus(errno,"unable to fork: ");
  qmail_puts(&qqt,dtline);
  if (buffer_copy(&bout,&bin) != 0)
    err_sys_plus(errno,"unable to read message: ");
  buffer_flush(&bout);

  strnum[fmt_ulong(strnum,qmail_qp(&qqt))] = 0;

  qmail_from(&qqt,sender);
  while (*++argv) qmail_to(&qqt,*argv);
  qqx = qmail_close(&qqt);
  if (*qqx) {
    err_sys_plus(*qqx == 'D' ? ESOFT : EHARD,qqx +1); }

//  strerr_die2x(0,"forward: qp ",num);
  buffer_puts(buffer_1,"forward: qp ");
  buffer_puts(buffer_1,strnum);
  buffer_puts(buffer_1,"\n");
  buffer_flush(buffer_1);

  return(0);
}
