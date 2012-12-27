TARGET = ../bin/libmonstershare.so
CC = g++
FLAGS = -Wall -ggdb -fPIC
SOFLAGS = -shared
LIB_PATH = ../../lib/
LIBS = -ltaomee -ltaomee++ -lxml2
INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2/ -I../../common/ -I../../common/pack/ -I../../common/timer/
SUBDIRS = ../../common/pack/
SRCS    = $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS    = $(patsubst %.cpp, %.o, $(SRCS))

all: DEBUG_FLAGS = -DDEBUG
release: DEBUG_FLAGS = -DNDEBUG

all: $(OBJS)
			 $(CC) $(SOFLAGS) -o $(TARGET) $(OBJS) $(LIBS)  $(INCLUDE)
			rm -fr $(OBJS)

release: $(OBJS)
			 $(CC) $(SOFLAGS) -o $(TARGET) $(OBJS) $(LIBS)  $(INCLUDE)
			rm -fr $(OBJS)

%.o: %.cpp
		$(CC) $(FLAGS) $(DEBUG_FLAGS) $(INCLUDE) -c -o $@ $<


.PHONY : all clean

clean:
		rm -f $(TARGET) $(OBJS)

rebuild: clean all

c: clean

r: rebuild


