XX       = g++
CFLAGS   = -Wall -fPIC -O -ggdb -Wno-non-virtual-dtor
INCLUDES = -I.
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET   = ./libringqueue.so.1.1.0

all: $(TARGET)

$(TARGET): $(OBJS)
	$(XX) -shared -Wl,-soname,libringqueue.so.1 -o $(TARGET) $(OBJS)
#	/sbin/ldconfig -n ..
	ln -sf libringqueue.so.1.1.0 ./libringqueue.so
	ln -sf libringqueue.so.1.1.0 ./libringqueue.so.1

%.o: %.cpp
	$(XX) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf *.o $(TARGET)
	rm -rf ./libringqueue.*
	
c: clean

r: clean all

