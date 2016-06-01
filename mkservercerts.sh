
#********************************************************************************
# mkservercerts                                                                 #
#                                                                               #
#  Author: Kai Peter, ©2013 (kp@openqmail.org)                                  #
#                                                                               #
# Version: 0.2                                                                  #
# Licence: This program is part of eQmail, see license information of eQmail.   #
#                                                                               #
# Description: Generate self-signed certificates for eQmail.  Initially taken   #
#              from Gentoo, by itself based on mkimapdcert from courier-imap.   #
#********************************************************************************
OPENSSL=$(which openssl 2>/dev/null)
if [ $? -ne 0 ] ; then echo "couldn't find openssl! Aborting!" ; exit 0 ; fi

CONFDIR="QMAIL/control"
PEMFILE="$CONFDIR/servercert.pem"
RNDFILE="$CONFDIR/servercert.rnd"       # rand(om) file
CNFFILE="$CONFDIR/servercert.cnf"       # certificate config file

DAYS="365"        # days before certificate expires

if test -f "$PEMFILE" ; then
	read -n 1 -p "$PEMFILE already exists. Overwrite (y/N)?: " -s C
    if [ $C != "y" ] ; then	echo -e "\nAborting!" ; exit 1 ; fi
fi

echo ; echo
echo "Please customize ${CNFFILE} before continuing!"
echo "Press ENTER to continue, or any other key to stop now."
read -r -n 1 -s C
if [ $C ] ; then exit ; fi

cleanup() { rm -f ${PEMFILE}; rm -f ${RNDFILE}; exit 1; }

dd if=/dev/urandom of=${RNDFILE} bs=64 count=1 2>/dev/null
chmod 600 ${RNDFILE}
echo "Creating self-signed certificate"
"$OPENSSL" req -new -x509 -days ${DAYS} -nodes \
	-config ${CNFFILE} -out ${PEMFILE} -keyout ${PEMFILE} -rand ${RNDFILE} || cleanup
echo "Certificate details"
"$OPENSSL" x509 -subject -dates -fingerprint -noout -in ${PEMFILE} || cleanup
chown UID:GID ${PEMFILE}
chmod 0640 ${PEMFILE}
[ -L ${CONFDIR}/clientcert.pem ] || ln -s ${PEMFILE} ${CONFDIR}/clientcert.pem
rm -f $RNDFILE
