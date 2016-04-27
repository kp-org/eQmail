  DOMAIN=$(echo `hostname -f` | cut -d. -f2- | tr '[A-Z]' '[a-z]')
  HOST=$(echo `hostname -f` | cut -d. -f1 | tr '[A-Z]' '[a-z]')

  [ -f QMAIL/control/me ] || (
	echo Putting "$HOST.$DOMAIN" into control/me...
    echo "$HOST.$DOMAIN" > QMAIL/control/me
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
	echo Adding "$HOST.$DOMAIN" to control/locals...
	echo "$HOST.$DOMAIN" >> QMAIL/control/locals
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

. ./config-spp
. ./config-bfrmt
