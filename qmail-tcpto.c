/*
 *  Revision 20180417, Kai Peter
 *  - switched to 'qlibs'
 *  - removed '#ifdef INET6'
 *  Revision 20160912, Kai Peter
 *  - added cast's to 'ip*_fmt' function calls
 *  Revision 20160713, Kai Peter
 *  - switched to 'buffer'
 *  Revision 20160509, Kai Peter
 *  - changed return type of main to int
 *  - added '<unistd.h>, 'open.h'
 */

/* XXX: this program knows quite a bit about tcpto's internals */

#include <unistd.h>
#include "qsocket.h"
#include "buffer.h"
#include "auto_qmail.h"
#include "fmt.h"
#include "ip.h"
#include "lock.h"
#include "errmsg.h"
#include "datetime.h"
#include "now.h"
#include "ipalloc.h"
#include "tcpto.h"
#include "open.h"

#define WHO "qmail-tcpto"

void die(n) int n; { buffer_flush(buffer_1); _exit(n); }

void warn(s) char *s;
{
  char *x;
  x = errstr(errno);
  buffer_puts(buffer_1,s);
  buffer_puts(buffer_1,": ");
  buffer_puts(buffer_1,x);
  buffer_puts(buffer_1,"\n");
}

void die_chdir() { warn("fatal: unable to chdir"); die(111); }
void die_open() { warn("fatal: unable to open tcpto"); die(111); }
void die_lock() { warn("fatal: unable to lock tcpto"); die(111); }
void die_read() { warn("fatal: unable to read tcpto"); die(111); }

struct tcpto_buf tcpto_buf[TCPTO_BUFSIZ];

char tmp[FMT_ULONG + IPFMT];
char *tmp2;

int main()
{
  int fdlock;
  int fd;
  int r;
  int i;
  int af;
  datetime_sec when;
  datetime_sec start;

  if (chdir(auto_qmail) == -1) die_chdir();
  if (chdir("queue/lock") == -1) die_chdir();

  fdlock = open_write("tcpto");
  if (fdlock == -1) die_open();
  fd = open_read("tcpto");
  if (fd == -1) die_open();
  if (lock_ex(fdlock) == -1) die_lock();
  r = read(fd,tcpto_buf,sizeof(tcpto_buf));
  close(fd);
  close(fdlock);

  if (r == -1) die_read();
  r /= sizeof(tcpto_buf[0]);

  start = now();

  for (i = 0;i < r;++i)
  {
    if (tcpto_buf[i].flag >= 1)
    {
      af = tcpto_buf[i].af;
      when = tcpto_buf[i].when;

//#ifdef INET6
      if (af == AF_INET)
        buffer_put(buffer_1,tmp,ip4_fmt(tmp,(char *)&tcpto_buf[i].addr.ip));
      else
        buffer_put(buffer_1,tmp,ip6_fmt(tmp,(char *)&tcpto_buf[i].addr.ip6));
/*
#else
      buffer_put(buffer_1,tmp,ip4_fmt(tmp,&tcpto_buf[i].addr.ip));
#endif
*/
      buffer_puts(buffer_1," timed out ");
      buffer_put(buffer_1,tmp,fmt_ulong(tmp,(unsigned long) (start - when)));
      buffer_puts(buffer_1," seconds ago; # recent timeouts: ");
      buffer_put(buffer_1,tmp,fmt_ulong(tmp,tcpto_buf[i].flag));
      buffer_puts(buffer_1,"\n");
    }
  }

  die(0);
}
