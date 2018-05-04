/*
 *  - switched to OS error constants
 *  Revision 20160509, Kai Peter
 *  - changed return type of main to int
 *  - added parentheses to outer if
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include "buffer.h"
#include "errmsg.h"
#include "byte.h"
#include "str.h"
#include "case.h"
#include "fmt.h"
#include "auto_usera.h"
#include "auto_break.h"
#include "qlx.h"

#define WHO "qmail-getpw"

#define GETPW_USERLEN 32

char *local;
struct passwd *pw;
char *dash;
char *extension;

int userext()
{
  char username[GETPW_USERLEN];
  struct stat st;

  extension = local + str_len(local);
  for (;;) {
    if (extension - local < sizeof(username))
      if (!*extension || (*extension == *auto_break)) {
    byte_copy(username,extension - local,local);
    username[extension - local] = 0;
    case_lowers(username);
    errno = 0;
    pw = getpwnam(username);
    if (errno == ETXTBSY) _exit(QLX_SYS);
    if (pw)
      if (pw->pw_uid)
        {
        if (stat(pw->pw_dir,&st) == 0) {
          if (st.st_uid == pw->pw_uid) {
        dash = "";

        if (*extension) { ++extension; dash = "-"; }
        return 1;
          }
        }
        else
//          if (error_temp(errno)) _exit(QLX_NFS);
          if (errno != ENOENT) errsys(errno); //_exit(QLX_NFS);
        }
    }
    if (extension == local) return 0;
    --extension;
  }
}

char num[FMT_ULONG];

int main(int argc,char **argv)
{
  local = argv[1];
  if (!local) _exit(100);

  if (!userext()) {
    extension = local;
    dash = "-";
    pw = getpwnam(auto_usera);
  }

  if (!pw) _exit(QLX_NOALIAS);

  buffer_puts(buffer_1,pw->pw_name);
  buffer_put(buffer_1,"",1);
  buffer_put(buffer_1,num,fmt_ulong(num,(long) pw->pw_uid));
  buffer_put(buffer_1,"",1);
  buffer_put(buffer_1,num,fmt_ulong(num,(long) pw->pw_gid));
  buffer_put(buffer_1,"",1);
  buffer_puts(buffer_1,pw->pw_dir);
  buffer_put(buffer_1,"",1);
  buffer_puts(buffer_1,dash);
  buffer_put(buffer_1,"",1);
  buffer_puts(buffer_1,extension);
  buffer_put(buffer_1,"",1);
  buffer_flush(buffer_1);

  _exit(0);
}
