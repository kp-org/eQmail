/*
 *  Revision 20160509, Kai Peter
 *  - added "close.h"
 */
#include <sys/types.h>
#include <sys/param.h>
//#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#ifndef SIOCGIFCONF /* whatever works */
#include <sys/sockio.h>
#endif
#include "hassalen.h"
#include "byte.h"
#include "ip.h"
#include "ipalloc.h"
#include "stralloc.h"
#include "ipme.h"

//#include <arpa/inet.h>
#include "close.h"

static int ipmeok = 0;
ipalloc ipme = {0};

int ipme_is(ip)
struct ip_address *ip;
{
  int i;
  if (ipme_init() != 1) return -1;
  for (i = 0;i < ipme.len;++i)
    if (ipme.ix[i].af == AF_INET && byte_equal(&ipme.ix[i].addr.ip,4,ip))
      return 1;
  return 0;
}

#ifdef INET6
int ipme_is6(ip)
struct ip6_address *ip;
{
  int i;
  if (ipme_init() != 1) return -1;
  for (i = 0;i < ipme.len;++i)
    if (ipme.ix[i].af == AF_INET6 && byte_equal(&ipme.ix[i].addr.ip6,16,ip))
      return 1;
  return 0;
}
#endif

static stralloc buf = {0};

int ipme_init()
{
  struct ifconf ifc;
  char *x;
  struct ifreq *ifr;
  struct sockaddr_in *sin;
#ifdef INET6
  struct sockaddr_in6 *sin6;
#endif
  int len;
  int s;
  struct ip_mx ix;
 
  if (ipmeok) return 1;
  if (!ipalloc_readyplus(&ipme,0)) return 0;
  ipme.len = 0;
  ix.pref = 0;
 
  /* 0.0.0.0 is a special address which always refers to 
   * "this host, this network", according to RFC 1122, Sec. 3.2.1.3a.
  */
  byte_copy(&ix.addr.ip,4,"\0\0\0\0");
  if (!ipalloc_append(&ipme,&ix)) { return 0; }
  if ((s = socket(AF_INET,SOCK_STREAM,0)) == -1) return -1;
 
  len = 256;
  for (;;) {
    if (!stralloc_ready(&buf,len)) { close(s); return 0; }
    buf.len = 0;
    ifc.ifc_buf = buf.s;
    ifc.ifc_len = len;
    if (ioctl(s,SIOCGIFCONF,&ifc) >= 0) /* > is for System V */
      if (ifc.ifc_len + sizeof(*ifr) + 64 < len) { /* what a stupid interface */
        buf.len = ifc.ifc_len;
        break;
      }
    if (len > 200000) { close(s); return -1; }
    len += 100 + (len >> 2);
  }
  x = buf.s;
  while (x < buf.s + buf.len) {
    ifr = (struct ifreq *) x;
#ifdef HASSALEN
    len = sizeof(ifr->ifr_name) + ifr->ifr_addr.sa_len;
#else   /* Kai */
    len = sizeof(*ifr);
#endif

    if (len < sizeof(*ifr))
      len = sizeof(*ifr);
    if (ifr->ifr_addr.sa_family == AF_INET) {
      sin = (struct sockaddr_in *) &ifr->ifr_addr;
      byte_copy(&ix.addr.ip,4,&sin->sin_addr);
      ix.af = AF_INET;
      if (ioctl(s,SIOCGIFFLAGS,x) == 0)
        if (ifr->ifr_flags & IFF_UP)
          if (!ipalloc_append(&ipme,&ix)) { close(s); return 0; }
    }
#ifdef INET6
	else //{
//      ix.af = AF_INET6;

	if (ifr->ifr_addr.sa_family == AF_INET6) {
      sin6 = (struct sockaddr_in6 *) &ifr->ifr_addr;
      byte_copy(&ix.addr.ip6,16,&sin6->sin6_addr);
      ix.af = AF_INET6;
      if (ioctl(s,SIOCGIFFLAGS,x) == 0)
        if (ifr->ifr_flags & IFF_UP)
          if (!ipalloc_append(&ipme,&ix)) { close(s); return 0; }
    }
#endif
/*
#else
    len = sizeof(*ifr);
    if (ioctl(s,SIOCGIFFLAGS,x) == 0)
      if (ifr->ifr_flags & IFF_UP)
        if (ioctl(s,SIOCGIFADDR,x) == 0)
	  if (ifr->ifr_addr.sa_family == AF_INET) {
	    sin = (struct sockaddr_in *) &ifr->ifr_addr;
        ix.af = AF_INET;
	    byte_copy(&ix.addr.ip,4,&sin->sin_addr);
	    if (!ipalloc_append(&ipme,&ix)) { close(s); return 0; }
	  }
#ifdef INET6
      else if (ifr->ifr_addr.sa_family == AF_INET6) {
	    sin6 = (struct sockaddr_in6 *) &ifr->ifr_addr;
//        ix.af = AF_INET6;
	    byte_copy(&ix.addr.ip6,16,&sin6->sin6_addr);
        ix.af = AF_INET6;
	    if (!ipalloc_append(&ipme,&ix)) { close(s); return 0; }
	  }
#endif
*/
//#endif
    x += len;
  }
  close(s);
  ipmeok = 1;
  return 1;
}
