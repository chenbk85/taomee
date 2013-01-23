DEBUG	= -g -DDEBUG
OPT	= -O3 -funroll-loops -D__USE_STRING_INLINES -fhosted
OPT	+= -DEPOLL_MODE -DUSE_SYSTEMV_SEM
#OPT	+= -DEPOLL_MODE

CC	= gcc
BINS	= ServerBench
SRCS 	= main.c log.c config.c daemon.c util.c dll.c notifier.c dispatcher.c worker.c shmq.c \
		lock.c net.c tcp.c tsc.c 

CFLAGS	= -Wall -MMD $(DEBUG) -D_REENTRANT -D_GNU_SOURCE $(OPT) -z defs 
#LIBS	= ns_api/libheart.a ns_api/libov.a
LIBS	+= -ldl -rdynamic  -lpthread 

all: $(BINS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<
	@-mv -f $*.d .dep.$@
$(BINS): $(SRCS:.c=.o)
	$(CC)  $(LDFLAGS) -o $@ $^ $(LIBS)
#	rm ../bin/$(BINS); mv $(BINS) ../bin/
clean:
	@-rm -f *.o *.po *.so *.d .dep.* $(BINS) core

-include /dev/null $(wildcard .dep.*)
