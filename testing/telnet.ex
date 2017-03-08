#(echo ehlo ; sleep 1 ; auth login) | telnet xaero.aldox.org 25 | tee ttt.txt

(echo ehlo ; echo "MAIL FROM: kp@localhost" ; echo "RCPT TO: kp@aldox.de" ; sleep 1) | telnet xaero.aldox.org 25 | tee ttt.txt


#example qmail-smtpd (with CRLF's)
(echo ehlo ; sleep 1 ;echo "MAIL FROM: kp@wrack.aldox.de" ; echo "RCPT TO: kp@wrack.aldox.de" ; sleep 1 ; echo "DATA" ; sleep 1 ; printf "dddd\r\n\r\n.\r\n") | /var/qmail/test/bin/qmail-smtpd /bin/false
