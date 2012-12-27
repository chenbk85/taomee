TARGET = libmonsterswitch.so
CC = g++
FLAGS = -Wall -DDEBUG -g -fPIC -MMD
SOFLAGS = -shared
LIB_PATH = ../../lib/
LIBS = -ltaomee -lxml2
INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2
SRCS = switch.cpp dispatch.cpp online.cpp shop.cpp everyday_fight.cpp utility.cpp

all: $(TARGET)

$(TARGET): $(SRCS:.cpp=.o)
				 $(CC) $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)
				 			mv $(TARGET) ../bin/

%.o: %.cpp Makefile
			$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<


clean:
			@-rm -f *.o *.d

