/*
 *  Revision 20180324, Kai Peter
 *  - removed 'stamp_make', code clean-up
 *  Revision 20170326, Kai Peter
 *  - deactivated 'stamp_make' (duplicate time stamp)
 *  Revision 20160713, Kai Peter
 *  - switched to 'buffer'
 *  Revision 20160504, Kai Peter
 *  - changed return type of main to int
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <syslog.h>
#include "buffer.h"
#include "errmsg.h"
#include "str.h"
#include "scan.h"

char buf[800];    /* syslog truncates long lines (or crashes); GPACIC */
int bufpos = 0;   /* 0 <= bufpos < sizeof(buf) */
int flagcont = 0;
int priority;     /* defined if flagcont */

void flush() {
  if (bufpos) {
    buf[bufpos] = 0;
    if (flagcont)
      syslog(priority,"%s",buf);
    else {
      priority = LOG_INFO;
      if (str_pcmp(buf,"warning:")) priority = LOG_WARNING;
      if (str_pcmp(buf,"alert:")) priority = LOG_ALERT;
      syslog(priority,"%s",buf);
      flagcont = 1;
    }
  }
  bufpos = 0;
}

int main(int argc,char **argv) {
  char ch;

  if (argv[1])
    if (argv[2]) {
      unsigned long facility;
      scan_ulong(argv[2],&facility);
      openlog(argv[1],0,facility << 3);
    }
    else
      openlog(argv[1],0,LOG_MAIL);
  else
    openlog("splogger",0,LOG_MAIL);

  for (;;) {
    if (buffer_get(buffer_0,&ch,1) < 1) _exit(0);
    if (ch == '\n') { flush(); flagcont = 0; continue; }
    if (bufpos == sizeof(buf) - 1) flush();
    if ((ch < 32) || (ch > 126)) ch = '?'; /* logger truncates at 0; GPACIC */
    buf[bufpos++] = ch;
  }
  return(0);  /* never reached */
}
