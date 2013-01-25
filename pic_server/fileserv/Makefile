CC = gcc

#CFLAGS  = -pipe -DDEBUG -g -D_GNU_SOURCE -shared -fPIC -D_REENTRANT -parallel -axSSE4.2 -ivdep -ansi-alias -restrict -fargument-alias -funroll-all-loops -opt-prefetch
CFLAGS  = -pipe -DDEBUG -g -D_GNU_SOURCE -shared -fPIC -D_REENTRANT
LIB 	= -ldl -lrt -lglib-2.0 -lgthread-2.0 -ltaomee -lgd 
INCLUDE = -I../include/ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2

OO	= dll_if.o save_file.o thumb.o fs_list.o change_thumb.o filelog.o
TARGETS = fileserv.so

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

