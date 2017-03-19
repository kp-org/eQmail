#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ndelay.h"
#include "select.h"
#include "error.h"
#include "ip.h"
#include "byte.h"
#include "timeoutconn.h"

#include "ipalloc.h"

void read();   /* temp */

int timeoutconn(int s,struct ip_address *ip,unsigned int port,int timeout)
//int s;
//struct ip_address *ip;
//unsigned int port;
//int timeout;
{
  char ch;
  struct sockaddr_in sin;
  char *x;
  fd_set wfds;
  struct timeval tv;

  byte_zero(&sin,sizeof(sin));
  byte_copy(&sin.sin_addr,4,ip);
  x = (char *) &sin.sin_port;
  x[1] = port; port >>= 8; x[0] = port;
  sin.sin_family = AF_INET;

  if (ndelay_on(s) == -1) return -1;

  /* XXX: could bind s */

  if (connect(s,(struct sockaddr *) &sin,sizeof(sin)) == 0) {
    ndelay_off(s);
    return 0;
  }
  if ((errno != error_inprogress) && (errno != error_wouldblock)) return -1;

  FD_ZERO(&wfds);
  FD_SET(s,&wfds);
  tv.tv_sec = timeout; tv.tv_usec = 0;

  if (select(s + 1,(fd_set *) 0,&wfds,(fd_set *) 0,&tv) == -1) return -1;
  if (FD_ISSET(s,&wfds)) {
    int dummy;
    dummy = sizeof(sin);
    if (getpeername(s,(struct sockaddr *) &sin,(socklen_t *)&dummy) == -1) {
      /* get return value of read to prevent warn_unused_result */
      //int x = 
      read(s,&ch,1);
      return -1;
    }
    ndelay_off(s);
    return 0;
  }

  errno = error_timeout; /* note that connect attempt is continuing */
  return -1;
}

#ifdef INET6
int timeoutconn6(int s,struct ip6_address *ip,unsigned int port,int timeout)
//int s;
//struct ip6_address *ip;
//unsigned int port;
//int timeout;
{
  char ch;
  struct sockaddr_in6 sin;
//  char *x;
  fd_set wfds;
  struct timeval tv;

  byte_zero(&sin,sizeof(sin));
  byte_copy(&sin.sin6_addr,16,ip);
  sin.sin6_port = htons(port);;
  sin.sin6_family = AF_INET6;

  if (ndelay_on(s) == -1) return -1;

  /* XXX: could bind s */

  if (connect(s,(struct sockaddr *) &sin,sizeof(sin)) == 0) {
    ndelay_off(s);
    return 0;
  }
  if ((errno != error_inprogress) && (errno != error_wouldblock)) return -1;

  FD_ZERO(&wfds);
  FD_SET(s,&wfds);
  tv.tv_sec = timeout; tv.tv_usec = 0;

  if (select(s + 1,(fd_set *) 0,&wfds,(fd_set *) 0,&tv) == -1) return -1;
  if (FD_ISSET(s,&wfds)) {
    int dummy;
    dummy = sizeof(sin);
    if (getpeername(s,(struct sockaddr *) &sin,(socklen_t *)&dummy) == -1) {
      /* get return value of read to prevent warn_unused_result */
      //int x = 
      read(s,&ch,1);
      return -1;
    }
    ndelay_off(s);
    return 0;
  }

  errno = error_timeout; /* note that connect attempt is continuing */
  return -1;
}
#endif
