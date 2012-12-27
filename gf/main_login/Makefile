CC = g++

#FLAGS   = -Wall -DNDEBUG -O3 -fPIC -MMD
FLAGS   = -Wall -DDEBUG -g -fPIC -MMD #-DTW_VER
SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/home/mole 
LIBPATH =
LIBS	= -ltaomee 

BINS	= liblogin.so
SRCS 	= dbproxy.cpp proto.cpp net.cpp init.cpp login.cpp ip_counter.cpp login.cpp ip_counter.cpp 

all: $(BINS)

$(BINS): $(SRCS:.cpp=.o)
	$(CC)  $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)
	-rm ./bin/$(BINS)
	mv $(BINS) ./bin

%.o: %.cpp Makefile
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
#@-mkdir /usr/include/libtaomee++/project
#	@-mkdir /usr/include/libtaomee++/project/main_login
#	-cp -v asyc_main_login*.h /usr/include/libtaomee++/project/main_login
	@-mv -f $*.d .dep.$@

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core

-include /dev/null $(wildcard .dep.*)
