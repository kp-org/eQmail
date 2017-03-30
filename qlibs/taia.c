/*
 *  Revision 20170329, Kai Peter
 *  - changed return type of 'tai_now()' to int
 *  Revision 20160728, Kai Peter
 *  - new file: consolidated single files
*/
#include <sys/types.h>
#include <sys/time.h>
#include "taia.h"
/* time with a precision of 1 attosecond */

/* Consolidate the "taia_*.c" functions into one source
   file. Original files taken from ucspi-tcp-0.88.
   'taia.o' will be included into a 'time.a' lib.
   Included files:  Size (bytes)   Date
     - taia_add.c           375    20000318
     - taia_approx.c        103    20000318
     - taia_frac.c          114    20000318
     - taia_less.c          277    20000318
     - taia_now.c           249    20000318
     - taia_pack.c          332    20000318
     - taia_sub.c           428    20000318
     - taia_uint.c          152    20000318          */

/* XXX: breaks tai encapsulation */

/* file: taia_add.c ************************************ */
void taia_add(struct taia *t,struct taia *u,struct taia *v)
{
  t->sec.x = u->sec.x + v->sec.x;
  t->nano = u->nano + v->nano;
  t->atto = u->atto + v->atto;
  if (t->atto > 999999999UL) {
    t->atto -= 1000000000UL;
    ++t->nano;
  }
  if (t->nano > 999999999UL) {
    t->nano -= 1000000000UL;
    ++t->sec.x;
  }
}
/* file: taia_approx.c ********************************* */
double taia_approx(struct taia *t)
{
  return tai_approx(&t->sec) + taia_frac(t);
}
/* file taia_frac.c ************************************ */
double taia_frac(struct taia *t)
{
  return (t->atto * 0.000000001 + t->nano) * 0.000000001;
}
/* file: taia_less.c *********************************** */
int taia_less(struct taia *t,struct taia *u)
{
  if (t->sec.x < u->sec.x) return 1;
  if (t->sec.x > u->sec.x) return 0;
  if (t->nano < u->nano) return 1;
  if (t->nano > u->nano) return 0;
  return t->atto < u->atto;
}
/* file: taia_now.c ************************************ */
int taia_now(struct taia *t)
{
  struct timeval now;
  if (gettimeofday(&now,(struct timezone *) 0) == 0) {
    tai_unix(&t->sec,now.tv_sec);
    t->nano = 1000 * now.tv_usec + 500;
    t->atto = 0;
    return(0);
  }
  t->nano = 0;
  t->atto = 0;
  return -1;
}
/* file: taia_pack.c *********************************** */
void taia_pack(char *s,struct taia *t)
{
  unsigned long x;

  tai_pack(s,&t->sec);
  s += 8;

  x = t->atto;
  s[7] = x & 255; x >>= 8;
  s[6] = x & 255; x >>= 8;
  s[5] = x & 255; x >>= 8;
  s[4] = x;
  x = t->nano;
  s[3] = x & 255; x >>= 8;
  s[2] = x & 255; x >>= 8;
  s[1] = x & 255; x >>= 8;
  s[0] = x;
}
/* file: taia_sub.c ************************************ */
void taia_sub(struct taia *t,struct taia *u,struct taia *v)
{
  unsigned long unano = u->nano;
  unsigned long uatto = u->atto;

  t->sec.x = u->sec.x - v->sec.x;
  t->nano = unano - v->nano;
  t->atto = uatto - v->atto;
  if (t->atto > uatto) {
    t->atto += 1000000000UL;
    --t->nano;
  }
  if (t->nano > unano) {
    t->nano += 1000000000UL;
    --t->sec.x;
  }
}
/* file: taia_uint ************************************* */
void taia_uint(struct taia *t,unsigned int s)
{
  t->sec.x = s;
  t->nano = 0;
  t->atto = 0;
}
