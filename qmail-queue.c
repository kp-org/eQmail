/*
 *  Revision 20180417, Kai Peter
 *  - replaced 'substdio' by 'buffer'
 *  - changed queue dir to 'var/queue'
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sig.h"
#include "open.h"
#include "seek.h"
#include "fmt.h"
#include "alloc.h"
#include "buffer.h"
#include "datetime.h"
#include "now.h"
#include "trigger.h"
#include "auto_qmail.h"
#include "auto_uids.h"
#include "date822fmt.h"
#include "fmtqfn.h"

#define WHO "qmail-queue"

/* the 'EXTRA' was in 'extra.h' before (used here only!) */
#define QUEUE_EXTRA ""
#define QUEUE_EXTRALEN 0

#define DEATH 86400 /* 24 hours; _must_ be below q-s's OSSIFIED (36 hours) */
#define ADDR 1003

char inbuf[2048];
buffer bin;
char outbuf[256];
buffer bout;

datetime_sec starttime;
struct datetime dt;
unsigned long mypid;
unsigned long uid;
char *pidfn;
struct stat pidst;
unsigned long messnum;
char *messfn;
char *todofn;
char *intdfn;
int messfd;
int intdfd;
int flagmademess = 0;
int flagmadeintd = 0;

void cleanup()
{
 if (flagmadeintd)
  {
   seek_trunc(intdfd,0);
   if (unlink(intdfn) == -1) return;
  }
 if (flagmademess)
  {
   seek_trunc(messfd,0);
   if (unlink(messfn) == -1) return;
  }
}

void die(e) int e; { _exit(e); }
void die_write() { cleanup(); die(53); }
void die_read() { cleanup(); die(54); }
void sigalrm() { /* thou shalt not clean up here */ die(52); }
void sigbug() { die(81); }

unsigned int receivedlen;
char *received;
/* "Received: (qmail-queue invoked by alias); 26 Sep 1995 04:46:54 -0000\n" */

static unsigned int receivedfmt(char *s)
{
  unsigned int i;
  unsigned int len;
  len = 0;
  i = fmt_str(s,"Received: (qmail "); len += i; if (s) s += i;
  i = fmt_ulong(s,mypid); len += i; if (s) s += i;
  i = fmt_str(s," invoked "); len += i; if (s) s += i;
  if (uid == auto_uida)
    { i = fmt_str(s,"by alias"); len += i; if (s) s += i; }
  else if (uid == auto_uidd)
    { i = fmt_str(s,"from network"); len += i; if (s) s += i; }
  else if (uid == auto_uids)
    { i = fmt_str(s,"for bounce"); len += i; if (s) s += i; }
  else
    {
      i = fmt_str(s,"by uid "); len += i; if (s) s += i;
      i = fmt_ulong(s,uid); len += i; if (s) s += i;
    }
  i = fmt_str(s,"); "); len += i; if (s) s += i;
  i = date822fmt(s,&dt); len += i; if (s) s += i;
  return len;
}

void received_setup()
{
  receivedlen = receivedfmt((char *) 0);
  received = alloc(receivedlen + 1);
  if (!received) die(51);
  receivedfmt(received);
}

unsigned int pidfmt(char *s,unsigned long seq)
{
  unsigned int i;
  unsigned int len;

  len = 0;
  i = fmt_str(s,"pid/"); len += i; if (s) s += i;
  i = fmt_ulong(s,mypid); len += i; if (s) s += i;
  i = fmt_str(s,"."); len += i; if (s) s += i;
  i = fmt_ulong(s,starttime); len += i; if (s) s += i;
  i = fmt_str(s,"."); len += i; if (s) s += i;
  i = fmt_ulong(s,seq); len += i; if (s) s += i;
  ++len; if (s) *s++ = 0;

  return len;
}

char *fnnum(char *dirslash,int flagsplit)
{
  char *s;

  s = alloc(fmtqfn((char *) 0,dirslash,messnum,flagsplit));
  if (!s) die(51);
  fmtqfn(s,dirslash,messnum,flagsplit);
  return s;
}

void pidopen()
{
  unsigned int len;
  unsigned long seq;

  seq = 1;
  len = pidfmt((char *) 0,seq);
  pidfn = alloc(len);
  if (!pidfn) die(51);

  for (seq = 1;seq < 10;++seq)
  {
    if (pidfmt((char *) 0,seq) > len) die(81); /* paranoia */
    pidfmt(pidfn,seq);
    messfd = open_excl(pidfn);
    if (messfd != -1) return;
  }

  die(63);
}

char tmp[FMT_ULONG];

int main()
{
  unsigned int len;
  char ch;

  sig_blocknone();
  umask(033);
  if (chdir(auto_qmail) == -1) die(61);
  if (chdir("var/queue") == -1) die(62);

  mypid = getpid();
  uid = getuid();
  starttime = now();
  datetime_tai(&dt,starttime);

  received_setup();

  sig_pipeignore();
  sig_miscignore();
  sig_alarmcatch(sigalrm);
  sig_bugcatch(sigbug);

  alarm(DEATH);

  pidopen();
  if (fstat(messfd,&pidst) == -1) die(63);

  messnum = pidst.st_ino;
  messfn = fnnum("mess/",1);
  todofn = fnnum("todo/",1);
  intdfn = fnnum("intd/",1);

  if (link(pidfn,messfn) == -1) die(64);
  if (unlink(pidfn) == -1) die(63);
  flagmademess = 1;

  buffer_init(&bout,write,messfd,outbuf,sizeof(outbuf));
  buffer_init(&bin,read,0,inbuf,sizeof(inbuf));

  if (buffer_put(&bout,received,receivedlen) == -1) die_write();

  switch(buffer_copy(&bout,&bin))
  {
    case -2: die_read();
    case -3: die_write();
  }

  if (buffer_flush(&bout) == -1) die_write();
  if (fsync(messfd) == -1) die_write();

  intdfd = open_excl(intdfn);
  if (intdfd == -1) die(65);
  flagmadeintd = 1;

  buffer_init(&bout,write,intdfd,outbuf,sizeof(outbuf));
  buffer_init(&bin,read,1,inbuf,sizeof(inbuf));

  if (buffer_put(&bout,"u",1) == -1) die_write();
  if (buffer_put(&bout,tmp,fmt_ulong(tmp,uid)) == -1) die_write();
  if (buffer_put(&bout,"",1) == -1) die_write();

  if (buffer_put(&bout,"p",1) == -1) die_write();
  if (buffer_put(&bout,tmp,fmt_ulong(tmp,mypid)) == -1) die_write();
  if (buffer_put(&bout,"",1) == -1) die_write();

  if (buffer_get(&bin,&ch,1) < 1) die_read();
  if (ch != 'F') die(91);
  if (buffer_put(&bout,&ch,1) == -1) die_write();
  for (len = 0;len < ADDR;++len)
  {
    if (buffer_get(&bin,&ch,1) < 1) die_read();
    if (buffer_put(&bout,&ch,1) == -1) die_write();
    if (!ch) break;
  }
  if (len >= ADDR) die(11);

  if (buffer_put(&bout,QUEUE_EXTRA,QUEUE_EXTRALEN) == -1) die_write();

  for (;;)
  {
    if (buffer_get(&bin,&ch,1) < 1) die_read();
    if (!ch) break;
    if (ch != 'T') die(91);
    if (buffer_put(&bout,&ch,1) == -1) die_write();
    for (len = 0;len < ADDR;++len)
    {
      if (buffer_get(&bin,&ch,1) < 1) die_read();
      if (buffer_put(&bout,&ch,1) == -1) die_write();
      if (!ch) break;
    }
    if (len >= ADDR) die(11);
  }

  if (buffer_flush(&bout) == -1) die_write();
  if (fsync(intdfd) == -1) die_write();

  if (link(intdfn,todofn) == -1) die(66);

  triggerpull();
  die(0);
}
