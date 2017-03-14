#include "byte.h"
/* Consolidate the "byte_*.c" functions into one source
   file. Original files were shipped with qmail-1.03.
   The "byte" functions will be linked to "str.a" only!
   Included files:  Size (bytes)
     - byte_chr.c           386
     - byte_copy.c          299
     - byte_cr.c            314
     - byte_diff.c          453
     - byte_rchr.c          433
     - byte_zero.c          230                      */

/* file: byte_chr.c */
unsigned int byte_chr(s,n,c)
char *s;
register unsigned int n;
int c;
{
  register char ch;
  register char *t;

  ch = c;
  t = s;
  for (;;) {
    if (!n) break; if (*t == ch) break; ++t; --n;
    if (!n) break; if (*t == ch) break; ++t; --n;
    if (!n) break; if (*t == ch) break; ++t; --n;
    if (!n) break; if (*t == ch) break; ++t; --n;
  }
  return t - s;
}

/* file: byte_copy.c */
void byte_copy(to,n,from)
register char *to;
register unsigned int n;
register char *from;
{
  for (;;) {
    if (!n) return; *to++ = *from++; --n;
    if (!n) return; *to++ = *from++; --n;
    if (!n) return; *to++ = *from++; --n;
    if (!n) return; *to++ = *from++; --n;
  }
}

/* file: byte_cr.c */
void byte_copyr(to,n,from)
register char *to;
register unsigned int n;
register char *from;
{
  to += n;
  from += n;
  for (;;) {
    if (!n) return; *--to = *--from; --n;
    if (!n) return; *--to = *--from; --n;
    if (!n) return; *--to = *--from; --n;
    if (!n) return; *--to = *--from; --n;
  }
}

/* file: byte_diff.c */
int byte_diff(s,n,t)
register char *s;
register unsigned int n;
register char *t;
{
  for (;;) {
    if (!n) return 0; if (*s != *t) break; ++s; ++t; --n;
    if (!n) return 0; if (*s != *t) break; ++s; ++t; --n;
    if (!n) return 0; if (*s != *t) break; ++s; ++t; --n;
    if (!n) return 0; if (*s != *t) break; ++s; ++t; --n;
  }
  return ((int)(unsigned int)(unsigned char) *s)
       - ((int)(unsigned int)(unsigned char) *t);
}

/* file: byte_rchr.c */
unsigned int byte_rchr(s,n,c)
char *s;
register unsigned int n;
int c;
{
  register char ch;
  register char *t;
  register char *u;

  ch = c;
  t = s;
  u = 0;
  for (;;) {
    if (!n) break; if (*t == ch) u = t; ++t; --n;
    if (!n) break; if (*t == ch) u = t; ++t; --n;
    if (!n) break; if (*t == ch) u = t; ++t; --n;
    if (!n) break; if (*t == ch) u = t; ++t; --n;
  }
  if (!u) u = t;
  return u - s;
}

/* file: byte_zero.c */
void byte_zero(s,n)
char *s;
register unsigned int n;
{
  for (;;) {
    if (!n) break; *s++ = 0; --n;
    if (!n) break; *s++ = 0; --n;
    if (!n) break; *s++ = 0; --n;
    if (!n) break; *s++ = 0; --n;
  }
}
