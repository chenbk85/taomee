CC = g++
WORKDIR=$(HOME)/DB
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE  -D_REENTRANT
LIB 	= -lpub 
LIBDIR 	= -L$(WORKDIR)/lib
INCLUDE = -I$(WORKDIR)/include

OBJS=save_log.o 
SYNC_USER=save_log

all: $(SYNC_USER)    clean
	
$(SYNC_USER): $(OBJS) 
	$(CC) -o$@  $(OBJS) $(LIBDIR)  $(LIB) 
	mv $(SYNC_USER)  $(WORKDIR)/bin
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
clean:
	rm -f *.o 
