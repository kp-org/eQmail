/*
 *  Revision 20160506, Kai Peter
 *  - added declarations for 'lseek', 'ftruncate'
 */
#include <sys/types.h>
#include "seek.h"
/* Consolidate  the "seek_*.c" functions into one source file. Original
   files were of date 19980615, shipped with qmail-1.03.
   The "seek" functions will be linked to "seek.a" only!
   Included files:    Size (bytes)    Date
     - seek_cur.c             134     19980615
     - seek_end.c             153     19980615
     - seek_set.c             173     19980615
     - seek_trunc.c           125     19980615                       */

off_t lseek(int fd, off_t offset, int whence);
int ftruncate(int fd, off_t length);

/* file: seek_cur.c ************************************************ */
#define CUR 1 /* sigh */

seek_pos seek_cur(int fd)
{ return lseek(fd,(off_t) 0,CUR); }

/* file: seek_end.c ************************************************ */
#define END 2 /* sigh */

int seek_end(int fd)
{ if (lseek(fd,(off_t) 0,END) == -1) return -1; return 0; }

/* file: seek_set.c ************************************************ */
#define SET 0 /* sigh */

int seek_set(int fd,seek_pos pos)
{ if (lseek(fd,(off_t) pos,SET) == -1) return -1; return 0; }

/* file: seek_trunc.c ********************************************** */
int seek_trunc(int fd,seek_pos pos)
{ return ftruncate(fd,(off_t) pos); }
