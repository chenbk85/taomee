CC = gcc

#CFLAGS  = -pipe -DDEBUG -g -D_GNU_SOURCE -shared -fPIC -D_REENTRANT -parallel -axSSE4.2 -ivdep -ansi-alias -restrict -fargument-alias -funroll-all-loops -opt-prefetch
CFLAGS  = -pipe -Wall -DDEBUG -g3 -O0 -D_GNU_SOURCE -shared -fPIC -D_REENTRANT
LIB 	= -ltaomee -lpthread -lglib-2.0
INCLUDE = -I./include/ -I./include/iter_serv/ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/
SOURCE = $(wildcard *.c)
OO = $(patsubst %.c,%.o,$(SOURCE))
#OO	= dll_if.o recv_data.o write_file.o
TARGETS = store_fcgi_data.so

all: $(OO) $(TARGETS)
	
$(TARGETS):
	$(CC) $(CFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)
#	rm ../bin/$(TARGETS); mv $(TARGETS) ../bin/
.c.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
#	echo $@

%:%.c
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
#	echo $@

clean:
	rm -f *.o
	rm -f $(TARGETS)

