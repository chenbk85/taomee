CC = g++
WORKDIR=$(HOME)/DB
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE  -D_REENTRANT
LIB 	= -lproto  -lpub 
LIBDIR 	= -L$(WORKDIR)/lib
INCLUDE = -I$(WORKDIR)/include

OBJS=sync_log.o 
SYNC_USER=synclog

all: $(SYNC_USER)    clean
	
$(SYNC_USER): $(OBJS) 
	$(CC) -o$@  $(OBJS) $(LIBDIR)  $(LIB) 
	mv $(SYNC_USER)  ../../bin
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
clean:
	rm -f *.o 
