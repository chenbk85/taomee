CC = g++

CFLAGS = -Wall -fPIC \
		 `mysql_config --cflags`

TARGET = libitl_common.a

SRCS	= $(wildcard *.cpp)
OBJS	= $(patsubst %.cpp, %.o, $(SRCS))

INCLUDE = `pkg-config --cflags glib-2.0 libxml-2.0`

LIBS = -ltaomee++ -ltaomee \
	   `mysql_config --libs`

all: dev
	
dev: CFLAGS += -ggdb
rel: CFLAGS += -O2
	

dev rel: $(OBJS)
	ar -rvs $(TARGET) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@


.PHONY : all clean

clean:
	@-rm -f $(TARGET) $(OBJS) 

rebuild: clean all

c: clean

r: rebuild
