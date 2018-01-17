
PATH="QPREFIX:$PATH"
PRG="QPREFIX/bin/PROGRAM"    # failsafe default

DoExec() { exec "$PRG" "$@"; exit; }

# silently ignore if config file doesn't exists
[ -f QPRFX/etc/CFGFILE ] || DoExec "$@"

# get plugin from config file and try to execute it
TMP=`head -1 QPRFX/etc/CFGFILE`
[ -x "$TMP" ] && PRG="$TMP" || ( \
  [ ! -z "$TMP" ] && \
  ErrMsg="warning: ignoring invalid value '$TMP' in config file!" && \
  echo "$ErrMsg" | splogger `basename $0` )

DoExec "$@"
