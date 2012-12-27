CC = g++

FLAGS  = -Wall -DDEBUG -g -fPIC -MMD  
ifndef RELEASE
FLAGS += -DENABLE_TRACE_LOG 
else
FLAGS += -DRELEASE_VER
endif

SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2
LIBPATH =
LIBS	= -lxml2 -ltaomee++ -lkfcommon -ltaomee

BINS	= libhome.so
SRCS 	= dll_interface.cpp fwd_decl.cpp dbproxy.cpp \
			player.cpp cli_proto.cpp  object.cpp home_svr.cpp home.cpp home_impl.cpp \
			home_manager.cpp plant.cpp summon.cpp home_attr.cpp decorate.cpp
all: $(BINS)

$(BINS): $(SRCS:.cpp=.o)
	$(CC)  $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)

%.o: %.cpp Makefile
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-mv -f $*.d .dep.$@

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core

-include /dev/null $(wildcard .dep.*)
