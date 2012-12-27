INC_DIR=-I../.. -I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include/
LIB_DIR=
LIB=-ltaomee
FLAG= -g -Wall
CC=g++

BINARY=reload

all: $(BINARY)

reload: reload.cpp reload.hpp
	$(CC) $(FLAG) $(INC_DIR) -o $@ $^ $(LIB)

clean:
	rm -rf *.o $(BINARY)
