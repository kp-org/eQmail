/*
 *  Revision 20170209, Kai Peter
 *  - new file: consolidated 'uint32*pack.c' files from ucspi-tcp-0.88
*/
#include "uint_t.h"

/* Consolidate  the "uint32_*pack.c"  functions into one source file.
   Original files are taken from ucspi-ssl-0.88 w/ IPv6 patch.
   Included files:   Size (bytes)   Date:
     - uint32_pack.c         294    20000318
     - uint32_unpack.c       548    20000318                        */


/* file: uint32_pack.c ******************************************** */
void uint32_pack(char s[4],uint32 u)
{
  s[0] = u & 255;
  u >>= 8;
  s[1] = u & 255;
  u >>= 8;
  s[2] = u & 255;
  s[3] = u >> 8;
}
void uint32_pack_big(char s[4],uint32 u)
{
  s[3] = u & 255;
  u >>= 8;
  s[2] = u & 255;
  u >>= 8;
  s[1] = u & 255;
  s[0] = u >> 8;
}

/* file: uint32_unpack.c ****************************************** */
void uint32_unpack(char s[4],uint32 *u)
{
  uint32 result;

  result = (unsigned char) s[3];
  result <<= 8;
  result += (unsigned char) s[2];
  result <<= 8;
  result += (unsigned char) s[1];
  result <<= 8;
  result += (unsigned char) s[0];

  *u = result;
}
void uint32_unpack_big(char s[4],uint32 *u)
{
  uint32 result;

  result = (unsigned char) s[0];
  result <<= 8;
  result += (unsigned char) s[1];
  result <<= 8;
  result += (unsigned char) s[2];
  result <<= 8;
  result += (unsigned char) s[3];

  *u = result;
}
