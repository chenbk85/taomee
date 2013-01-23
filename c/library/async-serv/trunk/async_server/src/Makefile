OPT = -ggdb
#OPT += -DENABLE_TRACE_LOG

CC  = g++
BINS = async_server
SRCS = config.cpp global.cpp log.cpp net.cpp reactor.cpp shm_revent.cpp tcp_accept.cpp tcp_socket.cpp \
       conn.cpp main.cpp plugin.cpp shmq.cpp shm_sevent.cpp tcp_linker.cpp work.cpp net.cpp

CFLAGS = -I./ -Wall -MMD $(DEBUG) -D_REENTRANT -D_GNU_SOURCE $(OPT) -z defs
LIBS += -ldl -rdynamic -lpthread

%.o: %.cpp Makefile
	$(CC) $(CFLAGS) -c -o $@ $<
	@-mv -f $*.d .dep.$@
$(BINS): $(SRCS:.cpp=.o)
	$(CC)  $(LDFLAGS) -o $@ $^ $(LIBS)
	@mv $(BINS) ../bin

clean:
	@-rm -f *.o *.po *.so *.d .dep.* $(BINS) core

#-include /dev/null $(wildcard .dep.*)
