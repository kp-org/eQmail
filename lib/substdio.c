#include "substdio.h"
/* Consolidate the "substdio*" functions into one source
   file. Original files shipped with qmail-1.03.
   These functions will be linked with other "sub" files
   to "substdio.a"!
   Included files:     Size (bytes)    Date
     - substdio.c              231     19980615
     - substdio_copy.c         347     19980615       */

/* file: substdio.c */
void substdio_fdbuf(s,op,fd,buf,len)
register substdio *s;
register int (*op)();
register int fd;
register char *buf;
register int len;
{
  s->x = buf;
  s->fd = fd;
  s->op = op;
  s->p = 0;
  s->n = len;
}

/* file: substdio_copy.c */
int substdio_copy(ssout,ssin)
register substdio *ssout;
register substdio *ssin;
{
  register int n;
  register char *x;

  for (;;) {
    n = substdio_feed(ssin);
    if (n < 0) return -2;
    if (!n) return 0;
    x = substdio_PEEK(ssin);
    if (substdio_put(ssout,x,n) == -1) return -3;
    substdio_SEEK(ssin,n);
  }
}
