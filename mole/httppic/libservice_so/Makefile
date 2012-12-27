CC = g++

CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -fPIC -shared -export-dynamic -D_REENTRANT
LIB 		= -lpthread -ldl -lrt
LDFLAGS	=  $(CFLAGS) $(INCLUDE)

TARGETS = libTMhttp.so
OO	= Service_So.o qdes.o http_processer.o base64.o safe_socket.o

all: $(OO) $(TARGETS)
	
$(TARGETS): $(OO)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)

$(OO): %.o: %.cpp %.h
	$(CC)  $(CFLAGS) -c  $(INCLUDE) $< -o $@
	echo $@

.PHONY : clean
clean:
	rm -f *.o
	rm -f $(TARGETS)

