
BINDIR="QMAIL/bin"		# this can be used in config file

# silently ignore if config file doesn't exists
if [ ! -f QMAIL/control/beforequeue ] ; then exec "qmail-queue" $@ && exit ; fi

# include the config file and try to execute the commands in it
(. QMAIL/control/beforequeue 2>/dev/null) || ( \
 ErrMsg="Something went wrong with the config file!" && \
 echo "$ErrMsg" | $BINDIR/splogger `basename $0` && \
 exec "qmail-queue" $@)
