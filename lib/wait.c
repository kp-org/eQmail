#include <sys/types.h>
#include <sys/wait.h>
//#include "haswaitp.h"
#include "error.h"
/* Consolidate  the "wait_*.c" functions into one source
   file.  Original files were of date 19980615,  shipped
   with  qmail-1.03.
   The "wait" functions will be linked to "wait.a" only!
   Included files:   Size (bytes)   Date:
     - wait_nohang.c         225    19980615
     - wait_pid.c            709    19980615          */
/* 20160405: removed HASWAITPID code (Kai Peter)      */

/* file: wait_nohang.c */
int wait_nohang(wstat) int *wstat;
{
//#ifdef HASWAITPID
  return waitpid(-1,wstat,WNOHANG);
//#else
//  return wait3(wstat,WNOHANG,(struct rusage *) 0);
//#endif
}

/* file: wait_pid.c */
//#ifdef HASWAITPID
int wait_pid(wstat,pid) int *wstat; int pid;
{
  int r;

  do
    r = waitpid(pid,wstat,0);
  while ((r == -1) && (errno == error_intr));
  return r;
}
//#else
/* XXX untested */
/* XXX breaks down with more than two children */
//static int oldpid = 0;
//static int oldwstat; /* defined if(oldpid) */

//int wait_pid(wstat,pid) int *wstat; int pid;
//{
//  int r;

//  if (pid == oldpid) { *wstat = oldwstat; oldpid = 0; return pid; }

//  do {
//    r = wait(wstat);
//    if ((r != pid) && (r != -1)) { oldwstat = *wstat; oldpid = r; continue; }
//  }
//  while ((r == -1) && (errno == error_intr));
//  return r;
//}
//#endif
