/* 
 * Rev. 20160503, Kai Peter
 *  - added 'qread', 'qwrite' to have it available for further
 *    development to avoid warnings of 'implicit declaration'
 */
#ifndef READWRITE_H
#define READWRITE_H

extern int read();
extern int write();

extern int qread();
extern int qwrite();

#endif
