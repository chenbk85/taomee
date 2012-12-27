CC = g++ 

WORKDIR=.
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIBDIR = -L/usr/lib -L./
LIB 	= -ldl -llog
INCLUDE = -I$(WORKDIR)/

OO	= CTcpServ.o CEpoll.o main.o
TARGETS = CheckIP

all: $(OO) $(TARGETS)
	
$(TARGETS): 
	$(CC) $(CFLAGS) $(INCLUDE) $(OO) -o $@ $(LIBDIR) $(LIB)
	rm -f *.o
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

%:%.cpp
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
	echo $@

clean:
	rm -f $(WORKDIR)/$(TARGETS)
