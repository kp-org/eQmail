/*
 *  Revision 20170317, Kai Peter
 *  - switched to 'buffer'
 *  - added 'rename.h'
 *  Revision 20170306, Kai Peter
 *  - updated to new(er) cdb lib from ucspi-tcp-0.88
 *  - changed return type of main to int
 *  - added '<sys/stat.h> to prevent compiler warnings
 */
#include <unistd.h>
#include <sys/stat.h>
#include "getln.h"
#include "strerr.h"
#include "stralloc.h"
#include "open.h"
#include "auto_qmail.h"
#include "cdbmake.h"
#include "case.h"
#include "rename.h"

#define FATL "qmail-newmrh: fatal: "

void die_read()
{
  strerr_die2sys(111,FATL,"unable to read control/morercpthosts: ");
}
void die_write()
{
  strerr_die2sys(111,FATL,"unable to write to control/morercpthosts.tmp: ");
}

char inbuf[1024];
buffer ssin;

int fd;
int fdtemp;

struct cdb_make c;
stralloc line = {0};
int match;

int main()
{
  umask(033);
  if (chdir(auto_qmail) == -1)
    strerr_die4sys(111,FAAL,"unable to chdir to ",auto_qmail,": ");

  fd = open_read("control/morercpthosts");
  if (fd == -1) die_read();

  buffer_init(&ssin,read,fd,inbuf,sizeof inbuf);

  fdtemp = open_trunc("control/morercpthosts.tmp");
  if (fdtemp == -1) die_write();

  if (cdb_make_start(&c,fdtemp) == -1) die_write();

  for (;;) {
  if (getln(&ssin,&line,&match,'\n') != 0) die_read();
    case_lowerb(line.s,line.len);
    while (line.len) {
      if (line.s[line.len - 1] == ' ') { --line.len; continue; }
      if (line.s[line.len - 1] == '\n') { --line.len; continue; }
      if (line.s[line.len - 1] == '\t') { --line.len; continue; }
      if (line.s[0] != '#')
  if (cdb_make_add(&c,line.s,line.len,"",0) == -1)
      die_write();
      break;
    }
    if (!match) break;
  }

  if (cdb_make_finish(&c) == -1) die_write();
  if (fsync(fdtemp) == -1) die_write();
  if (close(fdtemp) == -1) die_write(); /* NFS stupidity */
  if (rename("control/morercpthosts.tmp","control/morercpthosts.cdb") == -1)
    strerr_die2sys(111,FATL,"unable to move control/morercpthosts.tmp to control/morercpthosts.cdb");

  _exit(0);
}
