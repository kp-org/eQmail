/*
 *  Revision 20160714, Kai Peter
 *  - consolidated 'sgetopt.h' and 'subgetopt.h' into 'bgetopt.h'
*/
#ifndef BGETOPT_H
#define BGETOPT_H

#define optarg subgetoptarg
#define optind subgetoptind
#define optpos subgetoptpos
#define optproblem subgetoptproblem
#define optprogname subgetoptprogname
#define opteof subgetoptdone

#define SUBGETOPTDONE -1

extern int subgetopt(int,char **,char *);
extern char *subgetoptarg;
extern int subgetoptind;
extern int subgetoptpos;
extern int subgetoptproblem;
extern char *subgetoptprogname;
extern int subgetoptdone;

extern int opterr;

#endif
