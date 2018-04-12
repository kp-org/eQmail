/*
*/
#include <unistd.h>
#include <socket.h>
#include <netdb.h>
#include "buffer.h"
#include "getoptb.h"
#include "error.h"
#include "str.h"

#define WHO "hostname"

char host[256];

int main(int argc, char **argv)
{
int flagname = 0;

  if (argv[1])
    if (str_diff(argv[1],"-s") == 0) flagname = 1;

  host[0] = 0; /* sigh */
  gethostname(host,sizeof(host));
  host[sizeof(host) - 1] = 0;

  if (flagname == 1) {
    /* print the hostname w/o domain */
    buffer_puts(buffer_1,host);
  } else {
    /* get and print the fqdn */
    struct hostent* h;
    h = gethostbyname(host);
    buffer_puts(buffer_1,h->h_name);
  }

  buffer_puts(buffer_1,"\n");
  buffer_flush(buffer_1);
  _exit(0);
}
