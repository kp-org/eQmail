/*
 *  Revision 20160906, Kai Peter
 *  - code cleanup, added comments
 *  Revision 20160523, Kai Peter
 *  - added <error.h> and changed function die() to use
 *    'errno' instead of static '_exit(111)' always
 *  Revision 20160509, Kai Peter
 *  - changed return type of main to int
 *  - added '<unistd.h>', 'sys/stat.h>'
 */
#include <unistd.h>
#include <sys/stat.h>
#include <error.h>
#include "fd.h"
#include "prot.h"
#include "auto_uids.h"
/* @@ */

char *(qsargs[]) = { "qmail-send", 0 };
char *(qcargs[]) = { "qmail-clean", 0 };
char *(qlargs[]) = { "qmail-lspawn", "./Mailbox", 0 };
char *(qrargs[]) = { "qmail-rspawn", 0 };

void die(unsigned int e) { _exit(e); }

int pi0[2];
int pi1[2];
int pi2[2];
int pi3[2];
int pi4[2];
int pi5[2];
int pi6[2];

void close23456() { close(2); close(3); close(4); close(5); close(6); }

void closepipes() {
  close(pi1[0]); close(pi1[1]); close(pi2[0]); close(pi2[1]);
  close(pi3[0]); close(pi3[1]); close(pi4[0]); close(pi4[1]);
  close(pi5[0]); close(pi5[1]); close(pi6[0]); close(pi6[1]);
}

int main(int argc,char **argv)
{
  if (chdir("/") == -1) die(errno);
  umask(077);
  if (prot_gid(auto_gidq) == -1) die(errno);

  if (fd_copy(2,0) == -1) die(errno);
  if (fd_copy(3,0) == -1) die(errno);
  if (fd_copy(4,0) == -1) die(errno);
  if (fd_copy(5,0) == -1) die(errno);
  if (fd_copy(6,0) == -1) die(errno);

  if (argv[1]) {
    qlargs[1] = argv[1];
    ++argv;
  }

  if (argv[1]) {
    if (pipe(pi0) == -1) die(errno);
    switch(fork()) {
      case -1: die(errno);
      case 0:
        if (prot_gid(auto_gidn) == -1) die(errno);
        if (prot_uid(auto_uidl) == -1) die(errno);
        close(pi0[1]);
        if (fd_move(0,pi0[0]) == -1) die(errno);
        close23456();
        execvp(argv[1],argv + 1);
    die(errno);
    }
    close(pi0[0]);
    if (fd_move(1,pi0[1]) == -1) die(errno);
  }

  if (pipe(pi1) == -1) die(errno);
  if (pipe(pi2) == -1) die(errno);
  if (pipe(pi3) == -1) die(errno);
  if (pipe(pi4) == -1) die(errno);
  if (pipe(pi5) == -1) die(errno);
  if (pipe(pi6) == -1) die(errno);

  /* start qmail-lspawn */
  switch(fork()) {
    case -1: die(errno);
    case 0:
      if (fd_copy(0,pi1[0]) == -1) die(errno);
      if (fd_copy(1,pi2[1]) == -1) die(errno);
      close23456();
      closepipes();
      execvp(*qlargs,qlargs);
      die(errno);
  }

  /* start qmail-rspawn */
  switch(fork()) {
    case -1: die(errno);
    case 0:
      if (prot_uid(auto_uidr) == -1) die(errno);
      if (fd_copy(0,pi3[0]) == -1) die(errno);
      if (fd_copy(1,pi4[1]) == -1) die(errno);
      close23456();
      closepipes();
      execvp(*qrargs,qrargs);
      die(errno);
  }

  /* start qmail-clean */
  switch(fork()) {
    case -1: die(errno);
    case 0:
      if (prot_uid(auto_uidq) == -1) die(errno);
      if (fd_copy(0,pi5[0]) == -1) die(errno);
      if (fd_copy(1,pi6[1]) == -1) die(errno);
      close23456();
      closepipes();
      execvp(*qcargs,qcargs);
      die(errno);
  }

  if (prot_uid(auto_uids) == -1) die(errno);
  if (fd_copy(0,1) == -1) die(errno);
  if (fd_copy(1,pi1[1]) == -1) die(errno);
  if (fd_copy(2,pi2[0]) == -1) die(errno);
  if (fd_copy(3,pi3[1]) == -1) die(errno);
  if (fd_copy(4,pi4[0]) == -1) die(errno);
  if (fd_copy(5,pi5[1]) == -1) die(errno);
  if (fd_copy(6,pi6[0]) == -1) die(errno);
  closepipes();
  execvp(*qsargs,qsargs);  /* start qmail-send */
  die(errno);
  return(0);  /* never reached */
}
