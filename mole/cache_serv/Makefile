CC = g++ 

WORKDIR=$(HOME)/cache_svr

#VERSION=TW_VERSION

ifeq ($(VERSION),TW_VERSION)
	CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -DDEBUG -D_GNU_SOURCE -shared -fPIC -D_REENTRANT -fpermissive -DTW_VERSION 
else
	CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -DDEBUG -D_GNU_SOURCE -shared -fPIC -D_REENTRANT -fpermissive 
endif

LIBDIR = -L/usr/lib   -L$(WORKDIR)/pkg -L$(WORKDIR) 
LIB 	= -lproto -lpub -lmsglog -ltaomee
INCLUDE = -I$(WORKDIR)/pkg    -I$(WORKDIR)/ 

OO	= benchapi.o cmdprocess.o base.o globalvar.o gamescore.o user_time.o jy_lamu.o home_hot.o dining_room.o
TARGETS =libcache_serv.so

all: $(OO) $(TARGETS)
	-mv $(TARGETS) ./bin
	
$(TARGETS):
	$(CC) $(CFLAGS) $(INCLUDE) $(OO) -o $@ $(LIBDIR) $(LIB)
	rm -f *.o
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

clean:
	rm -rf	$(WORKDIR)/*.o
	
