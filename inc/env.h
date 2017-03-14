/*
 *  Revision 20160628, Kai Peter
 *  - updated code (*env_get) like in ucspi-tcp-0.88
 *  - commented out 'env_pick' and 'env_clear'
*/
#ifndef ENV_H
#define ENV_H

extern char **environ;

extern int env_isinit;

extern /*@null@*/char *env_get(char *);
extern int env_init();
extern int env_put();
extern int env_puts();
extern int env_unset();
//extern char *env_pick();
//extern void env_clear();
extern char *env_findeq();

#define env_put2 env_put

#endif
