#/bin/sh

TELNET=`which telnet`
OPENSSL=`which openssl`


exitcode() {
echo Exitcode: $?
sleep 3
}

CMD="(echo ehlo ; sleep 1)"
#CMD=(echo ehlo ; sleep 1 ;echo \"MAIL FROM: kp@wrack.aldox.de\" ; echo \"RCPT TO: kp@gorre.aldox.de\" ; sleep 1 ; echo \"DATA\" ; sleep 1 ; printf \"dddd\r\n\r\n.\r\n\")
 #| /var/qmail/test/bin/qmail-smtpd /bin/false

#echo $CMD
#$CMD | telnet 127.0.0.1 25
#exitcode

(echo ehlo ; sleep 1 ; echo "MAIL FROM: qmail-test@.aldox.de" ; \
             echo "RCPT TO: kp1@gorre.aldox.de" ; \
             echo "DATA" ; \
             echo "test telnet 127.0.0.1" ; echo "." ; echo "quit" \
           ) | nc localhost 25
exitcode
exit
(echo ehlo ; sleep 1 ; echo "MAIL FROM: qmail-test@.aldox.de" ; \
             echo "RCPT TO: kp1@gorre.aldox.de" ; \
             echo "DATA" ; \
             echo "test telnet 127.0.0.1" ; echo "." ; echo "quit" \
           ) | telnet 127.0.0.1 25
exitcode

#                       printf "test from localhost\r\n\r\n.\r\n" ) \

(echo ehlo ; sleep 1 ; echo "MAIL FROM: qmail-test@.aldox.de" ; \
             echo "RCPT TO: kp1@gorre.aldox.de" ; \
             echo "DATA" ; \
             echo "test telnet ::1" ; echo "." ; echo "quit" \
           ) | telnet ::1 25
exitcode

(echo ehlo ; sleep 1 ; echo "MAIL FROM: qmail-test@.aldox.de" ; \
             echo "RCPT TO: kp1@gorre.aldox.de" ; \
             echo "DATA" ; \
             printf "test tls 25 localhost\r\n\r\n.\r\n"
           ) | openssl s_client -starttls smtp -connect localhost:25
exitcode

exit
#             echo "test tls 25 localhost" ; echo "." ; echo "quit" \


(echo ehlo ; sleep 1 ; echo "MAIL FROM: qmail-test@.aldox.de" ; \
             echo "RCPT TO: kp1@gorre.aldox.de" ; \
             echo "DATA" ; \
             echo "test tls 25 ::1" ; echo "." ; echo "quit" \
           ) | openssl s_client -starttls smtp -connect ::1:25
exitcode
exit

#example qmail-smtpd (with CRLF's)
#(echo ehlo ; sleep 1 ;echo "MAIL FROM: kp@wrack.aldox.de" ; echo "RCPT TO: kp@wrack.aldox.de" ; sleep 1 ; echo "DATA" ; sleep 1 ; printf "dddd\r\n\r\n.\r\n") #| /var/qmail/test/bin/qmail-smtpd /bin/false
