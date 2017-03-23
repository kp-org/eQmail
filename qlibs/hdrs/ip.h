/*
 *  Revision 20170319, Kai Peter
 *  - rewrite (consolidation and compatibility)
 *  Revision 20170210, Kai Peter
 *  - added definition 'V4loopback' and redefinition 'ip4loopback'
*/

/*  Consolidated header files ip.h from *qmail (with IPv6) and ip4.h/ip6.h
    from libowfat. Thus it could be used with 'older' and 'newer' code.
*/

#ifndef IP_H
#define IP_H

#include "byte.h"

#define HOSTNAMELEN 1025

#define IP4_FMT 20          /* backwards compatibility */
#define IP6_FMT 40          /*            "            */
#define IPFMT 72            /* used in qmail-remote only (temp?) */
/* may be better: */
//#define IP4_FMT IPFMT     /* backwards compatibility */
//#define IP6_FMT IPFMT     /*            "            */
/* deprecated: */
#define FMT_IP4 IP4_FMT     /* more backwards compatibility */
#define FMT_IP6 IP6_FMT     /*               "              */

/* these structs are going deprecated (should replaced by socket lib) */
struct ip4_address { unsigned char d[4]; } ;
#define ip_address ip4_address           /* backwards compatibility */
#ifdef INET6
struct ip6_address { unsigned char d[16]; } ;
#endif

extern unsigned int ip4_scan(char *,char *);
extern unsigned int ip4_fmt(char *,char *);
#define ip_scan ip4_scan        /* backwards compatibility */
#define ip_fmt ip4_fmt          /*            "            */

/* usually 'ip_scanbracket (should) do IPv4 AND IPv6 */
extern unsigned int ip_scanbracket();
extern unsigned int ip46_scanbracket();   /* temp: replacement for IPv4 AND IPv6 */
#define ip6_scanbracket ip_scanbracket    /* backwards compatibility */

#ifdef INET6
#define LIBC_HAS_IP6 1  // @Kai: temp. static --> make a "try" (?)
#endif

extern unsigned int ip6_scan(const char *,char *);
extern unsigned int ip6_fmt(char *,char *);

/* renamed functions (to use equal naming scheme) */
extern unsigned int ip6_scan_flat(char *,char *);
extern unsigned int ip6_fmt_flat(char *,char *);
#define scan_ip6_flat ip6_scan_flat     /* backwards compatibility */
#define fmt_ip6_flat ip6_fmt_flat       /*            "            */

/*
 ip6 address syntax: (h = hex digit), no leading '0' required
   1. hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh
   2. any number of 0000 may be abbreviated as "::", but only once
   flat ip6 address syntax:
   hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
 */

extern const char V4loopback[4]; /*= {127,0,0,1}; */
#define ip4loopback V4loopback       /* backwards compatibility */
extern const char V4mappedprefix[12]; /*={0,0,0,0,0,0,0,0,0,0,0xff,0xff}; */
extern const char V6loopback[16]; /*={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}; */
extern const char V6any[16]; /*={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; */

#define ip6_isv4mapped(ip) (byte_equal(ip,12,V4mappedprefix))

#endif
