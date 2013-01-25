CC = g++
AR=ar
RANLIB=ranlib
LOGLIB=liblog.a
CGILIB=libcgic.a
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE -D_FASTCGI
LIB 	= -ldl -lrt -ltaomee++  -ltaomee -lgd -L./ -lcgic  -L../../../json/lib -ljson -L./ -llog 
INCLUDE = -I../../../include/ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 -I../../../json/include



#OO=tcpip.o cgi_upload.o cgi_mv_pic.o cgi_mv_logo.o
OO=tcpip.o cgi_upload.o cgi_upload_logo.o
UPLOAD_TARGETS=cgi_upload.cgi
UPLOAD_LOGO_TARGETS=cgi_upload_logo.cgi
#MVPIC_TARGETS=cgi_mv_pic.cgi
#MVLOGO_TARGETS=cgi_mv_logo.cgi


#all: $(LOGLIB) $(CGILIB) $(OO) $(UPLOAD_TARGETS) $(MVPIC_TARGETS) $(MVLOGO_TARGETS)

all: $(LOGLIB) $(CGILIB) $(OO) $(UPLOAD_TARGETS) $(UPLOAD_LOGO_TARGETS)	

$(LOGLIB): log.o log.h
	rm -f liblog.a
	$(AR) rc liblog.a log.o
	$(RANLIB) liblog.a
$(CGILIB): cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a



$(UPLOAD_TARGETS):
	$(CC) $(CFLAGS) cgi_upload.o tcpip.o -o $@ $(LIBDIR) $(LIB)
	cp $(UPLOAD_TARGETS) ../cgi-bin/
	chmod u+s ../cgi-bin/$(UPLOAD_TARGETS)

$(UPLOAD_LOGO_TARGETS):
	$(CC) $(CFLAGS) cgi_upload_logo.o tcpip.o -o $@ $(LIBDIR) $(LIB)
	cp $(UPLOAD_LOGO_TARGETS) ../cgi-bin/
	chmod u+s ../cgi-bin/$(UPLOAD_LOGO_TARGETS)



#$(MVPIC_TARGETS):
#	$(CC) $(CFLAGS) cgi_mv_pic.o tcpip.o -o $@ $(LIBDIR) $(LIB)
#	cp $(MVPIC_TARGETS) ../cgi-bin/
#	chmod u+s ../cgi-bin/$(MVPIC_TARGETS)

#$(MVLOGO_TARGETS):
#	$(CC) $(CFLAGS) cgi_mv_logo.o tcpip.o -o $@ $(LIBDIR) $(LIB)
#	cp $(MVLOGO_TARGETS) ../cgi-bin/
#	chmod u+s ../cgi-bin/$(MVLOGO_TARGETS)


.c.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

%:%.c
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
	echo $@

clean:
	rm -f *.o *.a *.cgi

