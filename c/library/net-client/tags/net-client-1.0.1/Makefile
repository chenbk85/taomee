# vim: set noexpandtab tabstop=4 softtabstop=4 shiftwidth=4:

CC       = g++
CFLAGS   = -std=c++0x -Werror -fPIC -ggdb -Wno-non-virtual-dtor
LDFLAGS  = -shared -Wl,-soname,libnetclient.so
LIBS     =
INCLUDES = -I./
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET   = libnetclient.so.1.0.1

.PHONY: all static clean c rebuild r

all: $(TARGET)
static: libnetclient.a
	
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)
	/sbin/ldconfig -n .

libnetclient.a: $(OBJS)
	ar rcs libnetclient.a $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean: 
	rm -rf $(OBJS)
	rm -rf $(TARGET)
	rm -rf lib*

rebuild: clean all
c: clean
r: rebuild
