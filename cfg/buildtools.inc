#!/bin/sh
#********************************************************************************
# Create compile, load, makelib

echo -n "Checking for compile ... "
CC=`head -1 conf-cc`"$CCOPTS"
(cat warn-auto.sh ; echo exec "$CC" -c '${1+"$@"}')  > compile
chmod 755 compile
echo "created!"

# load: auto-ld
echo -n "Checking for loader ... "
LD=`head -1 conf-ld`"$LDOPTS"
(cat warn-auto.sh ; echo 'main="$1"; shift' ; \
  echo exec "$LD" '-o "$main" "$main".o ${1+"$@"}') > load
chmod 755 load
echo "created!"

# makelib
echo -n "Checking for makelib ... "
(cat warn-auto.sh ; \
  echo 'main="$1"; shift' ; \
  echo 'rm -f "$main"' ; \
  echo 'ar cr "$main" ${1+"$@"}' ; \
  echo 'ranlib "$main"') > makelib
chmod 755 makelib
echo "created!"
