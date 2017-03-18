# Don't edit Makefile! Use conf-* for configuration.

SHELL=/bin/sh

# create libs (*.a)
# create object files (*.o)
# link (some) files

CFLAGS=-I.. -I../inc
SRC=*.c
COMPILE=../compile
MAKELIB=../makelib

default: check libs

check:
#	[ -f $(MAKELIB) ] || ( cd .. ; ./configure ; cd $($OLDPWD) ; )
#	[ -f $(COMPILE) ] || ( cd .. ; ./configure ; cd $($OLDPWD) ; )
	[ -f $(COMPILE) ] && [ -f $(MAKELIB) ] || ( cd .. ; ./configure ; cd $($OLDPWD) ; )

clean:
	@echo Cleaning up libs ...
	@rm -f *.o *.a
	@cd dns ; make -s clean

libs: obj alloc.a buffer.a case.a cdb.a env.a error.a fd.a fs.a getln.a getopt.a lock.a \
ndelay.a open.a seek.a sig.a substdio.a str.a stralloc.a strerr.a time.a wait.a \
strerr_buf.a getln_buf.a \
dns.a ip.a socket.a
	@echo -n Moving .o files ...
	@mv base64.o byte.o fmt.o prot.o slurpclose.o readclose.o \
	..
	@echo done!

obj:
# the link is temp for backwards compat
	@echo Making all in lib ...
	$(COMPILE) $(CFLAGS) $(SRC)
	cp readclose.o slurpclose.o

alloc.a:
# ../compile ../makelib alloc.c
	$(MAKELIB) ../alloc.a alloc.o

buffer.a:
	$(MAKELIB) ../buffer.a buffer.o buffer_0.o buffer_1.o buffer_2.o \
	buffer_copy.o buffer_get.o buffer_put.o

case.a:
	$(MAKELIB) ../case.a case.o

cdb.a:
	$(MAKELIB) ../cdb.a cdbread.o cdbmake.o cdbhash.o uint32p.o seek.o buffer.o

env.a:
	$(MAKELIB) ../env.a env.o

error.a: ../compile ../makelib error.c error_str.c error_temp.c
	../makelib ../error.a error.o error_str.o error_temp.o

fd.a: ../makelib fd.c
	../makelib ../fd.a fd.o

fs.a: ../makelib fmt.o scan.o
	../makelib ../fs.a fmt.o scan.o

getln.a: ../compile ../makelib getln.o
	../makelib ../getln.a getln.o

getln_buf.a:
	$(MAKELIB) ../getln_buf.a getln_buf.o

getopt.a:
	$(MAKELIB) ../getopt.a getoptb.o

lock.a:
# ../compile ../makelib lock.c
	$(MAKELIB) ../lock.a lock.o

ndelay.a:
	$(MAKELIB) ../ndelay.a ndelay.o

open.a:
	$(MAKELIB) ../open.a open.o

seek.a:
	$(MAKELIB) ../seek.a seek.o

sig.a:
	$(MAKELIB) ../sig.a sig.o

substdio.a: ../makelib substdio.o substdi.o substdo.o \
subfderr.o subfdout.o subfdouts.o subfdin.o subfdins.o
	../makelib ../substdio.a substdio.o substdi.o substdo.o \
	subfderr.o subfdout.o subfdouts.o subfdin.o subfdins.o

str.a:
	$(MAKELIB) ../str.a str.o byte.o

stralloc.a:
	$(MAKELIB) ../stralloc.a stralloc.o

strerr.a:
	$(MAKELIB) ../strerr.a strerr.o

# qmail-tcpsrv
strerr_buf.a:
	$(MAKELIB) ../strerr_buf.a strerr_buf.o

wait.a:
	$(MAKELIB) ../wait.a wait.o

#***  new from ucspi-tcp **************************************************
time.a: iopause.o tai.o taia.o
	$(MAKELIB) time.a iopause.o tai.o taia.o
	cp time.a ../time.a

#********************************************
# needed by qmail-tcpsrv - devel
dns.a:
	@echo "Building dns lib ..."
	@cd dns ; make --no-print-directory
	@cp dns/dns.a ../dns.a

socket.a:
	echo "Building socket lib ..."
	$(MAKELIB) ../socket.a socket.o socket_accept.o socket_bind.o \
	socket_connect.o socket_proto.o socket_local.o socket_remote.o ../ip.a

ip.a:
	$(MAKELIB) ../ip.a ip4.o ip6.o socket.o
