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

#define WHO "qmail-tcpok: "

struct tcpto_buf buf[TCPTO_BUFSIZ];
buffer b;

int main()
{
  int fd;
  int i;
  stralloc sa = {0};

  if (chdir(qprfxdir) == -1) {
	/* this was the former strerr line (just for comparison):
    strerr_die4sys(111,WHO,"unable to chdir to ",auto_qmail,": "); */
	if(!stralloc_copyb(&sa,"unable to chdir to ",19)) err_sys(errno);
	if(!stralloc_cats(&sa,qprfxdir)) err_sys(errno);
	if(!stralloc_catb(&sa,": ",2)) err_sys(errno);
	if(!stralloc_0(&sa)) err_sys(errno);
    err_sys_plus(EHARD,sa.s);
  }
  if (chdir("var/queue/lock") == -1) {
	if(!stralloc_copyb(&sa,"unable to chdir to ",19)) err_sys(errno);
	if(!stralloc_cats(&sa,qprfxdir)) err_sys(errno);
	if(!stralloc_catb(&sa,"/var/queue/lock: ",17)) err_sys(errno);
	if(!stralloc_0(&sa)) err_sys(errno);
    err_sys_plus(EHARD,sa.s);
  }

  fd = open_write("tcpto");
  if (fd == -1) {
	if(!stralloc_copyb(&sa,"unable to write ",16)) err_sys(errno);
	if(!stralloc_cats(&sa,qprfxdir)) err_sys(errno);
	if(!stralloc_catb(&sa,"/var/queue/lock/tcpto: ",23)) err_sys(errno);
	if(!stralloc_0(&sa)) err_sys(errno);
    err_sys_plus(EHARD,sa.s);
  }
  if (lock_ex(fd) == -1) {
	if(!stralloc_copyb(&sa,"unable to lock ",15)) err_sys(errno);
	if(!stralloc_cats(&sa,qprfxdir)) err_sys(errno);
	if(!stralloc_catb(&sa,"/var/queue/lock/tcpto: ",23)) err_sys(errno);
	if(!stralloc_0(&sa)) err_sys(errno);
    err_sys_plus(EHARD,sa.s);
  }
  buffer_init(&b,write,fd,(char *)buf,sizeof buf);
  for (i = 0;i < sizeof buf;++i) buffer_put(&b,"",1);
  if (buffer_flush(&b) == -1) {
	if(!stralloc_copyb(&sa,"unable to clear ",16)) err_sys(errno);
	if(!stralloc_cats(&sa,qprfxdir)) err_sys(errno);
	if(!stralloc_catb(&sa,"/var/queue/lock/tcpto: ",23)) err_sys(errno);
	if(!stralloc_0(&sa)) err_sys(errno);
    err_sys_plus(EHARD,sa.s);
  }
  _exit(0);
}
