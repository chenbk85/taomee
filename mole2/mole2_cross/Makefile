CC = g++

FLAGS   = -Wall -DNDEBUG -fPIC -MMD
#FLAGS   = -Wall -DDEBUG -g -fPIC -MMD -DLOG_USE_SYSLOG
SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/
LIBPATH =
LIBS	= -lmsglog

BINS	= libm2corss.so
SRCS 	= dispatcher.cpp cjob_dispatcher.cpp cross.cpp job_factory.cpp dbproxy.cpp

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
