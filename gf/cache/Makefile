CC = g++
RELEASE=0
FLAGS  = -Wall -DDEBUG -g -fPIC -MMD  
ifndef RELEASE
FLAGS += -DENABLE_TRACE_LOG 
else
FLAGS += -DRELEASE_VER
endif

SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 -I /usr/local/include
LIBPATH =
LIBS	= -ltaomee -ltaomee++ -lkfcommon  -lhiredis

BINS	= libcache.so
SRCS 	= dll_interface.cpp cache.cpp cli_proto.cpp 
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
