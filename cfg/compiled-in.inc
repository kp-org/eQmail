#!/bin/sh
#************************************************************************
# use 'conf-build' to consolidate all compiled-in values                *
#************************************************************************

oFN="buildins.c"
. ./conf-build

echo "char qmailhome[] = \"$QMAILDIR\";"      > $oFN
echo "char username_alias[] = \"$USRA\";"    >> $oFN
echo "char username_daemon[] = \"$USRD\";"   >> $oFN
echo "char username_log[] = \"$USRL\";"      >> $oFN
echo "char username_misc[] = \"$USRM\";"     >> $oFN
echo "char username_passwd[] = \"$USRP\";"   >> $oFN
echo "char username_queue[] = \"$USRQ\";"    >> $oFN
echo "char username_remote[] = \"$USRR\";"   >> $oFN
echo "char username_send[] = \"$USRS\";"     >> $oFN
echo "char grpname_qmail[] = \"$GRPS\";"     >> $oFN
echo "char grpname_nofiles[] = \"$GRPP\";"   >> $oFN
echo >> $oFN
echo "char break_delim[] = \"$BREAK\";"      >> $oFN
# paternalism
#p=`echo "ibase=8; \`head -1 ../conf-patrn\`" | bc`
p=`echo "ibase=8; $PATRN" | bc`
echo "int paternalism = $p;"                 >> $oFN
# silent spawn limit
fd_max=`expr \`ulimit -n\` - 4`
#fd_spn=`head -1 ../conf-spawn`
fd_spn=$SPAWN
[ $fd_spn -gt $fd_max ] && fd_spn=$fd_max
echo "int spawn_limit = $fd_spn;"            >> $oFN
echo "int queue_split = $SPLIT;"             >> $oFN
echo "char *queuedir = \"$QUEUEDIR\";"       >> $oFN
