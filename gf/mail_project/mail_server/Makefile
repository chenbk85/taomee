CC = g++

FLAGS   = -Wall -DDEBUG -g -fPIC -MMD 
SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/
LIBPATH =
LIBS	= -ltaomee -ltaomee++

BINS	= libmail_server.so
SRCS 	= online.cpp  mail_server.cpp dispatcher.cpp dbproxy.cpp

all: $(BINS)

$(BINS): $(SRCS:.cpp=.o)
	$(CC)  $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)

%.o: %.cpp Makefile
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-mv -f $*.d .dep.$@

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core

-include /dev/null $(wildcard .dep.*)
