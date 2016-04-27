#********************************************************************************
#
#echo -n "Checking users and groups"
#echo "int auto_uida = "$(id -u $(head -1 conf-users))";"            > auto_uids.c
UIDA="$(id -u $(head -1 conf-users))"
#echo "int auto_uidd = "$(id -u `head -2 conf-users | tail -1`)";"  >> auto_uids.c
UIDD="$(id -u `head -2 conf-users | tail -1`)"
#echo "int auto_uidl = "$(id -u `head -3 conf-users | tail -1`)";"  >> auto_uids.c
UIDL="$(id -u `head -3 conf-users | tail -1`)"
#echo "int auto_uido = "$(id -u `head -4 conf-users | tail -1`)";"  >> auto_uids.c
UIDO="$(id -u `head -4 conf-users | tail -1`)"
#echo "int auto_uidp = "$(id -u `head -5 conf-users | tail -1`)";"  >> auto_uids.c
UIDP="$(id -u `head -5 conf-users | tail -1`)"
#echo "int auto_uidq = "$(id -u `head -6 conf-users | tail -1`)";"  >> auto_uids.c
UIDQ="$(id -u `head -6 conf-users | tail -1`)"
#echo "int auto_uidr = "$(id -u `head -7 conf-users | tail -1`)";"  >> auto_uids.c
UIDR="$(id -u `head -7 conf-users | tail -1`)"
#echo "int auto_uids = "$(id -u `head -8 conf-users | tail -1`)";"  >> auto_uids.c
UIDS="$(id -u `head -8 conf-users | tail -1`)"

#echo "int auto_gidq = "$(getent group $(head -1 conf-groups) | cut -d: -f3)";"          >> auto_uids.c
GIDQ="$(getent group $(head -1 conf-groups) | cut -d: -f3)"
#echo "int auto_gidn = "$(getent group `head -2 conf-groups | tail -1` | cut -d: -f3)";" >> auto_uids.c
GIDN="$(getent group `head -2 conf-groups | tail -1` | cut -d: -f3)"

#echo " ... auto_uids.c created!"
