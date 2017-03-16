/*
 *  Revision 20160728, Kai Peter
 *  - new file: consolidated 'socket_tcp*.c', 'socket_udp*.c'
*/
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "close.h"         /* better use unistd.h ? */
#include "ndelay.h"
#include "socket.h"
#include "error.h"

#ifdef INET6
#define LIBC_HAS_IP6 1     /* just another dup */
#endif

/* file: socket_tcp.c *********************************************** */
//int socket_tcp(void)
int socket_tcp4(void)
{
  int s;

  s = socket(AF_INET,SOCK_STREAM,0);
  if (s == -1) return -1;
  if (ndelay_on(s) == -1) { close(s); return -1; }
  return s;
}

/* file: socket_tcp6.c ********************************************** */
#ifdef LIBC_HAS_IP6
int noipv6=0;
#else
int noipv6=1;
#endif

int socket_tcp6(void)
{
#ifdef LIBC_HAS_IP6
  int s;

  if (noipv6) goto compat;
  s = socket(PF_INET6,SOCK_STREAM,0);
  if (s == -1) {
    if (errno == EINVAL || errno == EAFNOSUPPORT) {
compat:
      s=socket(AF_INET,SOCK_STREAM,0);
      noipv6=1;
      if (s==-1) return -1;
    } else
    return -1;
  }
  if (ndelay_on(s) == -1) { close(s); return -1; }
#ifdef IPV6_V6ONLY
  {
    int zero=0;
    setsockopt(s,IPPROTO_IPV6,IPV6_V6ONLY,(void*)&zero,sizeof(zero));
  }
#endif
  return s;
#else
  return socket_tcp();
#endif
}

// udp:

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT EINVAL
#endif

/* file: socket_udp.c *********************************************** */
//int socket_udp(void)
int socket_udp4(void)
{
  int s;

  s = socket(AF_INET,SOCK_DGRAM,0);
  if (s == -1) return -1;
  if (ndelay_on(s) == -1) { close(s); return -1; }
  return s;
}

/* file: socket_udp6.c ********************************************** */
int socket_udp6(void)
{
#ifdef LIBC_HAS_IP6
  int s;

  if (noipv6) goto compat;
  s = socket(PF_INET6,SOCK_DGRAM,0);
  if (s == -1) {
    if (errno == EINVAL || errno == EAFNOSUPPORT) {
compat:
      s=socket(AF_INET,SOCK_DGRAM,0);
      noipv6=1;
      if (s==-1) return -1;
    } else
    return -1;
  }
#ifdef IPV6_V6ONLY
  {
    int zero=0;
    setsockopt(s,IPPROTO_IPV6,IPV6_V6ONLY,(void*)&zero,sizeof(zero));
  }
#endif
  return s;
#else
  return socket_udp();
#endif
}
