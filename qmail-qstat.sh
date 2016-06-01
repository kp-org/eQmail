cd QMAIL
DIRS=SPLIT

showHelp() {
#  echo
  echo "Usage: qmail-qstat [option [value]]"
  echo
  echo -e "  -h\tShow this help"
  echo -e "  -l\tCount all messages in queue (default)"
  echo -e "  -L\tCount messages with local receipient"
  echo -e "  -R\tCount messages with remote receipient"
  echo
  exit 0
}

countLocal() {
  localfiles=`find queue/local/* -print | wc -w`
  echo " messages with local receipients: "`expr $localfiles - $DIRS`
}
countRemote() {
  remotefiles=`find queue/remote/* -print | wc -w`
  echo "messages with remote receipients: "`expr $remotefiles - $DIRS`
}
default() {
  messfiles=`find queue/mess/* -print | wc -w`
  echo "         total messages in queue: "`expr $messfiles - $DIRS`
  countLocal
  countRemote
}

while getopts ":hlLRg:" o; do
    case "${o}" in
#        u) STARTUID=${OPTARG};;
#        g) STARTGID=${OPTARG};;
        L) countLocal ; exit;;
        R) countRemote ; exit;;
        h) showHelp;;
        l) default;;
        *) echo "Invalid option!" ; showHelp;;
    esac
done ; shift $((OPTIND-1))
if [ ! $1 ] ; then echo $1: ;default ; fi
exit 0

#tododirs=`echo queue/todo/* | wc -w`
todofiles=`find queue/todo/* -print | wc -w`
# bounces
#bouncedirs=`echo queue/bounce/* | wc -w`
bouncefiles=`find queue/bounce/* -print | wc -w`

#info
#infodirs=`echo queue/remote/* | wc -w`
infofiles=`find queue/remote/* -print | wc -w`


#echo "       bounced messages in queue: "`expr $bouncefiles - 0`
#echo "   messages not yet preprocessed: "`expr $todofiles - $DIRS`
