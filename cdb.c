/*
 *  Revision 20160509, Kai Peter
 *  - added '<unistd.h>'
 */
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "cdb.h"

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
/* Consolidated cdb_seek.c, cdb_hash.c, cdb_unpack.c to
   cdb.c, original files shipped with qmail-1.03.
   The cdb code differs from the package cdb-0.75.
   Files:
     - cdb_seek.c
     - cdb_hash.c
     - cdb_unpack.c                                  */

/* file: cdb_seek.c ************************************************* */
int cdb_bread(fd,buf,len)
int fd;
char *buf;
int len;
{
  int r;
  while (len > 0) {
    do
      r = read(fd,buf,len);
    while ((r == -1) && (errno == EINTR));
    if (r == -1) return -1;
    if (r == 0) { errno = EIO; return -1; }
    buf += r;
    len -= r;
  }
  return 0;
}

static int match(fd,key,len)
int fd;
char *key;
unsigned int len;
{
  char buf[32];
  int n;
  int i;

  while (len > 0) {
    n = sizeof(buf);
    if (n > len) n = len;
    if (cdb_bread(fd,buf,n) == -1) return -1;
    for (i = 0;i < n;++i) if (buf[i] != key[i]) return 0;
    key += n;
    len -= n;
  }
  return 1;
}

int cdb_seek(fd,key,len,dlen)
int fd;
char *key;
unsigned int len;
uint32 *dlen;
{
  char packbuf[8];
  uint32 pos;
  uint32 h;
  uint32 lenhash;
  uint32 h2;
  uint32 loop;
  uint32 poskd;

  h = cdb_hash(key,len);

  pos = 8 * (h & 255);
  if (lseek(fd,(off_t) pos,SEEK_SET) == -1) return -1;

  if (cdb_bread(fd,packbuf,8) == -1) return -1;

  pos = cdb_unpack(packbuf);
  lenhash = cdb_unpack(packbuf + 4);

  if (!lenhash) return 0;
  h2 = (h >> 8) % lenhash;

  for (loop = 0;loop < lenhash;++loop) {
    if (lseek(fd,(off_t) (pos + 8 * h2),SEEK_SET) == -1) return -1;
    if (cdb_bread(fd,packbuf,8) == -1) return -1;
    poskd = cdb_unpack(packbuf + 4);
    if (!poskd) return 0;
    if (cdb_unpack(packbuf) == h) {
      if (lseek(fd,(off_t) poskd,SEEK_SET) == -1) return -1;
      if (cdb_bread(fd,packbuf,8) == -1) return -1;
      if (cdb_unpack(packbuf) == len)
	switch(match(fd,key,len)) {
	  case -1:
	    return -1;
	  case 1:
	    *dlen = cdb_unpack(packbuf + 4);
	    return 1;
	}
    }
    if (++h2 == lenhash) h2 = 0;
  }
  return 0;
}

/* file: cdb_hash.c ************************************************* */
uint32 cdb_hash(buf,len)
unsigned char *buf;
unsigned int len;
{
  uint32 h;

  h = 5381;
  while (len) {
    --len;
    h += (h << 5);
    h ^= (uint32) *buf++;
  }
  return h;
}

/* file: cdb_unpack.c *********************************************** */
uint32 cdb_unpack(buf)
unsigned char *buf;
{
  uint32 num;
  num = buf[3]; num <<= 8;
  num += buf[2]; num <<= 8;
  num += buf[1]; num <<= 8;
  num += buf[0];
  return num;
}
