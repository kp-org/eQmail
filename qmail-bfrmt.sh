
BINDIR="QMAILHOME/bin"
PATH="$BINDIR:$PATH"
#BINDIR="QMAILHOME/bin"		# this can be used in config file

PRG="$BINDIR/qmail-remote"

DoExec() { exec "$PRG" "$@"; exit; }

# silently ignore if config file doesn't exists
#if [ ! -f QMAIL/control/beforemote ] ; then exec "qmail-remote" $@ && exit ; fi
[ -f QMAILHOME/etc/beforemote ] || DoExec "$@"

# include the config file and try to execute the commands in it
(. QMAILHOME/control/beforemote 2>/dev/null) || ( \
 ErrMsg="Something went wrong with the config file!" && \
 echo "$ErrMsg" | $BINDIR/splogger `basename $0` && \
 exec "qmail-remote" $@)
