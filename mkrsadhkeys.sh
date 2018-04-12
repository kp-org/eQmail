
# Update temporary RSA and DH keys
# Frederik Vermeulen 2004-05-31 GPL
# Modified by Kai Peter for eQmail-1.10, 2016-05-12

umask 0077 || exit 0

export PATH="$PATH:/usr/local/bin/ssl:/usr/sbin"

OPENSSL=$(which openssl 2>/dev/null)
if [ $? -ne 0 ] ; then
   echo "Couldn't find openssl! Aborting!" ; exit 0 ; fi

"$OPENSSL" genrsa -out QPRFX/etc/rsa2048.new 2048 &&
chmod 600 QPRFX/etc/rsa2048.new &&
chown UID:GID QPRFX/etc/rsa2048.new &&
mv -f QPRFX/etc/rsa2048.new QPRFX/etc/rsa2048.pem
echo

"$OPENSSL" dhparam -2 -out QPRFX/etc/dh2048.new 2048 &&
chmod 600 QPRFX/etc/dh2048.new &&
chown UID:GID QPRFX/etc/dh2048.new &&
mv -f QPRFX/etc/dh2048.new QPRFX/etc/dh2048.pem
