/*
 *  Revision 20160711, Kai Peter
 *  - switched to 'buffer' (in 'strerr')
 *  Revision 20160503, Kai Peter
 *  - changed return type of main to int
 *  - added 'unistd.h' to prevent compiler warnings, removed 'exit.h'
 */
#include <unistd.h>
#include "strerr.h"
#include "wait.h"
#include "error.h"

#define FATL "except: fatal: "

int main(int argc,char **argv)
{
  int pid;
  int wstat;

  if (!argv[1])
    strerr_die1x(100,"except: usage: except program [ arg ... ]");

  pid = fork();
  if (pid == -1)
    strerr_die2sys(111,FATL,"unable to fork: ");
  if (pid == 0) {
    execvp(argv[1],argv + 1);
    if (error_temp(errno)) _exit(111);
    _exit(100);
  }

  if (wait_pid(&wstat,pid) == -1)
    strerr_die2x(111,FATL,"wait failed");
  if (wait_crashed(wstat))
    strerr_die2x(111,FATL,"child crashed");
  switch(wait_exitcode(wstat)) {
    case 0: _exit(100);
    case 111: strerr_die2x(111,FATL,"temporary child error");
    default: _exit(0);
  }
  return(0);  /* never reached */
}
