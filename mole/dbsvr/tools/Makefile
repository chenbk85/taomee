CC = g++
WORKDIR=../
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE  -D_REENTRANT
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIB 	= -lproto  -lpub
LIBDIR 	= -L$(WORKDIR)/lib
INCLUDE = -I$(WORKDIR)/include


all: act clean
	

act:activecode.o 
	$(CC) -o $@  activecode.o  $(LIBDIR)  $(LIB) 

.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<

clean:
	rm -f *.o 
grade:
	$(CC) -o $@  grade.cpp
