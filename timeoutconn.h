/*
 *  Revision 20170324, Kai Peter
 *  - renamed 'timeoutconn()' -> 'timeoutconn4()'
 *  - new declaration 'timeoutconn()' (qmail-tcpsrv)
*/
#ifndef TIMEOUTCONN_H
#define TIMEOUTCONN_H

/* new: used by newer libs */
#include "uint_t.h"

//extern int timeoutconn(int,char *,uint16,unsigned int);
//extern int timeoutconn6(int,char *,uint16,unsigned int,uint32);
extern int timeoutconn(int,char *,uint16,unsigned int,uint32);


/* Old *qmail code: should be going deprecated */
extern int timeoutconn4();
extern int timeoutconn6();

#endif
