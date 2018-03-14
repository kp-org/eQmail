/*
 *  Revision 20180116, Kai Peter
 *  - changed ssout/ssin to bout/bin (substdio --> buffer)
 *  - switched to errmsg
 *  Revision 20160712, Kai Peter
 *  - switched to 'buffer'
 *  Revision 20160504, Kai Peter
 *  - changed return type of main to int
 */
#include <unistd.h>
#include "buffer.h"
#include "fd.h"
#include "getoptb.h"
#include "wait.h"
#include "env.h"
#include "sig.h"
#include "errmsg.h"

#define WHO "preline"

void die_usage()
{
  err_tmp_plus(EINVAL,"usage: preline cmd [ arg ... ]");
}

int flagufline = 1; char *ufline;
int flagrpline = 1; char *rpline;
int flagdtline = 1; char *dtline;

char outbuf[BUFFER_OUTSIZE];
char inbuf[BUFFER_INSIZE];
buffer bout = BUFFER_INIT(write,1,outbuf,sizeof outbuf);
buffer bin = BUFFER_INIT(read,0,inbuf,sizeof inbuf);

int main(int argc,char **argv)
{
  int opt;
  int pi[2];
  int pid;
  int wstat;

  sig_pipeignore();

  if (!(ufline = env_get("UFLINE"))) die_usage();	/* From_ line (UUCP style) */
  if (!(rpline = env_get("RPLINE"))) die_usage();	/* Return-Path line  */
  if (!(dtline = env_get("DTLINE"))) die_usage();	/* delivered-to line */

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
	err_sys_plus(EHARD,"unable to create pipe: ");

  pid = fork();
  if (pid == -1)
    err_sys_plus(EHARD,"unable to fork: ");

  if (pid == 0) {
    close(pi[1]);
    if (fd_move(0,pi[0]) == -1)
      err_sys_plus(EHARD,"unable to set up fds: ");
    sig_pipedefault();
    execvp(*argv,argv);
    err_sys_plus((errno),B("unable to run ",*argv,": "));
  }
  close(pi[0]);
  if (fd_move(1,pi[1]) == -1)
    err_sys_plus(EHARD,"unable to set up fds: ");

  if (flagufline) buffer_puts(&bout,ufline);
  if (flagrpline) buffer_puts(&bout,rpline);
  if (flagdtline) buffer_puts(&bout,dtline);
  if (buffer_copy(&bout,&bin) != 0)
    err_sys_plus(EHARD,"unable to copy input: ");
  buffer_flush(&bout);
  close(1);

  if (wait_pid(&wstat,pid) == -1)
    err_sys_plus(EHARD,"wait failed: ");
  if (wait_crashed(wstat))
    err_sys_plus(EHARD,"child crashed");
  _exit(wait_exitcode(wstat));
  return(0);  /* never reached */
}
