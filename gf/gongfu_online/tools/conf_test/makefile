INC_DIR=-I../.. -I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2
LIB_DIR=
LIB=-ltaomee -ltaomee++ -lxml2
FLAG= -g -Wall
CC=g++

BINARY=conf_test

all: $(BINARY)

conf_test: conf_test.cpp
	$(CC) $(FLAG) $(INC_DIR) -o $@ $^ $(LIB)

clean:
	rm -rf *.o $(BINARY)
