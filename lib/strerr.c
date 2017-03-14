#include "substdio.h"
#include "subfd.h"
#include "exit.h"
#include "error.h"
#include "strerr.h"

/* Consolidate the "strerr_*.c"  functions into  one source
   file. The original files were of date 19980615,  shipped
   with qmail-1.03.
   The "strerr_*" functions  will be linked  to  "strerr.a"
   only!
   Included files:   Size (bytes)   Date:
     - strerr_die.c          870    19980615
     - strerr_sys.c          192    19980615             */

/* file strerr_die.c **********************************  */
void strerr_warn(x1,x2,x3,x4,x5,x6,se)
char *x1; char *x2; char *x3; char *x4; char *x5; char *x6;
struct strerr *se;
{
  strerr_sysinit();

  if (x1) substdio_puts(subfderr,x1);
  if (x2) substdio_puts(subfderr,x2);
  if (x3) substdio_puts(subfderr,x3);
  if (x4) substdio_puts(subfderr,x4);
  if (x5) substdio_puts(subfderr,x5);
  if (x6) substdio_puts(subfderr,x6);
 
  while(se) {
    if (se->x) substdio_puts(subfderr,se->x);
    if (se->y) substdio_puts(subfderr,se->y);
    if (se->z) substdio_puts(subfderr,se->z);
    se = se->who;
  }

  substdio_puts(subfderr,"\n");
  substdio_flush(subfderr);
}

void strerr_die(e,x1,x2,x3,x4,x5,x6,se)
int e;
char *x1; char *x2; char *x3; char *x4; char *x5; char *x6;
struct strerr *se;
{
  strerr_warn(x1,x2,x3,x4,x5,x6,se);
  _exit(e);
}

/* file: strerr_sys.c */
struct strerr strerr_sys;

void strerr_sysinit()
{
  strerr_sys.who = 0;
  strerr_sys.x = error_str(errno);
  strerr_sys.y = "";
  strerr_sys.z = "";
}
