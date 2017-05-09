
# some basics ('nslookup' should be available always)
FQDN=`nslookup \`hostname\` | grep ^Name: | awk '{print $2}' | tr '[A-Z]' '[a-z]'`
[ "$FQDN" ] || FQDN=`uname -n`    # last try to get a FQDN ...
[ "$FQDN" ] || FQDN="localhost"   # ... or we use "localhost" simply
HOST=`echo "$FQDN" | cut -d. -f1`
DOMAIN=`echo "$FQDN" | cut -d. -f2-`
if [ ! "$DOMAIN" ] ; then
   echo -e "\n\033[1m >>> Couldn't determine DNS domain! Check your config!\033[0m\n" ; fi

# create fundamental config files (if not exists)
[ -f QMAIL/control/me ] || (
  echo -e "Putting \033[33m"$FQDN"\033[0m into control/me..."
  echo "$FQDN" > QMAIL/control/me
  chmod 644 QMAIL/control/me
  )

[ -f QMAIL/control/locals ] || (
  echo " "
  echo -e "Putting \033[33m"$FQDN"\033[0m into control/locals..."
  echo "$FQDN" > QMAIL/control/locals
  echo " "
  echo "If there are any other domain names that point to you, you will have to"
  echo "add them to QMAIL/control/locals."
  echo "You don\'t have to worry about aliases, i.e., domains with CNAME records."
  echo " "
  )

if [ "$DOMAIN" ] ; then
   [ -f QMAIL/control/defaultdomain ] || (
     echo -e "Putting \033[33m"$DOMAIN"\033[0m into control/defaultdomain..."
     echo "$DOMAIN" > QMAIL/control/defaultdomain
     chmod 644 QMAIL/control/defaultdomain
     )

   [ -f QMAIL/control/plusdomain ] || (
     echo -e "Putting \033[33m"$DOMAIN"\033[0m into control/plusdomain..."
     echo "$DOMAIN" > QMAIL/control/plusdomain
     chmod 644 QMAIL/control/plusdomain
     )
fi

[ -f QMAIL/control/rcpthosts ] || (
  echo "Copying QMAIL/control/locals to QMAIL/control/rcpthosts..."
  cp QMAIL/control/locals QMAIL/control/rcpthosts
  chmod 644 QMAIL/control/rcpthosts
  echo " "
  echo "Now eQmail will refuse to accept SMTP messages except to those hosts."
  echo "Make sure to change rcpthosts if you add hosts to locals or virtualdomains!"
  echo " "
  )

#********************************************************************************
# create control/smtpplugins
[ -f QMAIL/control/smtpplugins ] || \
( echo "Creating empty smtpplugins config file..."
printf "
# config file for qmail-spp\n#\n
# pathes are relativ to QMAIL\n#\n" > QMAIL/control/smtpplugins
printf "[connection]\n\n[helo]\n\n[mail]\n\n[rcpt]\n\n[data]\n\n#[auth]\n" \
       >> QMAIL/control/smtpplugins
chown root:qmail QMAIL/control/smtpplugins
chmod 644 QMAIL/control/smtpplugins	)

mkdir -p QMAIL/bin/plugins
chgrp qmail QMAIL/bin/plugins

# create config file for qmail-bfque
[ -f QMAIL/control/beforequeue ] || \
( echo "Creating config file beforequeue..."
printf "#!/bin/sh
# config file for qmail-bfqueue\n#\n
# Enter commands here, like

export QUEUE=\"\$BINDIR/qmail-queue\"
np=\"\$BINDIR/qmail-queue\" ; exec \$np \"\$@\"
"       >> QMAIL/control/beforequeue
chown root:qmail QMAIL/control/beforequeue
chmod 644 QMAIL/control/beforequeue     )

# create config file for qmail-bfrmt
[ -f QMAIL/control/beforemote ] || \
( echo "Creating config file beforemote..."
printf "/usr/local/qmail/bin/qmail-remote

See qmail-bfrmt(8), beforemote(5) and qmail picture for details.
"       >> QMAIL/control/beforemote
chown root:qmail QMAIL/control/beforemote
chmod 644 QMAIL/control/beforemote     )

#********************************************************************************
WITHTLS=$(cat conf.tmp | grep ^TLS=\"Yes\")
if [ "$WITHTLS" = "TLS=\"Yes\"" ] ; then
   # create servercert.cnf and TLS/SSL keys and certificates
   [ -f QMAIL/control/servercert.cnf ] || (
     cat servercert.cnf | sed s{localhost{"$FQDN"{g \
     > QMAIL/control/servercert.cnf
     chmod 640 QMAIL/control/servercert.cnf
     chown root:qmail QMAIL/control/servercert.cnf
     )
   mkdir -p QMAIL/control/tlshosts
   [ -f QMAIL/control/servercert.pem ] || ( \
     echo -e "\nCreating temporary keys and self-signed certificate ..."
     ./mkrsadhkeys
     ./mksrvrcerts
     )
fi
