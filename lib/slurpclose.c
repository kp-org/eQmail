/*
 *  Revision 20160506, Kai Peter
 *  - added '"close.h"' to prevent compiler warnings
 */
#include "stralloc.h"
#include "slurpclose.h"
#include "error.h"
#include "readwrite.h"
#include "close.h"

int slurpclose(fd,sa,bufsize)
int fd;
stralloc *sa;
int bufsize;
{
  int r;
  for (;;) {
    if (!stralloc_readyplus(sa,bufsize)) { close(fd); return -1; }
    r = read(fd,sa->s + sa->len,bufsize);
    if (r == -1) if (errno == error_intr) continue;
    if (r <= 0) { close(fd); return r; }
    sa->len += r;
  }
}
