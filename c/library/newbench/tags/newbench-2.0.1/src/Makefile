# vim: set noexpandtab tabstop=4 softtabstop=4 shiftwidth=4:

RING_QUEUE_DIR    = ../../../../ring-queue/tags/ring-queue-1.2.2/
NET_IO_SERVER_DIR = ./net-io-server/
INI_FILE_DIR      = ../../../../ini-file/tags/ini-file-1.1.1/

CC       = g++
BINS     = ../bin/newbench
INCLUDES = -I$(RING_QUEUE_DIR) -I$(NET_IO_SERVER_DIR) -I$(INI_FILE_DIR)
CFLAGS   = -ggdb -DDEBUG -DENABLE_TRACE_LOG -W -Wall -D_REENTRANT -D_GNU_SOURCE -Wno-non-virtual-dtor
LIBS     = -L$(RING_QUEUE_DIR) -lringqueue -L$(NET_IO_SERVER_DIR) -lnetioserver -L$(INI_FILE_DIR) -linifile -ldl -rdynamic -lpthread ./lib/$(shell getconf LONG_BIT)/libbrq.a
SRCS     = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp, %.o, $(SRCS))

.PHONY: ringqueue netioserver inifile all clean test rebuild a c r

all: $(OBJS) balanced ringqueue netioserver inifile
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(BINS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	cd pkg-filter/balanced/ && make clean
	@rm -f *.o *.so $(BINS)
	@rm -f ../bin/libringqueue* ../bin/libnetioserver*

balanced: $(OBJS) ringqueue netioserver inifile
	cd ./pkg-filter/balanced/ && make clean && make static && cp ./bin/libbrq.a ../../lib/$(shell getconf LONG_BIT)/
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(BINS)

ringqueue: 
	cd $(RING_QUEUE_DIR) && make clean all && cd -
	cp -f $(RING_QUEUE_DIR)/libringqueue* ../bin/

netioserver:
	cd $(NET_IO_SERVER_DIR) &&                                                 \
	make clean && make &&                                                      \
	cd - &&                                                                    \
	cp -f $(NET_IO_SERVER_DIR)/libnetioserver* ../bin/

inifile:
	cd $(INI_FILE_DIR) &&                                                      \
	make clean && make &&                                                      \
	cd - &&                                                                    \
	cp -f $(INI_FILE_DIR)/libinifile* ../bin/

c: clean
rebuild: clean all
r: rebuild
