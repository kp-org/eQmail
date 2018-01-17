
QMAILDIR="QPRFX"

# some escape sequences to format output
OFF=$'\e[0m'   # all attributes off
BLD=$'\e[1m'   # bold
ULN=$'\e[4m'   # underlined
YLW=$'\e[33m'  # yellow foreground
RED=$'\e[91m'  # light red foreground

$QPRFX/bin/qmail-print

printContent() {
  printf "\033[1m%s\033[0m" "$f:"
  printf " %s\n" "$COMMENT"
  # the format string $FMT can contain different values
  case "$FMT" in
    # print the default value and the related comment
    "0") printf "\t\033[33m%s\033[0m %s\n" "$DEFAULT" "$DEFCMNT";;
    # read the first line only
    "1") printf "\t\033[33m%s\033[0m\n" "`head -1 $CONFDIR/$f`" ;;
    # read multiple lines w/o having spaces, exclude comments ('#')
    "2") printf "\t\033[33m%s\n" $(cat $CONFDIR/$f | grep -v ^'\#')
         printf "\033[0m";;
    # 
    "3") VALUE=$(openssl x509 -enddate -noout -in $CONFDIR/$f.pem | cut -d= -f2)
         DEFAULT="valid until expire date on $VALUE"
         TODAY=`date +%s`
         case `uname | tr '[A-Z]' '[a-z]'` in    # check expiration date
          freebsd)
             EDATE=`date -j -f '%B %d %H:%M:%S %Y %Z' "$VALUE" '+%_s'`;;
          netbsd|openbsd)
             EDATE=`date -j -d "$VALUE" +%s`
             ;;
          *) EDATE=$(date +%_s -d "$VALUE");;
         esac
#         EDATE=`date -j -f '%B %d %H:%M:%S %Y' 'May 19 12:38:03 2017' +%s`
         if [ "$TODAY" -gt "$EDATE" ] ; then DEFAULT="$DEFAULT$RED (expired)" ; fi
         printf "\t\033[33m%s\033[0m\n" "$DEFAULT";;
    # read the first valid value (first line w/o '#'
    "4") printf "\t\033[33m%s\n" $(cat $CONFDIR/$f | grep -m 1 -v ^'\#')
         printf "\033[0m";;
    # print more in the future?
    "5") ;;
  esac
  FMT=0 ; COMMENT="" ; DEFCMNT=""
}

CONFDIR=$QMAILDIR/etc
if [ ! -d $CONFDIR ] ; then echo "error reading controls directory!" ; exit 1 ; fi

FILES="me defaultdomain locals defaulthost plusdomain rcpthosts morercpthosts \
       virtualdomains helohost localiphost envnoathost idhost \
       smtpgreeting concurrencylocal concurrencyremote queuelifetime \
       timeoutconnect timeoutremote timeoutsmtpd databytes \
       bouncefrom bouncehost doublebouncehost doublebounceto \
       defaultdelivery smtproutes qmqpservers \
       badmailfrom badmailto percenthack \
       smtpplugins checkpwtools \
"
if [ "$TLS" = "Yes" ] ; then
   FILES=" $FILES servercert clientcert tlsserverciphers tlsclientciphers \
          dh2048.pem rsa2048.pem"
fi

#********************************************************************************
# 
printf "\033[1m\033[4m""Configuration files:""\033[0m\n\n"
for f in $FILES
do
  case $f in
    badmailfrom)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      DEFAULT="Any 'MAIL FROM' is allowed."
      printContent;;
    beforequeue)
      ;;
    beforemote)
      ;;
    bouncefrom)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      COMMENT="bounce user name is:"
      DEFAULT="MAILER-DAEMON" ; DEFCMNT="(default)"
      printContent;;
    bouncehost)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      COMMENT="bounce host name is:"
      DEFAULT="$ME" ; DEFCMNT="(default: me)"
      printContent;;
    checkpwtools)  # don't show if not present
      if [ ! -f "$CONFDIR/$f" ] ; then continue ; else FMT="2" ; fi
      COMMENT="these checkpassword programs are given for (smtp) auth:"
      printContent;;
    concurrencylocal)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="local concurrency limit is:"
      DEFAULT="10" ; DEFCMNT="(default)"
      printContent;;
    concurrencyremote)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="remote concurrency limit is:"
      DEFAULT="20" ; DEFCMNT="(default)"
      printContent;;
    databytes)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="SMTP DATA SIZE limit of messages:"
      DEFAULT="not set here" ; DEFCMNT="(default: 0)"
      printContent;;
    defaultdelivery)
      if [ ! -f "$CONFDIR/$f" ] ; then continue ; else FMT="4" ; fi
      COMMENT="default instructions for all '.qmail' files:"
      DEFAULT=""
      printContent;;
    defaultdomain)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="default domain name is:"
      DEFAULT="not set!"
      printContent;;
    defaulthost)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="default host name is:"
      DEFAULT="$ME" ; DEFCMNT="(default: me)"
      printContent;;
    doublebouncehost)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      COMMENT="double bounce recipient host is:"
      DEFAULT="$ME" ; DEFCMNT="(default: me)"
      printContent;;
    doublebounceto)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      COMMENT="double bounce recipient name is:"
      DEFAULT="postmaster" ; DEFCMNT="(default)"
      printContent;;
    envnoathost)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="presumed domain name is:"
      DEFAULT="$ME" ; DEFCMNT="(default: me)"
      printContent;;
    helohost)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="SMTP client HELO host name is:"
      DEFAULT="$ME" ; DEFCMNT="(default: me)"
      printContent;;
    idhost)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="Message-ID host name is:"
      DEFAULT="$ME" ; DEFCMNT="(default: me)"
      printContent;;
    localiphost)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="local IP address becomes:"
      DEFAULT="$ME" ; DEFCMNT="(default: me)"
      printContent;;
    locals)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      COMMENT="messages to these domains will be delivered local:"
      DEFAULT=""
      printContent;;
    me)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="Our hostname:"
      DEFAULT="`cat $CONFDIR/$f`" ; ME="$DEFAULT"
      printContent;;
    morercpthosts)   # this is a bit more complex
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      DEFAULT="(doesn't exists - no effect)"
      COMMENT=" these additional domains are allowed to send mails to us:"
      # if file exists then check for morercpthosts.cdb
      if [ "$FMT" = "2" ] && [ ! -f "$CONFDIR/$f".cdb ] ; then
         DEFAULT="file exists, but database 'morercpthosts.cdb' doesn't - no effect!" ; FMT="X"
       else
         # check if morercpthosts.cdb is older than morercpthosts
         if [ "$CONFDIR/$f" -nt "$CONFDIR/$f".cdb ] ; then
            DEFAULT="database 'morercpthosts.cdb' exists, but is older than source file!" ; FMT="X"
         fi
         #DEFAULT="morercpthost exists, but morercpthost.cdb doesn't - no effect"
      fi
      # morercpthosts.cdb (database) exists but not morercpthosts (textfile)
      if [ "$FMT" = "0" ] && [ -f "$CONFDIR/$f".cdb ] ; then
         DEFAULT="database exists, but config file 'morercpthosts' doesn't - undefined behavior " ; FMT="X"
      fi
      if [ "$FMT" = "X" ] ; then FMT=0 ; fi
      if [ "$FMT" = "0" ] ; then COMMENT="" ; fi
      printContent;;
    percenthack)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      DEFAULT="The percent hack is not allowed."
      printContent;;
    plusdomain)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="this domain name will be added to addresses w/o a domain: "
      DEFAULT="not set"
      printContent;;
    qmqpservers)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      DEFAULT="no QMQP servers" ; DEFCMNT="(default)"
      printContent;;
    queuelifetime)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="Messages will be hold in the queue for max:"
      DEFAULT="604800 seconds (7 days)" ; DEFCMNT="(default)"
      printContent;;
    rcpthosts)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      COMMENT="these domains are allowed to send mails to us:"
      DEFAULT="none"
      printContent;;
    smtpgreeting)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="our SMTP greeting is:"
      DEFAULT="'220 `head -1 $CONFDIR/me` ESMTP'"  ; DEFCMNT="(default: me)"
      printContent;;
    smtproutes) # this is a bit more special
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      COMMENT="(potential auth credentials are not shown)"
      DEFAULT="(doesn't exists - no effect)"
      if [ "$FMT" = "2" ] ; then   # this is a bit more special here
         DEFAULT="$(cat $CONFDIR/$f | grep -v ^'\#' | cut -d\  -f1)" ; FMT="0" ; fi
      printContent;;
    timeoutconnect)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="client connection timeout is (seconds):"
      DEFAULT="60" ; DEFCMNT="(default)"
      printContent;;
    timeoutremote)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="SMTP client data timeout is (seconds):"
      DEFAULT="1200" ; DEFCMNT="(default)"
      printContent;;
    timeoutsmtpd)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="SMTP server data timeout is (seconds):"
      DEFAULT="1200" ; DEFCMNT="(default)"
      printContent;;
    virtualdomains)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      DEFAULT="No virtual domains defined."
      printContent;;
    clientcert|servercert)
      if [ ! -f "$CONFDIR/$f".pem ] ; then
         printf "\033[1m%s\n\t\033[91m%s\033[0m%s\n" "$f:" "Critical" ": not found!"
      else FMT="3" ; printContent ; fi
      ;;
    tlsclientciphers)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      DEFAULT="(not defined)"
      printContent;;
    tlsserverciphers)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="2" ; fi
      DEFAULT="(not defined)"
      printContent;;
    dh*.pem|rsa*.pem)
      if [ ! -f "$CONFDIR/$f" ] ; then FMT="0" ; else FMT="1" ; fi
      COMMENT="keyfile:"
      DEFAULT="not found - keys will be generated on the fly (slow)"
      if [ "$FMT" != "0" ] ; then
         DEFAULT="present: "$(file -b $CONFDIR/$f) ; FMT="0" ; fi
      printContent;;

    smtpplugins)
      if [ ! -f "$CONFDIR"/$f ] ; then FMT="0" ; else FMT="2" ; fi
      COMMENT="used by qmail-spp:"
      DEFAULT="not present"
      printContent;;

  esac
done
echo
printf "\033[1m%s\033[0m" "Hint:  "
echo "Some of the above could be set through an"
echo "environment variable too. This isn't considered!"


#tlshosts: I have no idea what this file does.
#servercert.cnf: I have no idea what this file do
#beforemote: I have no idea what this file does.
