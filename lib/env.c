/* env.c: environ library original by Daniel J. Bernstein
Depends on str.h, alloc.h.
Requires environ.
*/

/*
 * 20160630, new lib
 * - consolidated envread.c, env.c from qmail-1.03
 * - updated function 'env_get' to version of ucspi-tcp-0.88
 * - commented out 'env_pick' and 'env_clear'
 * - renamed 'env_goodbye' to 'env_del'
 * - renamed 'env_add' to 'env_set'
 * - renamed 'env_put' to 'env_puts'
 * - renamed 'env_put2' to 'env_put'
*/
#include "str.h"
#include "alloc.h"
//#include <stdlib.h>
#include "env.h"


extern /*@null@*/char *env_get(char *s)
{
  int i;
  unsigned int len;

  if(!s) return(0);
  len = str_len(s);
  for (i = 0;environ[i];++i)
    if (str_start(environ[i],s) && (environ[i][len] == '='))
      return environ[i] + len + 1;
  return 0;
}

/* find the '=' (equal sign) in an env variable string */
// used by env_add below only
extern char *env_findeq(char *s) {
  for (;*s;++s)
    if (*s == '=')
      return s;
  return 0;
}

// file: env.c from qmail 
int env_isinit = 0; /* if env_isinit: */
static int ea; /* environ is a pointer to ea+1 char*'s. */
static int en; /* the first en of those are ALLOCATED. environ[en] is 0. */

/* remove an env var */
static void env_del(int i)
{
  alloc_free(environ[i]);
  environ[i] = environ[--en];
  environ[en] = 0;
}

/* used by env_unset and env_set below */
static void env_unsetlen(char *s,int len)
{
  int i;
  for (i = en - 1;i >= 0;--i)
    if (!str_diffn(s,environ[i],len))
      if (environ[i][len] == '=')
        env_del(i);
}

int env_unset(s) char *s;
{
  if (!env_isinit) if (!env_init()) return 0;
  env_unsetlen(s,str_len(s));
  return 1;
}

/* set the (new) value of an existing env variable */
// used by env_put{2}
// this was formerly 'env_add'
// similar to envstr_set of bglibs (?)
static int env_set(char *s)
{
  char *t;
  t = env_findeq(s);
  if (t) env_unsetlen(s,t - s);
  if (en == ea)
  {
    ea += 30;
    if (!alloc_re(&environ,(en + 1) * sizeof(char *),(ea + 1) * sizeof(char *)))
      { ea = en; return 0; }
  }
  environ[en++] = s;
  environ[en] = 0;
  return 1;
}

/* Put var into the environment as one string, format: "NAME=value". */
int env_puts(char *s)
{
  char *u;

  if (!env_isinit) if (!env_init()) return 0;
  u = alloc(str_len(s) + 1);
  if (!u) return 0;
  str_copy(u,s);
  if (!env_set(u)) { alloc_free(u); return 0; }
  return 1;
}

/* Put the value of an env var with format NAME=value into the environment. */
int env_put(char *name,char *value)
{
  char *ln;
  int len;

  if (!env_isinit) if (!env_init()) return 0;
  len = str_len(name);
  ln = alloc(len + str_len(value) + 2);
  if (!ln) return 0;
  str_copy(ln,name);
  ln[len] = '=';
  str_copy(ln + len + 1,value);
  if (!env_set(ln)) { alloc_free(ln); return 0; }
  return 1;
}

/* check if an env var can be set overall (?),
   returns 1 on success, overwise 0 (false) */
int env_init()
{
  char **newenviron;
  int i;
  for (en = 0;environ[en];++en)
  ;
    ea = en + 10;
    newenviron = (char **) alloc((ea + 1) * sizeof(char *));
    if (!newenviron) return 0;
    for (en = 0;environ[en];++en)
    {
      newenviron[en] = alloc(str_len(environ[en]) + 1);
      if (!newenviron[en])
        {
          for (i = 0;i < en;++i) alloc_free(newenviron[i]);
          alloc_free(newenviron);
          return 0;
        }
      str_copy(newenviron[en],environ[en]);
    }
  newenviron[en] = 0;
  environ = newenviron;
  env_isinit = 1;
  return 1;
}

/* Next 2 function will not be used by eQmail */
/* clear the whole environ(ment) */
static char *null = 0;
void env_clear() {
  if (env_isinit) while (en) env_del(0);
  else environ = &null;
}
/* get all env vars from environ(ment) */
extern char *env_pick() { return environ[0]; }
