CC = gcc

CFLAGS  = -Wall -pipe -DDEBUG -g -D_GNU_SOURCE -shared -fPIC -D_REENTRANT
LIB 	= -ldl -lrt 
INCLUDE = -I../../ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ 

OO	= test.o
TARGETS = test.so

all: $(OO) $(TARGETS)
	
$(TARGETS):
	$(CC) $(CFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)
#	rm ../bin/$(TARGETS); mv $(TARGETS) ../bin/
.c.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

%:%.c
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
	echo $@

clean:
	rm -f *.o
	rm -f $(TARGETS)

