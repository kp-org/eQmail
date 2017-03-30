#!/bin/sh
#********************************************************************************
# Check for external tools

# check for system tools (required) !!!
for f in bash hostname [ test make perl awk
do
  echo -n "Checking for $f ... "
  which $f 2>/dev/null
  if [ $? -ne 0 ] ; then echo "not found!" ; exit 1 ; fi
done

# check for one of multiple tools
rm _MISSED.tmp 2>/dev/null
for f in "ip ifconfig ip" "nslookup dig"
do
  [ -f _MISSED.tmp ] && break
  echo "$f" > _MISSED.tmp
  echo $f | awk '{ for(i = 1; i <= NF; i++) { print $i; } }' | while IFS= read T
  do
    echo -n "Checking for $T ... "
    which $T 2>/dev/null
    if [ $? -ne 0 ] ; then echo "not found!"
    else echo `which "$T"` > _has.$T.tmp ; rm _MISSED.tmp ; break ; fi
  done
done
[ -f _MISSED.tmp ] && (echo ; echo "Couldn't find "`cat _MISSED.tmp`"! Aborting")
# devel: uncomment next line to abort configure
#[ -f _MISSED.tmp ] && rm _MISSED.tmp && exit

# check for optional tools
for f in svc start-stop-daemon tcpserver xinetd php
do
  echo -n "Checking for $f ... "
  which $f 2>/dev/null
  if [ $? -ne 0 ] ; then echo "not found! Ignoring!"
  else echo `which "$f"` > _has.$f.tmp ; fi
done
