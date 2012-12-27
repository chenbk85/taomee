CC = g++
WORKDIR=$(HOME)/DB
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE  -D_REENTRANT
#LIB 	= -lproto  -lpub 
#LIBDIR 	= -L$(WORKDIR)/lib
INCLUDE = -I$(WORKDIR)/include

OBJS=email_gen.o 
SYNC_USER=email_gen

all: $(SYNC_USER)    clean
	
$(SYNC_USER): $(OBJS) 
	$(CC) -o$@  $(OBJS) $(LIBDIR)  $(LIB) 
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
clean:
	rm -f *.o 
