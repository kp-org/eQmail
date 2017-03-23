/*
 *  Revision 20160729, Kai Peter
 *  - consolidated 'ip6_fmt.c', 'ip6_scan.c' from ucspi-tcp-0.88
*/
#include "fmt.h"
#include "byte.h"
#include "scan.h"
//#include "ip2.h"
#include "ip.h"

/* file: 'ip6_fmt.c' ************************************************ */
unsigned int ip6_fmt(char *s,char ip[16])
{
  unsigned long len,temp, k, pos0=0,len0=0, pos1=0, compr=0;

  for (k=0; k<16; k+=2) {
    if (ip[k]==0 && ip[k+1]==0) {
      if (!compr) {
        compr=1;
        pos1=k;
      }
      if (k==14) { k=16; goto last; }
    } else if (compr) {
    last:
      if ((temp=k-pos1) > len0) {
        len0=temp;
        pos0=pos1;
      }
      compr=0;
    }
  }

  for (len=0,k=0; k<16; k+=2) {
    if (k==12 && ip6_isv4mapped(ip)) {
      len += ip4_fmt(s,ip+12);
      break;
    }
    if (pos0==k && len0) {
      if (k==0) { ++len; if (s) *s++ = ':'; }
      ++len; if (s) *s++ = ':';
      k += len0-2;
      continue;
    }
    temp = ((unsigned long) (unsigned char) ip[k] << 8) +
            (unsigned long) (unsigned char) ip[k+1];
    temp = fmt_xlong(s,temp); len += temp; if (s) s += temp;
    if (k<14) { ++len; if (s) *s++ = ':'; }
  }

  return len;
}

static char tohex(char num) {
  if (num<10)
    return num+'0';
  else if (num<16)
    return num-10+'a';
  else
    return -1;
}

unsigned int ip6_fmt_flat(char *s,char ip[16])
{
  int i;
  for (i=0; i<16; i++) {
    *s++=tohex((unsigned char)ip[i] >> 4);
    *s++=tohex((unsigned char)ip[i] & 15);
  }
  return 32;
}

/* file: 'ip6_scan.c' *********************************************** */

/*
 * IPv6 addresses are really ugly to parse.
 * Syntax: (h = hex digit)
 *   1. hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh
 *   2. any number of 0000 may be abbreviated as "::", but only once
 *   3. The last two words may be written as IPv4 address
 */

//unsigned int scan_ip6(const char *s,char ip[16])
unsigned int ip6_scan(const char *s,char ip[16])
{
  unsigned int i;
  unsigned int len=0;
  unsigned long u;

  char suffix[16];
  int prefixlen=0;
  int suffixlen=0;

  if ((i=ip4_scan((char*)s,ip+12))) {
    for (len=0; len<12; ++len) ip[len]=V4mappedprefix[len];
    return i;
  }
  for (i=0; i<16; i++) ip[i]=0;
  for (;;) {
    if (*s == ':') {
        len++;
      if (s[1] == ':') {	/* Found "::", skip to part 2 */
    s+=2;
    len++;
    break;
      }
      s++;
    }
    i = scan_xlong((char*)s,&u);
    if (!i) return 0;
    if (prefixlen==12 && s[i]=='.') {
      /* the last 4 bytes may be written as IPv4 address */
      i=ip4_scan((char*)s,ip+12);
      if (i)
    return i+len;
      else
    return 0;
    }
    ip[prefixlen++] = (u >> 8);
    ip[prefixlen++] = (u & 255);
    s += i; len += i;
    if (prefixlen==16)
      return len;
  }

/* part 2, after "::" */
  for (;;) {
    if (*s == ':') {
      if (suffixlen==0)
    break;
      s++;
      len++;
    } else if (suffixlen!=0)
      break;
    i = scan_xlong((char*)s,&u);
    if (!i) {
      len--;
      break;
    }
    if (suffixlen+prefixlen<=12 && s[i]=='.') {
      int j=ip4_scan((char*)s,suffix+suffixlen);
      if (j) {
    suffixlen+=4;
    len+=j;
    break;
      } else
    prefixlen=12-suffixlen;	/* make end-of-loop test true */
    }
    suffix[suffixlen++] = (u >> 8);
    suffix[suffixlen++] = (u & 255);
    s += i; len += i;
    if (prefixlen+suffixlen==16)
      break;
  }
  for (i=0; i<suffixlen; i++)
    ip[16-suffixlen+i] = suffix[i];
  return len;
}
