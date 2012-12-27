CC = g++
WORKDIR=$(HOME)/DB
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE  -D_REENTRANT
LIB 	=  -lpub -ltaomee
LIBDIR 	= -L$(WORKDIR)/lib
INCLUDE = -I$(WORKDIR)/include

OBJS=sync_user_tw.o 
SYNC_USER=dv_syncuser

all: $(SYNC_USER)    clean
	
$(SYNC_USER): $(OBJS) 
	$(CC) -o$@  $(OBJS) $(LIBDIR)  $(LIB) 
	mv $@ ~/DB/bin/
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
clean:
	rm -f *.o 
