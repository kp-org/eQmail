/*
 *  Revision 20160729, Kai Peter
 *  - new file: consolidated 'pathexec_env.c', 'pathexec_run.c'
*/
#include <unistd.h>
#include "error.h"
#include "stralloc.h"
#include <alloc.h>
#include "str.h"
#include "byte.h"
#include "env.h"
#include "pathexec.h"

static stralloc plus;
static stralloc tmp;

/* file: pathexec_env.c ********************************************* */
int pathexec_env(const char *s,const char *t)
{
  if (!s) return 1;
  if (!stralloc_copys(&tmp,s)) return 0;
  if (t) {
    if (!stralloc_cats(&tmp,"=")) return 0;
    if (!stralloc_cats(&tmp,t)) return 0;
  }
  if (!stralloc_0(&tmp)) return 0;
  return stralloc_cat(&plus,&tmp);
}

void pathexec(char **argv)
{
  char **e;
  unsigned int elen;
  unsigned int i;
  unsigned int j;
  unsigned int split;
  unsigned int t;

  if (!stralloc_cats(&plus,"")) return;

  elen = 0;
  for (i = 0;environ[i];++i)
    ++elen;
  for (i = 0;i < plus.len;++i)
    if (!plus.s[i])
      ++elen;

  e = (char **) alloc((elen + 1) * sizeof(char *));
  if (!e) return;

  elen = 0;
  for (i = 0;environ[i];++i)
    e[elen++] = environ[i];

  j = 0;
  for (i = 0;i < plus.len;++i)
    if (!plus.s[i]) {
      split = str_chr(plus.s + j,'=');
      for (t = 0;t < elen;++t)
    if (byte_equal(plus.s + j,split,e[t]))
      if (e[t][split] == '=') {
        --elen;
        e[t] = e[elen];
        break;
      }
      if (plus.s[j + split])
    e[elen++] = plus.s + j;
      j = i + 1;
    }
  e[elen] = 0;

  pathexec_run(*argv,argv,e);
  alloc_free(e);
}

/* file: pathexec_run.c ********************************************* */
void pathexec_run(char *file,char **argv,char **envp)
{
  char *path;
  unsigned int split;
  int savederrno;

  if (file[str_chr(file,'/')]) {
    execve(file,argv,envp);
    return;
  }

  path = env_get("PATH");
  if (!path) path = "/bin:/usr/bin";

  savederrno = 0;
  for (;;) {
    split = str_chr(path,':');
    if (!stralloc_copyb(&tmp,path,split)) return;
    if (!split)
      if (!stralloc_cats(&tmp,".")) return;
    if (!stralloc_cats(&tmp,"/"))  return;
    if (!stralloc_cats(&tmp,file)) return;
    if (!stralloc_0(&tmp)) return;

    execve(tmp.s,argv,envp);
    if (errno != error_noent) {
      savederrno = errno;
      if ((errno != error_acces) && (errno != error_perm) && (errno != error_isdir)) return;
    }

    if (!path[split]) {
      if (savederrno) errno = savederrno;
      return;
    }
    path += split;
    path += 1;
  }
}
