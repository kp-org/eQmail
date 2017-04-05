#ifndef IPME_H
#define IPME_H

#include "ip.h"
#include "ipalloc.h"

extern ipalloc ipme;

int ipme_init();
int ipme_is();
int ipme_is4();
int ipme_is6();
//int ipme_is46();

#define ipme_is46 ipme_is   /* backwards compatibility */

#endif
