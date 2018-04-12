/*
 *  Revision 20171130, Kai Peter
 *  - changed folder name 'control' to 'etc'
 *  - code clean up
*/
#include "cdbread.h"
#include "byte.h"
#include "open.h"
#include "error.h"
#include "control.h"
#include "constmap.h"
#include "stralloc.h"
#include "rcpthosts.h"
#include "case.h"

static int flagrh = 0;
static stralloc rh = {0};
static struct constmap maprh;
static int fdmrh;

static struct cdb c;

int rcpthosts_init()
{
  flagrh = control_readfile(&rh,"etc/rcpthosts",0);
  if (flagrh != 1) return flagrh;
  if (!constmap_init(&maprh,rh.s,rh.len,0)) return flagrh = -1;
  fdmrh = open_read("etc/morercpthosts.cdb");
  if (fdmrh == -1) if (errno != error_noent) return flagrh = -1;
  return 0;
}

static stralloc host = {0};

int rcpthosts(char *buf,int len)
{
  int j;

  if (flagrh != 1) return 1;

  j = byte_rchr(buf,len,'@');
  if (j >= len) return 1; /* presumably envnoathost is acceptable */

  ++j; buf += j; len -= j;

  if (!stralloc_copyb(&host,buf,len)) return -1;
  buf = host.s;
  case_lowerb(buf,len);

  for (j = 0;j < len;++j)
    if (!j || (buf[j] == '.'))
      if (constmap(&maprh,buf + j,len - j)) return 1;

  if (fdmrh != -1) {
    int r;
    cdb_init(&c,fdmrh);
    r = cdb_find(&c,buf,len);
    cdb_free(&c);
    if (r) return r;
  }

  return 0;
}
