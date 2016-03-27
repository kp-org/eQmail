#include <sys/types.h>
#include <fcntl.h>
#include "open.h"
/* Consolidate the "open_*.c" functions into one source
   file. Original files were of date 19980615,  shipped
   with  qmail-1.03.
   These functions were be linked to "open.a" only!
   Included files   Size (bytes)   Date
     - open_append.c        158    19980615
     - open_excl.c          143    19980615
     - open_read.c          130    19980615
     - open_trunc.c         156    19980615
     - open_write.c         131    19980615          */

/* file: open_append.c */
int open_append(fn) char *fn;
{ return open(fn,O_WRONLY | O_NDELAY | O_APPEND | O_CREAT,0600); }

/* file: open_excl.c */
int open_excl(fn) char *fn;
{ return open(fn,O_WRONLY | O_EXCL | O_CREAT,0644); }

/* file: open_read.c */
int open_read(fn) char *fn;
{ return open(fn,O_RDONLY | O_NDELAY); }

/* file: open_trunc.c */
int open_trunc(fn) char *fn;
{ return open(fn,O_WRONLY | O_NDELAY | O_TRUNC | O_CREAT,0644); }

/* file: open_write.c */
int open_write(fn) char *fn;
{ return open(fn,O_WRONLY | O_NDELAY); }
