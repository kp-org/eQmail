/*
 *  Revision 20160316, Kai Peter
 *  - changed parameter declarations
 *  Revision 20160503, Kai Peter
 *  - added parentheses to while condition (case_lowers)
*/
#include "case.h"
/* Consolidate  the "case_*.c" functions into one source file. Original
   files  were of date 19980615, shipped with  qmail-1.03,  except file
   "case_startb.c" was shipped with qmail-smtpd-auth-0.52 patch.
   The "case" functions will be linked to "case.a" only!
   Included files:   Size (bytes)   Date:
     - case_diffb.c          423    19980615
     - case_diffs.c          393    19980615
     - case_lowerb.c         193    19980615
     - case_lowers.c         155    19980615
     - case_startb.c         397    20140914
     - case_starts.c         340    19980615                         */

/* file: case_diffb.c ********************************************** */
int case_diffb(register char *s,unsigned int len,register char *t)
{
  register unsigned char x;
  register unsigned char y;

  while (len > 0) {
    --len;
    x = *s++ - 'A';
    if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
    y = *t++ - 'A';
    if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
    if (x != y)
      return ((int)(unsigned int) x) - ((int)(unsigned int) y);
  }
  return 0;
}

/* file: case_diffs.c ********************************************** */
int case_diffs(register char *s,register char *t)
{
  register unsigned char x;
  register unsigned char y;

  for (;;) {
    x = *s++ - 'A';
    if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
    y = *t++ - 'A';
    if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
    if (x != y) break;
    if (!x) break;
  }
  return ((int)(unsigned int) x) - ((int)(unsigned int) y);
}

/* file: case_lowerb.c ********************************************* */
void case_lowerb(char *s,unsigned int len)
{
  unsigned char x;
  while (len > 0) {
    --len;
    x = *s - 'A';
    if (x <= 'Z' - 'A') *s = x + 'a';
    ++s;
  }
}

/* file: case_lowers.c ********************************************* */
void case_lowers(char *s)
{
  unsigned char x;
  while ((x = *s)) {
    x -= 'A';
    if (x <= 'Z' - 'A') *s = x + 'a';
    ++s;
  }
}

/* file: case_startb.c ********************************************* */
int case_startb(register char *s,unsigned int len,register char *t)
{
  register unsigned char x;
  register unsigned char y;

  for (;;) {
    y = *t++ - 'A';
    if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
    if (!y) return 1;
    if (!len) return 0;
    --len;
    x = *s++ - 'A';
    if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
    if (x != y) return 0;
  }
}

/* file: case_starts.c ********************************************* */
int case_starts(register char *s,register char *t)
{
  register unsigned char x;
  register unsigned char y;

  for (;;) {
    x = *s++ - 'A';
    if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
    y = *t++ - 'A';
    if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
    if (!y) return 1;
    if (x != y) return 0;
  }
}
