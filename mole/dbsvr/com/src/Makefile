CC = g++

WORKDIR = ../../
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT -fPIC
LIBDIR = -L/usr/lib -L$(WORKDIR)/lib
LIB 	= -ldl  -lpub -ltaomee 
INCLUDE = -I../src  -I$(WORKDIR)/include 
OO	= service.o  Croute.o 
TARGETS = libcom.so

all: clean  $(OO) $(TARGETS)
	
$(TARGETS):
	$(CC) $(CFLAGS) $(INCLUDE) $(OO) -o $@ $(LIBDIR) $(LIB)
	rm ../bin/$(TARGETS); mv $(TARGETS) ../bin/
	rm -f *.o
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

clean:
	rm -f *.o
	rm -f $(TARGETS)
