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

#stralloc.a: ../compile ../makelib stralloc.c
stralloc.a: ../makelib stralloc.c
	../makelib ../stralloc.a stralloc.o

#strerr.a: ../compile ../makelib strerr.c
strerr.a: ../makelib strerr.c
	../makelib ../strerr.a strerr.o

wait.a: ../makelib wait.c
	../makelib ../wait.a wait.o

#fmt.o: compile fmt.c
#	./compile fmt.c


#byte.o: ../compile byte.c
#	../compile $(CFLAGS) byte.c

#scan.o: compile scan.c scan.h
#	./compile scan.c

#********************************************

#auto-int8: \
#load auto-int8.o substdio.a error.a str.a fs.a
#	./load auto-int8 substdio.a error.a str.a fs.a 

#auto-int8.o: \
#compile auto-int8.c substdio.h exit.h scan.h fmt.h
#	./compile auto-int8.c

#auto_patrn.c: \
#auto-int8 conf-patrn
#	./auto-int8 auto_patrn `head -1 conf-patrn` > auto_patrn.c

#auto_patrn.o: \
#compile auto_patrn.c
#	./compile auto_patrn.c

#auto_qmail.o: compile auto_qmail.c
#	./compile auto_qmail.c

#auto_spawn.o: compile auto_spawn.c
#	./compile auto_spawn.c

#auto_split.o: compile auto_split.c
#	./compile auto_split.c

#auto_uids.o: compile auto_uids.c
#	./compile auto_uids.c

#auto_usera.o: compile auto_usera.c
#	./compile auto_usera.c

#cdbmss.o: compile cdbmss.c seek.h
# alloc.h cdbmss.h cdbmake.h
#	./compile cdbmss.c

#date822fmt.o: \
#compile date822fmt.c datetime.h fmt.h date822fmt.h
#	./compile date822fmt.c

#datetime.a: \
#makelib datetime.o datetime_un.o
#	./makelib datetime.a datetime.o datetime_un.o

#datetime.o: \
#compile datetime.c datetime.h
#	./compile datetime.c

dns.o: compile dns.c
	./compile dns.c

#ip.o: \
#compile ip.c fmt.h scan.h ip.h
#	./compile ip.c

#ipalloc.o: compile ipalloc.c
#	./compile ipalloc.c

#ipme.o: \
#compile ipme.c hassalen.h byte.h ip.h ipalloc.h ip.h gen_alloc.h \
#stralloc.h gen_alloc.h ipme.h ip.h ipalloc.h
#	./compile ipme.c

#ipmeprint: \
#load ipmeprint.o ipme.o ip.o ipalloc.o stralloc.a alloc.a substdio.a \
#error.a str.a fs.a socket.lib
#	./load ipmeprint ipme.o ip.o ipalloc.o stralloc.a alloc.a \
#	substdio.a error.a str.a fs.a  `cat socket.lib`

#ipmeprint.o: compile ipmeprint.c
#	./compile ipmeprint.c

#myctime.o: \
#compile myctime.c datetime.h fmt.h myctime.h
#	./compile myctime.c

#now.o: compile now.c
# datetime.h now.h datetime.h
#	./compile now.c

#readsubdir.o: compile readsubdir.c
#	./compile readsubdir.c

timeoutconn.o: compile timeoutconn.c
	./compile timeoutconn.c

timeoutread.o: compile timeoutread.c
	./compile timeoutread.c

timeoutwrite.o: compile timeoutwrite.c
	./compile timeoutwrite.c

token822.o: compile token822.c
	./compile token822.c

#trigger.o: \
#compile trigger.c hasnpbg1.h
# select.h open.h trigger.h
#	./compile trigger.c

#triggerpull.o: \
#compile triggerpull.c ndelay.h open.h triggerpull.h
#	./compile triggerpull.c
