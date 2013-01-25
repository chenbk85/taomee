CC = g++
AR=ar
RANLIB=ranlib
LOGLIB=liblog.a

CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE -D_FASTCGI
LIB 	= -ldl -lrt -ltaomee++  -ltaomee  -L../../../fcgi/lib -lfcgi -L./ -llog -L../../../json/lib -ljson
INCLUDE = -I../../../include/ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 -I../../../fcgi/include -I../../../json/include


#fcgi_change_thumb.o
OO	= tcpip.o fcgi_get_image.o fcgi_get_thumb.o fcgi_change_thumb.o fcgi_pic.o

TARGETS = fcgi_pic.fcgi

all: $(LOGLIB) $(OO) $(TARGETS) 
	
$(LOGLIB): log.o log.h
	rm -f liblog.a
	$(AR) rc liblog.a log.o
	$(RANLIB) liblog.a


$(TARGETS):
	$(CC) $(CFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)
	rm -f ../fcgi-bin/$(TARGETS)
	cp $(TARGETS) ../fcgi-bin/
.c.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

%:%.c
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
	echo $@

clean:
	rm -f *.o *.a
	rm -f $(TARGETS)

