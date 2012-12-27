TARGET = ../bin/libmonsterlogin.so
CC = g++
FLAGS = -Wall -DDEBUG -g -fPIC
SOFLAGS = -shared
LIBS = -ltaomee -ltaomee++ -lssl -lmainlogin -L/usr/local/lib/ 
INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I../../common/
SRCS    = $(wildcard *.cpp)
OBJS    = $(patsubst %.cpp, %.o, $(SRCS))

all: $(OBJS)
			 $(CC) $(FLAGS) $(SOFLAGS) -o $(TARGET) $(OBJS) $(LIBS)  $(INCLUDE)
			rm -fr $(OBJS)

%.o: %.cpp
		$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<


.PHONY : all clean

clean:
		rm -f $(TARGET) $(OBJS)

rebuild: clean all

c: clean

r: rebuild


