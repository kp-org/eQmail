#!/bin/sh
#********************************************************************************
# IMPORTANT: check for OpenSSL and its header files (needed with '-DTLS' only)
CCOPTS=" -I ./inc"

searchHeaders() {
  # search for headers given by the '-I' option in conf-cc
  SSLHDRS=""
  for f in $(head -1 conf-cc | cut -d\- -f2-)
  do
    echo "$f" | grep '^-I' | grep 'openssl' 2>&1 >/dev/null
    if [ $? == "0" ] ; then SSLHDRS=$(echo $f | cut -d- -f2 | cut -dI -f2) ; fi
  done
  # given include dir doesn't exists
  if [ ! -d "$SSLHDRS" ] ; then SSLHDRS="" ; fi
}
# needed with '-DTLS' only
if (head -1 conf-cc | grep -e '\-DTLS' > /dev/null) ; then
   # check for OpenSSL binary
   echo -n "Checking for OpenSSL binary ... "
   for f in openssl eopenssl
   do
       TMP=`which $f 2>/dev/null`
       if [ "$TMP" ] ; then
          SSLVER=`$TMP version | cut -d\  -f1 | tr [A-Z] [a-z]`
          if [ "$SSLVER" = "openssl" ] ; then SSLBIN="$TMP" ; fi
       fi
   done
   if [ ! "$SSLBIN" ] ; then echo "NOT FOUND!" ; CRITICAL="$CRITICAL openssl binary" ; fi
   echo "$SSLBIN" | tee ssl.bin
   # check for openssl headers
   echo -n "Checking for openssl headers ..."
   searchHeaders
   if [ ! -d "$SSLHDRS" ] ; then
      # if nothing was found check the standard location
      if [ ! -d /usr/include/openssl ] ; then
         echo -ne " NOT" ; CRITICAL="$CRITICAL openssl headers" ; fi
   fi
   echo " found!"
fi

#********************************************************************************
# check for other header files
echo -n "Checking for select.h ..."
if [ -f /usr/include/sys/select.h ] ; then CCOPTS="$CCOPTS -DHAS_SELECT_H"
   else echo -n " not" ; fi
echo " found!"

# On linux we have flock, other systems have lockf instead
echo -n "Checking for flock/lockf ..."
which flock 2>/dev/null >/dev/null
if [ $? -eq 0 ] ; then CCOPTS="$CCOPTS -DHASFLOCK=1" ; echo -n " flock"
   else echo -n " lockf" ; fi
echo " found!"
