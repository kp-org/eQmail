
# some basics ('nslookup' should be available always)
#FQDN=`nslookup \`hostname\` | grep ^Name: | awk '{print $2}' | tr '[A-Z]' '[a-z]'`
FQDN=`./hostname`
[ "$FQDN" ] || FQDN=`uname -n`    # last try to get a FQDN ...
[ "$FQDN" ] || FQDN="localhost"   # ... or we use "localhost" simply
#HOST=`echo "$FQDN" | cut -d. -f1`
DOMAIN=`echo "$FQDN" | cut -d. -f2-`
if [ ! "$DOMAIN" ] ; then
   echo -e "\n\033[1m >>> Couldn't determine DNS domain! Check your config!\033[0m\n" ; fi

# create fundamental config files (if not exists)
[ -f QPRFX/etc/me ] || (
  echo -e "Putting \033[33m"$FQDN"\033[0m into etc/me..."
  echo "$FQDN" > QPRFX/etc/me
  chmod 644 QPRFX/etc/me
  )

[ -f QPRFX/etc/locals ] || (
  echo " "
  echo -e "Putting \033[33m"$FQDN"\033[0m into etc/locals..."
  echo "$FQDN" > QPRFX/etc/locals
  echo " "
  echo "If there are any other domain names that point to you, you will have to"
  echo "add them to QPRFX/etc/locals."
  echo "You don\'t have to worry about aliases, i.e., domains with CNAME records."
  echo " "
  )

if [ "$DOMAIN" ] ; then
   [ -f QPRFX/etc/defaultdomain ] || (
     echo -e "Putting \033[33m"$DOMAIN"\033[0m into etc/defaultdomain..."
     echo "$DOMAIN" > QPRFX/etc/defaultdomain
     chmod 644 QPRFX/etc/defaultdomain
     )

   [ -f QPRFX/etc/plusdomain ] || (
     echo -e "Putting \033[33m"$DOMAIN"\033[0m into etc/plusdomain..."
     echo "$DOMAIN" > QPRFX/etc/plusdomain
     chmod 644 QPRFX/etc/plusdomain
     )
fi

[ -f QPRFX/etc/rcpthosts ] || (
  echo "Copying QPRFX/etc/locals to QPRFX/etc/rcpthosts..."
  cp QPRFX/etc/locals QPRFX/etc/rcpthosts
  chmod 644 QPRFX/etc/rcpthosts
  echo " "
  echo "Now eQmail will refuse to accept SMTP messages except to those hosts."
  echo "Make sure to change rcpthosts if you add hosts to locals or virtualdomains!"
  echo " "
  )

#********************************************************************************
# create etc/smtpplugins
[ -f QPRFX/etc/smtpplugins ] || \
( echo "Creating empty smtpplugins config file..."
printf "
# config file for qmail-spp\n#\n
# pathes are relativ to QPRFX\n#\n" > QPRFX/etc/smtpplugins
printf "[connection]\n\n[helo]\n\n[mail]\n\n[rcpt]\n\n[data]\n\n#[auth]\n" \
       >> QPRFX/etc/smtpplugins
chown root:qmail QPRFX/etc/smtpplugins
chmod 644 QPRFX/etc/smtpplugins	)

# create config file for qmail-bfque
[ -f QPRFX/etc/beforequeue ] || \
( echo "Creating config file beforequeue..."
printf "QPRFX/bin/qmail-queue

See qmail-bfrmt(8), beforequeue(5) and qmail picture for details.
"       >> QPRFX/etc/beforequeue )


# create config file for qmail-bfrmt
[ -f QPRFX/etc/beforemote ] || \
( echo "Creating config file beforemote..."
printf "QPRFX/bin/qmail-remote

See qmail-bfrmt(8), beforemote(5) and qmail picture for details.
"       >> QPRFX/etc/beforemote )

chown root:qmail QPRFX/etc/beforemote QPRFX/etc/beforequeue
chmod 644 QPRFX/etc/beforemote QPRFX/etc/beforequeue

#********************************************************************************
WITHTLS=$(cat conf.tmp | grep ^TLS=\"Yes\")
if [ "$WITHTLS" = "TLS=\"Yes\"" ] ; then
   # create servercert.cnf and TLS/SSL keys and certificates
   [ -f QPRFX/etc/servercert.cnf ] || (
     cat servercert.cnf | sed s{localhost{"$FQDN"{g \
     > QPRFX/etc/servercert.cnf
     chmod 640 QPRFX/etc/servercert.cnf
     chown root:qmail QPRFX/etc/servercert.cnf
     )
   mkdir -p QPRFX/etc/tlshosts
   [ -f QPRFX/etc/servercert.pem ] || ( \
     echo -e "\nCreating temporary keys and self-signed certificate ..."
     ./mkrsadhkeys
     ./mksrvrcerts
     )
fi
