#include <sys/types.h>
#include <sys/file.h>
#include <fcntl.h>
//#include "hasflock.h"
#include "lock.h"
/* Consolidate  the "lock_*.c" functions into one source
   file.  Original files were of date 19980615,  shipped
   with  qmail-1.03.
   The "lock" functions will be linked to "lock.a" only!
   Included files:   Size (bytes)   Date:
     - lock_ex.c             238    19980615
     - lock_exnb.c           252    19980615
     - lock_un.c             238    19980615          */

/* file: lock_ex.c */
#ifdef HASFLOCK
int lock_ex(fd) int fd; { return flock(fd,LOCK_EX); }
#else
int lock_ex(fd) int fd; { return lockf(fd,1,0); }
#endif

/* file: lock_exnb.c */
#ifdef HASFLOCK
int lock_exnb(fd) int fd; { return flock(fd,LOCK_EX | LOCK_NB); }
#else
int lock_exnb(fd) int fd; { return lockf(fd,2,0); }
#endif

/* file: lock_un.c */
#ifdef HASFLOCK
int lock_un(fd) int fd; { return flock(fd,LOCK_UN); }
#else
int lock_un(fd) int fd; { return lockf(fd,0,0); }
#endif
