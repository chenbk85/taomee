CC = g++ 

WORKDIR=.
CFLAGS  = -Wall -pipe -DDEBUG -g -D_GNU_SOURCE -shared -D_REENTRANT -fPIC
LIBDIR = -L/usr/lib 
LIB 	= -lm
INCLUDE = -I$(WORKDIR) -I../../ -I/usr/include/libxml2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/

OO	= football.o game.o 
TARGETS = libfootball.so

all: $(OO) $(TARGETS)
	
$(TARGETS): 
	$(CC) $(CFLAGS) $(INCLUDE) $(OO) -o $@ $(LIBDIR) $(LIB)
	rm -f *.o

.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<

%:%.cpp
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)

clean:
	rm -f *.o
	rm -f $(WORKDIR)/$(TARGETS)
