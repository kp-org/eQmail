/*
 *  Revision 20160509, Kai Peter
 *  - added 'fmt.h'
 *
 *  Revision 20160503, Kai Peter
 *  - changed return type of main to int
 */
#include <sys/types.h>
#include <sys/socket.h>
#include "subfd.h"
#include "substdio.h"
#include "ip.h"
#include "ipme.h"
#include "exit.h"
#include "fmt.h"

//#include <netinet/in.h>

char temp[IPFMT];

int main()
{
 int j;
 switch(ipme_init())
  {
   case 0: substdio_putsflush(subfderr,"out of memory\n"); _exit(111);
   case -1: substdio_putsflush(subfderr,"hard error\n"); _exit(100);
  }
 for (j = 0;j < ipme.len;++j)
  {
   switch(ipme.ix[j].af) {
//switch(salocal.sa.sa_family) {
   case AF_INET:
      substdio_put(subfdout,temp,ip_fmt(temp,&ipme.ix[j].addr.ip));
substdio_put(subfdout,temp,ipme.ix[j].af);
      break;
#ifdef INET6
   case AF_INET6:
      substdio_put(subfdout,temp,ip6_fmt(temp,&ipme.ix[j].addr.ip6));
      break;
#endif
   default:
      substdio_puts(subfdout,"Unknown address family = ");
      substdio_put(subfdout,temp,fmt_ulong(temp,ipme.ix[j].af));
//substdio_put(subfdout,temp,fmt_xlong(temp,ipme.ix[j].af));
//substdio_put(subfdout,temp,ip6_fmt(temp,&ipme.ix[j].addr.ip6));
   }
   substdio_puts(subfdout,"\n");
  }
 substdio_flush(subfdout);
 return(0);
}
