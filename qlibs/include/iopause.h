#ifndef IOPAUSE_H
#define IOPAUSE_H

/* sysdep: +poll */
#define IOPAUSE_POLL

#include <sys/types.h>
#include <poll.h>

typedef struct pollfd iopause_fd;
#define IOPAUSE_READ POLLIN
#define IOPAUSE_WRITE POLLOUT

#include "taia.h"

extern void iopause(iopause_fd *,unsigned int,struct taia *,struct taia *);

#endif

/* from alternative header w/o having 'poll' (iopause.h1),
   usually useless today, cause all *BSD/Linux have 'poll' */
//#ifndef IOPAUSE_H
//#define IOPAUSE_H

/* sysdep: -poll */

/*
typedef struct {
  int fd;
  short events;
  short revents;
} iopause_fd;

#define IOPAUSE_READ 1
#define IOPAUSE_WRITE 4

#include "taia.h"

extern void iopause(iopause_fd *,unsigned int,struct taia *,struct taia *);

#endif
*/
