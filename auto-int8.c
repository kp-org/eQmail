#include "substdio.h"
#include "readwrite.h"		/* the original definitions */
#include "exit.h"
#include "scan.h"
#include "fmt.h"
/* The function puts(s) was renamed to o_puts(s), include
   call of it (o_put stands for original). Reason is that
   puts() is a function in POSIX standard libraries.   */

char buf1[256];
substdio ss1 = SUBSTDIO_FDBUF(write,1,buf1,sizeof(buf1));

void o_puts(s)
char *s;
{
  if (substdio_puts(&ss1,s) == -1) _exit(111);
}

void main(argc,argv)
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

  o_puts("int ");
  o_puts(name);
  o_puts(" = ");
  o_puts(strnum);
  o_puts(";\n");
  if (substdio_flush(&ss1) == -1) _exit(111);
  _exit(0);
}
