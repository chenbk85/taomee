CC = g++

FLAGS   = -Wall -DDEBUG -g -fPIC -MMD 
#FLAGS   = -Wall -DDEBUG -g -fPIC -MMD -DENABLE_TRACE_LOG
#FLAGS   = -Wall -DDEBUG -g -fPIC -MMD -DLOG_USE_SYSLOG
SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/
LIBPATH =
LIBS	= -ltaomee -ltaomee++

BINS	= libsw.so
SRCS 	= online.cpp  switch.cpp dispatcher.cpp battle_room.cpp trade.cpp ap_toplist.cpp limit.cpp contest.cpp \
		  team_contest.cpp cache.cpp


all: $(BINS)

$(BINS): $(SRCS:.cpp=.o)
	$(CC)  $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)
	-rm ./bin/$(BINS)
	cp $(BINS) ./bin

%.o: %.cpp Makefile
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-mv -f $*.d .dep.$@

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core

-include /dev/null $(wildcard .dep.*)
