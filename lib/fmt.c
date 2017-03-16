/*
 *  Revision 20160711, Kai Peter
 *  - changed parameter declarations
 *  Revision 20160503, Kai Peter
 *  - added parentheses to inner while condition (line 27)
 */
#include "fmt.h"

/* Consolidate the "fmt_*.c" functions into one source file. Original files were
   of date 19980615, shipped with qmail-1.03,  except "fmt_xlong.c"  was shipped
   with the qmail-1.03-jms1.7.08-ipv6 patch.

   Included files:   Size (bytes)   Date:
     - fmt_str.c             226    19980615
     - fmt_strn.c            270    19980615
     - fmt_uint.c            150    19980615
     - fmt_uint0.c           228    19980615
     - fmt_ulong.c           320    19980615
     - fmt_xlong.c           455    20090316                                   */

/* file: fmt_str.c *********************************************************** */
unsigned int fmt_str(register char *s,register char *t)
{
  register unsigned int len;
  char ch;
  len = 0;
  if (s) { while ((ch = t[len])) s[len++] = ch; }
  else while (t[len]) len++;
  return len;
}

/* file: fmt_strn.c ********************************************************** */
unsigned int fmt_strn(register char *s,register char *t,register unsigned int n)
{
  register unsigned int len;
  char ch;
  len = 0;
  if (s) { while (n-- && (ch = t[len])) s[len++] = ch; }
  else while (n-- && t[len]) len++;
  return len;
}

/* file: fmt_uint.c */
unsigned int fmt_uint(register char *s,register unsigned int u)
{
  register unsigned long l; l = u; return fmt_ulong(s,l);
}

/* file: fmt_uint0.c ********************************************************* */
unsigned int fmt_uint0(char *s,unsigned int u,unsigned int n)
{
  unsigned int len;
  len = fmt_uint(FMT_LEN,u);
  while (len < n) { if (s) *s++ = '0'; ++len; }
  if (s) fmt_uint(s,u);
  return len;
}

/* file: fmt_ulong.c ********************************************************* */
unsigned int fmt_ulong(register char *s,register unsigned long u)
{
  register unsigned int len; register unsigned long q;
  len = 1; q = u;
  while (q > 9) { ++len; q /= 10; }
  if (s) {
    s += len;
    do { *--s = '0' + (u % 10); u /= 10; } while(u); /* handles u == 0 */
  }
  return len;
}

/* file: fmt_xlong.c ********************************************************* */
char tohex(char num) {
  if (num<10)
    return num+'0';
  else if (num<16)
    return num-10+'a';
  else
    return -1;
}

unsigned int fmt_xlong(register char *s,register unsigned long u)
{
  register unsigned int len; register unsigned long q;
  len = 1; q = u;
  while (q > 15) { ++len; q /= 16; }
  if (s) {
    s += len;
    do { *--s = tohex(u % 16); u /= 16; } while(u); /* handles u == 0 */
  }
  return len;
}
