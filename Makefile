# Don't edit Makefile! Use conf-* for configuration.

SHELL=/bin/sh

COMPILE=./compile
LOAD=./load
MAKELIB=./makelib

# create libs (*.a)
# create object files (*.o)
# link files
# create man pages

default: conf libs obj it mans

clean: TARGETS
	rm -f *.o *.a *.tmp `cat TARGETS`
	@cd qlibs ; make clean
	@cd man ; make clean

conf: configure
	./configure

# 'make install' will not work here usually ...
install: setup
# ... so we route 'install' to target 'setup'
setup:
	@./install

mans:
	@echo Creating man pages ...
	@cd man/ ; make

libs:
	cd qlibs ; make

obj:
	$(COMPILE) hfield.c
	$(COMPILE) maildir.c

auto_break.o:
	$(COMPILE) auto_break.c

auto_patrn.o:
	$(COMPILE) auto_patrn.c

auto_qmail.o:
	$(COMPILE) auto_qmail.c

auto_spawn.o:
	$(COMPILE) auto_spawn.c

auto_split.o:
	$(COMPILE) auto_split.c

auto_uids.o:
	$(COMPILE) auto_uids.c

auto_usera.o:
	$(COMPILE) auto_usera.c

bouncesaying:
	$(COMPILE) bouncesaying.c
	$(LOAD) bouncesaying errmsg.a buffer.a qstring.a wait.a

buildins.o:
	$(COMPILE) buildins.c

commands.o: compile commands.c
	./compile commands.c

condredirect:
	$(COMPILE) condredirect.c
	$(LOAD) condredirect qmail.o strerr.a fd.a sig.a wait.a \
	seek.a env.a buffer.a error.a str.a fs.a auto_qmail.o alloc.a substdio.a

mkconfig: \
warn-auto.sh mkconfig.sh conf-home conf-break conf-split
	cat warn-auto.sh mkconfig.sh \
	| sed s}QMAIL}"`head -1 conf-home`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g \
	> mkconfig
	chmod 755 mkconfig

constmap.o: compile constmap.c
	./compile constmap.c

control.o: compile control.c
	./compile control.c

date822fmt.o: compile date822fmt.c
	./compile date822fmt.c

datemail: \
warn-auto.sh datemail.sh conf-qmail conf-break conf-split
	cat warn-auto.sh datemail.sh \
	| sed s}QMAILHOME}"`head -1 conf-qmail`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g \
	> datemail
	chmod 755 datemail

datetime.a: compile makelib datetime.c
	./compile datetime.c
	./makelib datetime.a datetime.o

dns.o: compile dns.c
	./compile dns.c

except:
	$(COMPILE) except.c
	$(LOAD) except strerr.a error.a buffer.a str.a wait.a

fmtqfn.o: compile fmtqfn.c
	./compile fmtqfn.c

forward:
	$(COMPILE) forward.c
	$(LOAD) forward qmail.o errmsg.a fd.a wait.a sig.a \
	env.a buffer.a error.a str.a fs.a auto_qmail.o substdio.a
#	 alloc.a

gfrom.o: compile gfrom.c
	./compile gfrom.c

hassalen.h: \
trysalen.c compile
	( ./compile trysalen.c >/dev/null 2>&1 \
	&& echo \#define HASSALEN 1 || exit 0 ) > hassalen.h
#	rm -f trysalen.o

# needed by wait.o (wait_pid)
#haswaitp.h: trywaitp.c compile load
#	( ( ./compile trywaitp.c && ./load trywaitp ) >/dev/null 2>&1 \
#	&& echo \#define HASWAITPID 1 || exit 0 ) > haswaitp.h
##	rm -f trywaitp.o trywaitp

headerbody.o: compile headerbody.c
	./compile headerbody.c

#hfield.o: compile hfield.c
#	./compile hfield.c

#ip.o: compile ip.c
#	./compile ip.c

ipalloc.o: compile ipalloc.c
	./compile ipalloc.c

ipme.o: compile ipme.c
	./compile ipme.c

ipmeprint: compile load ipmeprint.c ipme.o ip.a ipalloc.o \
stralloc.a alloc.a substdio.a error.a str.a fs.a
	./compile ipmeprint.c
	./load ipmeprint ipme.o ip.a ipalloc.o stralloc.a alloc.a \
	substdio.a error.a str.a fs.a

it: all

all: \
qmail-local qmail-lspawn qmail-getpw qmail-remote qmail-rspawn \
qmail-clean qmail-send qmail-start splogger qmail-queue qmail-inject \
predate datemail mailsubj qmail-newu qmail-print \
qmail-pw2u qmail-qread qmail-qstat qmail-tcpto qmail-tcpok \
qmail-qmqpc qmail-qmqpd qmail-qmtpd \
qmail-smtpd sendmail tcp-env qmail-newmrh mkconfig \
qmail-tcpsrv tcprules \
qreceipt \
forward preline condredirect bouncesaying except maildirmake \
maildir2mbox \
qmail-before ipmeprint \
mkrsadhkeys mksrvrcerts qmail-fixq qmail-shcfg

maildir2mbox:
	$(COMPILE) maildir2mbox.c
	$(LOAD) maildir2mbox maildir.o prioq.o now.o myctime.o \
	gfrom.o lock.a getln.a env.a open.a strerr.a stralloc.a \
	alloc.a buffer.a error.a str.a fs.a datetime.a

maildirmake: compile \
load maildirmake.c strerr.a substdio.a error.a str.a
	./compile maildirmake.c
	./load maildirmake strerr.a buffer.a substdio.a error.a str.a

mailsubj: warn-auto.sh mailsubj.sh conf-qmail conf-break conf-split
	cat warn-auto.sh mailsubj.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g \
	> mailsubj
	chmod 755 mailsubj

myctime.o: compile myctime.c
	./compile myctime.c

newfield.o: compile newfield.c
	./compile newfield.c

now.o: compile now.c
	./compile now.c

predate:
	$(COMPILE) predate.c
	$(LOAD) predate datetime.a strerr.a sig.a fd.a wait.a \
	buffer.a error.a str.a fs.a

preline:
	$(COMPILE) preline.c
	$(LOAD) preline strerr.a fd.a wait.a sig.a env.a \
	getopt.a buffer.a error.a str.a alloc.a

prioq.o: compile prioq.c
	./compile prioq.c

qmail-before: qmail-before.sh
	cat warn-auto.sh qmail-before.sh \
	| sed s}QMAILHOME}"`head -1 conf-qmail`"}g \
	| sed s}CFGFILE}"beforequeue"}g \
	| sed s}PROGRAM}"qmail-queue"}g > qmail-bfque
	cat warn-auto.sh qmail-before.sh \
	| sed s}QMAILHOME}"`head -1 conf-qmail`"}g \
	| sed s}CFGFILE}"beforemote"}g \
	| sed s}PROGRAM}"qmail-remote"}g > qmail-bfrmt
	chmod 754 qmail-bfque qmail-bfrmt
	chgrp qmail qmail-bfque qmail-bfrmt

qmail-clean: auto_split.o fmtqfn.o
	$(COMPILE) qmail-clean.c
	$(LOAD) qmail-clean fmtqfn.o now.o getln.a sig.a stralloc.a \
	alloc.a buffer.a error.a str.a fs.a auto_qmail.o auto_split.o

qmail-fixq: qmail-fixq.sh
	sh ./qmail-fixq.sh

qmail-getpw: auto_break.o auto_usera.o
	$(COMPILE) qmail-getpw.c
	$(LOAD) qmail-getpw case.a buffer.a error.a str.a fs.a \
	auto_break.o auto_usera.o

qmail-inject: compile load qmail-inject.c headerbody.o hfield.o \
token822.o
#newfield.o quote.o now.o control.o date822fmt.o constmap.o qmail.o \
#case.a fd.a wait.a open.a getln.a sig.a getopt.a datetime.a  \
#env.a stralloc.a alloc.a substdio.a error.a str.a fs.a auto_qmail.o
	$(COMPILE) qmail-inject.c
	./load qmail-inject headerbody.o hfield.o newfield.o quote.o now.o \
	control.o date822fmt.o constmap.o qmail.o case.a fd.a wait.a open.a \
	getln.a sig.a getopt.a datetime.a token822.o env.a stralloc.a \
	alloc.a buffer.a error.a str.a fs.a auto_qmail.o strerr.a substdio.a

qmail-local: compile load qmail-local.c qmail.o quote.o now.o gfrom.o \
myctime.o slurpclose.o  datetime.a auto_qmail.o auto_patrn.o
	$(COMPILE) qmail-local.c
	./load qmail-local qmail.o quote.o now.o gfrom.o myctime.o \
	slurpclose.o case.a getln.a getopt.a sig.a open.a seek.a lock.a \
	buffer.a \
	fd.a wait.a env.a stralloc.a alloc.a strerr.a \
	error.a str.a fs.a datetime.a auto_qmail.o auto_patrn.o \
	substdio.a

qmail-lspawn: compile load qmail-lspawn.c spawn.o prot.o slurpclose.o \
sig.a wait.a case.a cdb.a fd.a open.a stralloc.a alloc.a \
substdio.a error.a str.a fs.a auto_qmail.o auto_uids.o auto_spawn.o
	./compile qmail-lspawn.c
	./load qmail-lspawn spawn.o prot.o slurpclose.o sig.a wait.a \
	case.a cdb.a fd.a open.a stralloc.a alloc.a substdio.a error.a \
	str.a fs.a auto_qmail.o auto_uids.o auto_spawn.o

qmail-newmrh:
	$(COMPILE) qmail-newmrh.c
	$(LOAD) qmail-newmrh cdb.a getln.a open.a \
	seek.a case.a stralloc.a alloc.a strerr.a buffer.a \
	error.a str.a auto_qmail.o

qmail-newu:
	$(COMPILE) qmail-newu.c
	$(LOAD) qmail-newu cdb.a getln.a open.a seek.a buffer.a \
	case.a stralloc.a alloc.a error.a str.a auto_qmail.o

qmail-print: buildins.o
	$(COMPILE) qmail-print.c
	$(LOAD) qmail-print errmsg.a stralloc.a str.a fs.a buildins.o buffer.a

qmail-pw2u:
	$(COMPILE) qmail-pw2u.c
	$(LOAD) qmail-pw2u constmap.o control.o open.a getln.a \
	case.a getopt.a stralloc.a alloc.a buffer.a error.a str.a \
	fs.a auto_usera.o auto_break.o auto_qmail.o substdio.a

qmail-qmqpc: \
load qmail-qmqpc.o slurpclose.o timeoutread.o timeoutwrite.o \
timeoutconn.o ip.a control.o auto_qmail.o sig.a ndelay.a open.a \
getln.a substdio.a stralloc.a alloc.a error.a str.a fs.a
	./load qmail-qmqpc slurpclose.o timeoutread.o \
	timeoutwrite.o timeoutconn.o ip.a control.o auto_qmail.o \
	sig.a ndelay.a open.a getln.a substdio.a stralloc.a alloc.a \
	error.a str.a fs.a buffer.a

qmail-qmqpc.o: \
compile qmail-qmqpc.c
	./compile qmail-qmqpc.c

qmail-qmqpd: \
load qmail-qmqpd.o received.o now.o date822fmt.o qmail.o auto_qmail.o \
env.a substdio.a sig.a error.a wait.a fd.a str.a datetime.a fs.a
	./load qmail-qmqpd received.o now.o date822fmt.o qmail.o \
	auto_qmail.o env.a substdio.a sig.a error.a wait.a fd.a \
	str.a datetime.a fs.a alloc.a

qmail-qmqpd.o: compile qmail-qmqpd.c
	./compile qmail-qmqpd.c

qmail-qmtpd: \
load qmail-qmtpd.o rcpthosts.o control.o constmap.o received.o \
date822fmt.o now.o qmail.o cdb.a fd.a wait.a datetime.a open.a \
getln.a sig.a case.a env.a stralloc.a alloc.a substdio.a error.a \
str.a fs.a auto_qmail.o
	./load qmail-qmtpd rcpthosts.o control.o constmap.o \
	received.o date822fmt.o now.o qmail.o cdb.a fd.a wait.a \
	datetime.a open.a getln.a sig.a case.a env.a stralloc.a \
	alloc.a substdio.a error.a str.a fs.a auto_qmail.o buffer.a

qmail-qmtpd.o: compile qmail-qmtpd.c
	./compile qmail-qmtpd.c

qmail-qread: compile load qmail-qread.c fmtqfn.o readsubdir.o date822fmt.o \
datetime.a open.a getln.a stralloc.a alloc.a substdio.a error.a str.a fs.a \
auto_qmail.o auto_split.o
	./compile qmail-qread.c
	./load qmail-qread fmtqfn.o readsubdir.o date822fmt.o \
	datetime.a open.a getln.a stralloc.a alloc.a substdio.a \
	error.a str.a fs.a auto_qmail.o auto_split.o buffer.a

qmail-qstat: warn-auto.sh qmail-qstat.sh conf-qmail conf-break conf-split
	cat warn-auto.sh qmail-qstat.sh \
	| sed s}QMAILHOME}"`head -1 conf-qmail`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g > qmail-qstat
	chmod 755 qmail-qstat

qmail-queue: compile load qmail-queue.c triggerpull.o fmtqfn.o now.o \
date822fmt.o datetime.a seek.a ndelay.a open.a sig.a alloc.a substdio.a \
error.a str.a fs.a auto_qmail.o auto_split.o auto_uids.o
	./compile qmail-queue.c
	./load qmail-queue triggerpull.o fmtqfn.o now.o date822fmt.o \
	datetime.a seek.a ndelay.a open.a sig.a alloc.a substdio.a \
	error.a str.a fs.a auto_qmail.o auto_split.o auto_uids.o 

qmail-remote: compile load qmail-remote.c control.o constmap.o timeoutread.o \
timeoutwrite.o timeoutconn.o tcpto.o now.o dns.o ip.a ipalloc.o ipme.o quote.o \
ndelay.a case.a sig.a open.a lock.a seek.a getln.a stralloc.a alloc.a substdio.a \
error.a str.a fs.a auto_qmail.o base64.o dns.lib tls.o ssl_timeoutio.o
	./compile qmail-remote.c
	./load qmail-remote control.o constmap.o timeoutread.o \
	timeoutwrite.o timeoutconn.o tcpto.o now.o dns.o ip.a \
	tls.o ssl_timeoutio.o -L/usr/local/ssl/lib -lssl -lcrypto \
	ipalloc.o ipme.o quote.o ndelay.a case.a sig.a open.a \
	lock.a seek.a getln.a stralloc.a alloc.a substdio.a error.a \
	str.a fs.a auto_qmail.o  base64.o `cat dns.lib` \
	buffer.a

#qmail-rspawn: compile load qmail-rspawn.c spawn.o tcpto_clean.o now.o \
#sig.a open.a seek.a lock.a wait.a fd.a stralloc.a alloc.a substdio.a error.a \
#str.a auto_qmail.o auto_uids.o auto_spawn.o
#	./compile qmail-rspawn.c
#	./load qmail-rspawn spawn.o tcpto_clean.o now.o \
#	sig.a open.a seek.a lock.a wait.a fd.a stralloc.a alloc.a \
#	substdio.a error.a stralloc.a str.a auto_qmail.o auto_uids.o auto_spawn.o

qmail-rspawn: compile load qmail-rspawn.c spawn.o tcpto_clean.o now.o
#sig.a open.a seek.a lock.a wait.a fd.a stralloc.a alloc.a substdio.a error.a \
#str.a auto_qmail.o auto_uids.o auto_spawn.o
	./compile qmail-rspawn.c
	./load qmail-rspawn spawn.o tcpto_clean.o now.o \
	sig.a open.a seek.a lock.a wait.a fd.a stralloc.a \
	errmsg.a buffer.a qstring.a substdio.a \
	error.a stralloc.a auto_qmail.o auto_uids.o auto_spawn.o

qmail-send: auto_split.o date822fmt.o newfield.o prioq.o qsutil.o readsubdir.o trigger.o
# compile load qmail-send.c control.o constmap.o \
# fmtqfn.o quote.o now.o qmail.o date822fmt.o \
#datetime.a case.a ndelay.a getln.a wait.a seek.a fd.a sig.a open.a lock.a \
#stralloc.a alloc.a substdio.a error.a str.a fs.a auto_qmail.o env.a
	./compile qmail-send.c
	./load qmail-send qsutil.o control.o constmap.o newfield.o \
	prioq.o trigger.o fmtqfn.o quote.o now.o readsubdir.o \
	qmail.o date822fmt.o datetime.a case.a ndelay.a getln.a \
	wait.a seek.a fd.a sig.a open.a lock.a stralloc.a alloc.a \
	buffer.a error.a str.a fs.a auto_qmail.o auto_split.o env.a \
	substdio.a


#	| sed s}QMAIL}"`head -1 conf-qmail`"}g \

qmail-shcfg: conf.tmp qmail-shcfg.sh warn-auto.sh
	@echo build $@
	@cat warn-auto.sh qmail-shcfg.sh | sed s}QMAILHOME}"`head -1 conf-home`"}g > qmail-shcfg
	@chmod 0755 qmail-shcfg

qmail-spp.o: compile qmail-spp.c
	./compile qmail-spp.c

qmail-smtpd: compile load qmail-smtpd.c rcpthosts.o commands.o \
timeoutread.o timeoutwrite.o ip.a ipme.o ipalloc.o control.o constmap.o \
received.o date822fmt.o now.o qmail.o cdb.a fd.a wait.a datetime.a \
getln.a open.a sig.a case.a env.a stralloc.a alloc.a substdio.a \
error.a str.a fs.a auto_qmail.o base64.o qmail-spp.o \
tls.o ssl_timeoutio.o ndelay.a
	./compile qmail-smtpd.c
	./load qmail-smtpd rcpthosts.o commands.o timeoutread.o \
	timeoutwrite.o ip.a ipme.o ipalloc.o control.o constmap.o \
	tls.o ssl_timeoutio.o ndelay.a -L/usr/local/ssl/lib -lssl -lcrypto \
	received.o date822fmt.o now.o qmail.o cdb.a fd.a wait.a \
	datetime.a getln.a open.a sig.a case.a qmail-spp.o env.a stralloc.a \
	alloc.a substdio.a error.a str.a fs.a auto_qmail.o base64.o buffer.a

qmail-start: compile load qmail-start.c prot.o fd.a auto_uids.o
	./compile qmail-start.c
	./load qmail-start prot.o fd.a auto_uids.o

qmail-tcpok: compile load qmail-tcpok.c open.a lock.a strerr.a \
substdio.a error.a str.a auto_qmail.o
	./compile qmail-tcpok.c
	./load qmail-tcpok open.a lock.a strerr.a substdio.a \
	error.a str.a auto_qmail.o buffer.a

qmail-tcpto: compile load qmail-tcpto.c ip.a now.o open.a lock.a \
substdio.a error.a str.a fs.a auto_qmail.o
	./compile qmail-tcpto.c
	./load qmail-tcpto ip.a now.o open.a lock.a substdio.a \
	error.a str.a fs.a auto_qmail.o 

qmail.o: compile qmail.c
	./compile qmail.c

qreceipt: compile load qreceipt.c headerbody.o hfield.o quote.o \
token822.o qmail.o getln.a fd.a wait.a sig.a env.a stralloc.a \
alloc.a substdio.a error.a str.a auto_qmail.o
	./compile qreceipt.c
	./load qreceipt headerbody.o hfield.o quote.o token822.o \
	qmail.o getln.a fd.a wait.a sig.a env.a stralloc.a alloc.a \
	substdio.a error.a str.a auto_qmail.o buffer.a

qsutil.o: compile qsutil.c
	./compile qsutil.c

quote.o: compile quote.c
	./compile quote.c

rcpthosts.o: compile rcpthosts.c 
	./compile rcpthosts.c

readsubdir.o: compile readsubdir.c
	./compile readsubdir.c

received.o: compile received.c
	./compile received.c

remoteinfo.o: compile remoteinfo.c
	./compile remoteinfo.c

sendmail:
	$(COMPILE) sendmail.c
	$(LOAD) sendmail env.a getopt.a alloc.a buffer.a \
	error.a str.a auto_qmail.o

spawn.o:
	$(COMPILE) spawn.c

splogger: compile load splogger.c substdio.a error.a str.a fs.a
	./compile splogger.c
	./load splogger substdio.a error.a str.a fs.a

tcp-env: compile load tcp-env.c dns.o remoteinfo.o timeoutread.o \
timeoutwrite.o timeoutconn.o ip.a ipalloc.o case.a ndelay.a sig.a \
env.a stralloc.a alloc.a substdio.a error.a str.a fs.a dns.lib hassalen.h
	./compile tcp-env.c
	./load tcp-env dns.o remoteinfo.o timeoutread.o \
	timeoutwrite.o timeoutconn.o ip.a ipalloc.o case.a ndelay.a \
	sig.a env.a getopt.a stralloc.a alloc.a substdio.a error.a \
	str.a fs.a `cat dns.lib` buffer.a

qmail-envars: libs remoteinfo6.o timeoutconn6.o
	./compile -g qmail-envars.c
	./load qmail-envars byte.o fmt.o qlibs/scan.o \
	dns.a ip.a case.a env.a alloc.a sig.a str.a stralloc.a \
	readclose.o time.a qlibs/uint16p.o qlibs/uint32p.o \
	open.a getopt.a errmsg.a buffer.a \
	remoteinfo6.o timeoutconn6.o socket.a ndelay.a

tcpto.o: compile tcpto.c
	./compile tcpto.c

tcpto_clean.o: compile tcpto_clean.c
	./compile tcpto_clean.c

timeoutconn.o: compile timeoutconn.c
	./compile timeoutconn.c

timeoutread.o: compile timeoutread.c
	./compile timeoutread.c

timeoutwrite.o: compile timeoutwrite.c
	./compile timeoutwrite.c

tls.o: compile tls.c
	./compile tls.c

ssl_timeoutio.o: compile ssl_timeoutio.c
	./compile ssl_timeoutio.c

token822.o: compile token822.c
	./compile token822.c

trigger.o: compile trigger.c
	./compile trigger.c

triggerpull.o: compile triggerpull.c
	./compile triggerpull.c

mkrsadhkeys: \
conf-qmail conf-users conf-groups mkrsadhkeys.sh warn-auto.sh
	@cat warn-auto.sh mkrsadhkeys.sh\
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	| sed s}UID}"`head -2 conf-users | tail -1`"}g \
	| sed s}GID}"`head -1 conf-groups`"}g \
	> $@
	@echo creating $@
	@chmod 755 $@

mksrvrcerts: mksrvrcerts.sh conf-users conf-groups warn-auto.sh
	@cat warn-auto.sh mksrvrcerts.sh \
	| sed s}OPENSSLBIN}"`head -1 ssl.bin`"}g \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	| sed s}UID}"`head -2 conf-users | tail -1`"}g \
	| sed s}GID}"`head -1 conf-groups`"}g \
	> $@
	@echo creating $@
	@chmod 755 $@

#*****************************************************************
# qmail-tcpsrv
remoteinfo6.o:
	$(COMPILE) remoteinfo6.c

timeoutconn6.o:
	$(COMPILE) timeoutconn6.c

rules.o:
	$(COMPILE) rules.c

tcprules: rules.o
	$(COMPILE) tcprules.c
	$(LOAD) tcprules rules.o fmt.o strerr.a qlibs/scan.o \
	alloc.a buffer.a case.a cdb.a error.a getln.a open.a \
	str.a stralloc.a \
	buffer.a

qmail-tcpsrv: rules.o remoteinfo6.o timeoutconn6.o
	$(COMPILE) qmail-tcpsrv.c
	$(LOAD) qmail-tcpsrv rules.o remoteinfo6.o timeoutconn6.o \
	readclose.o pathexec.o \
	alloc.a buffer.a case.a cdb.a dns.a error.a env.a fd.a fmt.o \
	getopt.a ip.a ndelay.a open.a prot.o qlibs/scan.o sig.a \
	str.a stralloc.a strerr.a time.a wait.a socket.a \
	qlibs/uint16p.o \
	case.a \
	qlibs/errstr.o
