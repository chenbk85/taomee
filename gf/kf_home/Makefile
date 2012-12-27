CC = g++

FLAGS   = -Wall -DDEBUG -g -fPIC -MMD -DENABLE_TRACE_LOG
#FLAGS   = -Wall -DDEBUG -g -fPIC -MMD -DLOG_USE_SYSLOG
SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/
LIBPATH =
LIBS	= -ltaomee -ltaomee++

BINS	= lib_kf_home.so
#SRCS 	= fwd_decl.cpp Player_new.cpp Map.cpp Home.cpp homeServer.cpp cli_proto.cpp player.cpp OnlineTask.cpp 
SRCS 	= fwd_decl.cpp Player.cpp Map.cpp Home.cpp homeServer.cpp Online.cpp 
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
