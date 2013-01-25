CC = g++

CFLAGS  = -Wall -pipe -DDEBUG -g -D_GNU_SOURCE -shared -fPIC -D_REENTRANT
LIB 	= -ldl -lrt -ltaomee  -ltaomee++    
INCLUDE = -I../../ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ 

OO	= service.o mmap.o
TARGETS = keyserv.so

all: $(OO) $(TARGETS)
	
$(TARGETS):
	$(CC) $(CFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)
#	rm ../bin/$(TARGETS); mv $(TARGETS) ../bin/
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

%:%.cpp
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
	echo $@

clean:
	rm -f *.o
	rm -f $(TARGETS)

