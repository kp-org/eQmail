
# some basics
DOMAIN=$(echo `hostname -f` | cut -d. -f2- | tr '[A-Z]' '[a-z]')
HOST=$(echo `hostname -f` | cut -d. -f1 | tr '[A-Z]' '[a-z]')
FQDN="$HOST"
if [ "$HOST" != "$DOMAIN" ] ; then FQDN="$HOST"."$DOMAIN" ; fi

[ -f QMAIL/control/me ] || (
  echo Putting "$FQDN" into control/me...
  echo "$FQDN" > QMAIL/control/me
  chmod 644 QMAIL/control/me
  )

[ -f QMAIL/control/defaultdomain ] || (
  echo Putting "$DOMAIN" into control/defaultdomain...
  echo "$DOMAIN" > QMAIL/control/defaultdomain
  chmod 644 QMAIL/control/defaultdomain
  )

[ -f QMAIL/control/plusdomain ] || (
  echo Putting "$DOMAIN" into control/plusdomain...
  echo "$DOMAIN" > QMAIL/control/plusdomain
  chmod 644 QMAIL/control/plusdomain
  )

[ -f QMAIL/control/locals ] || (
  echo ' '
  echo Adding "$FQDN" to control/locals...
  echo "$FQDN" >> QMAIL/control/locals
  )

echo ' '
echo If there are any other domain names that point to you,
echo you will have to add them to QMAIL/control/locals.
echo You don\'t have to worry about aliases, i.e., domains with CNAME records.
echo ' '

[ -f QMAIL/control/rcpthosts ] || (
  echo Copying QMAIL/control/locals to QMAIL/control/rcpthosts...
  cp QMAIL/control/locals QMAIL/control/rcpthosts
  chmod 644 QMAIL/control/rcpthosts
  )

echo 'Now qmail will refuse to accept SMTP messages except to those hosts.'
echo 'Make sure to change rcpthosts if you add hosts to locals or virtualdomains!'

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
printf "#!/bin/sh
# config file for qmail-bfrmt\n#\n
# Enter commands here, like

export DKREMOTE=\"\$BINDIR/qmail-remote\"
np=\"\$BINDIR/qmail-remote\" ; exec \$np \"\$@\"
"       >> QMAIL/control/beforemote
chown root:qmail QMAIL/control/beforemote
chmod 644 QMAIL/control/beforemote     )

#********************************************************************************
# create servercert.cnf and TLS/SSL keys and certificates
[ -f QMAIL/control/servercert.cnf ] || (
  cat servercert.cnf | sed s{localhost{"`hostname -f`"{g \
  > QMAIL/control/servercert.cnf
  chmod 640 QMAIL/control/servercert.cnf
  chown root:qmail QMAIL/control/servercert.cnf
#  echo
#  echo "Edit QMAIL/control/servercert.cnf before creating certificates."
  )
mkdir -p QMAIL/control/tlshosts
[ -f QMAIL/control/servercert.pem ] || ( \
  echo
  echo "Creating temporary keys and self-signed certificate ..."
  ./mkrsadhkeys
  ./mkservercerts
  )
