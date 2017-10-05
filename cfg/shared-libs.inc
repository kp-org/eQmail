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

echo -n "Checking for libcrypt ..."
((./load trylib -lcrypt) >/dev/null 2>&1 && echo "-lcrypt" || exit 0)> crypt.lib
[ -s crypt.lib ] || echo -n " not"
echo " found!"

# check for libssl and libcrypto (both from openssl or a derivative)
echo -n "Checking for libssl ..."
((./load trylib -lssl) >/dev/null 2>&1 && echo "-lssl" || exit 0)> ssl.lib
[ -s ssl.lib ] && HASSSL=1 || echo -n " not"
echo " found!"

echo -n "Checking for libcrypto ..."
((./load trylib -lssl) >/dev/null 2>&1 && echo "-lcrypto" || exit 0)> crypto.lib
[ -s crypto.lib ] || echo -n " not"
echo " found!"

echo -n "Checking for libdl ..."
((./load trylib -lssl) >/dev/null 2>&1 && echo "-ldl" || exit 0)> dl.lib
[ -s dl.lib ] || echo -n " not"
echo " found!"

# put libssl.so and libcrypto.so together
[ -s ssl.lib ] && [ -s crypto.lib ] && echo `cat ssl.lib` `cat crypto.lib` > ssl.lib
# add libdl
[ -s ssl.lib ] && [ -s dl.lib ] && echo `cat ssl.lib` `cat dl.lib` > ssl.lib
rm -f crypto.lib dl.lib
unset HASSSL


#|| echo -n " not"
#[ -s crypto.lib ] || echo -n " not"


# OpenBSD has 'eopenssl'
#if [ `grep -e openbsd systype` ] ; then
#  echo " -L/usr/local/lib/eopenssl" >> ssl.lib
#fi
[ `grep -e openbsd systype` ] && echo " -L/usr/local/lib/eopenssl" >> ssl.lib

rm -f trylib
