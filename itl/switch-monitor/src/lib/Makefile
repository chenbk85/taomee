XX       = g++
CFLAGS   = -Wall -O2 -ggdb -Wno-non-virtual-dtor -DHAVE_SYS_WAIT_H
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
                        
