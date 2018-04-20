#!/bin/sh
#********************************************************************************
# package.inc - define package related preparations

. configure.in/compiled-in

# get the id's from conf-build

UIDA="$(id -u $USRA)"
UIDD="$(id -u $USRD)"
UIDL="$(id -u $USRL)"
UIDO="$(id -u $USRO)"
UIDP="$(id -u $USRP)"
UIDQ="$(id -u $USRQ)"
UIDR="$(id -u $USRR)"
UIDS="$(id -u $USRS)"

GIDQ="$(getent group $GRPS | cut -d: -f3)"
GIDN="$(getent group $GRPP | cut -d: -f3)"

#********************************************************************************
# create file 'auto_uids.c'
#. cfg/getids.inc
echo -n "Checking users and groups"
echo "int auto_uida = $UIDA;"  > auto_uids.c
echo "int auto_uidd = $UIDD;" >> auto_uids.c
echo "int auto_uidl = $UIDL;" >> auto_uids.c
echo "int auto_uido = $UIDO;" >> auto_uids.c
echo "int auto_uidp = $UIDP;" >> auto_uids.c
echo "int auto_uidq = $UIDQ;" >> auto_uids.c
echo "int auto_uidr = $UIDR;" >> auto_uids.c
echo "int auto_uids = $UIDS;" >> auto_uids.c
echo "int auto_gidq = $GIDQ;" >> auto_uids.c
echo "int auto_gidn = $GIDN;" >> auto_uids.c
echo " ... auto_uids.c created!"

# fot tls key files
echo "$USRD" > pemid.tmp
echo "$GRPS" >> pemid.tmp
