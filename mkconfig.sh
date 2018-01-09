
# some basics ('nslookup' should be available always)
FQDN=`nslookup \`hostname\` | grep ^Name: | awk '{print $2}' | tr '[A-Z]' '[a-z]'`
[ "$FQDN" ] || FQDN=`uname -n`    # last try to get a FQDN ...
[ "$FQDN" ] || FQDN="localhost"   # ... or we use "localhost" simply
HOST=`echo "$FQDN" | cut -d. -f1`
DOMAIN=`echo "$FQDN" | cut -d. -f2-`
if [ ! "$DOMAIN" ] ; then
   echo -e "\n\033[1m >>> Couldn't determine DNS domain! Check your config!\033[0m\n" ; fi

# create fundamental config files (if not exists)
[ -f QMAIL/etc/me ] || (
  echo -e "Putting \033[33m"$FQDN"\033[0m into etc/me..."
  echo "$FQDN" > QMAIL/etc/me
  chmod 644 QMAIL/etc/me
  )

[ -f QMAIL/etc/locals ] || (
  echo " "
  echo -e "Putting \033[33m"$FQDN"\033[0m into etc/locals..."
  echo "$FQDN" > QMAIL/etc/locals
  echo " "
  echo "If there are any other domain names that point to you, you will have to"
  echo "add them to QMAIL/etc/locals."
  echo "You don\'t have to worry about aliases, i.e., domains with CNAME records."
  echo " "
  )

if [ "$DOMAIN" ] ; then
   [ -f QMAIL/etc/defaultdomain ] || (
     echo -e "Putting \033[33m"$DOMAIN"\033[0m into etc/defaultdomain..."
     echo "$DOMAIN" > QMAIL/etc/defaultdomain
     chmod 644 QMAIL/etc/defaultdomain
     )

   [ -f QMAIL/etc/plusdomain ] || (
     echo -e "Putting \033[33m"$DOMAIN"\033[0m into etc/plusdomain..."
     echo "$DOMAIN" > QMAIL/etc/plusdomain
     chmod 644 QMAIL/etc/plusdomain
     )
fi

[ -f QMAIL/etc/rcpthosts ] || (
  echo "Copying QMAIL/etc/locals to QMAIL/etc/rcpthosts..."
  cp QMAIL/etc/locals QMAIL/etc/rcpthosts
  chmod 644 QMAIL/etc/rcpthosts
  echo " "
  echo "Now eQmail will refuse to accept SMTP messages except to those hosts."
  echo "Make sure to change rcpthosts if you add hosts to locals or virtualdomains!"
  echo " "
  )

#********************************************************************************
# create etc/smtpplugins
[ -f QMAIL/etc/smtpplugins ] || \
( echo "Creating empty smtpplugins config file..."
printf "
# config file for qmail-spp\n#\n
# pathes are relativ to QMAIL\n#\n" > QMAIL/etc/smtpplugins
printf "[connection]\n\n[helo]\n\n[mail]\n\n[rcpt]\n\n[data]\n\n#[auth]\n" \
       >> QMAIL/etc/smtpplugins
chown root:qmail QMAIL/etc/smtpplugins
chmod 644 QMAIL/etc/smtpplugins	)

# create config file for qmail-bfque
[ -f QMAIL/etc/beforequeue ] || \
( echo "Creating config file beforequeue..."
printf "QMAIL/bin/qmail-queue

See qmail-bfrmt(8), beforequeue(5) and qmail picture for details.
"       >> QMAIL/etc/beforequeue )


# create config file for qmail-bfrmt
[ -f QMAIL/etc/beforemote ] || \
( echo "Creating config file beforemote..."
printf "QMAIL/bin/qmail-remote

See qmail-bfrmt(8), beforemote(5) and qmail picture for details.
"       >> QMAIL/etc/beforemote )

chown root:qmail QMAIL/etc/beforemote QMAIL/etc/beforequeue
chmod 644 QMAIL/etc/beforemote QMAIL/etc/beforequeue

#********************************************************************************
WITHTLS=$(cat conf.tmp | grep ^TLS=\"Yes\")
if [ "$WITHTLS" = "TLS=\"Yes\"" ] ; then
   # create servercert.cnf and TLS/SSL keys and certificates
   [ -f QMAIL/etc/servercert.cnf ] || (
     cat servercert.cnf | sed s{localhost{"$FQDN"{g \
     > QMAIL/etc/servercert.cnf
     chmod 640 QMAIL/etc/servercert.cnf
     chown root:qmail QMAIL/etc/servercert.cnf
     )
   mkdir -p QMAIL/etc/tlshosts
   [ -f QMAIL/etc/servercert.pem ] || ( \
     echo -e "\nCreating temporary keys and self-signed certificate ..."
     ./mkrsadhkeys
     ./mksrvrcerts
     )
fi
