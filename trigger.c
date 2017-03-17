#include "select.h"
#include "open.h"
#include "trigger.h"
#include "close.h"

static int fd = -1;

void trigger_set()
{
  if (fd != -1)
    close(fd);
  fd = open_read("lock/trigger");
}

void trigger_selprep(int *nfds,fd_set *rfds)
{
  if (fd != -1)
  {
    FD_SET(fd,rfds);
    if (*nfds < fd + 1) *nfds = fd + 1;
  }
}

int trigger_pulled(fd_set *rfds)
{
  if (fd != -1) if (FD_ISSET(fd,rfds)) return 1;
  return 0;
}
