CC			= g++ 

CFLAGS		= -Wall -pipe -g -shared -fPIC -D_REENTRANT
LIB			= -ltaomee
INCLUDE		= -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I../

OO			= ip_counter.o
TARGETS		= ip_counter.so

all: $(TARGETS)
	
$(TARGETS): $(OO)
	$(CC) $(CFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)
	-mv $(TARGETS) ./bin

.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

%:%.cpp
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
	echo $@

clean:
	rm -f *.o
	rm -f $(TARGETS)
