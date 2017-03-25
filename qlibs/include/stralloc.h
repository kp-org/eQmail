#ifndef STRALLOC_H
#define STRALLOC_H

//#include "gen_alloc.h"
/* file: gen_alloc.h */
#define GEN_ALLOC_typedef(ta,type,field,len,a) \
  typedef struct ta { type *field; unsigned int len; unsigned int a; } ta;
GEN_ALLOC_typedef(stralloc,char,s,len,a)

//#include "gen_allocdefs.h"

extern int stralloc_ready(stralloc *,unsigned int);
extern int stralloc_readyplus(stralloc *,unsigned int);
extern int stralloc_copy(stralloc *,stralloc *);
extern int stralloc_cat(stralloc *,stralloc *);
extern int stralloc_copys(stralloc *,const char *);
extern int stralloc_cats(stralloc *,const char *);
extern int stralloc_copyb(stralloc *,const char *,unsigned int);
extern int stralloc_catb(stralloc *,const char *,unsigned int);
extern int stralloc_append(stralloc *,char *); /* beware: this takes a pointer to 1 char */
extern int stralloc_starts(stralloc *,const char *);

#define stralloc_0(sa) stralloc_append(sa,"")

extern int stralloc_catulong0(stralloc *,unsigned long,unsigned int);
extern int stralloc_catlong0(stralloc *,long,unsigned int);

extern void stralloc_free(stralloc *);

#define stralloc_catlong(sa,l) (stralloc_catlong0((sa),(l),0))
#define stralloc_catuint0(sa,i,n) (stralloc_catulong0((sa),(i),(n)))
#define stralloc_catint0(sa,i,n) (stralloc_catlong0((sa),(i),(n)))
#define stralloc_catint(sa,i) (stralloc_catlong0((sa),(i),0))

/* file: gen_allocdefs.h (--> @Kai: IMHO libowfat has better code) */
#define GEN_ALLOC_ready(ta,type,field,len,a,i,n,x,base,ta_ready) \
int ta_ready(x,n) register ta *x; register unsigned int n; \
{ register unsigned int i; \
  if (x->field) { \
    i = x->a; \
    if (n > i) { \
      x->a = base + n + (n >> 3); \
      if (alloc_re(&x->field,i * sizeof(type),x->a * sizeof(type))) return 1; \
      x->a = i; return 0; } \
    return 1; } \
  x->len = 0; \
  return !!(x->field = (type *) alloc((x->a = n) * sizeof(type))); }

#define GEN_ALLOC_readyplus(ta,type,field,len,a,i,n,x,base,ta_rplus) \
int ta_rplus(x,n) register ta *x; register unsigned int n; \
{ register unsigned int i; \
  if (x->field) { \
    i = x->a; n += x->len; \
    if (n > i) { \
      x->a = base + n + (n >> 3); \
      if (alloc_re(&x->field,i * sizeof(type),x->a * sizeof(type))) return 1; \
      x->a = i; return 0; } \
    return 1; } \
  x->len = 0; \
  return !!(x->field = (type *) alloc((x->a = n) * sizeof(type))); }

#define GEN_ALLOC_append(ta,type,field,len,a,i,n,x,base,ta_rplus,ta_append) \
int ta_append(x,i) register ta *x; register type *i; \
{ if (!ta_rplus(x,1)) return 0; x->field[x->len++] = *i; return 1; }

#endif
