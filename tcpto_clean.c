#include <unistd.h>		/* replace "readwrite.h" */
#include "ipalloc.h"
#include "tcpto.h"
#include "open.h"
#include "buffer.h"

struct tcpto_buf tcpto_cleanbuf[TCPTO_BUFSIZ];

void tcpto_clean() /* running from queue/mess */
{
  int fd;
  int i;
  buffer b;

  fd = open_write("../lock/tcpto");
  if (fd == -1) return;
  buffer_init(&b,write,fd,(char *)tcpto_cleanbuf,sizeof(tcpto_cleanbuf));
  for (i = 0;i < sizeof(tcpto_cleanbuf);++i)
    { buffer_put(&b,"",1); }
  buffer_flush(&b); /* if it fails, bummer */
  close(fd);
}
