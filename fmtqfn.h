#ifndef FMTQFN_H
#define FMTQFN_H

extern unsigned int fmt_qfn(char *,char *,unsigned long,int);

#define fmtqfn fmt_qfn   /* temp: backwards compatibility */

#define FMTQFN 40 /* maximum space needed, if len(dirslash) <= 10 */

#endif
