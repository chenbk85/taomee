XX       = g++
CFLAGS   = -Wall -ggdb -Wno-non-virtual-dtor
INCLUDE  = -I ../
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET   = $(OBJS)

all: $(TARGET)

%.o: %.cpp
	$(XX) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(TARGET)

c: clean

r: clean all
                        
