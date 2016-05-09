/*
 *  Revision 20160506, Kai Peter
 *  - added '<sys/types.h>' and declaration of 'read'
 */
#include <sys/types.h>
#include "substdio.h"
#include "subfd.h"

ssize_t read(int fd, void *buf, size_t count);

int subfd_read(fd,buf,len) int fd; char *buf; int len;
{
  if (substdio_flush(subfdout) == -1) return -1;
  return read(fd,buf,len);
}

char subfd_inbuf[SUBSTDIO_INSIZE];
static substdio it = SUBSTDIO_FDBUF(subfd_read,0,subfd_inbuf,SUBSTDIO_INSIZE);
substdio *subfdin = &it;
