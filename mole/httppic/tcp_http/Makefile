CC = g++ 
HOME = /data/home/crow/current

CFLAGS = -Wall -g -pipe -D_NEW_LIC -D_GNU_SOURCE -fPIC -D_REENTRANT 
#CFLAGS = -Wall -g -pipe -D_NEW_LIC -D_GNU_SOURCE -fPIC -D_REENTRANT -DUSE_EPOLL

LDFLAGS =  $(CFLAGS) $(INCLUDE) $(LIBDIR) $(LIB) 
LIB = -ldl -rdynamic  -lpthread -lrt
OO = Common.o Log.o Options.o Thread_Main.o Net.o

BIN = Tcp_Http

all: $(OO) $(BIN) 
.cpp.o:
	$(CC)  $(CFLAGS) -c  $(INCLUDE) $< 
	echo $@
	
%:%.cpp 
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS) 
	echo $@

clean:
	rm -f *.o
	rm -f $(BIN)
