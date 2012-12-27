INC_DIR=-I../.. -I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include/
LIB_DIR=
LIB=-ltaomee
FLAG= -g -Wall
CC=g++

BINARY=send_mcast

all: $(BINARY)

send_mcast: send_mcast.cpp send_mcast.hpp
	$(CC) $(FLAG) $(INC_DIR) -o $@ $^ $(LIB)

clean:
	rm -rf *.o $(BINARY)
