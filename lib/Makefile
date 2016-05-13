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

libs: obj alloc.a case.a env.a error.a fd.a fs.a getln.a getopt.a lock.a \
ndelay.a open.a seek.a sig.a substdio.a str.a stralloc.a strerr.a wait.a
	@echo -n Moving .o files ...
	@mv base64.o byte.o coe.o fmt.o fmtqfn.o slurpclose.o \
	..
	@echo done!

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

getln.a: ../compile ../makelib getln.o
	../makelib ../getln.a getln.o

getopt.a: ../makelib subgetopt.o sgetopt.o
	../makelib ../getopt.a subgetopt.o sgetopt.o

lock.a: ../compile ../makelib lock.c
	../makelib ../lock.a lock.o

ndelay.a: ../makelib ndelay.c
	../makelib ../ndelay.a ndelay.o

open.a: ../makelib open.c
	../makelib ../open.a open.o

seek.a: ../makelib seek.c
	../makelib ../seek.a seek.o

sig.a: ../makelib sig.c
	../makelib ../sig.a sig.o

substdio.a: ../makelib substdio.o substdi.o substdo.o \
subfderr.o subfdout.o subfdouts.o subfdin.o subfdins.o
	../makelib ../substdio.a substdio.o substdi.o substdo.o \
	subfderr.o subfdout.o subfdouts.o subfdin.o subfdins.o

str.a: ../compile ../makelib str.c byte.o
	../makelib ../str.a str.o byte.o

stralloc.a: ../makelib stralloc.c
	../makelib ../stralloc.a stralloc.o

strerr.a: ../makelib strerr.c
	../makelib ../strerr.a strerr.o

wait.a: ../makelib wait.c
	../makelib ../wait.a wait.o

#dns.o: compile dns.c
#	./compile dns.c

#timeoutconn.o: compile timeoutconn.c
#	./compile timeoutconn.c

#timeoutread.o: compile timeoutread.c
#	./compile timeoutread.c

#timeoutwrite.o: compile timeoutwrite.c
#	./compile timeoutwrite.c

#token822.o: compile token822.c
#	./compile token822.c
