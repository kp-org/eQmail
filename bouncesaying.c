/*
 *  Revision 20170428, Kai Peter
 *  - switched to 'errmsg', removed 'strerr'
 *  Revision 20160712, Kai Peter
 *  - switched to 'buffer' (strerr)
 *  - changed parameter declarations
 *  Revision 20160503, Kai Peter
 *  - changed return type of main to int
 *  - added 'unistd.h' to prevent compiler warnings, removed 'exit.h'
 */
#include <unistd.h>
#include "wait.h"
#include "sig.h"
#include "error.h"
#include "buffer.h"

#define WHO "bouncesaying: "

int main(int argc,char **argv)
{
  int pid;
  int wstat;

  if (!argv[1])
      err_tmp_plus(EINVAL,"usage: bouncesaying error [ program [ arg ... ] ]");

  if (argv[2]) {
    pid = fork();
    if (pid == -1)
      err_sys_plus(EHARD,"unable to fork");
    if (pid == 0) {
      execvp(argv[2],argv + 2);
//      if (errno) _exit(111);
      if (errno) err_sys_plus(EHARD,errstr(errno));
      _exit(100);
//      err_sys(ESOFT);
    }
    if (wait_pid(&wstat,pid) == -1)
      err_sys_plus(EHARD,"wait failed");
    if (wait_crashed(wstat))
      err_sys_plus(EHARD,"child crashed");
    switch(wait_exitcode(wstat)) {
      case 0: break;
      case 111: err_tmp_plus(ESOFT,"temporary child error");
      case EHARD: err_tmp_plus(ESOFT,"temporary child error");
      default: _exit(0);
    }
  }

  buffer_puts(buffer_1,argv[1]);
  buffer_puts(buffer_1,"\n");
  buffer_flush(buffer_1);
  _exit(100);
  err_sys(ESOFT);
}
