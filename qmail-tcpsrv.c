/*
 *  Revision 20180412, Kai Peter
 *  - replaced 'strerr' by 'errmsg'
 *  - replaced 'forcev6' and 'fakev4' by 'ipv4socket'
 *  - removed "TCP6*" env vars
 *  - added function 'socket_ipoptionskill'
 *  Revision 20180131, Kai Peter
 *  - changed call of function 'dns_name6()' --> 'dns_name()
 *  Revision 20180119, Kai Peter
 *  - prevent 'misleading-indentation' warning
*/
#include <sys/types.h>
#include <sys/param.h>
#include <netdb.h>
#include <unistd.h>
#include "uint_t.h"
#include "ip.h"
#include "fd.h"
#include "env.h"
#include "prot.h"
#include "open.h"
#include "wait.h"
#include "buffer.h"
#include "errmsg.h"
#include "getoptb.h"
#include "pathexec.h"
#include "qsocket.h"
#include "ndelay.h"
#include "remoteinfo.h"
#include "rules.h"
#include "sig.h"
#include "dns.h"
#include "qstrings.h"

#define WHO "qmail-tcpsrv"

int verbosity = 1;
int flagkillopts = 1;
int flagdelay = 1;
char *banner = "";
int flagremoteinfo = 0;
int flagremotehost = 1;
int flagparanoid = 0;
unsigned long timeout = 26;
uint32 netif = 0;

int ipv4socket;

static stralloc tcpremoteinfo;

uint16 localport;
char localportstr[FMT_ULONG];
char localip[16];
char localipstr[IP6_FMT];
static stralloc localhostsa;
char *localhost = 0;

uint16 remoteport;
char remoteportstr[FMT_ULONG];
char remoteip[16];
char remoteipstr[IP6_FMT];
static stralloc remotehostsa;
char *remotehost = 0;

char strnum[FMT_ULONG];

static stralloc tmp;
static stralloc fqdn;
static stralloc addresses;

char bspace[16];
buffer b;



/* ---------------------------- child */

int flagdeny = 0;
int flagallownorules = 0;
char *fnrules = 0;

void cats(char *s) { if (!stralloc_cats(&tmp,s)) errmem; }
void append(char *ch) { if (!stralloc_append(&tmp,ch)) errmem; }

void safecats(char *s) {
  char ch;
  int i;

  for (i = 0;i < 100;++i) {
    ch = s[i];
    if (!ch) return;
    if (ch < 33) ch = '?';
    if (ch > 126) ch = '?';
    if (ch == '%') ch = '?'; /* logger stupidity */
/*    if (ch == ':') ch = '?'; */
    append(&ch);
  }
  cats("...");
}
void env(const char *s,const char *t) { if (!pathexec_env(s,t)) errmem; }
void drop_rules(void) { errint(111,B("unable to read ",fnrules,": ")); }

/* socket option: ipotionskill */
int socket_ipoptionskill(int s) {
  if (ipv4socket)
    return setsockopt(s,IPPROTO_IP,1,(char *) 0,0); /* 1 == IP_OPTIONS */

  return setsockopt(s,IPPROTO_IPV6,1,(char *) 0,0); /* 1 == IP_OPTIONS */
}

void found(char *data,unsigned int datalen)
{
  unsigned int next0;
  unsigned int split;

  while ((next0 = byte_chr(data,datalen,0)) < datalen) {
    switch(data[0]) {
      case 'D':
    flagdeny = 1;
    break;
      case '+':
    split = str_chr(data + 1,'=');
    if (data[1 + split] == '=') {
      data[1 + split] = 0;
      env(data + 1,data + 1 + split + 1);
    }
    break;
    }
    ++next0;
    data += next0; datalen -= next0;
  }
}

void doit(int t)
{
  int j;
  uint32 scope_id;

  if (ip6_isv4mapped(remoteip))
    remoteipstr[ip4_fmt(remoteipstr,remoteip+12)] = 0;
  else
    remoteipstr[ip6_fmt(remoteipstr,remoteip)] = 0;

  if (verbosity >= 2) {
    strnum[fmt_ulong(strnum,getpid())] = 0;
    log(B("pid ",strnum," from ",remoteipstr,0));
  }

  if (flagkillopts)
    socket_ipoptionskill(t);
  if (!flagdelay)
    socket_tcpnodelay(t);

  if (*banner) {
    buffer_init(&b,write,t,bspace,sizeof bspace);
    if (buffer_putsflush(&b,banner) == -1)
      errint(EHARD,"unable to print banner: ");
  }

  if (socket_local(t,localip,&localport,&scope_id) == -1)
    errint(EHARD,"unable to get local address: ");

  if (ip6_isv4mapped(localip))
    localipstr[ip4_fmt(localipstr,localip+12)] = 0;
  else
    localipstr[ip6_fmt(localipstr,localip)] = 0;
  remoteportstr[fmt_ulong(remoteportstr,remoteport)] = 0;

  if (!localhost)
    if (dns_name(&localhostsa,localip) == 0)
      if (localhostsa.len) {
    if (!stralloc_0(&localhostsa)) errmem;
    localhost = localhostsa.s;
      }
  env("PROTO","TCP");

  env("TCPLOCALIP",localipstr);
  env("TCPLOCALPORT",localportstr);
  env("TCPLOCALHOST",localhost);

  if (flagremotehost)
    if (dns_name(&remotehostsa,remoteip) == 0)
      if (remotehostsa.len) {
    if (flagparanoid)
      if (dns_ip6(&tmp,&remotehostsa) == 0)
        for (j = 0;j + 16 <= tmp.len;j += 16)
          if (byte_equal(remoteip,16,tmp.s + j)) {
        flagparanoid = 0;
        break;
          }
    if (!flagparanoid) {
      if (!stralloc_0(&remotehostsa)) errmem;
      remotehost = remotehostsa.s;
    }
      }
  env("TCPREMOTEIP",remoteipstr);
  env("TCPREMOTEPORT",remoteportstr);
  env("TCPREMOTEHOST",remotehost);

  if (flagremoteinfo) {
    if (remoteinfo6(&tcpremoteinfo,remoteip,remoteport,localip,localport,timeout,netif) == -1)
      flagremoteinfo = 0;
    if (!stralloc_0(&tcpremoteinfo)) errmem;
  }
  env("TCPREMOTEINFO",flagremoteinfo ? tcpremoteinfo.s : 0);

  if (fnrules) {
    int fdrules;
    fdrules = open_read(fnrules);
    if (fdrules == -1) {
      if (errno != error_noent) drop_rules();
      if (!flagallownorules) drop_rules();
    }
    else {
      char* temp;
	  if (ip6_isv4mapped(remoteip))
        temp=remoteipstr+7;
      else
        temp=remoteipstr;
      if (rules(found,fdrules,temp,remotehost,flagremoteinfo ? tcpremoteinfo.s : 0) == -1) drop_rules();
      close(fdrules);
//      log(B("checking tcp rules for ",remotehost,": pass"));
    }
  }

  if (verbosity >= 2) {
    strnum[fmt_ulong(strnum,getpid())] = 0;
    if (!stralloc_copys(&tmp,"qmail-tcpsrv: ")) errmem;
    safecats(flagdeny ? "deny" : "ok");
    cats(" "); safecats(strnum);
    cats(" "); if (localhost) safecats(localhost);
    cats(":"); safecats(localipstr);
    cats(":"); safecats(localportstr);
    cats(" "); if (remotehost) safecats(remotehost);
    cats(":"); safecats(remoteipstr);
//    cats(":<remote info>"); if (flagremoteinfo) safecats(tcpremoteinfo.s);
    cats(":"); if (flagremoteinfo) safecats(tcpremoteinfo.s);
    cats(":"); safecats(remoteportstr);
    cats("\n");
    buffer_putflush(buffer_2,tmp.s,tmp.len);
  }

  if (flagdeny) _exit(100);
}


/* ---------------------------- parent */

void usage(void) {
  errlog(0,ERROR,B("usage: qmail-tcpsrv [ -41UXpPhHrRoOdDqQv ] \
[ -c limit ] [ -x rules.cdb ] [ -B banner ] \
[ -g gid ] [ -u uid ] [ -b backlog ] [ -l localname ] \
[ -t timeout ] [ -I interface ] host port program"));
}

unsigned long limit = 40;
unsigned long numchildren = 0;

int flag1 = 0;
unsigned long backlog = 20;
unsigned long uid = 0;
unsigned long gid = 0;

void printstatus(void) {
  if (verbosity < 2) return;
  log(B("status: ",fmtnum(numchildren),"/",fmtnum(limit)));
}

void sigterm() { _exit(0); }

void sigchld()
{
  int wstat;
  int pid;

  while ((pid = wait_nohang(&wstat)) > 0) {
    if (verbosity >= 2) {
      errlog(0,NOTICE,B("end ",fmtnum(pid)," status ",fmtnum(wstat)));
    }
    if (numchildren) { --numchildren; } printstatus();
  }
}

int main(int argc,char **argv)
{
  char *hostname;
//  char *portname;
  int opt;
  struct servent *se;
  char *x;
  unsigned long u;
  int s;
  int t;

  while ((opt = getopt(argc,argv,"4dDvqQhHrR1UXx:t:u:g:l:b:B:c:I:pPoO")) != opteof)
    switch(opt) {
      case 'b': scan_ulong(optarg,&backlog); break;
      case 'c': scan_ulong(optarg,&limit); break;
      case 'X': flagallownorules = 1; break;
      case 'x': fnrules = optarg; break;
      case 'B': banner = optarg; break;
      case 'd': flagdelay = 1; break;
      case 'D': flagdelay = 0; break;
      case 'v': verbosity = 2; break;
      case 'q': verbosity = 0; break;
      case 'Q': verbosity = 1; break;
      case 'P': flagparanoid = 0; break;
      case 'p': flagparanoid = 1; break;
      case 'O': flagkillopts = 1; break;
      case 'o': flagkillopts = 0; break;
      case 'H': flagremotehost = 0; break;
      case 'h': flagremotehost = 1; break;
//      case 'R': flagremoteinfo = 0; break;
      case 'r': flagremoteinfo = 1; break;
      case 't': scan_ulong(optarg,&timeout); break;
      case 'U': x = env_get("UID"); if (x) scan_ulong(x,&uid);
        x = env_get("GID"); if (x) scan_ulong(x,&gid); break;
      case 'u': scan_ulong(optarg,&uid); break;
      case 'g': scan_ulong(optarg,&gid); break;
      case 'I': netif=socket_getifidx(optarg); break;
      case '1': flag1 = 1; break;
//      case '4': noipv6 = 1; break;
      case '4': ipv4socket = 1; break;
//      case '6': forcev6 = 1; break;
      case 'l': localhost = optarg; break;
      default: usage();
    }
  argc -= optind;
  argv += optind;

  if (!verbosity)
    buffer_2->fd = -1;

  hostname = *argv++;
  if (!hostname) usage();
  if (str_equal(hostname,"")) hostname = "0";

  x = *argv++;
  if (!x) usage();
  if (!x[scan_ulong(x,&u)])
    localport = u;
  else {
    se = getservbyname(x,"tcp");
    if (!se)
      errint(EHARD,B("unable to figure out port number for ",x));
    uint16_unpack_big((char*)&se->s_port,&localport);
  }

  if (!*argv) usage();

  sig_block(sig_child);
  sig_catch(sig_child,sigchld);
  sig_catch(sig_term,sigterm);
  sig_ignore(sig_pipe);

  if (str_equal(hostname,"0")) {
    byte_zero(localip,sizeof localip);
  } else {
    if (!stralloc_copys(&tmp,hostname)) errmem;
    if (dns_ip6_qualify(&addresses,&fqdn,&tmp) == -1)
      errint(EHARD,B("temporarily unable to figure out IP address for ",hostname,": "));
    if (addresses.len < 16)
	  errint(EHARD,B("no IP address for ",hostname));
    byte_copy(localip,16,addresses.s);
    if (ip6_isv4mapped(localip))
      ipv4socket = 1;
  }

  s = socket_tcp();
  if (s == -1)
    errint(EHARD,"unable to create socket: ");
  if (socket_bind_reuse(s,localip,localport,netif) == -1)
    errint(EHARD,"unable to bind: ");

  if (!ipv4socket) ipv4socket = ip6_isv4mapped(localip);

  if (socket_local(s,localip,&localport,&netif) == -1)
    errint(EHARD,"unable to get local address: ");
  if (socket_listen(s,backlog) == -1)
    errint(EHARD,"unable to listen: ");
  ndelay_off(s);

  if (gid) if (prot_gid(gid) == -1)
    errint(EHARD,"unable to set gid: ");
  if (uid) if (prot_uid(uid) == -1)
    errint(EHARD,"unable to set uid: ");


  localportstr[fmt_ulong(localportstr,localport)] = 0;
  if (flag1) {
    buffer_init(&b,write,1,bspace,sizeof bspace);
    buffer_puts(&b,localportstr);
    buffer_puts(&b,"\n");
    buffer_flush(&b);
  }

  close(0);
  close(1);
  printstatus();

  for (;;) {
    while (numchildren >= limit) sig_pause();

    sig_unblock(sig_child);
    t = socket_accept(s,remoteip,&remoteport,&netif);
    sig_block(sig_child);

    if (t == -1) continue;
    ++numchildren; printstatus();

    switch(fork()) {
      case 0:
        close(s);
        doit(t);
        if ((fd_move(0,t) == -1) || (fd_copy(1,0) == -1))
          errint(EHARD,"unable to set up descriptors: ");
        sig_uncatch(sig_child);
        sig_unblock(sig_child);
        sig_uncatch(sig_term);
        sig_uncatch(sig_pipe);
        pathexec(argv);
        errint(EHARD,B("unable to run ",*argv,": "));
      case -1:
        errlog(ESOFT,NOTICE,"unable to fork: ");
        --numchildren; printstatus();
    }
    close(t);
  }
}
