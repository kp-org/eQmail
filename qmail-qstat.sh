QMAILDIR="QPRFX"
cd $QMAILDIR
DIRS=`ls queue/mess/ | wc -l | xargs`   # check and count queue split
# simple try to check out the 'service start' process
for s in eqmaild qmail svscan    # xinetd ???
do
  [ -f "/etc/init.d/$s" ] && BC="$s" && break
done

showHelp() {
  echo "Usage: qmail-qstat [option [value]]"
  echo
  echo -e "  -h\t\tShow this help"
  echo -e "  -D <id>\tDelete message with id 'id' from queue"
  echo -e "  -d\t\tdelete all messages from queue"
  echo -e "  -i\t\tList ID's of all messages in queue"
  echo -e "  -l\t\tList all messages in queue"
  echo -e "  -L\t\tCount messages with local receipient"
  echo -e "  -m <id>\tShow message with id 'id'"
  echo -e "  -R\t\tCount messages with remote receipient"
  echo
  exit 0
}

countBounces() {
  bouncefiles=`ls queue/bounce | wc -w | xargs`
  echo "           messages with bounces: "$bouncefiles
}
countLocal() {
  localfiles=`find queue/local/* -print | wc -w`
  echo " messages with local receipients: "`expr $localfiles - $DIRS`
}
countRemote() {
  remotefiles=`find queue/remote/* -print | wc -w`
  echo "messages with remote receipients: "`expr $remotefiles - $DIRS`
}
countTodo() {
  todofiles=`find queue/todo/* -print | wc -w`
  echo "  messages not pre-processed yet: "`expr $todofiles - $DIRS`
}
default() {
  echo -e "\nSummary:"
  messfiles=`find queue/mess/* -print | wc -w`
  echo "         total messages in queue: "`expr $messfiles - $DIRS`
  countLocal
  countRemote
  countTodo
  countBounces
  echo -e "\nDirectory split of the queue is $DIRS.\n"
}
listMessages() {
  [ "$QSUBDIR" ] || QSUBDIR="mess"
  [ "$N" ] && FARGS="-name $N"
  for M in $(find queue/$QSUBDIR/*/* $FARGS -print 2>/dev/null)
  do echo -e "\nMessage-Id: `basename $M`" ; cat $M ; done
}
listMsgIDs() {
  echo "ID's of messages in queue:"
  [ "$QSUBDIR" ] || QSUBDIR="mess"
  for ID in $(find queue/$QSUBDIR/*/* -print 2>/dev/null)
  do echo `basename "$ID"` ; done
}
stopServices() {
  # return and continue w/o stopping services
  [ ! "$BC" ] && echo "Warning: couldn't stop services ..." && return
  echo "stopping services ..."
  # try to stop service and if it fails - bummer!
  /etc/init.d/"$BC" stop 2>&1 >/dev/null
}
startServices() {
  [ ! "$BC" ] && echo "Please restart 'qmail-send' now!" && return
  echo "restarting services ..."
  #
  /etc/init.d/"$BC" start #2>&1 >/dev/null
}
deleteMessage() {
  stopServices
  [ "$QSUBDIR" ] || QSUBDIR="mess"
  [ "$N" ] && FARGS="-name $N"	# set 'find' arguments for a single message
  echo -n "deleting message"
  [ "$N" ] && echo -n " $N" || echo -n "s"	# be a bit verbose ...
  # find and delete message(s)
  for M in $(find queue/*/*/* $FARGS 2>/dev/null | xargs)
  do
    Q=`echo $M | cut -d/ -f2`
    rm -f "$M"
  done
  if [ "$M" ] ; then echo "... done" ;		# print result
    else [ "$FARGS" ] && \
      echo " ... failed: no such message!" || \
      echo " ... no messages in queue found!"
  fi
  startServices
}

if [ ! $1 ] ; then default ; exit 0 ; fi
while getopts ":hliLRm:D:d" o; do
    case "${o}" in
        L) QSUBDIR="local" ; listMessages ; echo ; countLocal ; echo ; exit;;
        R) QSUBDIR="remote" ; listMessages ; echo ; countRemote ; echo ; exit;;
        h) showHelp;;
        i) listMsgIDs;;
        l) listMessages ; default;;
        m) N=${OPTARG} ; listMessages ; echo;;
        D) N=${OPTARG} ; deleteMessage ;;
        d) deleteMessage;;

        *) echo "Invalid option!" ; showHelp;;
    esac
done ; shift $((OPTIND-1))

exit 0
