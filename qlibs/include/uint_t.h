/* Define integer types and size through <stdint.h>
 *
 * Revision 20160708, Kai Peter
*/
#include <stdint.h>

#ifndef UINT16_H
#define UINT16_H

typedef uint16_t uint16;

extern void uint16_pack(char *,uint16);
extern void uint16_pack_big(char *,uint16);
extern void uint16_unpack(char *,uint16 *);
extern void uint16_unpack_big(char *,uint16 *);
#endif

#ifndef UINT32_H
#define UINT32_H

typedef uint32_t uint32;

extern void uint32_pack(char *,uint32);
extern void uint32_pack_big(char *,uint32);
extern void uint32_unpack(char *,uint32 *);
extern void uint32_unpack_big(char *,uint32 *);
#endif

#ifndef UINT64_H
#define UINT64_H

typedef uint64_t uint64;
#endif
