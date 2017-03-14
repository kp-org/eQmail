/* Use <dirent.h> always because any modern
   system should have it (instead of using
   BSD's <sys/dir.h>).
*/
#ifndef DIRENTRY_H
#define DIRENTRY_H

#include <sys/types.h>
#include <dirent.h>
#define direntry struct dirent

#endif
