/*
 *  Revision 20160714, Kai Peter
 *  - updated some declarations no new(er) one's from ucspi-tcp-0.88
*/
#ifndef SIG_H
#define SIG_H

/* new(er) declarations from ucspi-tcp-0.88: */
extern int sig_alarm;
extern int sig_child;
extern int sig_cont;
extern int sig_hangup;
extern int sig_pipe;
extern int sig_term;

extern void (*sig_defaulthandler)();
extern void (*sig_ignorehandler)();

extern void sig_catch(int,void (*)());
#define sig_ignore(s) (sig_catch((s),sig_ignorehandler))
#define sig_uncatch(s) (sig_catch((s),sig_defaulthandler))

extern void sig_block(int);
extern void sig_unblock(int);
extern void sig_blocknone(void);
extern void sig_pause(void);

extern void sig_dfl(int);

/* declaration of (net)qmail package (untouched) */
extern void sig_miscignore();
extern void sig_bugcatch();

extern void sig_pipeignore();
extern void sig_pipedefault();

extern void sig_contblock();
extern void sig_contunblock();
extern void sig_contcatch();
extern void sig_contdefault();

extern void sig_termblock();
extern void sig_termunblock();
extern void sig_termcatch();
extern void sig_termdefault();

extern void sig_alarmblock();
extern void sig_alarmunblock();
extern void sig_alarmcatch();
extern void sig_alarmdefault();

extern void sig_childblock();
extern void sig_childunblock();
extern void sig_childcatch();
extern void sig_childdefault();

extern void sig_hangupblock();
extern void sig_hangupunblock();
extern void sig_hangupcatch();
extern void sig_hangupdefault();

#endif
