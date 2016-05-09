/*
 *  Revision 20160509, Kai Peter
 *  - added 'close.h', declaration of 'write'
 */
#include "ndelay.h"
#include "open.h"
#include "triggerpull.h"

#include "close.h"
void write(int _fd, const char *_buf, int b);

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
