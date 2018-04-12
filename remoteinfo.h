#ifndef REMOTEINFO_H
#define REMOTEINFO_H

#include "stralloc.h"
#include "uint_t.h"

extern int remoteinfo6(stralloc *info,char *remoteip,uint16 remoteport,
            char *localip,uint16 localport,unsigned int timeout,uint32 netifidx);

/* DEPRECATED!
   backwards compatibility with (net)qmail "original" code
*/
extern int remoteinfo(stralloc *,char *,uint16,char *,uint16,unsigned int);
extern char *remoteinfo_get();

#endif
