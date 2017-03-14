#!/bin/sh

TELNET=`which telnet`
OPENSSL=`which openssl`

exitcode() { echo Exitcode: $? ; echo ; }

SENDER="MAIL FROM: qmail-test@aldox.de"
RCPTTO="RCPT TO: kp1@gorre.aldox.de"

# test telnet
for Z in 127.0.0.1 ::1 localhost
do
  ( echo ehlo ; sleep 1 ; echo "$SENDER" ; echo "$RCPTTO" ; \
    echo "DATA" ; echo ; echo "test telnet $Z" ; echo "." ; echo "quit" ; \
    sleep 3 ) | "$TELNET" "$Z" 25 | grep qp
  exitcode
done

# test STARTTLS witi IPv4 and IPv6
for IPV in 4 6
do
  ( sleep 1 ; echo ehlo ; sleep 1 ; echo "$SENDER" ; echo "$RCPTTO" ; echo "DATA" ; \
    printf "\r\ntest tls 25 localhost IPv$IPV\r\n\r\n.\r\n" ; echo "quit" ; \
    sleep 2 ) | openssl s_client -$IPV -starttls smtp -connect localhost:25
  exitcode
  sleep 2
done

echo send with nullmailer from razor
ssh razor NULLMAILER_USER=qmail-test NULLMAILER_FLAGS=s \
sendmail kp1@gorre.aldox.de << EOF
from:qmail-test@razor.aldox.de

test from razor
EOF
exitcode

echo send with opensmtpd from sorlag
ssh sorlag  "echo from sorlag | sendmail -f qmail-test@aldox.de kp1@gorre.aldox.de"
exitcode


exit

#(echo ehlo ; sleep 1 ; echo "MAIL FROM: qmail-test@.aldox.de" ; \
#             echo "RCPT TO: kp1@gorre.aldox.de" ; \
#             echo "DATA" ; \
#             printf "\r\ntest tls 25 localhost IPv$IPV\r\n\r\n.\r\n " ; echo "quit" \
#           ) | openssl s_client -6 -starttls smtp -connect localhost:25
#exitcode

#exit
#             echo "test tls 25 localhost" ; echo "." ; echo "quit" \


#(echo ehlo ; sleep 1 ; echo "MAIL FROM: qmail-test@.aldox.de" ; \
#             echo "RCPT TO: kp1@gorre.aldox.de" ; \
#             echo "DATA" ; \
#             echo "test tls port 25 ipv4" ; echo "." ; echo "quit" \
#           ) | openssl s_client -4 -starttls smtp -connect localhost:25
#exitcode

#exit

#example qmail-smtpd (with CRLF's)
#(echo ehlo ; sleep 1 ;echo "MAIL FROM: kp@wrack.aldox.de" ; echo "RCPT TO: kp@wrack.aldox.de" ; sleep 1 ; echo "DATA" ; sleep 1 ; printf "dddd\r\n\r\n.\r\n") #| /var/qmail/test/bin/qmail-smtpd /bin/false
