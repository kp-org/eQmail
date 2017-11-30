#!/bin/sh

CFGFILE="conf.tmp"
OUTFILE="qmail-fixq"

[ -f "$CFGFILE" ] || exit 1

# the 'stat' command is different on Linux/BSD
STATOPTS="-f %Lp"
cat systype | grep -i linux >/dev/null
if [ $? -eq 0 ] ; then STATOPTS="-c %a" ; fi

cat warn-auto.sh > "$OUTFILE"
echo "" >> "$OUTFILE"
echo $(cat "$CFGFILE" | grep ^HOMEDIR | sed s}HOMEDIR}QMAILDIR}g) >> "$OUTFILE"
echo $(cat "$CFGFILE" | grep ^UIDQ)                               >> "$OUTFILE"
echo $(cat "$CFGFILE" | grep ^UIDR)                               >> "$OUTFILE"
echo $(cat "$CFGFILE" | grep ^UIDS)                               >> "$OUTFILE"
echo $(cat "$CFGFILE" | grep ^GIDQ)                               >> "$OUTFILE"
echo $(cat "$CFGFILE" | grep ^SPLIT)                              >> "$OUTFILE"
echo "" >> "$OUTFILE"

echo "
showHelp() {
  echo \"Usage: $OUTFILE [option [-m]\"
  echo
  echo -e \"  -c\tcheck structure of the queue for issues (default)\"
  echo -e \"  -f\tfix issues of the queue\"
  echo -e \"  -h\tShow this help\"
#  echo -e \"  -m\tincludes check of messages (this could take a long time)\"  # useless in practice, messages
                                                                               # will be fixed always
  echo
  exit 0
}" >> "$OUTFILE"

echo "fixqueue() {"                                               >> "$OUTFILE"
echo "  echo -n \"Fixing queue ...\""                             >> "$OUTFILE"
# the code comes from the 'install' script
fln=`cat install | grep -n '^# queue' | cut -d: -f1`   # get the first line
lln=`cat install | grep -n '^# bin' | cut -d: -f1`     # lastline
fln=`expr $fln + 1`                                    # skip one line on begin
lines=`expr $lln - $fln - 1`                           # ... and on the end
#cat install | grep -A $l '^# queue' >> $OUTFILE
cat install | tail -n +$fln | head -n $lines | sed 's/^/  /'      >> "$OUTFILE"

echo "  echo \"done!\"" >> "$OUTFILE"
echo "}"                >> "$OUTFILE"
echo "chkqueue() {
  echo \"Checking queue for issues ...\"
  SPLITMESS=\`find \$QMAILDIR/queue/mess/*   -type d -print | wc -l\`
  SPLITINFO=\`find \$QMAILDIR/queue/info/*   -type d -print | wc -l\`
  SPLITINTD=\`find \$QMAILDIR/queue/intd/*   -type d -print | wc -l\`
  SPLITLOCL=\`find \$QMAILDIR/queue/local/*  -type d -print | wc -l\`
  SPLITREMT=\`find \$QMAILDIR/queue/remote/* -type d -print | wc -l\`
  SPLITTODO=\`find \$QMAILDIR/queue/todo/*   -type d -print | wc -l\`
  if [ \"\$SPLIT\" -ne \"\$SPLITMESS\" ]; then SPLITERR=1 ; fi
  if [ \"\$SPLIT\" -ne \"\$SPLITINFO\" ]; then SPLITERR=1 ; fi
  if [ \"\$SPLIT\" -ne \"\$SPLITINTD\" ]; then SPLITERR=1 ; fi
  if [ \"\$SPLIT\" -ne \"\$SPLITLOCL\" ]; then SPLITERR=1 ; fi
  if [ \"\$SPLIT\" -ne \"\$SPLITREMT\" ]; then SPLITERR=1 ; fi
  if [ \"\$SPLIT\" -ne \"\$SPLITTODO\" ]; then SPLITERR=1 ; fi

  if [ \"\$SPLITERR\" ] ; then
      echo \"invalid queue split\" ; ERR=1
   else
      echo \"splitting ok!\"
  fi
  # check the directory structure
  # group have to be \'qmail\' always (recursive)
  GRPNAME=\`getent group \$GIDQ | awk -F: '{print \$1}'\`
  find \$QMAILDIR/queue/* -type d -print | xargs ls -ld | awk '{print \$4}' | grep -v \$GRPNAME >/dev/null
  if [ \$? -eq 0 ] ; then GRPERR=1 ; fi

  # check for uid qmailq/qmails on splitted directories
  USRNAME=\`id -un \$UIDQ\`
  for f in intd mess todo local remote info
  do
    if [ \"\$f\" = \"local\" ] ; then USRNAME=\`id -un \$UIDS\` ; fi

#echo \$f
    ls -ld \$QMAILDIR/queue/\$f | awk '{print \$3}' | grep -v \$USRNAME >/dev/null
    if [ \$? -eq 0 ] ; then USRERR=1 ; fi
#find \$QMAILDIR/queue/\$f/*
    find \$QMAILDIR/queue/\$f/* | xargs ls -ld | awk '{print \$3}' | grep -v \$USRNAME >/dev/null
    if [ \$? -eq 0 ] ; then USRERR=1 ; fi
  done
  # check pid, bounce
  ls -ld \$QMAILDIR/queue/bounce | awk '{print \$3}' | grep -v \$USRNAME >/dev/null
    if [ \$? -eq 0 ] ; then USRERR=1 ; fi
  USRNAME=\`id -un \$UIDQ\`   # until now USRNAME was 'qmails'
  ls -ld \$QMAILDIR/queue/pid    | awk '{print \$3}' | grep -v \$USRNAME >/dev/null
    if [ \$? -eq 0 ] ; then USRERR=1 ; fi

  # special: lock
#echo U: \$USRNAME
  ls -ld \$QMAILDIR/queue/lock   | awk '{print \$3}' | grep -v \$USRNAME >/dev/null
  if [ \$? -eq 0 ] ; then USRERR=1 ; fi
  USRNAME=\`id -un \$UIDS\`   # ... back to USRNAME was 'qmails'
  ls -l \$QMAILDIR/queue/lock/trigger   | awk '{print \$3}' | grep -v \$USRNAME >/dev/null
  if [ \$? -eq 0 ] ; then USRERR=1 ; fi
#echo U: \$USRNAME
  ls -l \$QMAILDIR/queue/lock/sendmutex | awk '{print \$3}' | grep -v \$USRNAME >/dev/null
  if [ \$? -eq 0 ] ; then USRERR=1 ; fi

  USRNAME=\`id -un \$UIDR\`   # ... and now we need USRNAME 'qmailr'
#echo U: \$USRNAME
  ls -l \$QMAILDIR/queue/lock/tcpto | awk '{print \$3}' | grep -v \$USRNAME >/dev/null
  if [ \$? -eq 0 ] ; then USRERR=1 ; fi

  if [ \"\$USRERR\" ] || [ \"\$GRPERR\" ] ; then
     echo \"invalid owner(s) or group!\"
   else
     echo \"user/group ok!\"
  fi

  # check permissions w/ and w/o subdirectories
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/mess\`     | grep -v 750 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/mess/*\`   | grep -v 750 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/todo\`     | grep -v 750 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/todo/*\`   | grep -v 750 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/info\`     | grep -v 700 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/info/*\`   | grep -v 700 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/intd\`     | grep -v 700 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/intd/*\`   | grep -v 700 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/local\`    | grep -v 700 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/local/*\`  | grep -v 700 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/remote\`   | grep -v 700 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi
  stat $STATOPTS \`ls -d \$QMAILDIR/queue/remote/*\` | grep -v 700 >/dev/null
  if [ \$? -eq 0 ] ; then PRMERR=1 ; fi

  if [ \"\$PRMERR\" ] ; then
     echo \"invalid permissions(s)!\"
   else
     echo \"permissions ok!\"
  fi

#  echo
}" >> "$OUTFILE"

echo "
if [ ! \"\$1\" ] ; then chkqueue ; exit 0 ; fi
while getopts ":hcf" o; do
    case \"\${o}\" in
        h) showHelp;;
        f) fixqueue;;
        c) chkqueue;;
        m) FIXMSG=\"Y\" ; fixqueue;;
        *) echo "Invalid option!" ; showHelp;;
    esac
done ; shift $((OPTIND-1))

exit 0" >> "$OUTFILE"

chmod 755 $OUTFILE
