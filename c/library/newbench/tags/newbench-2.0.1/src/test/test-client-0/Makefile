# vim: set noexpandtab tabstop=4 softtabstop=4 shiftwidth=4:

NET_CLIENT_SRC_DIR = ../../../../../net-client/trunk/

CC       = g++
CFLAGS   = -Wall -ggdb
INCLUDES = -I$(NET_CLIENT_SRC_DIR)
LIBS     = -L$(NET_CLIENT_SRC_DIR) -lnetclient
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET   = ./test_client

.PHONY: all clean c rebuild r netclient

all: $(TARGET)

$(TARGET): $(OBJS) netclient
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

netclient:
	@current_dir=`pwd` &&                                  \
	cd $(NET_CLIENT_SRC_DIR) &&                            \
	make rebuild &&                                        \
	cp ./libnetclient.* $$current_dir/ &&                  \
	cd $$current_dir
