/*
 *  Revision 20160728, Kai Peter
 *  - new file: consolidated some smaller files:
 *    socket_getifname.c, socket_opts.c, socket_v6any.c, socket_delay.c,
 *    socket_ip4loopback.c, socket_v6loopback.c, socket_getifidx.c,
 *    socket_listen.c, socket_v4mappedprefix.c
*/
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include "socket.h"

const unsigned char V4loopback[4] = {127,0,0,1};
/* the 'V4mappedprefix' constant is needed by 'ip.a' too */
const unsigned char V4mappedprefix[12]={0,0,0,0,0,0,0,0,0,0,0xff,0xff};
const unsigned char V6any[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const unsigned char V6loopback[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};

/* file: socket_listen.c *********************************************** */
int socket_listen(int s,int backlog)
{
  return listen(s,backlog);
}
/* file: socket_opts.c ************************************************* */
int socket_ipoptionskill(int s)
{
  return setsockopt(s,IPPROTO_IP,1,(char *) 0,0); /* 1 == IP_OPTIONS */
}
/* file: socket_delay.c ************************************************ */
int socket_tcpnodelay(int s)
{
  int opt = 1;
  return setsockopt(s,IPPROTO_TCP,1,&opt,sizeof opt); /* 1 == TCP_NODELAY */
}
/* file: socket_getifidx.c ********************************************* */
uint32 socket_getifidx(const char* ifname) {
  return if_nametoindex(ifname);
}
/* file: socket_getifname.c ******************************************** */
static char ifname[IFNAMSIZ];

const char* socket_getifname(uint32 interface) {
  char *tmp=if_indextoname(interface,ifname);
  if (tmp)
    return tmp;
  else
    return "[unknown]";
}
