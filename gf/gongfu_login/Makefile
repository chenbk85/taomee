CC = g++

#FLAGS   = -Wall -DNDEBUG -O3 -fPIC -MMD
FLAGS   = -Wall -DDEBUG -g -fPIC -MMD -DENABLE_TRACE_LOG
SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/home/mole -I/usr/include/libtaomee++/project/main_login -I../
LIBPATH = -L./bin
LIBS	= -llogin -ltaomee -ltaomee++ -lplayer_attr

BINS	= libgf_login.so
SRCS 	= myproto.cpp mynet.cpp myinit.cpp mysvrlist.cpp myprocess.cpp

all: $(BINS)

$(BINS): $(SRCS:.cpp=.o)
	$(CC)  $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)
	-mv libgf_login.so ./lib/
#-rm ./bin/$(BINS)
#	mv $(BINS) ./bin

%.o: %.cpp Makefile
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-mv -f $*.d .dep.$@

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core

-include /dev/null $(wildcard .dep.*)
