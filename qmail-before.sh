
PATH="QMAILHOME:$PATH"
PRG="QMAILHOME/bin/PROGRAM"    # failsafe default

DoExec() { exec "$PRG" "$@"; exit; }

# silently ignore if config file doesn't exists
[ -f QMAILHOME/etc/CFGFILE ] || DoExec "$@"

# get plugin from config file and try to execute it
TMP=`head -1 QMAILHOME/etc/CFGFILE`
[ -x "$TMP" ] && PRG="$TMP" || ( \
  [ ! "$TMP" = "NOLOG" ] && \
  ErrMsg="warning: ignoring invalid value '$TMP' in config file!" && \
  echo "$ErrMsg" | splogger `basename $0` )

DoExec "$@"
