/*
 *  Revision 20180306, Kai Peter
 *  - switched to buffer
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "buffer.h"
#include "ip.h"
#include "ipme.h"
#include "exit.h"
#include "fmt.h"

char ipaddr[IPFMT];

int main()
{
 int j;

 switch (ipme_init()) {
   case  0: buffer_putsflush(buffer_2,"out of memory\n"); _exit(111);
   case -1: buffer_putsflush(buffer_2,"hard error\n"); _exit(100);
 }

 for (j = 0;j < ipme.len;++j) {
   switch(ipme.ix[j].af) {
     case AF_INET:
       buffer_put(buffer_1,ipaddr,ip4_fmt(ipaddr,(char *)&ipme.ix[j].addr));
       break;
     case AF_INET6:
       buffer_put(buffer_1,ipaddr,ip6_fmt(ipaddr,(char *)&ipme.ix[j].addr));
       break;
     default:
       buffer_puts(buffer_1,"Unknown address family = ");
       buffer_put(buffer_1,ipaddr,fmt_ulong(ipaddr,ipme.ix[j].af));
   }
	buffer_puts(buffer_1,"\n");
 }
  buffer_flush(buffer_1);
 _exit(0);
}
