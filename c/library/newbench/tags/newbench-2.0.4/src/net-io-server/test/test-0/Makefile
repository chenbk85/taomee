# vim: set noexpandtab tabstop=4 softtabstop=4 shiftwidth=4:

NET_IO_SERVER_SRC_DIR = ../../

CC       = g++
CFLAGS   = -Wall -Wno-non-virtual-dtor -ggdb
INCLUDES = -I$(NET_IO_SERVER_SRC_DIR)/../
LIBS     = -L. -lnetioserver
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET   = ./test_0

.PHONY: all clean c rebuild r netioserver

all: $(TARGET)

$(TARGET): $(OBJS) netioserver
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)
	rm -f $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
	rm -f lib*

rebuild: clean all
c: clean
r: rebuild

netioserver:
	@current_dir=`pwd` &&                                  \
	cd $(NET_IO_SERVER_SRC_DIR) &&                            \
	make rebuild &&                                        \
	cp ./libnetioserver.* $$current_dir/ &&                  \
	cd $$current_dir
