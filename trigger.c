/* used by qmail-send only */
/*
 *  Revision 20180319, Kai Peter
 *  - incorporated former file 'triggerpull.c'
*/
#include "select.h"
#include "ndelay.h"
#include "open.h"
#include "trigger.h"
#include "close.h"

static int fd = -1;

void trigger_set() {
  if (fd != -1) close(fd);
  fd = open_read("lock/trigger");
}

void trigger_selprep(int *nfds,fd_set *rfds) {
  if (fd != -1)
  {
    FD_SET(fd,rfds);
    if (*nfds < fd + 1) *nfds = fd + 1;
  }
}

int trigger_pulled(fd_set *rfds) {
  if (fd != -1) if (FD_ISSET(fd,rfds)) return 1;
  return 0;
}

/* file triggerpull.c   */
void write();   /* compiler warning (temp) */

void triggerpull() {
 int fd;

 fd = open_write("lock/trigger");
 if (fd >= 0)
  {
//   ndelay_on(fd);
   ndelay(fd,ON);
   write(fd,"",1); /* if it fails, bummer */
   close(fd);
  }
}
