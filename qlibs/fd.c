/*
 *  Revision 20160912, Kai Peter
 *  - consolidated 'coe.c'
 *  Revision 20160712, Kai Peter
 *  - removed 'close.h', added seperate declaration
 *  Revision 20160628, Kai Peter
 *  - changed parameter declarations
 *  Revision 20160503, Kai Peter
 *  - removed duplicate '#include "fd.h"'
 */
#include <fcntl.h>
#include "fd.h"
/* Consolidate the "fd_*.c"  functions into one source
   file. Original files were of date 19980615, shipped
   with qmail-1.03.
   The "fd" functions will be linked to "fd.a" only!
   Included files:   Size (bytes)   Date:
     - fd_copy.c             225    19980615
     - fd_move.c             156    19980615
     - coe.c                  91    19980615        */

int close(int __fd);  /* we won't use unistd.h here */

/* file: fd_copy.c ******************************** */
int fd_copy(int to,int from)
{
  if (to == from) return 0;
  if (fcntl(from,F_GETFL,0) == -1) return -1;
  close(to);
  if (fcntl(from,F_DUPFD,to) == -1) return -1;
  return 0;
}

/* file: fd_move.c ******************************** */
int fd_move(int to,int from)
{
  if (to == from) return 0;
  if (fd_copy(to,from) == -1) return -1;
  close(from);
  return 0;
}

/* file: coe.c ************************************ */
int fd_coe(int fd) {return fcntl(fd,F_SETFD,1); }
