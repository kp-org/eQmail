#ifndef REMOTEINFO_H
#define REMOTEINFO_H

#include "stralloc.h"
#include "uint_t.h"

extern int remoteinfo(stralloc *,char *,uint16,char *,uint16,unsigned int);
extern int remoteinfo6(stralloc *,char *,uint16,char *,uint16,unsigned int,uint32);

/* DEPRECATED!
   backwards compatibility with (net)qmail "original" code
*/
extern char *remoteinfo_get();

#endif
