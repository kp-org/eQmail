#ifndef ERROR_H
#define ERROR_H

#include <errno.h>
                                      /* Comparison of error codes and constants:
                                          intern   Linux    BSD                */
extern int error_intr;                /*    -1       4       4    EINTR        */
extern int error_nomem;               /*    -2      12      12    ENOMEM       */
extern int error_noent;               /*    -3       2       2    ENOENT       */
extern int error_txtbsy;              /*    -4      26      26    ETXTBSY      */
extern int error_io;                  /*    -5       5       5    EIO          */
extern int error_exist;               /*    -6      17      17    EEXIST       */
extern int error_timeout;             /*    -7     110      60    ETIMEDOUT    */
extern int error_inprogress;          /*    -8     115      36    EINPROGRESS  */
extern int error_wouldblock;          /*    -9    EAGAIN  EAGAIN  EWOULDBLOCK  */
extern int error_again;               /*   -10      11      35    EAGAIN       */
extern int error_pipe;                /*   -11      32      32    EPIPE        */
extern int error_perm;                /*   -12       1       1    EPERM        */
extern int error_acces;               /*   -13      13      13    EACCES       */
// not used:
extern int error_nodevice;            /*   -14?     (6)     (6)   ENXIO        */ // or intern ENXIO (see 'error.c')
extern int error_proto;               /*   -15      71      92    EPROTO       */
extern int error_isdir;               /*   -16      21      21    EISDIR       */
extern int error_connrefused;         /*   -17     111      61    ECONNREFUSED */
// not used:
extern int error_notdir;              /*   -18      20      20    ENOTDIR      */
extern int error_rofs;                /*   -19      30      30    EROFS        */
extern int error_connreset;           /*   -20     104      54    ECONNRESET   */

extern char *error_str();
extern int error_temp();

#endif
