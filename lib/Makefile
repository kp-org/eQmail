# Don't edit Makefile! Use conf-* for configuration.

SHELL=/bin/sh

# create libs (*.a)
# create object files (*.o)
# link (some) files

CFLAGS=-I.. -I../inc
SRC=*.c

default: libs

clean:
	rm -f *.o *.a

libs: obj alloc.a case.a env.a error.a fd.a fs.a ndelay.a open.a str.a
	@echo -n Moving .o files ...
	@mv byte.o coe.o fmt.o \
	..
	@echo done!
#	../compile $(CFLAGS) $(OBJ)
#	../makelib alloc.a alloc.o

obj: ../compile $(SRC)
	@echo Making all in lib ...
	../compile $(CFLAGS) $(SRC)

alloc.a: ../compile ../makelib alloc.c
	../makelib ../alloc.a alloc.o

case.a: ../compile ../makelib case.c
	../makelib ../case.a case.o

env.a: ../compile ../makelib
	../makelib ../env.a env.o envread.o

error.a: ../compile ../makelib error.c error_str.c error_temp.c
	../makelib ../error.a error.o error_str.o error_temp.o

fd.a: ../makelib fd.c
	../makelib ../fd.a fd.o

fs.a: ../makelib fmt.o scan.o
	../makelib ../fs.a fmt.o scan.o

#lock.a: compile makelib lock.c hasflock.h
#	./compile lock.c
#	./makelib lock.a lock.o

ndelay.a: ../makelib ndelay.c
	../makelib ../ndelay.a ndelay.o

open.a: ../makelib open.c
	../makelib ../open.a open.o


str.a: ../makelib str.c byte.o
	../makelib ../str.a str.o byte.o

#fifo.o: compile fifo.c hasmkffo.h fifo.h
#	./compile fifo.c

#fmt.o: compile fmt.c
#	./compile fmt.c

#byte.o: ../compile byte.c
#	../compile $(CFLAGS) byte.c

#scan.o: compile scan.c scan.h
#	./compile scan.c

#********************************************
auto-gid: \
compile load auto-gid.c substdio.a error.a str.a fs.a
	./compile auto-gid.c
	./load auto-gid substdio.a error.a str.a fs.a 

auto-int8: \
load auto-int8.o substdio.a error.a str.a fs.a
	./load auto-int8 substdio.a error.a str.a fs.a 

auto-int8.o: \
compile auto-int8.c substdio.h exit.h scan.h fmt.h
	./compile auto-int8.c

auto_break.o: compile auto_break.c
	./compile auto_break.c

auto_patrn.c: \
auto-int8 conf-patrn
	./auto-int8 auto_patrn `head -1 conf-patrn` > auto_patrn.c

auto_patrn.o: \
compile auto_patrn.c
	./compile auto_patrn.c

auto_qmail.o: compile auto_qmail.c
	./compile auto_qmail.c

auto_spawn.o: compile auto_spawn.c
	./compile auto_spawn.c

auto_split.o: compile auto_split.c
	./compile auto_split.c

auto_uids.o: compile auto_uids.c
	./compile auto_uids.c

auto_usera.o: compile auto_usera.c
	./compile auto_usera.c

base64.o: \
compile base64.c base64.h stralloc.h substdio.h str.h
	./compile base64.c

bouncesaying: \
load bouncesaying.o strerr.a error.a substdio.a str.a wait.a
	./load bouncesaying strerr.a error.a substdio.a str.a \
	wait.a 

bouncesaying.o: compile bouncesaying.c
	./compile bouncesaying.c



cdb.a: \
makelib cdb_hash.o cdb_unpack.o cdb_seek.o
	./makelib cdb.a cdb_hash.o cdb_unpack.o cdb_seek.o

cdb_hash.o: compile cdb_hash.c cdb.h 
	./compile cdb_hash.c

cdb_seek.o: compile cdb_seek.c cdb.h 
	./compile cdb_seek.c

cdb_unpack.o: compile cdb_unpack.c cdb.h 
	./compile cdb_unpack.c

cdbmake.a: \
makelib cdbmake_pack.o cdbmake_hash.o cdbmake_add.o
	./makelib cdbmake.a cdbmake_pack.o cdbmake_hash.o \
	cdbmake_add.o

cdbmake_add.o: compile cdbmake_add.c
# cdbmake.h alloc.h 
	./compile cdbmake_add.c

cdbmake_hash.o: compile cdbmake_hash.c cdbmake.h 
	./compile cdbmake_hash.c

cdbmake_pack.o: compile cdbmake_pack.c cdbmake.h 
	./compile cdbmake_pack.c

cdbmss.o: compile cdbmss.c seek.h
# alloc.h cdbmss.h cdbmake.h
	./compile cdbmss.c

#check: it man
#	./instcheck

chkshsgr: \
load chkshsgr.o
	./load chkshsgr 

chkshsgr.o: \
compile chkshsgr.c exit.h
	./compile chkshsgr.c

chkspawn: \
load chkspawn.o substdio.a error.a str.a fs.a auto_spawn.o
	./load chkspawn substdio.a error.a str.a fs.a auto_spawn.o 

chkspawn.o: \
compile chkspawn.c 
#substdio.h subfd.h substdio.h fmt.h select.h \
#exit.h auto_spawn.h
	./compile chkspawn.c


commands.o: \
compile commands.c commands.h substdio.h stralloc.h gen_alloc.h str.h \
case.h
	./compile commands.c

condredirect: \
load condredirect.o qmail.o strerr.a fd.a sig.a wait.a seek.a env.a \
substdio.a error.a str.a fs.a auto_qmail.o
	./load condredirect qmail.o strerr.a fd.a sig.a wait.a \
	seek.a env.a substdio.a error.a str.a fs.a auto_qmail.o 

condredirect.o: \
compile condredirect.c sig.h exit.h env.h error.h \
wait.h seek.h qmail.h substdio.h strerr.h fmt.h
	./compile condredirect.c

config: \
warn-auto.sh config.sh conf-qmail conf-break conf-split
	cat warn-auto.sh config.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g \
	> config
	chmod 755 config

config-fast: \
warn-auto.sh config-fast.sh conf-qmail conf-break conf-split
	cat warn-auto.sh config-fast.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g \
	> config-fast
	chmod 755 config-fast

config-bfrmt: warn-auto.sh config-bfrmt.sh conf-qmail
	cat warn-auto.sh config-bfrmt.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g > config-bfrmt
	chmod 755 config-bfrmt

config-spp: warn-auto.sh config-spp.sh conf-qmail
	cat warn-auto.sh config-spp.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g > config-spp
	chmod 755 config-spp

constmap.o: compile constmap.c
	./compile constmap.c

control.o: compile control.c
	./compile control.c

date822fmt.o: \
compile date822fmt.c datetime.h fmt.h date822fmt.h
	./compile date822fmt.c

datemail: \
warn-auto.sh datemail.sh conf-qmail conf-break conf-split
	cat warn-auto.sh datemail.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g \
	> datemail
	chmod 755 datemail

datetime.a: \
makelib datetime.o datetime_un.o
	./makelib datetime.a datetime.o datetime_un.o

datetime.o: \
compile datetime.c datetime.h
	./compile datetime.c

datetime_un.o: \
compile datetime_un.c datetime.h
	./compile datetime_un.c

dns.lib: \
tryrsolv.c compile load socket.lib dns.o ipalloc.o ip.o stralloc.a \
alloc.a error.a fs.a str.a
	( ( ./compile tryrsolv.c && ./load tryrsolv dns.o \
	ipalloc.o ip.o stralloc.a alloc.a error.a fs.a str.a \
	-lresolv `cat socket.lib` ) >/dev/null 2>&1 \
	&& echo -lresolv || exit 0 ) > dns.lib
	rm -f tryrsolv.o tryrsolv

dns.o: compile dns.c
	./compile dns.c

dnscname: \
load dnscname.o dns.o dnsdoe.o ip.o ipalloc.o stralloc.a alloc.a \
substdio.a error.a str.a fs.a dns.lib socket.lib
	./load dnscname dns.o dnsdoe.o ip.o ipalloc.o stralloc.a \
	alloc.a substdio.a error.a str.a fs.a  `cat dns.lib` `cat \
	socket.lib`

dnscname.o: compile dnscname.c
	./compile dnscname.c

dnsdoe.o: \
compile dnsdoe.c substdio.h subfd.h substdio.h exit.h dns.h dnsdoe.h
	./compile dnsdoe.c

dnsfq: \
load dnsfq.o dns.o dnsdoe.o ip.o ipalloc.o stralloc.a alloc.a \
substdio.a error.a str.a fs.a dns.lib socket.lib
	./load dnsfq dns.o dnsdoe.o ip.o ipalloc.o stralloc.a \
	alloc.a substdio.a error.a str.a fs.a  `cat dns.lib` `cat \
	socket.lib`

dnsfq.o: compile dnsfq.c
	./compile dnsfq.c

dnsip: \
load dnsip.o dns.o dnsdoe.o ip.o ipalloc.o stralloc.a alloc.a \
substdio.a error.a str.a fs.a dns.lib socket.lib
	./load dnsip dns.o dnsdoe.o ip.o ipalloc.o stralloc.a \
	alloc.a substdio.a error.a str.a fs.a  `cat dns.lib` `cat \
	socket.lib`

dnsip.o: \
compile dnsip.c substdio.h subfd.h substdio.h stralloc.h gen_alloc.h \
dns.h dnsdoe.h ip.h ipalloc.h ip.h gen_alloc.h exit.h
	./compile dnsip.c

dnsmxip: \
load dnsmxip.o dns.o dnsdoe.o ip.o ipalloc.o now.o stralloc.a alloc.a \
substdio.a error.a str.a fs.a dns.lib socket.lib
	./load dnsmxip dns.o dnsdoe.o ip.o ipalloc.o now.o \
	stralloc.a alloc.a substdio.a error.a str.a fs.a  `cat \
	dns.lib` `cat socket.lib`

dnsmxip.o: \
compile dnsmxip.c substdio.h subfd.h substdio.h stralloc.h \
gen_alloc.h fmt.h dns.h dnsdoe.h ip.h ipalloc.h ip.h gen_alloc.h \
now.h datetime.h exit.h
	./compile dnsmxip.c

dnsptr: \
load dnsptr.o dns.o dnsdoe.o ip.o ipalloc.o stralloc.a alloc.a \
substdio.a error.a str.a fs.a dns.lib socket.lib
	./load dnsptr dns.o dnsdoe.o ip.o ipalloc.o stralloc.a \
	alloc.a substdio.a error.a str.a fs.a  `cat dns.lib` `cat \
	socket.lib`

dnsptr.o: \
compile dnsptr.c substdio.h subfd.h substdio.h stralloc.h gen_alloc.h \
str.h scan.h dns.h dnsdoe.h ip.h exit.h
	./compile dnsptr.c

elq: \
warn-auto.sh elq.sh conf-qmail conf-break conf-split
	cat warn-auto.sh elq.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g \
	> elq
	chmod 755 elq

except: load except.o strerr.a error.a substdio.a str.a wait.a
	./load except strerr.a error.a substdio.a str.a wait.a

except.o: compile except.c
	./compile except.c

fmtqfn.o: \
compile fmtqfn.c fmtqfn.h fmt.h auto_split.h
	./compile fmtqfn.c

getln.a: \
makelib getln.o getln2.o
	./makelib getln.a getln.o getln2.o

getln.o: \
compile getln.c substdio.h byte.h stralloc.h gen_alloc.h getln.h
	./compile getln.c

getln2.o: \
compile getln2.c substdio.h stralloc.h gen_alloc.h byte.h getln.h
	./compile getln2.c

getopt.a: \
makelib subgetopt.o sgetopt.o
	./makelib getopt.a subgetopt.o sgetopt.o

gfrom.o: \
compile gfrom.c str.h gfrom.h
	./compile gfrom.c

# needed by lock.o
hasflock.h: tryflock.c compile load
	( ( ./compile tryflock.c && ./load tryflock ) >/dev/null 2>&1 \
	&& echo \#define HASFLOCK 1 || exit 0 ) > hasflock.h
	rm -f tryflock.o tryflock

hasmkffo.h: trymkffo.c compile load
	( ( ./compile trymkffo.c && ./load trymkffo ) >/dev/null \
	2>&1 \
	&& echo \#define HASMKFIFO 1 || exit 0 ) > hasmkffo.h
#	rm -f trymkffo.o trymkffo

# needed by trigger.o
hasnpbg1.h: trynpbg1.c compile load open.a fifo.o
	( ( ./compile trynpbg1.c \
	&& ./load trynpbg1 fifo.o open.a && ./trynpbg1 ) >/dev/null 2>&1 \
	&& echo \#define HASNAMEDPIPEBUG1 1 || exit 0 ) > hasnpbg1.h
#	rm -f trynpbg1.o trynpbg1

hassalen.h: \
trysalen.c compile
	( ./compile trysalen.c >/dev/null 2>&1 \
	&& echo \#define HASSALEN 1 || exit 0 ) > hassalen.h
#	rm -f trysalen.o

# needed by sig.o (sig_catch)
hassgact.h: trysgact.c compile load
	( ( ./compile trysgact.c && ./load trysgact ) >/dev/null 2>&1 \
	&& echo \#define HASSIGACTION 1 || exit 0 ) > hassgact.h
#	rm -f trysgact.o trysgact

# needed by sig.o (sig_pause, sig_block)
hassgprm.h: trysgprm.c compile load
	( ( ./compile trysgprm.c && ./load trysgprm ) >/dev/null 2>&1 \
	&& echo \#define HASSIGPROCMASK 1 || exit 0 ) > hassgprm.h
#	rm -f trysgprm.o trysgprm

hasshsgr.h: \
chkshsgr warn-shsgr tryshsgr.c compile load
	./chkshsgr || ( cat warn-shsgr; exit 1 )
	( ( ./compile tryshsgr.c \
	&& ./load tryshsgr && ./tryshsgr ) >/dev/null 2>&1 \
	&& echo \#define HASSHORTSETGROUPS 1 || exit 0 ) > \
	hasshsgr.h
#	rm -f tryshsgr.o tryshsgr

# needed by wait.o (wait_pid)
haswaitp.h: trywaitp.c compile load
	( ( ./compile trywaitp.c && ./load trywaitp ) >/dev/null 2>&1 \
	&& echo \#define HASWAITPID 1 || exit 0 ) > haswaitp.h
#	rm -f trywaitp.o trywaitp

headerbody.o: \
compile headerbody.c stralloc.h gen_alloc.h substdio.h getln.h \
hfield.h headerbody.h
	./compile headerbody.c

hfield.o: \
compile hfield.c hfield.h
	./compile hfield.c

hier.o: \
compile hier.c auto_qmail.h auto_split.h auto_uids.h fmt.h fifo.h
	./compile hier.c

hostname: \
load hostname.o substdio.a error.a str.a dns.lib socket.lib
	./load hostname substdio.a error.a str.a  `cat dns.lib` \
	`cat socket.lib`

hostname.o: \
compile hostname.c substdio.h subfd.h exit.h
	./compile hostname.c

idedit: load idedit.o strerr.a substdio.a error.a str.a fs.a wait.a open.a seek.a
	./load idedit strerr.a substdio.a error.a str.a fs.a wait.a open.a seek.a

idedit.o: compile idedit.c
	./compile idedit.c

install: \
load install.o fifo.o hier.o auto_qmail.o auto_split.o auto_uids.o \
strerr.a substdio.a open.a error.a str.a fs.a
	./load install fifo.o hier.o auto_qmail.o auto_split.o \
	auto_uids.o strerr.a substdio.a open.a error.a str.a fs.a 

install-big: \
load install-big.o fifo.o install.o auto_qmail.o auto_split.o \
auto_uids.o strerr.a substdio.a open.a error.a str.a fs.a
	./load install-big fifo.o install.o auto_qmail.o \
	auto_split.o auto_uids.o strerr.a substdio.a open.a error.a \
	str.a fs.a 

install-big.o: \
compile install-big.c auto_qmail.h auto_split.h auto_uids.h fmt.h \
fifo.h
	./compile install-big.c

install.o: \
compile install.c substdio.h strerr.h error.h open.h exit.h
	./compile install.c

instcheck: \
load instcheck.o fifo.o hier.o auto_qmail.o auto_split.o auto_uids.o \
strerr.a 
	./load instcheck fifo.o hier.o auto_qmail.o auto_split.o \
	auto_uids.o strerr.a substdio.a str.a fs.a error.a

instcheck.o: compile instcheck.c 
	./compile instcheck.c

ip.o: \
compile ip.c fmt.h scan.h ip.h
	./compile ip.c

ipalloc.o: compile ipalloc.c
	./compile ipalloc.c

ipme.o: \
compile ipme.c hassalen.h byte.h ip.h ipalloc.h ip.h gen_alloc.h \
stralloc.h gen_alloc.h ipme.h ip.h ipalloc.h
	./compile ipme.c

ipmeprint: \
load ipmeprint.o ipme.o ip.o ipalloc.o stralloc.a alloc.a substdio.a \
error.a str.a fs.a socket.lib
	./load ipmeprint ipme.o ip.o ipalloc.o stralloc.a alloc.a \
	substdio.a error.a str.a fs.a  `cat socket.lib`

ipmeprint.o: compile ipmeprint.c
	./compile ipmeprint.c

maildir.o: compile maildir.c 
	./compile maildir.c

maildir2mbox: \
load maildir2mbox.o maildir.o prioq.o now.o myctime.o gfrom.o lock.a \
getln.a env.a open.a strerr.a stralloc.a alloc.a substdio.a error.a \
str.a fs.a datetime.a
	./load maildir2mbox maildir.o prioq.o now.o myctime.o \
	gfrom.o lock.a getln.a env.a open.a strerr.a stralloc.a \
	alloc.a substdio.a error.a str.a fs.a datetime.a 

maildir2mbox.o: compile maildir2mbox.c
	./compile maildir2mbox.c

maildirmake: \
load maildirmake.o strerr.a substdio.a error.a str.a
	./load maildirmake strerr.a substdio.a error.a str.a 

maildirmake.o: \
compile maildirmake.c strerr.h exit.h
	./compile maildirmake.c

maildirwatch: \
load maildirwatch.o hfield.o headerbody.o maildir.o prioq.o now.o \
getln.a env.a open.a strerr.a stralloc.a alloc.a substdio.a error.a \
str.a
	./load maildirwatch hfield.o headerbody.o maildir.o \
	prioq.o now.o getln.a env.a open.a strerr.a stralloc.a \
	alloc.a substdio.a error.a str.a 

maildirwatch.o: \
compile maildirwatch.c getln.h substdio.h subfd.h substdio.h prioq.h \
datetime.h gen_alloc.h stralloc.h gen_alloc.h str.h exit.h hfield.h \
open.h headerbody.h maildir.h strerr.h
	./compile maildirwatch.c

mailsubj: \
warn-auto.sh mailsubj.sh conf-qmail conf-break conf-split
	cat warn-auto.sh mailsubj.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g \
	> mailsubj
	chmod 755 mailsubj

#man: dot-qmail.5 qmail-bfrmt.8 qmail-control.5 qmail-getpw.8 \
#qmail-limits.7 qmail-newu.8 qmail-newmrh.8 qmail-pw2u.8 \
#qmail-send.8 qmail-start.8 qmail-users.5 smtpplugins.5

myctime.o: \
compile myctime.c datetime.h fmt.h myctime.h
	./compile myctime.c

newfield.o: \
compile newfield.c fmt.h datetime.h stralloc.h gen_alloc.h \
date822fmt.h newfield.h stralloc.h
	./compile newfield.c

now.o: compile now.c
# datetime.h now.h datetime.h
	./compile now.c

pinq: \
warn-auto.sh pinq.sh conf-qmail conf-break conf-split
	cat warn-auto.sh pinq.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g \
	> pinq
	chmod 755 pinq

predate: \
load predate.o datetime.a strerr.a sig.a fd.a wait.a substdio.a \
error.a str.a fs.a
	./load predate datetime.a strerr.a sig.a fd.a wait.a \
	substdio.a error.a str.a fs.a 

predate.o: \
compile predate.c datetime.h wait.h fd.h fmt.h strerr.h \
substdio.h subfd.h exit.h
	./compile predate.c

preline: \
load preline.o strerr.a fd.a wait.a sig.a env.a getopt.a substdio.a \
error.a str.a
	./load preline strerr.a fd.a wait.a sig.a env.a getopt.a \
	substdio.a error.a str.a 

preline.o: \
compile preline.c fd.h sgetopt.h subgetopt.h strerr.h \
substdio.h exit.h wait.h env.h sig.h error.h
	./compile preline.c

prioq.o: compile prioq.c
	./compile prioq.c

prot.o: \
compile prot.c hasshsgr.h prot.h
	./compile prot.c

qail: \
warn-auto.sh qail.sh conf-qmail conf-break conf-split
	cat warn-auto.sh qail.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g \
	> qail
	chmod 755 qail

qbiff: \
load qbiff.o headerbody.o hfield.o getln.a env.a open.a stralloc.a \
alloc.a substdio.a error.a str.a
	./load qbiff headerbody.o hfield.o getln.a env.a open.a \
	stralloc.a alloc.a substdio.a error.a str.a 

qbiff.o: \
compile qbiff.c stralloc.h gen_alloc.h subfd.h \
substdio.h open.h byte.h str.h headerbody.h hfield.h env.h exit.h
	./compile qbiff.c

qmail-bfrmt: qmail-bfrmt.sh
	cat warn-auto.sh qmail-bfrmt.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g > qmail-bfrmt
	chmod 755 qmail-bfrmt && \
	chgrp qmail qmail-bfrmt

qmail-clean: \
load qmail-clean.o fmtqfn.o now.o getln.a sig.a stralloc.a alloc.a \
substdio.a error.a str.a fs.a auto_qmail.o auto_split.o
	./load qmail-clean fmtqfn.o now.o getln.a sig.a stralloc.a \
	alloc.a substdio.a error.a str.a fs.a auto_qmail.o \
	auto_split.o 

qmail-clean.o: \
compile qmail-clean.c sig.h now.h datetime.h str.h \
substdio.h byte.h scan.h fmt.h error.h exit.h fmtqfn.h auto_qmail.h
	./compile qmail-clean.c

qmail-getpw: \
load qmail-getpw.o case.a substdio.a error.a str.a fs.a auto_break.o \
auto_usera.o
	./load qmail-getpw case.a substdio.a error.a str.a fs.a \
	auto_break.o auto_usera.o 

qmail-getpw.o: compile qmail-getpw.c 
#substdio.h subfd.h \
#error.h exit.h byte.h str.h case.h fmt.h auto_usera.h auto_break.h \
#qlx.h
	./compile qmail-getpw.c

qmail-inject: \
load qmail-inject.o headerbody.o hfield.o newfield.o quote.o now.o \
control.o date822fmt.o constmap.o qmail.o case.a fd.a wait.a open.a \
getln.a sig.a getopt.a datetime.a token822.o env.a stralloc.a alloc.a \
substdio.a error.a str.a fs.a auto_qmail.o
	./load qmail-inject headerbody.o hfield.o newfield.o \
	quote.o now.o control.o date822fmt.o constmap.o qmail.o \
	case.a fd.a wait.a open.a getln.a sig.a getopt.a datetime.a \
	token822.o env.a stralloc.a alloc.a substdio.a error.a \
	str.a fs.a auto_qmail.o 

qmail-inject.o: compile qmail-inject.c
	./compile qmail-inject.c

qmail-local: \
load qmail-local.o qmail.o quote.o now.o gfrom.o myctime.o \
slurpclose.o case.a getln.a getopt.a sig.a open.a seek.a lock.a fd.a \
wait.a env.a stralloc.a alloc.a strerr.a substdio.a error.a str.a \
fs.a datetime.a auto_qmail.o auto_patrn.o socket.lib
	./load qmail-local qmail.o quote.o now.o gfrom.o myctime.o \
	slurpclose.o case.a getln.a getopt.a sig.a open.a seek.a \
	lock.a fd.a wait.a env.a stralloc.a alloc.a strerr.a \
	substdio.a error.a str.a fs.a datetime.a auto_qmail.o \
	auto_patrn.o  `cat socket.lib`

qmail-local.o: compile qmail-local.c
	./compile qmail-local.c

qmail-lspawn: \
load qmail-lspawn.o spawn.o prot.o slurpclose.o coe.o sig.a wait.a \
case.a cdb.a fd.a open.a stralloc.a alloc.a substdio.a error.a str.a \
fs.a auto_qmail.o auto_uids.o auto_spawn.o
	./load qmail-lspawn spawn.o prot.o slurpclose.o coe.o \
	sig.a wait.a case.a cdb.a fd.a open.a stralloc.a alloc.a \
	substdio.a error.a str.a fs.a auto_qmail.o auto_uids.o \
	auto_spawn.o 

qmail-lspawn.o: \
compile qmail-lspawn.c fd.h wait.h prot.h substdio.h stralloc.h \
gen_alloc.h scan.h exit.h error.h cdb.h case.h \
slurpclose.h auto_qmail.h auto_uids.h qlx.h
	./compile qmail-lspawn.c
 
qmail-newu: \
load qmail-newu.o cdbmss.o getln.a open.a seek.a cdbmake.a case.a \
stralloc.a alloc.a substdio.a error.a str.a auto_qmail.o
	./load qmail-newu cdbmss.o getln.a open.a seek.a cdbmake.a \
	case.a stralloc.a alloc.a substdio.a error.a str.a \
	auto_qmail.o 

qmail-newu.o: compile qmail-newu.c
	./compile qmail-newu.c

qmail-qmqpc: \
load qmail-qmqpc.o slurpclose.o timeoutread.o timeoutwrite.o \
timeoutconn.o ip.o control.o auto_qmail.o sig.a ndelay.a open.a \
getln.a substdio.a stralloc.a alloc.a error.a str.a fs.a socket.lib
	./load qmail-qmqpc slurpclose.o timeoutread.o \
	timeoutwrite.o timeoutconn.o ip.o control.o auto_qmail.o \
	sig.a ndelay.a open.a getln.a substdio.a stralloc.a alloc.a \
	error.a str.a fs.a  `cat socket.lib`

qmail-qmqpc.o: \
compile qmail-qmqpc.c substdio.h getln.h exit.h \
stralloc.h gen_alloc.h slurpclose.h error.h sig.h ip.h timeoutconn.h \
timeoutread.h timeoutwrite.h auto_qmail.h control.h fmt.h
	./compile qmail-qmqpc.c

qmail-qmqpd: \
load qmail-qmqpd.o received.o now.o date822fmt.o qmail.o auto_qmail.o \
env.a substdio.a sig.a error.a wait.a fd.a str.a datetime.a fs.a
	./load qmail-qmqpd received.o now.o date822fmt.o qmail.o \
	auto_qmail.o env.a substdio.a sig.a error.a wait.a fd.a \
	str.a datetime.a fs.a 

qmail-qmqpd.o: compile qmail-qmqpd.c

qmail-queue: \
load qmail-queue.o triggerpull.o fmtqfn.o now.o date822fmt.o \
datetime.a seek.a ndelay.a open.a sig.a alloc.a substdio.a error.a \
str.a fs.a auto_qmail.o auto_split.o auto_uids.o
	./load qmail-queue triggerpull.o fmtqfn.o now.o \
	date822fmt.o datetime.a seek.a ndelay.a open.a sig.a \
	alloc.a substdio.a error.a str.a fs.a auto_qmail.o \
	auto_split.o auto_uids.o 

qmail-showctl: \
load qmail-showctl.o auto_uids.o control.o open.a getln.a stralloc.a \
alloc.a substdio.a error.a str.a fs.a auto_qmail.o auto_break.o \
auto_patrn.o auto_spawn.o auto_split.o
	./load qmail-showctl auto_uids.o control.o open.a getln.a \
	stralloc.a alloc.a substdio.a error.a str.a fs.a \
	auto_qmail.o auto_break.o auto_patrn.o auto_spawn.o \
	auto_split.o 

qmail-showctl.o: \
compile qmail-showctl.c substdio.h subfd.h substdio.h exit.h fmt.h \
auto_uids.h auto_qmail.h auto_break.h auto_patrn.h auto_spawn.h \
auto_split.h
	./compile qmail-showctl.c

qmail-spp.o: \
compile qmail-spp.c stralloc.h substdio.h control.h str.h \
byte.h env.h exit.h wait.h fd.h fmt.h getln.h
	./compile qmail-spp.c

qmail-start: load qmail-start.o prot.o fd.a auto_uids.o
	./load qmail-start prot.o fd.a auto_uids.o

qmail-start.o: compile qmail-start.c 
	./compile qmail-start.c

qmail-tcpok: \
load qmail-tcpok.o open.a lock.a strerr.a substdio.a error.a str.a \
auto_qmail.o
	./load qmail-tcpok open.a lock.a strerr.a substdio.a \
	error.a str.a auto_qmail.o 

qmail-tcpok.o: \
compile qmail-tcpok.c strerr.h substdio.h lock.h open.h \
auto_qmail.h exit.h
	./compile qmail-tcpok.c

qmail-tcpto: \
load qmail-tcpto.o ip.o now.o open.a lock.a substdio.a error.a str.a \
fs.a auto_qmail.o
	./load qmail-tcpto ip.o now.o open.a lock.a substdio.a \
	error.a str.a fs.a auto_qmail.o 

qmail-tcpto.o: compile qmail-tcpto.c
	./compile qmail-tcpto.c

qmail-upq: \
warn-auto.sh qmail-upq.sh conf-qmail conf-break conf-split
	cat warn-auto.sh qmail-upq.sh \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	| sed s}BREAK}"`head -1 conf-break`"}g \
	| sed s}SPLIT}"`head -1 conf-split`"}g \
	> qmail-upq
	chmod 755 qmail-upq

rcpthosts.o: compile rcpthosts.c 
	./compile rcpthosts.c

readsubdir.o: compile readsubdir.c
	./compile readsubdir.c

received.o: \
compile received.c fmt.h qmail.h substdio.h now.h datetime.h \
datetime.h date822fmt.h received.h
	./compile received.c

remoteinfo.o: \
compile remoteinfo.c byte.h substdio.h ip.h fmt.h timeoutconn.h \
timeoutread.h timeoutwrite.h remoteinfo.h
	./compile remoteinfo.c

seek.a: makelib seek.o
	./makelib seek.a seek.o

seek.o: compile seek.c
	./compile seek.c

sendmail: \
load sendmail.o env.a getopt.a alloc.a substdio.a error.a str.a \
auto_qmail.o
	./load sendmail env.a getopt.a alloc.a substdio.a error.a \
	str.a auto_qmail.o 

sendmail.o: compile sendmail.c
	./compile sendmail.c

#setup: it man
#	./install

sgetopt.o: compile sgetopt.c
	./compile sgetopt.c

sig.a: makelib sig.o
	./makelib sig.a sig.o

# the "hassg*.h" files will be created dynamically
sig.o: compile sig.c \
hassgprm.h hassgact.h
	./compile sig.c

slurpclose.o: \
compile slurpclose.c stralloc.h gen_alloc.h slurpclose.h \
error.h
	./compile slurpclose.c

socket.lib: \
trylsock.c compile load
	( ( ./compile trylsock.c && \
	./load trylsock -lsocket -lnsl ) >/dev/null 2>&1 \
	&& echo -lsocket -lnsl || exit 0 ) > socket.lib
	rm -f trylsock.o trylsock

spawn.o: compile chkspawn spawn.c
	./chkspawn
	./compile spawn.c

splogger: \
load splogger.o substdio.a error.a str.a fs.a syslog.lib socket.lib
	./load splogger substdio.a error.a str.a fs.a  `cat \
	syslog.lib` `cat socket.lib`

splogger.o: \
compile splogger.c error.h substdio.h subfd.h substdio.h exit.h str.h \
scan.h fmt.h
	./compile splogger.c


stralloc.a: makelib stralloc.o
	./makelib stralloc.a stralloc.o

stralloc.o: compile stralloc.c 
	./compile stralloc.c

strerr.a: \
makelib strerr_sys.o strerr_die.o
	./makelib strerr.a strerr_sys.o strerr_die.o

strerr_die.o: \
compile strerr_die.c substdio.h subfd.h substdio.h exit.h strerr.h
	./compile strerr_die.c

strerr_sys.o: \
compile strerr_sys.c error.h strerr.h
	./compile strerr_sys.c

subfderr.o: compile subfderr.c
	./compile subfderr.c

subfdin.o: compile subfdin.c
	./compile subfdin.c

subfdins.o: compile subfdins.c
	./compile subfdins.c

subfdout.o: compile subfdout.c
	./compile subfdout.c

subfdouts.o: compile subfdouts.c
	./compile subfdouts.c

subgetopt.o: compile subgetopt.c
	./compile subgetopt.c

substdi.o: compile substdi.c
	./compile substdi.c

substdio.a: makelib substdio.o substdi.o substdo.o substdio_copy.o \
subfderr.o subfdout.o subfdouts.o subfdin.o subfdins.o
	./makelib substdio.a substdio.o substdi.o substdo.o subfderr.o \
	subfdout.o subfdouts.o subfdin.o subfdins.o substdio_copy.o

substdio.o: compile substdio.c
	./compile substdio.c

substdio_copy.o: compile substdio_copy.c
	./compile substdio_copy.c

substdo.o: compile substdo.c
	./compile substdo.c

syslog.lib: \
trysyslog.c compile load
	( ( ./compile trysyslog.c && \
	./load trysyslog -lgen ) >/dev/null 2>&1 \
	&& echo -lgen || exit 0 ) > syslog.lib
#	rm -f trysyslog.o trysyslog

tcp-env: \
load tcp-env.o dns.o remoteinfo.o timeoutread.o timeoutwrite.o \
timeoutconn.o ip.o ipalloc.o case.a ndelay.a sig.a env.a getopt.a \
stralloc.a alloc.a substdio.a error.a str.a fs.a dns.lib socket.lib
	./load tcp-env dns.o remoteinfo.o timeoutread.o \
	timeoutwrite.o timeoutconn.o ip.o ipalloc.o case.a ndelay.a \
	sig.a env.a getopt.a stralloc.a alloc.a substdio.a error.a \
	str.a fs.a  `cat dns.lib` `cat socket.lib`

tcp-env.o: \
compile tcp-env.c sig.h stralloc.h gen_alloc.h str.h env.h fmt.h \
scan.h subgetopt.h ip.h dns.h byte.h remoteinfo.h exit.h case.h
	./compile tcp-env.c

tcpto.o: \
compile tcpto.c tcpto.h open.h lock.h seek.h now.h datetime.h ip.h \
byte.h datetime.h
	./compile tcpto.c

tcpto_clean.o: \
compile tcpto_clean.c tcpto.h open.h substdio.h
	./compile tcpto_clean.c

timeoutconn.o: compile timeoutconn.c
	./compile timeoutconn.c

timeoutread.o: compile timeoutread.c
	./compile timeoutread.c

timeoutwrite.o: compile timeoutwrite.c
	./compile timeoutwrite.c

qmail-smtpd: tls.o ssl_timeoutio.o ndelay.a
qmail-remote: tls.o ssl_timeoutio.o
qmail-smtpd.o: tls.h ssl_timeoutio.h
qmail-remote.o: tls.h ssl_timeoutio.h

tls.o: \
compile tls.c exit.h error.h
	./compile tls.c

ssl_timeoutio.o: \
compile ssl_timeoutio.c
# ssl_timeoutio.h select.h error.h ndelay.h
	./compile ssl_timeoutio.c

token822.o: compile token822.c
	./compile token822.c

trigger.o: \
compile trigger.c hasnpbg1.h
# select.h open.h trigger.h
	./compile trigger.c

triggerpull.o: \
compile triggerpull.c ndelay.h open.h triggerpull.h
	./compile triggerpull.c

wait.a: compile makelib wait.c haswaitp.h
	./compile wait.c
	./makelib wait.a wait.o

cert cert-req: Makefile-cert
	@$(MAKE) -sf $< $@

Makefile-cert: \
conf-qmail conf-users conf-groups Makefile-cert.mk
	@cat Makefile-cert.mk \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	> $@

update_tmprsadh: \
conf-qmail conf-users conf-groups update_tmprsadh.sh
	@cat update_tmprsadh.sh\
	| sed s}UGQMAILD}"`head -2 conf-users|tail -1`:`head -1 conf-groups`"}g \
	| sed s}QMAIL}"`head -1 conf-qmail`"}g \
	> $@
	chmod 755 update_tmprsadh 

tmprsadh: update_tmprsadh
	echo "Creating new temporary RSA and DH parameters"
	./update_tmprsadh
