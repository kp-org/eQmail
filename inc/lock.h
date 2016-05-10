/*
 *  Revision 20160510, Kai Peter
 *  - added prototypes for flock/lockf
 */
#ifndef LOCK_H
#define LOCK_H

#ifdef HASFLOCK
extern int flock();
#else
extern int lockf();
//int      lockf(int, int, off_t);
#endif

extern int lock_ex();
extern int lock_un();
extern int lock_exnb();

#endif
