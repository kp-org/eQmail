/*
 *  - excluded "bounces", "todo"
 *  Revision 20171130, Kai Peter
 *  - changed folder name 'control' to 'etc'
 *  Revision 20160712, Kai Peter
 *  - switched to 'buffer'
 *  Revision 20160509, Kai Peter
 *  - changed return type of main to int
 *  - added parentheses to conditions
 *  - remove unused var 'int z;' from functon 'todo_do'
 *  - added 'utime.h' and cast pointer of utime()
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sig.h"
#include "direntry.h"
#include "control.h"
#include "select.h"
#include "open.h"
#include "seek.h"
#include "lock.h"
#include "ndelay.h"
#include "now.h"
#include "getln.h"
#include "buffer.h"
#include "alloc.h"
#include "errmsg.h"
#include "stralloc.h"
#include "str.h"
#include "byte.h"
#include "fmt.h"
#include "scan.h"
#include "case.h"
#include "fmtqfn.h"
#include "auto_qmail.h"
#include "trigger.h"
#include "newfield.h"
#include "quote.h"
#include "qmail.h"
#include "qsutil.h"
#include "prioq.h"
#include "constmap.h"
#include "readsubdir.h"
#include <utime.h>

#define WHO "qmail-send"

/* critical timing feature #1: if not triggered, do not busy-loop */
/* critical timing feature #2: if triggered, respond within fixed time */
/* important timing feature: when triggered, respond instantly */
#define SLEEP_TODO 1500 /* check todo/ every 25 minutes in any case */
#define SLEEP_FUZZ 1 /* slop a bit on sleeps to avoid zeno effect */
#define SLEEP_FOREVER 86400 /* absolute maximum time spent in select() */
#define SLEEP_CLEANUP 76431 /* time between cleanups */
#define SLEEP_SYSFAIL 123
#define OSSIFIED 129600 /* 36 hours; _must_ exceed q-q's DEATH (24 hours) */

int lifetime = 604800;

stralloc percenthack = {0};
struct constmap mappercenthack;
stralloc locals = {0};
struct constmap maplocals;
stralloc vdoms = {0};
struct constmap mapvdoms;
stralloc envnoathost = {0};
stralloc bouncefrom = {0};
stralloc bouncehost = {0};
stralloc doublebounceto = {0};
stralloc doublebouncehost = {0};

char strnum2[FMT_ULONG];
char strnum3[FMT_ULONG];

#define CHANNELS 2
char *chanaddr[CHANNELS] = { "local/", "remote/" };
char *chanstatusmsg[CHANNELS] = { " local ", " remote " };
char *tochan[CHANNELS] = { " to local ", " to remote " };
int chanfdout[CHANNELS] = { 1, 3 };
int chanfdin[CHANNELS] = { 2, 4 };
int chanskip[CHANNELS] = { 10, 20 };

int flagexitasap = 0; void sigterm() { flagexitasap = 1; }
int flagrunasap = 0; void sigalrm() { flagrunasap = 1; }
int flagreadasap = 0; void sighup() { flagreadasap = 1; }

//void cleandied() { log1("alert: oh no! lost qmail-clean connection! dying...\n");
void cleandied() {
  log("alert: lost qmail-clean connection! dying ...\n");
  flagexitasap = 1;
}

int flagspawnalive[CHANNELS];
//void spawndied(c) int c; { log1("alert: oh no! lost spawn connection! dying...\n");
void spawndied(int c) {
  log("alert: lost spawn connection! dying...\n");
  flagspawnalive[c] = 0; flagexitasap = 1;
}

#define REPORTMAX 10000

datetime_sec recent;


/* this file is too long ----------------------------------------- FILENAMES */

stralloc fn = {0};
stralloc fn2 = {0};
char fnmake_strnum[FMT_ULONG];

void fnmake_init() {
  while (!stralloc_ready(&fn,FMTQFN)) nomem();
  while (!stralloc_ready(&fn2,FMTQFN)) nomem();
}

void fnmake_info(id) unsigned long id; { fn.len = fmt_qfn(fn.s,"info/",id,1); }
void fnmake_todo(id) unsigned long id; { fn.len = fmt_qfn(fn.s,"todo/",id,1); }
void fnmake_mess(id) unsigned long id; { fn.len = fmt_qfn(fn.s,"mess/",id,1); }
void fnmake_foop(id) unsigned long id; { fn.len = fmt_qfn(fn.s,"foop/",id,0); }
void fnmake_split(id) unsigned long id; { fn.len = fmt_qfn(fn.s,"",id,1); }
void fnmake2_bounce(id) unsigned long id;
{ fn2.len = fmt_qfn(fn2.s,"bounce/",id,0); }
void fnmake_chanaddr(id,c) unsigned long id; int c;
{ fn.len = fmt_qfn(fn.s,chanaddr[c],id,1); }


/* this file is too long ----------------------------------------- REWRITING */

stralloc rwline = {0};

/* 1 if by land, 2 if by sea, 0 if out of memory. not allowed to barf. */
/* may trash recip. must set up rwline, between a T and a \0. */
int rewrite(char *recip)
{
  int i;
  int j;
  char *x;
  static stralloc addr = {0};
  int at;

  if (!stralloc_copys(&rwline,"T")) return 0;
  if (!stralloc_copys(&addr,recip)) return 0;

  i = byte_rchr(addr.s,addr.len,'@');
  if (i == addr.len) {
    if (!stralloc_cats(&addr,"@")) return 0;
    if (!stralloc_cat(&addr,&envnoathost)) return 0;
  }

  while (constmap(&mappercenthack,addr.s + i + 1,addr.len - i - 1)) {
    j = byte_rchr(addr.s,i,'%');
    if (j == i) break;
    addr.len = i;
    i = j;
    addr.s[i] = '@';
  }

  at = byte_rchr(addr.s,addr.len,'@');

  if (constmap(&maplocals,addr.s + at + 1,addr.len - at - 1)) {
    if (!stralloc_cat(&rwline,&addr)) return 0;
    if (!stralloc_0(&rwline)) return 0;
    return 1;
  }

  for (i = 0;i <= addr.len;++i)
    if (!i || (i == at + 1) || (i == addr.len) || ((i > at) && (addr.s[i] == '.')))
      if ((x = constmap(&mapvdoms,addr.s + i,addr.len - i))) {
        if (!*x) break;
        if (!stralloc_cats(&rwline,x)) return 0;
        if (!stralloc_cats(&rwline,"-")) return 0;
        if (!stralloc_cat(&rwline,&addr)) return 0;
        if (!stralloc_0(&rwline)) return 0;
        return 1;
      }
 
  if (!stralloc_cat(&rwline,&addr)) return 0;
  if (!stralloc_0(&rwline)) return 0;
  return 2;
}

void senderadd(sa,sender,recip)
stralloc *sa;
char *sender;
char *recip;
{
 int i;
 int j;
 int k;

 i = str_len(sender);
 if (i >= 4)
   if (str_equal(sender + i - 4,"-@[]"))
    {
     j = byte_rchr(sender,i - 4,'@');
     k = str_rchr(recip,'@');
     if (recip[k] && (j + 5 <= i))
      {
       /* owner-@host-@[] -> owner-recipbox=reciphost@host */
       while (!stralloc_catb(sa,sender,j)) nomem();
       while (!stralloc_catb(sa,recip,k)) nomem();
       while (!stralloc_cats(sa,"=")) nomem();
       while (!stralloc_cats(sa,recip + k + 1)) nomem();
       while (!stralloc_cats(sa,"@")) nomem();
       while (!stralloc_catb(sa,sender + j + 1,i - 5 - j)) nomem();
       return;
      }
    }
 while (!stralloc_cats(sa,sender)) nomem();
}


/* this file is too long ---------------------------------------------- INFO */

int getinfo(stralloc *sa,datetime_sec *dt,unsigned long id)
{
  int fdinfo;
  struct stat st;
  static stralloc line = {0};
  int match;
//  substdio ss;
  buffer ss;
  char buf[128];

  fnmake_info(id);
  fdinfo = open_read(fn.s);
  if (fdinfo == -1) return 0;
  if (fstat(fdinfo,&st) == -1) { close(fdinfo); return 0; }
//  substdio_fdbuf(&ss,read,fdinfo,buf,sizeof(buf));
  buffer_init(&ss,read,fdinfo,buf,sizeof(buf));
  if (getln(&ss,&line,&match,'\0') == -1) { close(fdinfo); return 0; }
  close(fdinfo);
  if (!match) return 0;
  if (line.s[0] != 'F') return 0;

  *dt = st.st_mtime;
  while (!stralloc_copys(sa,line.s + 1)) nomem();
  while (!stralloc_0(sa)) nomem();
  return 1;
}


/* this file is too long ------------------------------------- COMMUNICATION */

buffer sstoqc; char sstoqcbuf[1024];
buffer ssfromqc; char ssfromqcbuf[1024];
stralloc comm_buf[CHANNELS] = { {0}, {0} };
int comm_pos[CHANNELS];

void comm_init()
{
 int c;
  buffer_init(&sstoqc,write,5,sstoqcbuf,sizeof(sstoqcbuf));
  buffer_init(&ssfromqc,read,6,ssfromqcbuf,sizeof(ssfromqcbuf));
 for (c = 0;c < CHANNELS;++c)
   if (ndelay_on(chanfdout[c]) == -1)
   /* this is so stupid: NDELAY semantics should be default on write */
     spawndied(c); /* drastic, but better than risking deadlock */
}

int comm_canwrite(int c)
{
 /* XXX: could allow a bigger buffer; say 10 recipients */
 if (comm_buf[c].s && comm_buf[c].len) return 0;
 return 1;
}

void comm_write(c,delnum,id,sender,recip)
int c;
int delnum;
unsigned long id;
char *sender;
char *recip;
{
 char ch;
 if (comm_buf[c].s && comm_buf[c].len) return;
 while (!stralloc_copys(&comm_buf[c],"")) nomem();
 ch = delnum;
 while (!stralloc_append(&comm_buf[c],&ch)) nomem();
 fnmake_split(id);
 while (!stralloc_cats(&comm_buf[c],fn.s)) nomem();
 while (!stralloc_0(&comm_buf[c])) nomem();
 senderadd(&comm_buf[c],sender,recip);
 while (!stralloc_0(&comm_buf[c])) nomem();
 while (!stralloc_cats(&comm_buf[c],recip)) nomem();
 while (!stralloc_0(&comm_buf[c])) nomem();
 comm_pos[c] = 0;
}

void comm_selprep(nfds,wfds)
int *nfds;
fd_set *wfds;
{
 int c;
 for (c = 0;c < CHANNELS;++c)
   if (flagspawnalive[c])
     if (comm_buf[c].s && comm_buf[c].len)
      {
       FD_SET(chanfdout[c],wfds);
       if (*nfds <= chanfdout[c])
         *nfds = chanfdout[c] + 1;
      }
}

void comm_do(fd_set *wfds)
{
 int c;
 for (c = 0;c < CHANNELS;++c)
   if (flagspawnalive[c])
     if (comm_buf[c].s && comm_buf[c].len)
       if (FD_ISSET(chanfdout[c],wfds))
        {
         int w;
         int len;
         len = comm_buf[c].len;
         w = write(chanfdout[c],comm_buf[c].s + comm_pos[c],len - comm_pos[c]);
         if (w <= 0)
      {
       if ((w == -1) && (errno == error_pipe))
         spawndied(c);
       else
         continue; /* kernel select() bug; can't avoid busy-looping */
      }
     else
      {
       comm_pos[c] += w;
       if (comm_pos[c] == len)
         comm_buf[c].len = 0;
      }
        }
}


/* this file is too long ------------------------------------------ CLEANUPS */

int flagcleanup; /* if 1, cleanupdir is initialized and ready */
readsubdir cleanupdir;
datetime_sec cleanuptime;

void cleanup_init()
{
 flagcleanup = 0;
 cleanuptime = now();
}

void cleanup_selprep(wakeup)
datetime_sec *wakeup;
{
 if (flagcleanup) *wakeup = 0;
 if (*wakeup > cleanuptime) *wakeup = cleanuptime;
}

void cleanup_do()
{
 char ch;
 struct stat st;
 unsigned long id;

 if (!flagcleanup)
  {
   if (recent < cleanuptime) return;
   readsubdir_init(&cleanupdir,"mess",pausedir);
   flagcleanup = 1;
  }

 switch(readsubdir_next(&cleanupdir,&id))
  {
   case 1:
     break;
   case 0:
     flagcleanup = 0;
     cleanuptime = recent + SLEEP_CLEANUP;
   default:
     return;
  }

 fnmake_mess(id);
 if (stat(fn.s,&st) == -1) return; /* probably qmail-queue deleted it */
 if (recent <= st.st_atime + OSSIFIED) return;

 fnmake_info(id);
 if (stat(fn.s,&st) == 0) return;
 if (errno != error_noent) return;
 fnmake_todo(id);
 if (stat(fn.s,&st) == 0) return;
 if (errno != error_noent) return;

  fnmake_foop(id);
  if (buffer_putflush(&sstoqc,fn.s,fn.len) == -1) { cleandied(); return; }
  if (buffer_get(&ssfromqc,&ch,1) != 1) { cleandied(); return; }
  if (ch != '+')
    log(B("warning: qmail-clean unable to clean up ",fn.s,"\n"));
}


/* this file is too long ----------------------------------- PRIORITY QUEUES */

prioq pqdone = {0}; /* -todo +info; HOPEFULLY -local -remote */
prioq pqchan[CHANNELS] = { {0}, {0} };
/* pqchan 0: -todo +info +local ?remote */
/* pqchan 1: -todo +info ?local +remote */
prioq pqfail = {0}; /* stat() failure; has to be pqadded again */

void pqadd(id)
unsigned long id;
{
 struct prioq_elt pe;
 struct prioq_elt pechan[CHANNELS];
 int flagchan[CHANNELS];
 struct stat st;
 int c;

#define CHECKSTAT if (errno != ENOENT) goto fail;

 fnmake_info(id);
 if (stat(fn.s,&st) == -1)
  {
   CHECKSTAT
   return; /* someone yanking our chain */
  }

 fnmake_todo(id);
 if (stat(fn.s,&st) != -1) return; /* look, ma, dad crashed writing info! */
 CHECKSTAT

 for (c = 0;c < CHANNELS;++c)
  {
   fnmake_chanaddr(id,c);
   if (stat(fn.s,&st) == -1) { flagchan[c] = 0; CHECKSTAT }
   else { flagchan[c] = 1; pechan[c].id = id; pechan[c].dt = st.st_mtime; }
  }

 for (c = 0;c < CHANNELS;++c)
   if (flagchan[c])
     while (!prioq_insert(&pqchan[c],&pechan[c])) nomem();

 for (c = 0;c < CHANNELS;++c) if (flagchan[c]) break;
 if (c == CHANNELS)
  {
   pe.id = id; pe.dt = now();
   while (!prioq_insert(&pqdone,&pe)) nomem();
  }

 return;

 fail:
 log(B("warning: unable to stat ",fn.s,"; will try again later\n"));
 pe.id = id; pe.dt = now() + SLEEP_SYSFAIL;
 while (!prioq_insert(&pqfail,&pe)) nomem();
}

void pqstart()
{
 readsubdir rs;
 int x;
 unsigned long id;

 readsubdir_init(&rs,"info",pausedir);

 while ((x = readsubdir_next(&rs,&id)))
   if (x > 0)
     pqadd(id);
}

void pqfinish()
{
 int c;
 struct prioq_elt pe;
 time_t ut[2]; /* XXX: more portable than utimbuf, but still worrisome */

 for (c = 0;c < CHANNELS;++c)
   while (prioq_min(&pqchan[c],&pe))
    {
     prioq_delmin(&pqchan[c]);
     fnmake_chanaddr(pe.id,c);
     ut[0] = ut[1] = pe.dt;
     if (utime(fn.s, (const struct utimbuf *) &ut) == -1)
       log(B("warning: unable to utime ",fn.s,"; message will be retried too soon\n"));
    }
}

void pqrun()
{
 int c;
 int i;
 for (c = 0;c < CHANNELS;++c)
   if (pqchan[c].p)
     if (pqchan[c].len)
       for (i = 0;i < pqchan[c].len;++i)
	 pqchan[c].p[i].dt = recent;
}


/* this file is too long ---------------------------------------------- JOBS */

struct job
 {
  int refs; /* if 0, this struct is unused */
  unsigned long id;
  int channel;
  datetime_sec retry;
  stralloc sender;
  int numtodo;
  int flaghiteof;
  int flagdying;
 }
;

int numjobs;
struct job *jo;

void job_init()
{
 int j;
 while (!(jo = (struct job *) alloc(numjobs * sizeof(struct job)))) nomem();
 for (j = 0;j < numjobs;++j)
  {
   jo[j].refs = 0;
   jo[j].sender.s = 0;
  }
}

int job_avail()
{
 int j;
 for (j = 0;j < numjobs;++j) if (!jo[j].refs) return 1;
 return 0;
}

int job_open(id,channel)
unsigned long id;
int channel;
{
 int j;
 for (j = 0;j < numjobs;++j) if (!jo[j].refs) break;
 if (j == numjobs) return -1;
 jo[j].refs = 1;
 jo[j].id = id;
 jo[j].channel = channel;
 jo[j].numtodo = 0;
 jo[j].flaghiteof = 0;
 return j;
}

void job_close(j)
int j;
{
 struct prioq_elt pe;
 struct stat st;

 if (0 < --jo[j].refs) return;

 pe.id = jo[j].id;
 pe.dt = jo[j].retry;
 if (jo[j].flaghiteof && !jo[j].numtodo)
  {
   fnmake_chanaddr(jo[j].id,jo[j].channel);
   if (unlink(fn.s) == -1)
    {
     log(B("warning: unable to unlink ",fn.s,"; will try again later\n"));
     pe.dt = now() + SLEEP_SYSFAIL;
    }
   else
    {
     int c;
     for (c = 0;c < CHANNELS;++c) if (c != jo[j].channel)
      {
       fnmake_chanaddr(jo[j].id,c);
       if (stat(fn.s,&st) == 0) return; /* more channels going */
       if (errno != error_noent)
    {
         log(B("warning: unable to stat ",fn.s,"\n"));
     break; /* this is the only reason for HOPEFULLY */
    }
      }
     pe.dt = now();
     while (!prioq_insert(&pqdone,&pe)) nomem();
     return;
    }
  }

 while (!prioq_insert(&pqchan[jo[j].channel],&pe)) nomem();
}


/* this file is too long ------------------------------------------- BOUNCES */
#include "qmail-send-bounces.c"

/* this file is too long ---------------------------------------- DELIVERIES */

struct del {
  int used;
  int j;
  unsigned long delid;
  seek_pos mpos;
  stralloc recip;
};

unsigned long masterdelid = 1;
unsigned int concurrency[CHANNELS] = { 10, 20 };
unsigned int concurrencyused[CHANNELS] = { 0, 0 };
struct del *d[CHANNELS];
stralloc dline[CHANNELS];
char delbuf[2048];

void del_status()
{
  int c;

//  log("status:");
  for (c = 0;c < CHANNELS;++c) {
    strnum2[fmt_ulong(strnum2,(unsigned long) concurrencyused[c])] = 0;
    strnum3[fmt_ulong(strnum3,(unsigned long) concurrency[c])] = 0;
//    o_log2(chanstatusmsg[c],strnum2);
//    o_log2("/",strnum3);
    log(B("status:",chanstatusmsg[c],strnum2,"/",strnum3));
//    log(B("/",strnum3));
  }
  if (flagexitasap) log(" exitasap\n");
//  log("\n");
}

void del_init()
{
 int c;
 int i;
 for (c = 0;c < CHANNELS;++c)
  {
   flagspawnalive[c] = 1;
   while (!(d[c] = (struct del *) alloc(concurrency[c] * sizeof(struct del))))
     nomem();
   for (i = 0;i < concurrency[c];++i)
    { d[c][i].used = 0; d[c][i].recip.s = 0; }
   dline[c].s = 0;
   while (!stralloc_copys(&dline[c],"")) nomem();
  }
 del_status();
}

int del_canexit()
{
 int c;
 for (c = 0;c < CHANNELS;++c)
   if (flagspawnalive[c]) /* if dead, nothing we can do about its jobs */
     if (concurrencyused[c]) return 0;
 return 1;
}

int del_avail(c)
int c;
{
  return flagspawnalive[c] && comm_canwrite(c) && (concurrencyused[c] < concurrency[c]);
}

void del_start(j,mpos,recip)
int j;
seek_pos mpos;
char *recip;
{
 int i;
 int c;

 c = jo[j].channel;
 if (!flagspawnalive[c]) return;
 if (!comm_canwrite(c)) return;

 for (i = 0;i < concurrency[c];++i) if (!d[c][i].used) break;
 if (i == concurrency[c]) return;

 if (!stralloc_copys(&d[c][i].recip,recip)) { nomem(); return; }
 if (!stralloc_0(&d[c][i].recip)) { nomem(); return; }
 d[c][i].j = j; ++jo[j].refs;
 d[c][i].delid = masterdelid++;
 d[c][i].mpos = mpos;
 d[c][i].used = 1; ++concurrencyused[c];

 comm_write(c,i,jo[j].id,jo[j].sender.s,recip);

  strnum2[fmt_ulong(strnum2,d[c][i].delid)] = 0;
  strnum3[fmt_ulong(strnum3,jo[j].id)] = 0;
  log(B("starting delivery ",strnum2));
  log(B(": msg ",strnum3,tochan[c]));
 logsafe(recip);
  log("\n");
  del_status();
}

void markdone(int c,unsigned long id,seek_pos pos)
//int c;
//unsigned long id;
//seek_pos pos;
{
  struct stat st;
  int fd;
  fnmake_chanaddr(id,c);
  for (;;) {
    fd = open_write(fn.s);
    if (fd == -1) break;
    if (fstat(fd,&st) == -1) { close(fd); break; }
    if (seek_set(fd,pos) == -1) { close(fd); break; }
    if (write(fd,"D",1) != 1) { close(fd); break; }
    /* further errors -> double delivery without us knowing about it, oh well */
    close(fd);
    return;
  }
 log(B("warning: trouble marking ",fn.s,"; message will be delivered twice!\n"));
}

void del_dochan(int c) {
  int r;
  char ch;
  int i;
  int delnum;
  r = read(chanfdin[c],delbuf,sizeof(delbuf));
  if (r == -1) return;
  if (r == 0) { spawndied(c); return; }
  for (i = 0;i < r;++i)
  {
    ch = delbuf[i];
    while (!stralloc_append(&dline[c],&ch)) nomem();
    if (dline[c].len > REPORTMAX)
      dline[c].len = REPORTMAX;
      /* qmail-lspawn and qmail-rspawn are responsible for keeping it short */
      /* but from a security point of view, we don't trust rspawn */
    if (!ch && (dline[c].len > 1))
    {
     delnum = (unsigned int) (unsigned char) dline[c].s[0];
     if ((delnum < 0) || (delnum >= concurrency[c]) || !d[c][delnum].used)
       log("warning: internal error: delivery report out of range\n");
     else
      {
       strnum3[fmt_ulong(strnum3,d[c][delnum].delid)] = 0;
       if (dline[c].s[1] == 'Z')
     if (jo[d[c][delnum].j].flagdying)
      {
       dline[c].s[1] = 'D';
       --dline[c].len;
       while (!stralloc_cats(&dline[c],"I'm not going to try again; this message has been in the queue too long.\n")) nomem();
       while (!stralloc_0(&dline[c])) nomem();
      }
       switch(dline[c].s[1])
	{
	 case 'K':
	   log(B("delivery ",strnum3,": success: "));
	   logsafe(dline[c].s + 2);
	   log("\n");
	   markdone(c,jo[d[c][delnum].j].id,d[c][delnum].mpos);
	   --jo[d[c][delnum].j].numtodo;
	   break;
	 case 'Z':
	   log(B("delivery ",strnum3,": deferral: "));
	   logsafe(dline[c].s + 2);
	   log("\n");
	   break;
	 case 'D':
	   log(B("delivery ",strnum3,": failure: "));
	   logsafe(dline[c].s + 2);
	   log("\n");
	   addbounce(jo[d[c][delnum].j].id,d[c][delnum].recip.s,dline[c].s + 2);
	   markdone(c,jo[d[c][delnum].j].id,d[c][delnum].mpos);
       --jo[d[c][delnum].j].numtodo;
       break;
     default:
       log(B("delivery ",strnum3,": report mangled, will defer\n"));
    }
       job_close(d[c][delnum].j);
       d[c][delnum].used = 0; --concurrencyused[c];
       del_status();
      }
     dline[c].len = 0;
    }
  }
}

void del_selprep(nfds,rfds)
int *nfds;
fd_set *rfds;
{
 int c;
 for (c = 0;c < CHANNELS;++c)
   if (flagspawnalive[c])
    {
     FD_SET(chanfdin[c],rfds);
     if (*nfds <= chanfdin[c])
       *nfds = chanfdin[c] + 1;
    }
}

void del_do(rfds)
fd_set *rfds;
{
 int c;
 for (c = 0;c < CHANNELS;++c)
   if (flagspawnalive[c])
     if (FD_ISSET(chanfdin[c],rfds))
       del_dochan(c);
}


/* this file is too long -------------------------------------------- PASSES */

struct
{
  unsigned long id; /* if 0, need a new pass */
  int j; /* defined if id; job number */
  int fd; /* defined if id; reading from {local,remote} */
  seek_pos mpos; /* defined if id; mark position */
  buffer ss;
  char buf[128];
}
pass[CHANNELS];

void pass_init()
{
 int c;
 for (c = 0;c < CHANNELS;++c) pass[c].id = 0;
}

void pass_selprep(wakeup)
datetime_sec *wakeup;
{
 int c;
 struct prioq_elt pe;
 if (flagexitasap) return;
 for (c = 0;c < CHANNELS;++c)
   if (pass[c].id)
     if (del_avail(c))
      { *wakeup = 0; return; }
 if (job_avail())
   for (c = 0;c < CHANNELS;++c)
     if (!pass[c].id)
       if (prioq_min(&pqchan[c],&pe))
         if (*wakeup > pe.dt)
           *wakeup = pe.dt;
 if (prioq_min(&pqfail,&pe))
   if (*wakeup > pe.dt)
     *wakeup = pe.dt;
 if (prioq_min(&pqdone,&pe))
   if (*wakeup > pe.dt)
     *wakeup = pe.dt;
}

static datetime_sec squareroot(x) /* result^2 <= x < (result + 1)^2 */
datetime_sec x; /* assuming: >= 0 */
{
 datetime_sec y;
 datetime_sec yy;
 datetime_sec y21;
 int j;

 y = 0; yy = 0;
 for (j = 15;j >= 0;--j)
  {
   y21 = (y << (j + 1)) + (1 << (j + j));
   if (y21 <= x - yy) { y += (1 << j); yy += y21; }
  }
 return y;
}

datetime_sec nextretry(birth,c)
datetime_sec birth;
int c;
{
 int n;

 if (birth > recent) n = 0;
 else n = squareroot(recent - birth); /* no need to add fuzz to recent */
 n += chanskip[c];
 return birth + n * n;
}

void pass_dochan(c)
int c;
{
 datetime_sec birth;
 struct prioq_elt pe;
 static stralloc line = {0};
 int match;

 if (flagexitasap) return;

 if (!pass[c].id)
  {
   if (!job_avail()) return;
   if (!prioq_min(&pqchan[c],&pe)) return;
   if (pe.dt > recent) return;
   fnmake_chanaddr(pe.id,c);

   prioq_delmin(&pqchan[c]);
   pass[c].mpos = 0;
   pass[c].fd = open_read(fn.s);
   if (pass[c].fd == -1) goto trouble;
   if (!getinfo(&line,&birth,pe.id)) { close(pass[c].fd); goto trouble; }
   pass[c].id = pe.id;
   buffer_init(&pass[c].ss,read,pass[c].fd,pass[c].buf,sizeof(pass[c].buf));
   pass[c].j = job_open(pe.id,c);
   jo[pass[c].j].retry = nextretry(birth,c);
   jo[pass[c].j].flagdying = (recent > birth + lifetime);
   while (!stralloc_copy(&jo[pass[c].j].sender,&line)) nomem();
  }

 if (!del_avail(c)) return;

 if (getln(&pass[c].ss,&line,&match,'\0') == -1)
  {
   fnmake_chanaddr(pass[c].id,c);
   log(B("warning: trouble reading ",fn.s,"; will try again later\n"));
   close(pass[c].fd);
   job_close(pass[c].j);
   pass[c].id = 0;
   return;
  }
 if (!match)
  {
   close(pass[c].fd);
   jo[pass[c].j].flaghiteof = 1;
   job_close(pass[c].j);
   pass[c].id = 0;
   return;
  }
 switch(line.s[0])
  {
   case 'T':
     ++jo[pass[c].j].numtodo;
     del_start(pass[c].j,pass[c].mpos,line.s + 1);
     break;
   case 'D':
     break;
   default:
     fnmake_chanaddr(pass[c].id,c);
     log(B("warning: unknown record type in ",fn.s,"!\n"));
     close(pass[c].fd);
     job_close(pass[c].j);
     pass[c].id = 0;
     return;
  }

 pass[c].mpos += line.len;
 return;

 trouble:
 log(B("warning: trouble opening ",fn.s,"; will try again later\n"));
 pe.dt = recent + SLEEP_SYSFAIL;
 while (!prioq_insert(&pqchan[c],&pe)) nomem();
}

void messdone(id)
unsigned long id;
{
 char ch;
 int c;
 struct prioq_elt pe;
 struct stat st;

 for (c = 0;c < CHANNELS;++c)
  {
   fnmake_chanaddr(id,c);
   if (stat(fn.s,&st) == 0) return; /* false alarm; consequence of HOPEFULLY */
   if (errno != error_noent)
    {
     log(B("warning: unable to stat ",fn.s,"; will try again later\n"));
     goto fail;
    }
  }

 fnmake_todo(id);
 if (stat(fn.s,&st) == 0) return;
 if (errno != error_noent)
  {
   log(B("warning: unable to stat ",fn.s,"; will try again later\n"));
   goto fail;
  }

 fnmake_info(id);
 if (stat(fn.s,&st) == -1)
  {
   if (errno == error_noent) return;
   log(B("warning: unable to stat ",fn.s,"; will try again later\n"));
   goto fail;
  }

  /* -todo +info -local -remote ?bounce */
  if (!injectbounce(id))
    goto fail; /* injectbounce() produced error message */

  strnum3[fmt_ulong(strnum3,id)] = 0;
  log(B("end msg ",strnum3,"\n"));

  /* -todo +info -local -remote -bounce */
  fnmake_info(id);
  if (unlink(fn.s) == -1) {
    log(B("warning: unable to unlink ",fn.s,"; will try again later\n"));
    goto fail;
  }

  /* -todo -info -local -remote -bounce; we can relax */
  fnmake_foop(id);
  if (buffer_putflush(&sstoqc,fn.s,fn.len) == -1) { cleandied(); return; }
  if (buffer_get(&ssfromqc,&ch,1) != 1) { cleandied(); return; }
  if (ch != '+')
    log(B("warning: qmail-clean unable to clean up ",fn.s,"\n"));

 return;

 fail:
 pe.id = id; pe.dt = now() + SLEEP_SYSFAIL;
 while (!prioq_insert(&pqdone,&pe)) nomem();
}

void pass_do()
{
 int c;
 struct prioq_elt pe;

 for (c = 0;c < CHANNELS;++c) pass_dochan(c);
 if (prioq_min(&pqfail,&pe))
   if (pe.dt <= recent)
    {
     prioq_delmin(&pqfail);
     pqadd(pe.id);
    }
 if (prioq_min(&pqdone,&pe))
   if (pe.dt <= recent)
    {
     prioq_delmin(&pqdone);
     messdone(pe.id);
    }
}


#include "qmail-send-todo.c"

/* this file is too long ---------------------------------------------- MAIN */

int getcontrols() { if (control_init() == -1) return 0;
 if (control_readint(&lifetime,"etc/queuelifetime") == -1) return 0;
 if (control_readint((int *)&concurrency[0],"etc/concurrencylocal") == -1) return 0;
 if (control_readint((int *)&concurrency[1],"etc/concurrencyremote") == -1) return 0;
 if (control_rldef(&envnoathost,"etc/envnoathost",1,"envnoathost") != 1) return 0;
 if (control_rldef(&bouncefrom,"etc/bouncefrom",0,"MAILER-DAEMON") != 1) return 0;
 if (control_rldef(&bouncehost,"etc/bouncehost",1,"bouncehost") != 1) return 0;
 if (control_rldef(&doublebouncehost,"etc/doublebouncehost",1,"doublebouncehost") != 1) return 0;
 if (control_rldef(&doublebounceto,"etc/doublebounceto",0,"postmaster") != 1) return 0;
 if (!stralloc_cats(&doublebounceto,"@")) return 0;
 if (!stralloc_cat(&doublebounceto,&doublebouncehost)) return 0;
 if (!stralloc_0(&doublebounceto)) return 0;
 if (control_readfile(&locals,"etc/locals",1) != 1) return 0;
 if (!constmap_init(&maplocals,locals.s,locals.len,0)) return 0;
 switch(control_readfile(&percenthack,"etc/percenthack",0))
  {
   case -1: return 0;
   case 0: if (!constmap_init(&mappercenthack,"",0,0)) return 0; break;
   case 1: if (!constmap_init(&mappercenthack,percenthack.s,percenthack.len,0)) return 0; break;
  }
 switch(control_readfile(&vdoms,"etc/virtualdomains",0))
  {
   case -1: return 0;
   case 0: if (!constmap_init(&mapvdoms,"",0,1)) return 0; break;
   case 1: if (!constmap_init(&mapvdoms,vdoms.s,vdoms.len,1)) return 0; break;
  }
 return 1; }

stralloc newlocals = {0};
stralloc newvdoms = {0};

void regetcontrols()
{
 int r;

 if (control_readfile(&newlocals,"etc/locals",1) != 1)
  { log("alert: unable to reread etc/locals\n"); return; }
 r = control_readfile(&newvdoms,"etc/virtualdomains",0);
 if (r == -1)
  { log("alert: unable to reread etc/virtualdomains\n"); return; }

 constmap_free(&maplocals);
 constmap_free(&mapvdoms);

 while (!stralloc_copy(&locals,&newlocals)) nomem();
 while (!constmap_init(&maplocals,locals.s,locals.len,0)) nomem();

 if (r)
  {
   while (!stralloc_copy(&vdoms,&newvdoms)) nomem();
   while (!constmap_init(&mapvdoms,vdoms.s,vdoms.len,1)) nomem();
  }
 else
   while (!constmap_init(&mapvdoms,"",0,1)) nomem();
}

void reread()
{
 if (chdir(auto_qmail) == -1)
  {
   log("alert: unable to reread controls: unable to switch to home directory\n");
   return;
  }
 regetcontrols();
// while (chdir("queue") == -1)
 while (chdir("var/queue") == -1)
  {
   log("alert: unable to switch back to queue directory; HELP! sleeping...\n");
   sleep(10);
  }
}

int main()
{
 int fd;
 datetime_sec wakeup;
 fd_set rfds;
 fd_set wfds;
 int nfds;
 struct timeval tv;
 int c;

 if (chdir(auto_qmail) == -1)
  { errhard("alert: cannot start: unable to switch to home directory\n"); } //_exit(111); }
 if (!getcontrols())
  { errhard("alert: cannot start: unable to read controls\n"); } //_exit(111); }
// if (chdir("queue") == -1)
 if (chdir("var/queue") == -1)
  { errhard("alert: cannot start: unable to switch to queue directory\n"); } //_exit(111); }
 sig_pipeignore();
 sig_termcatch(sigterm);
 sig_alarmcatch(sigalrm);
 sig_hangupcatch(sighup);
 sig_childdefault();
 umask(077);

 fd = open_write("lock/sendmutex");
 if (fd == -1)
//  { log1("alert: cannot start: unable to open mutex\n"); _exit(111); }
  { errhard("alert: cannot start: unable to open mutex\n"); }
 if (lock_exnb(fd) == -1)
//  { log1("alert: cannot start: qmail-send is already running\n"); _exit(111); }
  { errhard("alert: cannot start: qmail-send is already running\n"); }

 numjobs = 0;
 for (c = 0;c < CHANNELS;++c)
  {
   char ch;
   int u;
   int r;
   do
     r = read(chanfdin[c],&ch,1);
   while ((r == -1) && (errno == error_intr));
   if (r < 1)
    { errhard("alert: cannot start: hath the daemon spawn no fire?\n"); } //_exit(111); }
   u = (unsigned int) (unsigned char) ch;
   if (concurrency[c] > u) concurrency[c] = u;
   numjobs += concurrency[c];
  }

 fnmake_init();

 comm_init();

 pqstart();
 job_init();
 del_init();
 pass_init();
 todo_init();
 cleanup_init();

 while (!flagexitasap || !del_canexit())
  {
   recent = now();

   if (flagrunasap) { flagrunasap = 0; pqrun(); }
   if (flagreadasap) { flagreadasap = 0; reread(); }

   wakeup = recent + SLEEP_FOREVER;
   FD_ZERO(&rfds);
   FD_ZERO(&wfds);
   nfds = 1;

   comm_selprep(&nfds,&wfds);
   del_selprep(&nfds,&rfds);
   pass_selprep(&wakeup);
   todo_selprep(&nfds,&rfds,&wakeup);
   cleanup_selprep(&wakeup);

   if (wakeup <= recent) tv.tv_sec = 0;
   else tv.tv_sec = wakeup - recent + SLEEP_FUZZ;
   tv.tv_usec = 0;

   if (select(nfds,&rfds,&wfds,(fd_set *) 0,&tv) == -1)
     if (errno == EINTR)
       ;
     else
       log("warning: trouble in select\n");
   else
    {
     recent = now();

     comm_do(&wfds);
     del_do(&rfds);
     todo_do(&rfds);
     pass_do();
     cleanup_do();
    }
  }
 pqfinish();
  log("status: exiting\n");
  _exit(0);
// return(0);  /* never reached */
}
