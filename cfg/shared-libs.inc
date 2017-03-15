#!/bin/sh
#********************************************************************************
# Checking for shared libraries                                                 *
#                                                                               *
# Results will be written to temp. files *.lib. These files will be used in the *
# Makefile to link the libs.                                                    *
#********************************************************************************

./compile trylib.c
# Linux has a libresolv, likely BSD systems does not
echo -n "Checking for libresolv ..."
((./load trylib -lresolv) >/dev/null 2>&1 && echo "-lresolv" || exit 0)> dns.lib
[ -s dns.lib ] || echo -n " not"
echo " found!"

# OpenBSD has 'eopenssl'
#if [ `grep -e openbsd systype` ] ; then
#  echo " -L/usr/local/lib/eopenssl" >> ssl.lib
#fi
[ `grep -e openbsd systype` ] && echo " -L/usr/local/lib/eopenssl" >> ssl.lib

rm -f trylib
