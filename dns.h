/*
 *  Revision 20160504, Kai Peter
 *  - added 'dns_ptr6()'
 */
#ifndef DNS_H
#define DNS_H

#define DNS_SOFT -1
#define DNS_HARD -2
#define DNS_MEM -3

void dns_init();
int dns_cname();
int dns_mxip();
int dns_ip();
int dns_ptr();

#ifdef INET6
int dns_ptr6();
#endif

#endif
