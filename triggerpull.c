/* used by qmail-queue only */
#include "ndelay.h"
#include "open.h"
#include "triggerpull.h"
#include "close.h"

void write();   /* compiler warning (temp?) */

void triggerpull()
{
 int fd;

 fd = open_write("lock/trigger");
 if (fd >= 0)
  {
   ndelay_on(fd);
   write(fd,"",1); /* if it fails, bummer */
   close(fd);
  }
}
