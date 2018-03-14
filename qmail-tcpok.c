/*
 *  Revision 20180123, Kai Peter
 *  - removed 'auto_qmail', use 'buildins' instead
 *  - replaced strerr by errmsg
 *  - switched to buffer, unistd.h
 *  - changed 'queue/' --> 'var/queue/'

 NOTE: replacing strerr by errmsg and  using  stralloc_* functions to concatenate
       the error message string seems to be weird on the first view. But I wanted
       to show the differences against the (hard readable) '#defines' in strerr.h
       here. It's just readability against shortness of code from my POV.
*/
#include <unistd.h>
#include "buffer.h"
#include "error.h"
#include "lock.h"
#include "open.h"
#include "buildins.h"
#include "ipalloc.h"
#include "tcpto.h"

#define WHO "qmail-tcpok"

struct tcpto_buf buf[TCPTO_BUFSIZ];
buffer b;

int main()
{
  int fd;
  int i;

  if (chdir(qprfxdir) == -1) {
    err_sys_plus(errno,B("unable to chdir to ",qprfxdir,": "));
  }
  if (chdir("var/queue/lock") == -1) {
	err_sys_plus(errno,B("unable to chdir to ",qprfxdir,"/var/queue/lock: "));
  }

  fd = open_write("tcpto");
  if (fd == -1) {
	err_sys_plus(errno,B("unable to write ",qprfxdir,"/var/queue/lock/tcpto: "));
  }
  if (lock_ex(fd) == -1) {
	err_sys_plus(errno,B("unable to lock ",qprfxdir,"/var/queue/lock/tcpto: "));
  }
  buffer_init(&b,write,fd,(char *)buf,sizeof buf);
  for (i = 0;i < sizeof buf;++i) buffer_put(&b,"",1);
  if (buffer_flush(&b) == -1) {
	err_sys_plus(errno,B("unable to clear ",qprfxdir,"/var/queue/lock/tcpto: "));
  }
  _exit(0);
}
