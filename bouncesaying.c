/*
 *  Revision 20160712, Kai Peter
 *  - switched to 'buffer' (strerr)
 *  - changed parameter declarations
 *  Revision 20160503, Kai Peter
 *  - changed return type of main to int
 *  - added 'unistd.h' to prevent compiler warnings, removed 'exit.h'
 */
#include <unistd.h>
#include "strerr.h"
#include "error.h"
#include "wait.h"
#include "sig.h"

#define FATAL "bouncesaying: fatal: "

int main(int argc,char **argv)
{
  int pid;
  int wstat;
  int e;

  if (!argv[1])
    strerr_die1x(100,"bouncesaying: usage: bouncesaying error [ program [ arg ... ] ]");

  if (argv[2]) {
    pid = fork();
    if (pid == -1)
      strerr_die2sys(111,FATAL,"unable to fork: ");
    if (pid == 0) {
      execvp(argv[2],argv + 2);
      if (error_temp(errno)) _exit(111);
//      if (error_str(errno)) _exit(111);
      _exit(100);
    }
    if (wait_pid(&wstat,pid) == -1)
      strerr_die2x(111,FATAL,"wait failed");
    if (wait_crashed(wstat))
      strerr_die2x(111,FATAL,"child crashed");
    switch(wait_exitcode(wstat)) {
      case 0: break;
      case 111: strerr_die2x(111,FATAL,"temporary child error");
      default: _exit(0);
    }
  }

  strerr_die1x(100,argv[1]);
  return(0);  /* never reached */
}
