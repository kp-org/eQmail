/*
 *  Revision 20160714, Kai Peter
 *  - consolidated 'sgetopt.h' and 'subgetopt.h' into 'bgetopt.h'
*/
#ifndef BGETOPT_H
#define BGETOPT_H

//#ifndef SGETOPTNOSHORT
//#define getopt sgetoptmine	// use unistd.h in *.c file
#define optarg subgetoptarg
#define optind subgetoptind
#define optpos subgetoptpos
//#define opterr sgetopterr				// declare opterr direct
#define optproblem subgetoptproblem
//#define optprogname sgetoptprogname	// circle, see below
#define optprogname subgetoptprogname
#define opteof subgetoptdone
//#endif

//#include "subgetopt.h"
//#ifndef SUBGETOPT_H
//#define SUBGETOPT_H

//#ifndef SUBGETOPTNOSHORT
//#define sgopt subgetopt        // was used by tcp-env omly --> changed
//#define sgoptarg subgetoptarg  // was used by tcp-env omly --> changed
//#define sgoptind subgetoptind  // was used by tcp-env omly --> changed
//#define sgoptpos subgetoptpos             // never used
//#define sgoptproblem subgetoptproblem		// never used
//#define sgoptprogname subgetoptprogname   // circle
//#define sgoptdone subgetoptdone			// circle
//#endif

#define SUBGETOPTDONE -1

extern int subgetopt(int,char **,char *);
extern char *subgetoptarg;
extern int subgetoptind;
extern int subgetoptpos;
extern int subgetoptproblem;
extern char *subgetoptprogname;
extern int subgetoptdone;

//#endif

//extern int sgetoptmine(int,char **,char *); // use unistd.h, remove #declare above
//extern int sgetopterr;
extern int opterr;
//extern char *sgetoptprogname;		// cirle/dup to subgetoptprogname

#endif
