/*
 *  Revision 20160712, Kai Peter
 *  - switched to 'buffer', 'unistd.h'
 *  Revision 20160509, Kai Peter
 *  - added declaration for function 'sleep'
 */
#include <unistd.h>
#include "stralloc.h"
#include "buffer.h"
#include "qsutil.h"
#include "errmsg.h"

//void sleep(int seconds);

static stralloc foo = {0};

static char errbuf[1];
//static struct substdio sserr = SUBSTDIO_FDBUF(write,0,errbuf,1);
// Hmmm, we write to stdin (fd0) - very special!?
static struct buffer sserr = BUFFER_INIT(write,0,errbuf,1);

//void flush() { buffer_flush(&sserr); }

/* consolidate all logX functions */
void qslog(char *m) { buffer_putsflush(&sserr,m); }

void logsa(sa) stralloc *sa; { buffer_putflush(&sserr,sa->s,sa->len); }
//void log1(s1) char *s1; {
// buffer_putsflush(&sserr,s1);
// flush();
// }
//void o_log2(s1,s2) char *s1; char *s2; {
// buffer_putsflush(&sserr,s1);
// buffer_putsflush(&sserr,s2); }
//void log3(s1,s2,s3) char *s1; char *s2; char *s3; {
// buffer_putsflush(&sserr,s1);
// buffer_putsflush(&sserr,s2);
// buffer_putsflush(&sserr,s3); }
void nomem() { qslog("alert: out of memory, sleeping...\n"); sleep(10); }

void pausedir(dir) char *dir;
{ qslog(B("alert: unable to opendir ",dir,", sleeping...\n")); sleep(10); }

static int issafe(ch) char ch;
{
 if (ch == '%') return 0; /* general principle: allman's code is crap */
 if (ch < 32) return 0;
// if (ch < 33) return 0;   // djb orig
 if (ch > 126) return 0;
 return 1;
}

void logsafe(s) char *s;
{
 int i;
 while (!stralloc_copys(&foo,s)) nomem();
 for (i = 0;i < foo.len;++i)
   if (foo.s[i] == '\n')
     foo.s[i] = '/';
   else
     if (!issafe(foo.s[i]))
       foo.s[i] = ' ';
//       foo.s[i] = '_';
 logsa(&foo);
}
