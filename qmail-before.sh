
BINDIR="QMAILDIR/bin"		   # can be used in config file too

# silently ignore if config file doesn't exists
if [ ! -f QMAILDIR/control/BEFORE ] ; then
   exec "$BINDIR/PROG" $@ && exit ; fi

# include the config file and try to execute the commands in it
(. QMAILDIR/control/BEFORE 2>/dev/null) || ( \
 ErrMsg="Something went wrong with the config file!" && \
 echo "$ErrMsg" | "$BINDIR/splogger" `basename $0` && \
 exec "$BINDIR/PROG" $@)
