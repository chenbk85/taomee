CC = g++
WORKDIR=../../
#CFLAGS  = -Wall -pipe -DLOG_PROTO_FLAG -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE  -D_REENTRANT
CFLAGS  = -Wall -pipe  -g3 -O0 -DDEBUG -D_NEW_LIC  -D_GNU_SOURCE  -D_REENTRANT -fPIC
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIB 	= -lproto  -lpub  -lpthread -ltaomee
LIBDIR 	= -L$(WORKDIR)/lib
INCLUDE = -I$(WORKDIR)/include 

TARGETS= libproto.a 
TARGETSOBJ= Cclientproto.o 
TT=tt
TTFILE=ttfile
TTOBJ=t.o 

all: $(TARGETS) $(TT) $(TTFILE)
	
$(TARGETS): $(TARGETSOBJ)  
	ar -rv  $@ $(TARGETSOBJ)  $(WORKDIR)/lib/libpub.a
	cp $(TARGETS) $(WORKDIR)/lib 

$(TT): $(TTOBJ)
	$(CC) -o$@  $(TTOBJ) $(LIBDIR)  $(LIB) 
	mv $@  $(WORKDIR)/bin/

$(TTFILE): ttfile.o
	$(CC) -o $@  ttfile.o  $(LIBDIR)  $(LIB) 
	mv $@  $(WORKDIR)/bin/

.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<

clean:
	rm -f *.o 
