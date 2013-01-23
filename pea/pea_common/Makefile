CC = g++
FLAGS = -g -Wall -fPIC -DENABLE_TRACE_LOG -DUSE_TLOG

GEN_PROTO_PATH = ../../gen_proto/

TARGET = libpea_common.a

SRCS	= $(wildcard *.cpp)
OBJS	= $(patsubst %.cpp, %.o, $(SRCS))

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 
LIBS = -ltaomee -ltaomee++

all: $(TARGET)

$(TARGET): $(OBJS)
	ar -rvs $(TARGET) $(OBJS)
	@-rm -rf $(OBJS)

%.o: %.cpp
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<


.PHONY : all clean

clean:
	@-rm -f $(TARGET) $(OBJS) 

rebuild: clean all

c: clean

r: rebuild


a: gen all
