XX	= g++
CFLAGS	= -Wall -O3 -ggdb -pipe -D_GNU_SOURCE
INCLUDE	= -I. -I../../base -I../
SOURCES	= $(wildcard *.cpp)
OBJS	= $(patsubst %.cpp,%.o,$(SOURCES))
TARGET	= $(OBJS)

all: $(TARGET)
%.o: %.cpp
	$(XX) $(CFLAGS) $(INCLUDE) -c $< -o $@ $(LIBS)
clean:
	rm -fr *.o $(TARGET)
c: clean
r: clean all
