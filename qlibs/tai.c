/*
 *  Revision 20160728, Kai Peter
 *  - new file: consolidated 'tai_*.c' files from libowfat-0.30
*/
#include <time.h>
#include "tai.h"

/* Consolidate  the "tai_*.c" functions into one source file. Original
   files are taken from libowfat-0.30.
   Included files:   Size (bytes)   Date:
     - tai_add.c             112    20020916
     - tai_now.c             102    20020916
     - tai_pack.c            297    20140314
     - tai_sub.c             297    20020916
     - tai_uint.c             78    20020916
     - tai_unpack.c          386    20020916                        */


/* file: tai_add.c ************************************************ */
void tai_add(struct tai *t,const struct tai *u,const struct tai *v)
{
  t->x = u->x + v->x;
}
/* file: tai_now.c ************************************************ */
void tai_now(struct tai *t)
{
  tai_unix(t,time((time_t *) 0));
}
/* file: tai_pack.c *********************************************** */
void tai_pack(char *s,const struct tai *t)
{
  uint64 x;

  x = t->x;
  s[7] = (char)x; x >>= 8;
  s[6] = (char)x; x >>= 8;
  s[5] = (char)x; x >>= 8;
  s[4] = (char)x; x >>= 8;
  s[3] = (char)x; x >>= 8;
  s[2] = (char)x; x >>= 8;
  s[1] = (char)x; x >>= 8;
  s[0] = (char)x;
}
/* file: tai_sub.c ************************************************ */
void tai_sub(struct tai *t,const struct tai *u,const struct tai *v)
{
  t->x = u->x - v->x;
}
/* file: tai_uint.c *********************************************** */
void tai_uint(struct tai *t,unsigned int u)
{
  t->x = u;
}
/* file: tai_unpack.c ********************************************* */
void tai_unpack(const char *s,struct tai *t)
{
  uint64 x;

  x = (unsigned char) s[0];
  x <<= 8; x += (unsigned char) s[1];
  x <<= 8; x += (unsigned char) s[2];
  x <<= 8; x += (unsigned char) s[3];
  x <<= 8; x += (unsigned char) s[4];
  x <<= 8; x += (unsigned char) s[5];
  x <<= 8; x += (unsigned char) s[6];
  x <<= 8; x += (unsigned char) s[7];
  t->x = x;
}
