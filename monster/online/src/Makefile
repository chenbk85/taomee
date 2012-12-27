TARGET = ../bin/libmonsteronline.so
CC = g++
FLAGS = -Wall -ggdb -fPIC
SOFLAGS = -shared
LIB_PATH = ../../lib/
LIBS = -ltaomee -ltaomee++ -lxml2 -lssl -lpthread -lglib-2.0
INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2/ -I../../common/ -I../../common/pack/
SUBDIRS = ../../common/pack/
SRCS    = $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS    = $(patsubst %.cpp, %.o, $(SRCS))

all: DEBUG_FLAGS = -DDEBUG -DUSE_TLOG
release: DEBUG_FLAGS = -DNDEBUG -DUSE_TLOG

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
