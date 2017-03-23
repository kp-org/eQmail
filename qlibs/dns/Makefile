
COMPILE=../compile
MAKELIB=../makelib
CCFLAGS=-I../hdrs

default: clean check dns.a

check:
	@[ -f $(COMPILE) ] && [ -f $(MAKELIB) ] || ( cd .. ; ./configure ; )

clean:
	rm -f *.o dns.a

dns.a: ../compile ../makelib dns_domain.c dns_dtda.c dns.h dns_ip6.c dns_ip.c \
dns_ipq.c dns_name.c dns_nd.c dns_packet.c dns_random.c dns_rcip.c \
dns_rcrw.c dns_resolve.c dns_sortip.c dns_transmit.c dns_txt.c
	$(COMPILE) $(CCFLAGS) dns_domain.c dns_dfd.c dns_dtda.c dns_ip6.c dns_ip.c \
	dns_ipq.c dns_name.c dns_nd.c dns_packet.c dns_random.c dns_rcip.c \
	dns_rcrw.c dns_resolve.c dns_sortip.c dns_transmit.c dns_txt.c
	$(MAKELIB) dns.a dns_domain.o dns_dfd.o dns_dtda.o dns_ip6.o dns_ip.o \
	dns_ipq.o dns_name.o dns_nd.o dns_packet.o dns_random.o dns_rcip.o \
	dns_rcrw.o dns_resolve.o dns_sortip.o dns_transmit.o dns_txt.o
#	@cp dns.a ..
