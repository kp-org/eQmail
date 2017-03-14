/*
 * Revision 20170210, Kai Peter
 * - added definition 'V4loopback' and redefinition 'ip4loopback'
*/

// taken from libowfat
#ifndef IP_H
#define IP_H

#include "byte.h"
//#include "haveip6.h"

#define HOSTNAMELEN 1025

/* these structs are going deprecated */
//struct ip4_address { unsigned char d[4]; } ;
//#define ip_address ip4_address           /* backwards compatibility */
//#ifdef INET6
//struct ip6_address { unsigned char d[16]; } ;
//#endif

extern unsigned int ip4_scan(char *,char *);
extern unsigned int ip4_fmt(char *,char *);
/* for backwards compatibility */
#define ip_scan ip4_scan
#define ip_fmt ip4_fmt

#define IPFMT 72  // used in qmail-remote --> Achtung: nur mit INET6!!! - temp.
// end temporarily

#define IP4_FMT 20

// Kai: temp., see end of this file
//extern const char ip4loopback[4]; /* = {127,0,0,1}; */
//extern const char V4loopback[4]; /* = {127,0,0,1}; */

/* */
extern unsigned int ip_scanbracket();	// --> deprecated(?)
extern unsigned int ip46_scanbracket();  // new to replace ip_scanbracket

#ifdef INET6
#define LIBC_HAS_IP6 1		// Kai: temp. static --> make a "try"!
#endif

extern unsigned int ip6_scan(const char *,char *);
extern unsigned int ip6_fmt(char *,char *);

extern unsigned int scan_ip6_flat(char *,char *);
extern unsigned int fmt_ip6_flat(char *,char *);
/* @Kai: renamed functions */
#define ip6_scan_flat scan_ip6_flat
#define ip6_fmt_flat fmt_ip6_flat

/*
 ip6 address syntax: (h = hex digit), no leading '0' required
   1. hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh
   2. any number of 0000 may be abbreviated as "::", but only once
 flat ip6 address syntax:
   hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
 */

#define IP6_FMT 40

extern const unsigned char V4loopback[4]; /*= {127,0,0,1}; */
//extern const char V4loopback[4]; /*= {127,0,0,1}; */
extern const unsigned char V4mappedprefix[12]; /*={0,0,0,0,0,0,0,0,0,0,0xff,0xff}; */
extern const unsigned char V6loopback[16]; /*={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}; */
extern const unsigned char V6any[16]; /*={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; */

//extern const unsigned char V4loopback[4] = {127,0,0,1};
/*
const char V4loopback[4] = {127,0,0,1};
const unsigned char V4mappedprefix[12] ={0,0,0,0,0,0,0,0,0,0,0xff,0xff};
const unsigned char V6loopback[16] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
const unsigned char V6any[16] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
*/
//#define V4loopback ip4loopback   // Rev. 20170210
//#define ip4loopback V4loopback   // Rev. 20170210

#define ip6_isv4mapped(ip) (byte_equal(ip,12,V4mappedprefix))

#endif
