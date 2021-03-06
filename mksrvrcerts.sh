
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
OPENSSL="OPENSSLBIN"
if [ ! -f "$OPENSSL" ] ; then echo "Couldn't find openssl! Aborting!"; exit 0; fi

CONFDIR="QPRFX/etc"
PEMFILE="$CONFDIR/servercert.pem"
RNDFILE="$CONFDIR/servercert.rnd"       # rand(om) file
CNFFILE="$CONFDIR/servercert.cnf"       # certificate config file

DAYS="365"        # days before certificate expires

echo -e "\033[1m"
if test -f "$PEMFILE" ; then
   echo "The file '$PEMFILE' exists already and will be overwritten!"
 else
echo "We are using defaults to create the certificates now."
echo "Please customize ${CNFFILE} and run mksrvcerts later again!"
fi
i=10
echo -e "Wait $i seconds to continue, or press \033[31mCTRL-C\033[37m to stop now."
printf "\033[0m\033[33m >>> " ; sleep 1
until [ $i -eq 0 ] ; do echo -n "$i " ; sleep 1 ; i=$(($i-1)) ; done
echo -e "\033[0m"

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
cd ${CONFDIR}
[ -L clientcert.pem ] || ln -s `basename ${PEMFILE}` clientcert.pem
cd $OLDPWD
rm -f $RNDFILE
