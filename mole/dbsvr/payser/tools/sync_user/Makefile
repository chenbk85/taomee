CC = g++
WORKDIR=$(HOME)/DB
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE  -D_REENTRANT
LIB 	= -lproto  -lpub 
LIBDIR 	= -L$(WORKDIR)/lib
INCLUDE = -I$(WORKDIR)/include

OBJS=sync_user.o 
SYNC_USER=pay_syncuser 

all: $(SYNC_USER)    clean
	
$(SYNC_USER): $(OBJS) 
	$(CC) -o$@  $(OBJS) $(LIBDIR)  $(LIB) 
	cp $(SYNC_USER)  ../../../bin/dv_syncuser
	cp $(SYNC_USER)  ../../../bin
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
clean:
	rm -f *.o 
