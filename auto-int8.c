/*
 *  Revision 20160503, Kai Peter
 *  - renamed function puts(s) to qputs(s), because puts()
 *    is a POSIX function ('q' is as of 'q'mail)
 *  - changed return type of main to int
 */
#include "substdio.h"
#include "readwrite.h"     /* the original definitions */
#include "exit.h"
#include "scan.h"
#include "fmt.h"

char buf1[256];
substdio ss1 = SUBSTDIO_FDBUF(write,1,buf1,sizeof(buf1));

void qputs(s)
char *s;
{
  if (substdio_puts(&ss1,s) == -1) _exit(111);
}

int main(argc,argv)
int argc;
char **argv;
{
  char *name;
  char *value;
  unsigned long num;
  char strnum[FMT_ULONG];

  name = argv[1];
  if (!name) _exit(100);
  value = argv[2];
  if (!value) _exit(100);

  scan_8long(value,&num);
  strnum[fmt_ulong(strnum,num)] = 0;

  qputs("int ");
  qputs(name);
  qputs(" = ");
  qputs(strnum);
  qputs(";\n");
  if (substdio_flush(&ss1) == -1) _exit(111);
  _exit(0);
}
