/*
 *  Revision 20160712, Kai Peter
 *  - changed parameter declarations
 *  Revision 20160503, Kai Peter
 *  - changed return type of main to int
 *  - added 'unistd.h', 'sys/stat.h' to prevent compiler warnings,
 *    removed '#include "exit.h"
 */
#include "strerr.h"
#include <sys/stat.h>
#include <unistd.h>

#define FATL "maildirmake: fatal: "

int main(int argc,char **argv)
{
  umask(077);
  if (!argv[1])
    strerr_die1x(100,"maildirmake: usage: maildirmake name");
  if (mkdir(argv[1],0700) == -1)
    strerr_die4sys(111,FATL,"unable to mkdir ",argv[1],": ");
  if (chdir(argv[1]) == -1)
    strerr_die4sys(111,FATL,"unable to chdir to ",argv[1],": ");
  if (mkdir("tmp",0700) == -1)
    strerr_die4sys(111,FATL,"unable to mkdir ",argv[1],"/tmp: ");
  if (mkdir("new",0700) == -1)
    strerr_die4sys(111,FATL,"unable to mkdir ",argv[1],"/new: ");
  if (mkdir("cur",0700) == -1)
    strerr_die4sys(111,FATL,"unable to mkdir ",argv[1],"/cur: ");
  return(0);
}
