/*
 *  Revision 20160729, Kai Peter
 *  - included file 'dns_ipq6.c'
 *  - cast of some pointers
*/
#include "stralloc.h"
#include "case.h"
#include "byte.h"
#include "str.h"
#include "dns.h"

static int doit(stralloc *work,const char *rule)
{
  char ch;
  unsigned int colon;
  unsigned int prefixlen;

  ch = *rule++;
  if ((ch != '?') && (ch != '=') && (ch != '*') && (ch != '-')) return 1;
  colon = str_chr(rule,':');
  if (!rule[colon]) return 1;

  if (work->len < colon) return 1;
  prefixlen = work->len - colon;
  if ((ch == '=') && prefixlen) return 1;
  if (case_diffb((char *)rule,colon,work->s + prefixlen)) return 1;
  if (ch == '?') {
    if (byte_chr(work->s,prefixlen,'.') < prefixlen) return 1;
//Kai: added for IPv6:
#ifdef INET6
    if (byte_chr(work->s,prefixlen,':') < prefixlen) return 1;
#endif
    if (byte_chr(work->s,prefixlen,'[') < prefixlen) return 1;
    if (byte_chr(work->s,prefixlen,']') < prefixlen) return 1;
  }

  work->len = prefixlen;
  if (ch == '-') work->len = 0;
  return stralloc_cats(work,rule + colon + 1);
}

int dns_ip4_qualify_rules(stralloc *out,stralloc *fqdn,const stralloc *in,const stralloc *rules)
{
  unsigned int i;
  unsigned int j;
  unsigned int plus;
  unsigned int fqdnlen;

  if (!stralloc_copy(fqdn,(stralloc *)in)) return -1;

  for (j = i = 0;j < rules->len;++j)
    if (!rules->s[j]) {
      if (!doit(fqdn,rules->s + i)) return -1;
      i = j + 1;
    }

  fqdnlen = fqdn->len;
  plus = byte_chr(fqdn->s,fqdnlen,'+');
  if (plus >= fqdnlen)
    return dns_ip4(out,fqdn);

  i = plus + 1;
  for (;;) {
    j = byte_chr(fqdn->s + i,fqdnlen - i,'+');
    byte_copy(fqdn->s + plus,j,fqdn->s + i);
    fqdn->len = plus + j;
    if (dns_ip4(out,fqdn) == -1) return -1;
    if (out->len) return 0;
    i += j;
    if (i >= fqdnlen) return 0;
    ++i;
  }
}

int dns_ip4_qualify(stralloc *out,stralloc *fqdn,const stralloc *in)
{
  static stralloc rules;
  if (dns_resolvconfrewrite(&rules) == -1) return -1;
  return dns_ip4_qualify_rules(out,fqdn,in,&rules);
}

/* file: dns_ipq6.c ************************************************* */


/* Kai: this function is except one line identical with ip4 version above
static int doit(stralloc *work,const char *rule)
{
  char ch;
  unsigned int colon;
  unsigned int prefixlen;

  ch = *rule++;
  if ((ch != '?') && (ch != '=') && (ch != '*') && (ch != '-')) return 1;
  colon = str_chr(rule,':');
  if (!rule[colon]) return 1;

  if (work->len < colon) return 1;
  prefixlen = work->len - colon;
  if ((ch == '=') && prefixlen) return 1;
  if (case_diffb((char *)rule,colon,work->s + prefixlen)) return 1;
  if (ch == '?') {
    if (byte_chr(work->s,prefixlen,'.') < prefixlen) return 1;
    if (byte_chr(work->s,prefixlen,':') < prefixlen) return 1;
    if (byte_chr(work->s,prefixlen,'[') < prefixlen) return 1;
    if (byte_chr(work->s,prefixlen,']') < prefixlen) return 1;
  }

  work->len = prefixlen;
  if (ch == '-') work->len = 0;
  return stralloc_cats(work,rule + colon + 1);
}
*/

int dns_ip6_qualify_rules(stralloc *out,stralloc *fqdn,const stralloc *in,const stralloc *rules)
{
  unsigned int i;
  unsigned int j;
  unsigned int plus;
  unsigned int fqdnlen;

  if (!stralloc_copy(fqdn,(stralloc *)in)) return -1;

  for (j = i = 0;j < rules->len;++j)
    if (!rules->s[j]) {
      if (!doit(fqdn,rules->s + i)) return -1;
      i = j + 1;
    }

  fqdnlen = fqdn->len;
  plus = byte_chr(fqdn->s,fqdnlen,'+');
  if (plus >= fqdnlen)
    return dns_ip6(out,fqdn);

  i = plus + 1;
  for (;;) {
    j = byte_chr(fqdn->s + i,fqdnlen - i,'+');
    byte_copy(fqdn->s + plus,j,fqdn->s + i);
    fqdn->len = plus + j;
    if (dns_ip6(out,fqdn) == -1) return -1;
    if (out->len) return 0;
    i += j;
    if (i >= fqdnlen) return 0;
    ++i;
  }
}

int dns_ip6_qualify(stralloc *out,stralloc *fqdn,const stralloc *in)
{
  static stralloc rules;
  if (dns_resolvconfrewrite(&rules) == -1) return -1;
  return dns_ip6_qualify_rules(out,fqdn,in,&rules);
}
