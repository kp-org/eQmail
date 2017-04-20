#ifndef ALLOC_H
#define ALLOC_H

extern /*@null@*//*@out@*/char *alloc();
extern void alloc_free();
extern int alloc_re();

/* use these names in the future */
#define qfree alloc_free
#define qrealloc alloc_re

#endif
