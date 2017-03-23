/*
 *
 *  - consolidated 'socket_bind6.c' into 'socket_bind.c' (from ucspi-tcp-0.88)
*/

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "byte.h"
#include "socket.h"
//#include "ip6.h"
#include "ip2.h"
//#include "haveip6.h"
#include "error.h"

/* file: socket_bind.c ***************************************************** */
int socket_bind4(int s,const char ip[4],uint16 port)
{
  struct sockaddr_in sa;

  byte_zero(&sa,sizeof sa);
  sa.sin_family = AF_INET;
  uint16_pack_big((char *) &sa.sin_port,port);
  byte_copy((char *) &sa.sin_addr,4,ip);

  return bind(s,(struct sockaddr *) &sa,sizeof sa);
}

int socket_bind4_reuse(int s,const char ip[4],uint16 port)
{
  int opt = 1;
  setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof opt);
  return socket_bind4(s,ip,port);
}

/* seems not to be used here
void socket_tryreservein(int s,int size)
{
  while (size >= 1024) {
    if (setsockopt(s,SOL_SOCKET,SO_RCVBUF,&size,sizeof size) == 0) return;
    size -= (size >> 5);
  }
}
*/

/* file: socket_bind6.c ************************************************ */
int socket_bind6(int s,const char ip[16],uint16 port,uint32 scope_id)
{
#ifdef LIBC_HAS_IP6
  struct sockaddr_in6 sa;

  if (noipv6) {
#endif
    int i;
    for (i=0; i<16; i++)
      if (ip[i]!=0) break;
    if (i==16 || ip6_isv4mapped(ip))
      return socket_bind4(s,ip+12,port);
#ifdef LIBC_HAS_IP6
  }
  byte_zero(&sa,sizeof sa);
  sa.sin6_family = AF_INET6;
  uint16_pack_big((char *) &sa.sin6_port,port);
/*  implicit: sa.sin6_flowinfo = 0; */
  byte_copy((char *) &sa.sin6_addr,16,ip);
  sa.sin6_scope_id=scope_id;

  return bind(s,(struct sockaddr *) &sa,sizeof sa);
#else
  errno=error_proto;
  return -1;
#endif
}

int socket_bind6_reuse(int s,const char ip[16],uint16 port,uint32 scope_id)
{
  int opt = 1;
  setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof opt);
  return socket_bind6(s,ip,port,scope_id);
}
