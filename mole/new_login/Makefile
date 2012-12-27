CC = g++

FLAGS  = -Wall -DDEBUG -g -fPIC -MMD 

ifndef RELEASE
FLAGS += -DENABLE_TRACE_LOG 
else
FLAGS += -DRELEASE_VER
endif

SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 -I/usr/local/include
LIBPATH = -L /usr/local/lib/
LIBS	=  -lmainlogin  -ltaomee++ -ltaomee -lxml2 -lgd -lrt -lssl

BINS	= liblogin.so
SRCS 	= login.cpp login_impl.cpp switch_impl.cpp time_stamp_impl.cpp mole_common.cpp main.cpp mulcast.cpp

all: $(BINS)

$(BINS): $(SRCS:.cpp=.o)
	$(CC)  $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)

%.o: %.cpp Makefile
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-mv -f $*.d .dep.$@

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core

-include /dev/null $(wildcard .dep.*)
