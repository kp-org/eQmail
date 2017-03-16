#ifndef IPALLOC_H
#define IPALLOC_H

#include "ip.h"
#include "stralloc.h"

#ifdef TLS
# define IX_FQDN 1
#endif

#ifdef IX_FQDN
struct ip_mx {
  unsigned short af;
  union {
    struct ip_address ip;
#ifdef INET6
    struct ip6_address ip6;
#endif
  } addr;
  int pref;
  char *fqdn;
};
#else
struct ip_mx {
  unsigned short af;
  union {
	struct ip_address ip;
#ifdef INET6
    struct ip6_address ip6;
#endif
  } addr;
  int pref;
};

#endif

GEN_ALLOC_typedef(ipalloc,struct ip_mx,ix,len,a)
extern int ipalloc_readyplus();
extern int ipalloc_append();

#endif
