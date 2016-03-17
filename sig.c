#include <signal.h>
#include "sig.h"
/* Consolidate the "sig_*.c" functions into one source
   file. Original files were of date 19980615, shipped
   with qmail-1.03.
   The "sig" functions will be linked to "sig.a" only!
   Included files:   Size (bytes)   Date
     - sig_alarm.c           252    19980615
     - sig_block.c           643    19980615
     - sig_catch.c           349    19980615
     - sig_pause.c           185    19980615
     - sig_pipe.c            147    19980615
     - sig_child.c           252    19980615
     - sig_hup.c             252    19980615
     - sig_term.c            248    19980615
     - sig_bug.c             281    19980615
     - sig_misc.c            318    19980615        */

/* file: sig_alarm.c */
void sig_alarmblock() { sig_block(SIGALRM); }
void sig_alarmunblock() { sig_unblock(SIGALRM); }
void sig_alarmcatch(f) void (*f)(); { sig_catch(SIGALRM,f); }
void sig_alarmdefault() { sig_catch(SIGALRM,SIG_DFL); }

/* file: sig_block.c */
#include "hassgprm.h"

void sig_block(sig)
int sig;
{
#ifdef HASSIGPROCMASK
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss,sig);
  sigprocmask(SIG_BLOCK,&ss,(sigset_t *) 0);
#else
  sigblock(1 << (sig - 1));
#endif
}

void sig_unblock(sig)
int sig;
{
#ifdef HASSIGPROCMASK
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss,sig);
  sigprocmask(SIG_UNBLOCK,&ss,(sigset_t *) 0);
#else
  sigsetmask(sigsetmask(~0) & ~(1 << (sig - 1)));
#endif
}

void sig_blocknone()
{
#ifdef HASSIGPROCMASK
  sigset_t ss;
  sigemptyset(&ss);
  sigprocmask(SIG_SETMASK,&ss,(sigset_t *) 0);
#else
  sigsetmask(0);
#endif
}

/* file: sig_catch.c */
#include "hassgact.h"

void sig_catch(sig,f)
int sig;
void (*f)();
{
#ifdef HASSIGACTION
  struct sigaction sa;
  sa.sa_handler = f;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaction(sig,&sa,(struct sigaction *) 0);
#else
  signal(sig,f); /* won't work under System V, even nowadays---dorks */
#endif
}

/* file: sig_pause.c */
void sig_pause()
{
#ifdef HASSIGPROCMASK
  sigset_t ss;
  sigemptyset(&ss);
  sigsuspend(&ss);
#else
  sigpause(0);
#endif
}

/* file: sig_pipe.c */
void sig_pipeignore() { sig_catch(SIGPIPE,SIG_IGN); }
void sig_pipedefault() { sig_catch(SIGPIPE,SIG_DFL); }

/* file: sig_child.c */
void sig_childblock() { sig_block(SIGCHLD); }
void sig_childunblock() { sig_unblock(SIGCHLD); }
void sig_childcatch(f) void (*f)(); { sig_catch(SIGCHLD,f); }
void sig_childdefault() { sig_catch(SIGCHLD,SIG_DFL); }

/* file: sig_hup.c */
void sig_hangupblock() { sig_block(SIGHUP); }
void sig_hangupunblock() { sig_unblock(SIGHUP); }
void sig_hangupcatch(f) void (*f)(); { sig_catch(SIGHUP,f); }
void sig_hangupdefault() { sig_catch(SIGHUP,SIG_DFL); }

/* file: sig_term.c */
void sig_termblock() { sig_block(SIGTERM); }
void sig_termunblock() { sig_unblock(SIGTERM); }
void sig_termcatch(f) void (*f)(); { sig_catch(SIGTERM,f); }
void sig_termdefault() { sig_catch(SIGTERM,SIG_DFL); }

/* file: sig_bug.c */
void sig_bugcatch(f) void (*f)();
{
  sig_catch(SIGILL,f);
  sig_catch(SIGABRT,f);
  sig_catch(SIGFPE,f);
  sig_catch(SIGBUS,f);
  sig_catch(SIGSEGV,f);
#ifdef SIGSYS
  sig_catch(SIGSYS,f);
#endif
#ifdef SIGEMT
  sig_catch(SIGEMT,f);
#endif
}

/* file: sig_misc.c */
void sig_miscignore()
{
  sig_catch(SIGVTALRM,SIG_IGN);
  sig_catch(SIGPROF,SIG_IGN);
  sig_catch(SIGQUIT,SIG_IGN);
  sig_catch(SIGINT,SIG_IGN);
  sig_catch(SIGHUP,SIG_IGN);
#ifdef SIGXCPU
  sig_catch(SIGXCPU,SIG_IGN);
#endif
#ifdef SIGXFSZ
  sig_catch(SIGXFSZ,SIG_IGN);
#endif
}
