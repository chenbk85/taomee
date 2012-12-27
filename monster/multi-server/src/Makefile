TARGET = libmultiserver.so
CC = g++
FLAGS = -Wall -DDEBUG -g -fPIC -MMD
SOFLAGS = -shared
LIBS = -ltaomee -lxml2
INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2  -I../../common/utility/
SRCS = dll_interface.cpp work_proc.cpp ../../common/utility/utility.cpp

all: $(TARGET)

$(TARGET): $(SRCS:.cpp=.o)
			 $(CC) $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBS)
			mv $(TARGET) ../bin/

%.o: %.cpp Makefile
		$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<


clean:
		@-rm -f *.o *.d


