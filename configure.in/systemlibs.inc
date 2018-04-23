#!/bin/sh
#********************************************************************************
# Checking for shared libraries                                                 *
#                                                                               *
# Results will be written to temp. files *.lib. These files will be used in the *
# Makefile to link the libs.                                                    *
#********************************************************************************

L="resolv crypt ssl crypto dl"
./compile trylib.c

for SL in ${L}
do
  echo -n "checking for lib$SL ..."
  ((./load trylib -l"$SL") 2>/dev/null && echo -n "-l$SL" || exit 0) > "$SL".lib
  [ -s "$SL.lib" ] || echo -n " not"
  echo " found!"
done
rm -f trylib

# put libssl.so and libcrypto.so together
#[ -s ssl.lib ] && [ -s crypto.lib ] && echo `cat ssl.lib` `cat crypto.lib` > ssl.lib
# add libdl
#[ -s ssl.lib ] && [ -s dl.lib ] && echo `cat ssl.lib` `cat dl.lib` > ssl.lib
#rm -f crypto.lib dl.lib
