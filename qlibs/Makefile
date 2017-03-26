# Makefile for qlibs

SHELL=/bin/sh

# 1. create object files (*.o)
# 2. create libs (*.a)
# 3. link/copy (some) files

#CFLAGS=-Iinc -I.. -I../inc
SRC=*.c
COMPILE=./compile
MAKELIB=./makelib

default: clean check libs setup

check:
	@[ -f $(COMPILE) ] && [ -f $(MAKELIB) ] || ./configure

clean:
	@echo -n Cleaning up libs ...
	@rm -f *.o *.a compile makelib *.tmp
	@cd dns ; make -s clean
	@echo " done!"

libs: obj alloc.a buffer.a case.a cdb.a env.a error.a fd.a fs.a getln.a getopt.a lock.a \
ndelay.a open.a seek.a sig.a substdio.a str.a stralloc.a strerr.a time.a wait.a \
dns.a ip.a socket.a
#	./install

obj:
	@echo Making all in qlibs ...
	$(COMPILE) $(SRC)
	@cp readclose.o slurpclose.o

install: setup

setup:
	@./install

alloc.a:
	$(MAKELIB) alloc.a alloc.o

buffer.a:
	$(MAKELIB) buffer.a buffer.o buffer_0.o buffer_1.o buffer_2.o \
	buffer_copy.o buffer_get.o buffer_put.o

case.a:
	$(MAKELIB) case.a case.o

cdb.a:
	$(MAKELIB) cdb.a cdbread.o cdbmake.o cdbhash.o uint32p.o seek.o buffer.o

env.a:
	$(MAKELIB) env.a env.o

error.a:
	$(MAKELIB) error.a error.o error_str.o error_temp.o

fd.a:
	$(MAKELIB) fd.a fd.o

fs.a:
	$(MAKELIB) fs.a fmt.o scan.o

getln.a:
	$(MAKELIB) getln.a getln.o

getopt.a:
	$(MAKELIB) getopt.a getoptb.o

lock.a:
	$(MAKELIB) lock.a lock.o

ndelay.a:
	$(MAKELIB) ndelay.a ndelay.o

open.a:
	$(MAKELIB) open.a open.o

seek.a:
	$(MAKELIB) seek.a seek.o

sig.a:
	$(MAKELIB) sig.a sig.o

substdio.a: ./makelib substdio.o substdi.o substdo.o \
subfderr.o subfdout.o subfdouts.o subfdin.o subfdins.o
	./makelib substdio.a substdio.o substdi.o substdo.o \
	subfderr.o subfdout.o subfdouts.o subfdin.o subfdins.o

str.a:
	$(MAKELIB) str.a str.o byte.o

stralloc.a:
	$(MAKELIB) stralloc.a stralloc.o

strerr.a:
	$(MAKELIB) strerr.a strerr.o

wait.a:
	$(MAKELIB) wait.a wait.o

#***  new from ucspi-tcp **************************************************
time.a: iopause.o tai.o taia.o
	$(MAKELIB) time.a iopause.o tai.o taia.o

#********************************************
# needed by qmail-tcpsrv - devel
dns.a:
	@echo "Building dns lib ..."
	@cd dns ; make --no-print-directory
	@cp dns/dns.a dns.a

socket.a:
	@echo "Building socket lib ..."
	$(MAKELIB) socket.a socket.o socket_accept.o socket_bind.o \
	socket_connect.o socket_proto.o socket_local.o socket_remote.o ip.a

ip.a:
	$(MAKELIB) ip.a ip4.o ip6.o socket.o
