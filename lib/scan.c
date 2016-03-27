#include "scan.h"
/* Consolidate the "scan_*.c" functions into one source
   file. Original files were of date 19980615,  shipped
   with  qmail-1.03,  except  "scan_0x.c",  was shipped
   with qmail-1.03-jms1.7.08-ipv6 patch.
   The "scan"  functions will be linked to "fs.a" only!
   Included files   Size (bytes)   Date
     - scan_0x.c            525    20090316
     - scan_8long.c         342    19980615
     - scan_ulong.c         344    19980615          */

/* file: scan_0x.c */
static long int fromhex(unsigned char c) {
  if (c>='0' && c<='9')
    return c-'0';
  else if (c>='A' && c<='F')
    return c-'A'+10;
  else if (c>='a' && c<='f')
    return c-'a'+10;
  return -1;
}
unsigned int scan_0x(s, u) register char *s; register unsigned int *u;
{
  register unsigned int pos = 0;
  register unsigned long result = 0;
  register long int c;
  while ((c = fromhex((unsigned char) (s[pos]))) >= 0) {
    result = (result << 4) + c;
    ++pos;
  }
  *u = result;
  return pos;
}

/* file: scan_8long.c */
unsigned int scan_8long(s,u) register char *s; register unsigned long *u;
{
  register unsigned int pos; register unsigned long result;
  register unsigned long c;
  pos = 0; result = 0;
  while ((c = (unsigned long) (unsigned char) (s[pos] - '0')) < 8)
    { result = result * 8 + c; ++pos; }
  *u = result; return pos;
}

/* file: scan_ulong.c */
unsigned int scan_ulong(s,u) register char *s; register unsigned long *u;
{
  register unsigned int pos; register unsigned long result;
  register unsigned long c;
  pos = 0; result = 0;
  while ((c = (unsigned long) (unsigned char) (s[pos] - '0')) < 10)
    { result = result * 10 + c; ++pos; }
  *u = result; return pos;
}
