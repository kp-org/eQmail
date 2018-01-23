/*
 *  Revision 20180123, Kai Peter
 *  - prevented 'misleading-indentation' warning
 *  Revision 20170515, Kai Peter
 *  - switched to stralloc (instead of <string.h>)
 *  Revision 20160712, Kai Peter
 *  - switched to 'buffer'
 *  Revision 20160608, Kai Peter
 *  - replaced 'vfork' with 'fork'
*/
#include <unistd.h>
#include <sys/stat.h>
#include "fd.h"
#include "wait.h"
#include "buffer.h"
#include "error.h"
#include "ipalloc.h"
#include "tcpto.h"
#include "str.h"
#include "auto_qmail.h"

#define WHO "qmail-rspawn: "

void initialize(int argc,char **argv) { tcpto_clean(); }
int truncreport = 0;
char *np = "qmail-bfrmt";   /* next program to call */

void report(buffer *b,int wstat,char *s,int len)
{
  int j;
  int k;
  int result;
  int orr;

  if (wait_crashed(wstat))
  { buffer_puts(b,"Z");
    buffer_puts(b,np);
    buffer_puts(b," crashed.\n"); return; }

  switch(wait_exitcode(wstat))
  {
    case 0: break;
    case 111: buffer_puts(b,"Z111_Unable to run ");
              buffer_puts(b,np);
              buffer_puts(b,".\n"); return;
    case 112: buffer_puts(b,"Z112_Unable to run ");
              buffer_puts(b,np);
              buffer_puts(b,".\n"); return;
// test: suppress bounces
    case 113: buffer_puts(b,"Z113_Stopped bounces ");
              //buffer_puts(b,np);
              buffer_puts(b,".\n"); return;

   default:   buffer_puts(b,"D000_Unable to run ");
              buffer_puts(b,np);
              buffer_puts(b,".\n"); return;
  }
  if (!len)
  { buffer_puts(b,"Z");
    buffer_puts(b,np);
    buffer_puts(b," produced no output.\n"); return; }

  result = -1;
  j = 0;
  for (k = 0;k < len;++k)
  {
    if (!s[k])
    {
      if (s[j] == 'K') { result = 1; break; }
      if (s[j] == 'Z') { result = 0; break; }
      if (s[j] == 'D') break;
      j = k + 1;
    }
  }

  orr = result;
  switch(s[0]) {
    case 's': orr = 0; break;
    case 'h': orr = -1;
  }

  switch(orr) {
    case  1: buffer_put(b,"K",1); break;
    case  0: buffer_put(b,"Z",1); break;
    case -1: buffer_put(b,"D",1); break;
  }

  for (k = 1;k < len;)
    if (!s[k++])
    {
      buffer_puts(b,s + 1);
      if (result <= orr)
        if (k < len)
          switch(s[k])
          {
            case 'Z': case 'D': case 'K':
              buffer_puts(b,s + k + 1);
          }
      break;
    }
}

int spawn(int fdmess,int fdout,char *s,char *r,int at)
{
  struct stat st;
  stralloc sa = {0};

  if(!stralloc_copys(&sa,auto_qmail)) err_sys(errno);
  if(!stralloc_catb(&sa,"/bin/qmail-bfrmt",16)) err_sys(errno); /* len: 16 + \0 */
  if(!stralloc_0(&sa)) err_sys(errno);
  char *x = sa.s;
  if (stat(x,&st) != 0) { np = "qmail-remote"; }

  int f;
  char *(args[5]);
  args[0] = np;
  args[1] = r + at + 1;
  args[2] = s;
  args[3] = r;
  args[4] = 0;

  if (!(f = fork()))
  {
    if (fd_move(0,fdmess) == -1) err_sys(errno);
    if (fd_move(1,fdout) == -1) err_sys(errno);
    if (fd_copy(2,1) == -1) err_sys(errno);
    execvp(*args,args);
    if (errno) err_sys(errno);
    _exit(100);  /* TODO: check exit code */
  }
  return f;
}
