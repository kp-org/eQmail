
# Update temporary RSA and DH keys
# Frederik Vermeulen 2004-05-31 GPL
# Modified by Kai Peter for eQmail-1.10, 2016-05-12

umask 0077 || exit 0

export PATH="$PATH:/usr/local/bin/ssl:/usr/sbin"

OPENSSL=$(which openssl 2>/dev/null)
if [ $? -ne 0 ] ; then 
   echo "Couldn't find openssl! Aborting!" ; exit 0 ; fi

"$OPENSSL" genrsa -out QMAIL/control/rsa2048.new 2048 &&
chmod 600 QMAIL/control/rsa2048.new &&
chown UID.GID QMAIL/control/rsa2048.new &&
mv -f QMAIL/control/rsa2048.new QMAIL/control/rsa2048.pem
echo

"$OPENSSL" dhparam -2 -out QMAIL/control/dh2048.new 2048 &&
chmod 600 QMAIL/control/dh2048.new &&
chown UID.GID QMAIL/control/dh2048.new &&
mv -f QMAIL/control/dh2048.new QMAIL/control/dh2048.pem
