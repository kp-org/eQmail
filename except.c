/*
 *  Revision 20180423, Kai Peter
 *  - switched to 'errmsg'
 *  Revision 20160711, Kai Peter
 *  - switched to 'buffer' (in 'strerr')
 *  Revision 20160503, Kai Peter
 *  - changed return type of main to int
 *  - added 'unistd.h' to prevent compiler warnings, removed 'exit.h'
 */
#include <unistd.h>
#include "errmsg.h"
#include "wait.h"

#define WHO "except"

int main(int argc,char **argv)
{
  int pid;
  int wstat;

  if (!argv[1])
    errint(ESOFT,"usage: except program [ arg ... ]");

  pid = fork();
  if (pid == -1)
    errint(EHARD,"unable to fork: ");
  if (pid == 0) {
    execvp(argv[1],argv + 1);
    if (errno) _exit(111);
    _exit(100);
  }

  if (wait_pid(&wstat,pid) == -1)
    errint(EHARD,"wait failed");
  if (wait_crashed(wstat))
    errint(EHARD,"child crashed");
  switch(wait_exitcode(wstat)) {
    case 0: _exit(100);
    case 111: errint(EHARD,"temporary child error");
    default: _exit(0);
  }
  return(0);  /* never reached */
}
