# vim: set noexpandtab tabstop=4 softtabstop=4 shiftwidth=4:

CC       = gcc
XX       = g++
CFLAGS   = -Wall -ggdb
XFLAGS   = -Wall -ggdb
INCLUDES = -I./ -I../ -I../others/
SOURCES  = $(wildcard *.cpp) $(wildcard *.c)
COBJS    = $(patsubst %.c,%.o,$(SOURCES))
XOBJS    = $(patsubst %.cpp,%.o,$(SOURCES))

.PHONY: all clean c rebuild r

all: $(COBJS) $(XOBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(XX) $(XFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf *.o
	
c: clean
r: clean all
rebuild: clean all
