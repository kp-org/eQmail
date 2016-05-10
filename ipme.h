/*
 *  Revision 20160510, Kai Peter
 *  - added declaration 'ipme_is6'
 */
#ifndef IPME_H
#define IPME_H

#include "ip.h"
#include "ipalloc.h"

extern ipalloc ipme;

extern int ipme_init();
extern int ipme_is();

#ifdef INET6
//int ipme_is6(struct ip6_address *);
extern int ipme_is6();
#endif

#endif
