# vim: set noexpandtab tabstop=4 softtabstop=4 shiftwidth=4:

XX       = g++
CFLAGS   = -Wall -fPIC -ggdb -Wno-non-virtual-dtor
INCLUDES = -I.
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET   = libnetioserver.so.2.0.0

.PHONY: all clean c rebuild r

all: $(TARGET)

$(TARGET): $(OBJS)
	$(XX) -shared -Wl,-soname,libnetioserver.so -o $(TARGET) $(OBJS)
	/sbin/ldconfig -n .

%.o: %.cpp
	$(XX) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf *.o $(TARGET)
	rm -rf libnetioserver.so*
	
c: clean
r: clean all
rebuild: clean all
