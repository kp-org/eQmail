#include <sys/types.h>
#include <sys/socket.h>
#include "subfd.h"
#include "substdio.h"
#include "ip.h"
#include "ipme.h"
#include "exit.h"

char temp[IPFMT];

void main()
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
   case AF_INET:
      substdio_put(subfdout,temp,ip_fmt(temp,&ipme.ix[j].addr.ip));
      break;
#ifdef INET6
   case AF_INET6:
      substdio_put(subfdout,temp,ip6_fmt(temp,&ipme.ix[j].addr.ip6));
      break;
#endif
   default:
      substdio_puts(subfdout,"Unknown address family = ");
      substdio_put(subfdout,temp,fmt_ulong(temp,ipme.ix[j].af));
   }
   substdio_puts(subfdout,"\n");
  }
 substdio_flush(subfdout);
 _exit(0);
}
