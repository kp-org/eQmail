#!/bin/sh
#********************************************************************************
# Check for external tools
# 1. required tools: break if not found
# 2. optional tools: continue if not found (but deactivate related features)
# 3. multiple tools: use first out of multiple
#********************************************************************************

requiredTools() {
  local f
  for f in ${RT}
  do
	echo -n "Checking for $f ... "
	which $f 2>/dev/null
	if [ $? -ne 0 ] ; then echo "not found!" ; exit 1 ; fi
  done
}

# check for optional tools
optionalTools() {
  local f
  for f in ${OT}
  do
	echo -n "Checking for $f ... "
	which $f 2>/dev/null
	if [ $? -ne 0 ] ; then echo "not found! Ignoring!"
	else echo `which "$f"` > _has.$f.tmp ; fi
  done
}
# check for system tools (required) !!!
RT="sh env sh [ test make perl awk"
requiredTools "$RT"

OT="svc start-stop-daemon tcpserver xinetd php"
optionalTools "$OT"

# UNUSED at the moment !!!!
#********************************************************************
# check for one of multiple tools
multis() {
rm _MISSED.tmp 2>/dev/null
#AT="\\"ip ifconfig ip\\"\\ \\"nslookup dig\\""
MT="ip ipconfig"

#for f in ${AT}
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
}

