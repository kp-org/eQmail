/*
 *  Revision 20180413, Kai Peter
 *  - switched to new qsocket lib (qlibs)
 *  - commented out old timeout* functions
 *  Revision 20160912, Kai Peter
 *  - added cast to function call ip4_scan
 *  Revision 20160713, Kai Peter
 *  - switched to 'buffer'
 *  Revision 20160509, Kai Peter
 *  - changed return type of main to int
 */
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
#include "qsocket.h"
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include "buffer.h"
#include "getln.h"
#include "stralloc.h"
#include "readclose.h"
#include "errmsg.h"
#include "sig.h"
#include "ip.h"
//#include "timeoutconn.h"
//#include "timeoutread.h"
//#include "timeoutwrite.h"
#include "auto_qmail.h"
#include "control.h"
//#include "fmt.h"

#define PORT_QMQP 628

void die_success() { _exit(0); }
void die_perm() { _exit(31); }
void nomem() { _exit(51); }
void die_read() { if (errno == error_nomem) nomem(); _exit(54); }
void die_control() { _exit(55); }
void die_socket() { _exit(56); }
void die_home() { _exit(61); }
void die_temp() { _exit(71); }
void die_conn() { _exit(74); }
void die_format() { _exit(91); }

int lasterror = 55;
int qmqpfd;

//int saferead(fd,buf,len) int fd; char *buf; int len;
ssize_t saferead(int fd,char *buf,int len)
{
  int r;
//  r = timeoutread(60,qmqpfd,buf,len);
  r = timeoutread(qmqpfd,buf,len,60);
  if (r <= 0) die_conn();
  return r;
}
//int safewrite(fd,buf,len) int fd; char *buf; int len;
ssize_t safewrite(int fd,char *buf,int len)
{
  int r;
//  r = timeoutwrite(60,qmqpfd,buf,len);
  r = timeoutwrite(qmqpfd,buf,len,60);
  if (r <= 0) die_conn();
  return r;
}

char buf[1024];
//substdio to = SUBSTDIO_FDBUF(safewrite,-1,buf,sizeof buf);
//substdio from = SUBSTDIO_FDBUF(saferead,-1,buf,sizeof buf);
//substdio envelope = SUBSTDIO_FDBUF(read,1,buf,sizeof buf);
buffer to = BUFFER_INIT(safewrite,-1,buf,sizeof buf);
buffer from = BUFFER_INIT(saferead,-1,buf,sizeof buf);
buffer envelope = BUFFER_INIT(read,1,buf,sizeof buf);
/* WARNING: can use only one of these at a time! */

stralloc beforemessage = {0};
stralloc message = {0};
stralloc aftermessage = {0};

char strnum[FMT_ULONG];
stralloc line = {0};

void getmess()
{
  int match;

  if (readclose(0,&message,1024) == -1) die_read();

  strnum[fmt_ulong(strnum,(unsigned long) message.len)] = 0;
  if (!stralloc_copys(&beforemessage,strnum)) nomem();
  if (!stralloc_cats(&beforemessage,":")) nomem();
  if (!stralloc_copys(&aftermessage,",")) nomem();

  if (getln(&envelope,&line,&match,'\0') == -1) die_read();
  if (!match) die_format();
  if (line.len < 2) die_format();
  if (line.s[0] != 'F') die_format();

  strnum[fmt_ulong(strnum,(unsigned long) line.len - 2)] = 0;
  if (!stralloc_cats(&aftermessage,strnum)) nomem();
  if (!stralloc_cats(&aftermessage,":")) nomem();
  if (!stralloc_catb(&aftermessage,line.s + 1,line.len - 2)) nomem();
  if (!stralloc_cats(&aftermessage,",")) nomem();

  for (;;) {
    if (getln(&envelope,&line,&match,'\0') == -1) die_read();
    if (!match) die_format();
    if (line.len < 2) break;
    if (line.s[0] != 'T') die_format();

    strnum[fmt_ulong(strnum,(unsigned long) line.len - 2)] = 0;
    if (!stralloc_cats(&aftermessage,strnum)) nomem();
    if (!stralloc_cats(&aftermessage,":")) nomem();
    if (!stralloc_catb(&aftermessage,line.s + 1,line.len - 2)) nomem();
    if (!stralloc_cats(&aftermessage,",")) nomem();
  }
}

void doit(char *server)
{
  struct ip_address ip;
  char ch;

  if (!ip4_scan(server,(char *)&ip)) return;

  qmqpfd = socket(AF_INET,SOCK_STREAM,0);
  if (qmqpfd == -1) die_socket();

//  if (timeoutconn4(qmqpfd,&ip,PORT_QMQP,10) != 0) {
  if (timeoutconn(qmqpfd,(char *)&ip,PORT_QMQP,10,0) != 0) {
    lasterror = 73;
//    if (errno == error_timeout) lasterror = 72;
    if (errno == ETIMEDOUT) lasterror = 72;
    close(qmqpfd);
    return;
  }

  strnum[fmt_ulong(strnum,(unsigned long) (beforemessage.len + message.len + aftermessage.len))] = 0;
/*
  substdio_puts(&to,strnum);
  substdio_puts(&to,":");
  substdio_put(&to,beforemessage.s,beforemessage.len);
  substdio_put(&to,message.s,message.len);
  substdio_put(&to,aftermessage.s,aftermessage.len);
  substdio_puts(&to,",");
  substdio_flush(&to);
*/
  buffer_puts(&to,strnum);
  buffer_puts(&to,":");
  buffer_put(&to,beforemessage.s,beforemessage.len);
  buffer_put(&to,message.s,message.len);
  buffer_put(&to,aftermessage.s,aftermessage.len);
  buffer_puts(&to,",");
  buffer_flush(&to);

  for (;;) {
//    substdio_get(&from,&ch,1);
    buffer_get(&from,&ch,1);
    if (ch == 'K') die_success();
    if (ch == 'Z') die_temp();
    if (ch == 'D') die_perm();
  }
}

stralloc servers = {0};

int main()
{
  int i;
  int j;

  sig_pipeignore();

  if (chdir(auto_qmail) == -1) die_home();
  if (control_init() == -1) die_control();
  if (control_readfile(&servers,"control/qmqpservers",0) != 1) die_control();

  getmess();

  i = 0;
  for (j = 0;j < servers.len;++j)
    if (!servers.s[j]) {
      doit(servers.s + i);
      i = j + 1;
    }

  _exit(lasterror);
  return(0);
}
