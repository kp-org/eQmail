/*
 *  Revision 20160729, Kai Peter
 *  - new file: consolidated 'ip4_fmt.c', 'ip4_scan.c' from ucspi-tcp-0.88
 *  - added function 'ip_scanbracket' from original (qmail) 'ip.c'
*/
#include "fmt.h"
#include "scan.h"
#include "str.h"
#include "ip.h"

/* file: ip4_fmt.c ***************************************************** */
unsigned int ip4_fmt(char *s,char ip[4])
{
  unsigned int len;
  unsigned int i;

  len = 0;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[0]); len += i; if (s) s += i;
  if (s) *s++ = '.'; ++len;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[1]); len += i; if (s) s += i;
  if (s) *s++ = '.'; ++len;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[2]); len += i; if (s) s += i;
  if (s) *s++ = '.'; ++len;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[3]); len += i; if (s) s += i;
  return len;
}

/* file: ip4_scan.c **************************************************** */
unsigned int ip4_scan(char *s,char ip[4])
{
  unsigned int i;
  unsigned int len;
  unsigned long u;

  len = 0;
  i = scan_ulong(s,&u); if (!i) return 0; ip[0] = u; s += i; len += i;
  if (*s != '.') return 0; ++s; ++len;
  i = scan_ulong(s,&u); if (!i) return 0; ip[1] = u; s += i; len += i;
  if (*s != '.') return 0; ++s; ++len;
  i = scan_ulong(s,&u); if (!i) return 0; ip[2] = u; s += i; len += i;
  if (*s != '.') return 0; ++s; ++len;
  i = scan_ulong(s,&u); if (!i) return 0; ip[3] = u; s += i; len += i;
  return len;
}

/* Taken from the original 'ip.c', used by 'qmail-smtpd' and the original
 * 'dns.c' (from qmail) --> should be replaced (like in 'ucspi-tcp')   */
/*
  Params: *s is an IP address enclosed in brackets
*/
//unsigned int ip_scanbracket(char *s,char ip[4])
/*
unsigned int ip4_scanbracket(char *s,char ip[4])
{
  unsigned int len;

  if (*s != '[') return 0;
  len = ip4_scan(s + 1,ip);
  if (!len) return 0;
  if (s[len + 1] != ']') return 0;
  return len + 2;
}
*/

/* scan IPv4 or IPv6 ip address enclosed in brackets */
// temp., not "enough" tested yet !!!
//unsigned int ip46_scanbracket(char *s,char *ip_str)
unsigned int ip_scanbracket(char *s,char *ip_str)
{
  unsigned int len;

  if (*s != '[') return 0;
  if (str_chr(ip_str,':')) {
     len = ip6_scan(s + 1,ip_str);
  } else {
     len = ip4_scan(s + 1,ip_str);
  }

  if (!len) return 0;
  if (s[len + 1] != ']') return 0;
  return len + 2;
}
