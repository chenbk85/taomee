CC = gcc

FLAGS   = -g -Wall -DNDEBUG -fPIC -MMD
#FLAGS   = -Wall -DDEBUG -g -fPIC -MMD -DLOG_USE_SYSLOG
SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/
LIBPATH =
LIBS	= -lmsglog -ltaomee

BINS	= libhomeserv.so
SRCS 	= hs_dispatcher.c hs_online.c homeserv.c

all: $(BINS)

$(BINS): $(SRCS:.c=.o)
	$(CC)  $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)
	-rm ./bin/$(BINS)
	cp $(BINS) ./bin

%.o: %.c Makefile
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-mv -f $*.d .dep.$@

clean:
	@-rm -f *.o  *.po *.so *.d .dep.* $(BINS) core

-include /dev/null $(wildcard .dep.*)
