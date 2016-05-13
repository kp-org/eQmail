#!/bin/sh
#**********************************************************************
# Extract search parameters given with the '-I' option from 'conf-cc' *
# and put the values into file 'cc-inc.tmp', one per line.            *
# Used by the configure script in function 'searchHeaders'.           *
#**********************************************************************
rm -f cc-*.tmp ; i=1
for f in $(head -1 conf-cc | cut -d\- -f2-)
do
  echo $f | grep '^-[ID]' 2>&1 >/dev/null
  if [[ $? -eq "0" ]] ; then
     if [[ "$f" == "-I"* ]] ; then
        echo $(echo $f | cut -d- -f2 | cut -dI -f2) >> cc-inc.tmp
     fi
  fi
  i=$(($i+1))
done
