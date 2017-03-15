#!/bin/sh
#********************************************************************************
# Checking conf-* files
for cf in $(ls conf-*)
do
  t=`echo $cf | cut -d- -f2`
  if [ ! "$t" = "cc" ] && [ ! "$t" = "ld" ] && \
     [ ! "$t" = "groups" ] && [ ! "$t" = "patrn" ] && [ ! "$t" = "opt" ] ; then
     echo -n "Checking config file $cf ... "
     # user alias
     if [ "$t" = "users" ] ; then t="usera" ; fi
     echo "char auto_$t[] = \""`head -1 $cf`"\";" > auto_"$t".c
     if [ $? -eq 0 ] ; then
        echo "auto_$t.c created!"
      else
        echo "failed!" ; exit 1
     fi
  fi
done

# check for compiler/linker options
head -1 conf-cc | grep -e '-DTLS' conf-cc >/dev/null
if [ $? -eq 0 ] ; then TLS="Yes" ; else TLS="No" ; fi
head -1 conf-cc | grep -e '-DINET6' conf-cc >/dev/null
if [ $? -eq 0 ] ; then IP6="Yes" ; else IP6="No" ; fi

# queue split
echo "int auto_split = `head -1 conf-split`;" > auto_split.c
# paternalism
p=`echo "ibase=8; \`head -1 conf-patrn\`" | bc`
echo "int auto_patrn = $p;" > auto_patrn.c
# silent spawn limit
fd_max=`expr \`ulimit -n\` - 4`
fd_spn=`head -1 conf-spawn`
if [ $fd_spn -gt $fd_max ] ; then fd_spn=$fd_max ; fi
echo "int auto_spawn = $fd_spn;" > auto_spawn.c

#********************************************************************************
# checking that users from conf-users are exists (excluding 'alias')
for i in 2 3 4 5 6 7 8
do
  id -u `head -$i conf-users | tail -1` 2>&1 >/dev/null
  if [ $? -ne 0 ] ; then echo "Create users first!" ; exit 1 ; fi
done

#********************************************************************************
# create file 'auto_uids.c'
. cfg/getids.inc
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

. ./cfg/save-conf.inc      # save values of conf-* files temporarily
