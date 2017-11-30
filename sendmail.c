/*
 *  Revision 20160713, Kai Peter
 *  - switched to 'buffer'
 *  Revision 20160504, Kai Peter
 *  - changed return type of main to int
 *  - added '<unistd.h>' to prevent compiler warnings
 */
#include <unistd.h>
#include "getoptb.h"
#include "buffer.h"
#include "alloc.h"
#include "auto_qmail.h"
#include "env.h"
#include "str.h"

void nomem()
{
  buffer_putsflush(buffer_2,"sendmail: fatal: out of memory\n");
  _exit(111);
}

void die_usage()
{
  buffer_putsflush(buffer_2,"sendmail: usage: sendmail [ -t ] [ -fsender ] [ -Fname ] [ -bp ] [ -bs ] [ arg ... ]\n");
  _exit(100);
}

char *smtpdarg[] = { "bin/qmail-smtpd", 0 };
void smtpd()
{
  if (!env_get("PROTO")) {
    if (!env_puts("RELAYCLIENT=")) nomem();
    if (!env_puts("DATABYTES=0")) nomem();
    if (!env_puts("PROTO=TCP")) nomem();
    if (!env_puts("TCPLOCALIP=127.0.0.1")) nomem();
    if (!env_puts("TCPLOCALHOST=localhost")) nomem();
    if (!env_puts("TCPREMOTEIP=127.0.0.1")) nomem();
    if (!env_puts("TCPREMOTEHOST=localhost")) nomem();
    if (!env_puts("TCPREMOTEINFO=sendmail-bs")) nomem();
  }
  execv(*smtpdarg,smtpdarg);
  buffer_putsflush(buffer_2,"sendmail: fatal: unable to run qmail-smtpd\n");
  _exit(111);
}

char *qreadarg[] = { "bin/qmail-qread", 0 };
void mailq()
{
  execv(*qreadarg,qreadarg);
  buffer_putsflush(buffer_0,"sendmail: fatal: unable to run qmail-qread\n");
  _exit(111);
}

//void do_sender(const char *s)
void do_sender(char *s)
{
  char *x;
  int n;
  int a;
  int i;

  env_unset("QMAILNAME");
  env_unset("MAILNAME");
  env_unset("NAME");
  env_unset("QMAILHOST");
  env_unset("MAILHOST");

  n = str_len(s);
  a = str_rchr(s, '@');
  if (a == n)
  {
    env_put("QMAILUSER", s);
    return;
  }
  env_put("QMAILHOST",s + a + 1);

  x = (char *) alloc((a + 1) * sizeof(char));
  if (!x) nomem();
  for (i = 0; i < a; i++)
    x[i] = s[i];
  x[i] = 0;
  env_put("QMAILUSER", x);
  alloc_free(x);
}

int flagh;
char *sender;

int main(int argc,char **argv)
{
  int opt;
  char **qiargv;
  char **arg;
  int i;

  if (chdir(auto_qmail) == -1) {
    buffer_putsflush(buffer_2,"sendmail: fatal: unable to switch to qmail home directory\n");
    _exit(111);
  }

  flagh = 0;
  sender = 0;
  while ((opt = getopt(argc,argv,"vimte:f:p:o:B:F:EJxb:")) != opteof)
    switch(opt) {
      case 'B': break;
      case 't': flagh = 1; break;
      case 'f': sender = optarg; break;
      case 'F': if (!env_put("MAILNAME",optarg)) nomem(); break;
      case 'p': break; /* could generate a Received line from optarg */
      case 'v': break;
      case 'i': break; /* what an absurd concept */
      case 'x': break; /* SVR4 stupidity */
      case 'm': break; /* twisted-paper-path blindness, incompetent design */
      case 'e': break; /* qmail has only one error mode */
      case 'o':
        switch(optarg[0]) {
      case 'd': break; /* qmail has only one delivery mode */
      case 'e': break; /* see 'e' above */
      case 'i': break; /* see 'i' above */
      case 'm': break; /* see 'm' above */
    }
        break;
      case 'E': case 'J': /* Sony NEWS-OS */
        while (argv[optind][optpos]) ++optpos; /* skip optional argument */
        break;
      case 'b':
    switch(optarg[0]) {
      case 'm': break;
      case 'p': mailq();
      case 's': smtpd();
      default: die_usage();
    }
    break;
      default:
    die_usage();
    }
  argc -= optind;
  argv += optind;

  if (str_equal(optprogname,"mailq"))
    mailq();

  if (str_equal(optprogname,"newaliases")) {
    buffer_putsflush(buffer_2,"sendmail: fatal: please use fastforward/newaliases instead\n");
    _exit(100);
  }

  qiargv = (char **) alloc((argc + 10) * sizeof(char *));
  if (!qiargv) nomem();

  arg = qiargv;
  *arg++ = "bin/qmail-inject";
  *arg++ = (flagh ? "-H" : "-a");
  if (sender) {
    *arg++ = "-f";
    *arg++ = sender;
    do_sender(sender);
  }
  *arg++ = "--";
  for (i = 0;i < argc;++i) *arg++ = argv[i];
  *arg = 0;

  execv(*qiargv,qiargv);
  buffer_putsflush(buffer_2,"sendmail: fatal: unable to run qmail-inject\n");
  _exit(111);
//  return(0);  /* never reached */
}
