#ifndef READCLOSE_H
#define READCLOSE_H

#include "stralloc.h"
#include "close.h"

extern int readclose_append(int,stralloc *,unsigned int);
extern int readclose(int,stralloc *,unsigned int);

#define slurpclose readclose /* backwards compatibility */

#endif
