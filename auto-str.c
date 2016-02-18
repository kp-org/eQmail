#include "substdio.h"
#include "inc/readwrite.h"		/* the original definitions */
#include "exit.h"
/* The function puts(s) was renamed to o_puts(s) includes
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
  unsigned char ch;
  char octal[4];

  name = argv[1];
  if (!name) _exit(100);
  value = argv[2];
  if (!value) _exit(100);

  o_puts("char ");
  o_puts(name);
  o_puts("[] = \"\\\n");

  while (ch = *value++) {
    o_puts("\\");
    octal[3] = 0;
    octal[2] = '0' + (ch & 7); ch >>= 3;
    octal[1] = '0' + (ch & 7); ch >>= 3;
    octal[0] = '0' + (ch & 7);
    o_puts(octal);
  }

  o_puts("\\\n\";\n");
  if (substdio_flush(&ss1) == -1) _exit(111);
  _exit(0);
}
