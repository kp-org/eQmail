#include <fcntl.h>
#include "fd.h"
/* Consolidate the "fd_*.c"  functions into one source
   file. Original files were of date 19980615, shipped
   with qmail-1.03.
   The "fd" functions will be linked to "fd.a" only!
   Included files:   Size (bytes)   Date:
     - fd_copy.c             225    19980615
     - fd_move.c             156    19980615        */

/* file: fd_copy.c */
int fd_copy(to,from)
int to;
int from;
{
  if (to == from) return 0;
  if (fcntl(from,F_GETFL,0) == -1) return -1;
  close(to);
  if (fcntl(from,F_DUPFD,to) == -1) return -1;
  return 0;
}
#include "fd.h"

/* file: fd_move.c */
int fd_move(to,from)
int to;
int from;
{
  if (to == from) return 0;
  if (fd_copy(to,from) == -1) return -1;
  close(from);
  return 0;
}
