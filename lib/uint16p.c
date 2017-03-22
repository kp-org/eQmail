/*
 *  Revision 20170209, Kai Peter
 *  - new file: consolidated 'uint16*pack.c' files from ucspi-tcp-0.88
*/
#include "uint_t.h"

/* Consolidate  the "uint16_*pack.c"  functions into  one source file.
   Original files are taken from ucspi-ssl-0.88 w/ IPv6 patch.
   Included files:   Size (bytes)   Date:
     - uint16_pack.c         178    20000318
     - uint16_unpack.c       348    20000318                        */

/* file: uint16_pack.c ******************************************** */
void uint16_pack(char s[2],uint16 u)
{
  s[0] = u & 255;
  s[1] = u >> 8;
}

void uint16_pack_big(char s[2],uint16 u)
{
  s[1] = u & 255;
  s[0] = u >> 8;
}

/* file: uint16_unpack.c ****************************************** */
void uint16_unpack(char s[2],uint16 *u)
{
  uint16 result;

  result = (unsigned char) s[1];
  result <<= 8;
  result += (unsigned char) s[0];

  *u = result;
}
void uint16_unpack_big(char s[2],uint16 *u)
{
  uint16 result;

  result = (unsigned char) s[0];
  result <<= 8;
  result += (unsigned char) s[1];

  *u = result;
}
