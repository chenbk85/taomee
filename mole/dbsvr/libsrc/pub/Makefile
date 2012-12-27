CC = g++
WORKDIR=../../
CFLAGS  = -g3 -O0 -Wall -pipe -DDEBUG -D_NEW_LIC -D_GNU_SOURCE -shared -D_REENTRANT -fPIC
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIBDIR = -L/usr/lib
LIB 	= -ldl  
INCLUDE =  -I$(WORKDIR)/include

OO	=  md5.o msglog64.o Csync_user.o pub.o tcpip.o logsavefile.o  Csem.o Csync_log.o
TARGETS = libpub.a

all: $(OO) $(TARGETS)
	
$(TARGETS):
	ar -rv  $@ $(OO) 
	cp $(TARGETS) $(WORKDIR)/lib/
.c.o:
	gcc  $(CFLAGS) -c $(INCLUDE) $<
	echo $@
.cpp.o:
	g++  $(CFLAGS) -c $(INCLUDE) $<
	echo $@


clean:
	rm -f $(TARGETS)  $(OO)  
