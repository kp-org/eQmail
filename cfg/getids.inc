#!/bin/sh
#************************************************************************
# Get users/groups from 'conf-{users,groups}'.  Used by the 'configure' *
# and 'install' scripts.                                                *
#************************************************************************
UIDA="$(id -u `head -1 conf-users | tail -1`)"
UIDD="$(id -u `head -2 conf-users | tail -1`)"
UIDL="$(id -u `head -3 conf-users | tail -1`)"
UIDO="$(id -u `head -4 conf-users | tail -1`)"
UIDP="$(id -u `head -5 conf-users | tail -1`)"
UIDQ="$(id -u `head -6 conf-users | tail -1`)"
UIDR="$(id -u `head -7 conf-users | tail -1`)"
UIDS="$(id -u `head -8 conf-users | tail -1`)"

GIDQ="$(getent group `head -1 conf-groups | tail -1` | cut -d: -f3)"
GIDN="$(getent group `head -2 conf-groups | tail -1` | cut -d: -f3)"
