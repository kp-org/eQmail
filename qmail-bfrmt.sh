
BINDIR="QMAIL/bin"		# this can be used in config file

# silently ignore if config file doesn't exists
if [ ! -f QMAIL/control/beforemote ] ; then exec "qmail-remote" $@ && exit ; fi

# include the config file and try to execute the commands in it
(. QMAIL/control/beforemote 2>/dev/null) || ( \
 ErrMsg="Something went wrong with the config file!" && \
 echo "$ErrMsg" | $BINDIR/splogger `basename $0` && \
 exec "qmail-remote" $@)
