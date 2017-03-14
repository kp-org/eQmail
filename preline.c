/*
 *  Revision 20160712, Kai Peter
 *  - switched to 'buffer'
 *  Revision 20160504, Kai Peter
 *  - changed return type of main to int
 */
#include "fd.h"
#include <unistd.h>
#include "strerr.h"
#include "buffer.h"
#include "getoptb.h"
#include "wait.h"
#include "env.h"
#include "sig.h"
#include "error.h"

#define FATAL "preline: fatal: "

void die_usage()
{
  strerr_die1x(100,"preline: usage: preline cmd [ arg ... ]");
}

int flagufline = 1; char *ufline;
int flagrpline = 1; char *rpline;
int flagdtline = 1; char *dtline;

char outbuf[BUFFER_OUTSIZE];
char inbuf[BUFFER_INSIZE];
buffer ssout = BUFFER_INIT(write,1,outbuf,sizeof outbuf);
buffer ssin = BUFFER_INIT(read,0,inbuf,sizeof inbuf);

int main(int argc,char **argv)
{
  int opt;
  int pi[2];
  int pid;
  int wstat;

  sig_pipeignore();

  if (!(ufline = env_get("UFLINE"))) die_usage();
  if (!(rpline = env_get("RPLINE"))) die_usage();
  if (!(dtline = env_get("DTLINE"))) die_usage();

  while ((opt = getopt(argc,argv,"frdFRD")) != opteof)
    switch(opt) {
      case 'f': flagufline = 0; break;
      case 'r': flagrpline = 0; break;
      case 'd': flagdtline = 0; break;
      case 'F': flagufline = 1; break;
      case 'R': flagrpline = 1; break;
      case 'D': flagdtline = 1; break;
      default: die_usage();
    }
  argc -= optind;
  argv += optind;
  if (!*argv) die_usage();

  if (pipe(pi) == -1)
    strerr_die2sys(111,FATAL,"unable to create pipe: ");

  pid = fork();
  if (pid == -1)
    strerr_die2sys(111,FATAL,"unable to fork: ");

  if (pid == 0) {
    close(pi[1]);
    if (fd_move(0,pi[0]) == -1)
      strerr_die2sys(111,FATAL,"unable to set up fds: ");
    sig_pipedefault();
    execvp(*argv,argv);
    strerr_die4sys(error_temp(errno) ? 111 : 100,FATAL,"unable to run ",*argv,": ");
  }
  close(pi[0]);
  if (fd_move(1,pi[1]) == -1)
    strerr_die2sys(111,FATAL,"unable to set up fds: ");

  if (flagufline) buffer_puts(&ssout,ufline);
  if (flagrpline) buffer_puts(&ssout,rpline);
  if (flagdtline) buffer_puts(&ssout,dtline);
  if (buffer_copy(&ssout,&ssin) != 0)
    strerr_die2sys(111,FATAL,"unable to copy input: ");
  buffer_flush(&ssout);
  close(1);

  if (wait_pid(&wstat,pid) == -1)
    strerr_die2sys(111,FATAL,"wait failed: ");
  if (wait_crashed(wstat))
    strerr_die2x(111,FATAL,"child crashed");
  _exit(wait_exitcode(wstat));
  return(0);  /* never reached */
}
