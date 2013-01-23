# vim: set noexpandtab tabstop=4 softtabstop=4 shiftwidth=4:

RING_QUEUE_DIR    = ../../../ring-queue/tags/ring-queue-1.2.2/
NET_IO_SERVER_DIR = ./net-io-server/
INI_FILE_DIR      = ../../../ini-file/tags/ini-file-1.1.1/

CC       = g++
BINS     = ../bin/newbench
INCLUDES = -I./others/ -I$(RING_QUEUE_DIR) -I$(NET_IO_SERVER_DIR) -I$(INI_FILE_DIR)
CFLAGS   = -ggdb -DDEBUG -DENABLE_TRACE_LOG -W -Wall -D_REENTRANT -D_GNU_SOURCE -Wno-non-virtual-dtor
LIBS     = -L$(RING_QUEUE_DIR) -L$(NET_IO_SERVER_DIR) -L$(INI_FILE_DIR) -ldl -rdynamic -lpthread ./lib/$(shell getconf LONG_BIT)/libbrq.a ./libnetioserver.a ./libringqueue.a ./libinifile.a
SRCS     = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp, %.o, $(SRCS)) ./others/log.o ./others/setproctitle.o

.PHONY: balanced ringqueue netioserver inifile all clean test rebuild a c r

all: $(OBJS) balanced ringqueue netioserver inifile
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(BINS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	cd pkg-filter/balanced/ && make clean
	rm -f *.o *.a $(BINS)

balanced: $(OBJS) ringqueue netioserver inifile
	cd ./pkg-filter/balanced/ && make clean && make static && cp ./bin/libbrq.a ../../lib/$(shell getconf LONG_BIT)/
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(BINS)

ringqueue: 
	cd $(RING_QUEUE_DIR) &&                                                    \
	make clean all &&                                                          \
	ar r libringqueue.a *.o &&                                                 \
	cd - &&                                                                    \
	cp -f $(RING_QUEUE_DIR)/libringqueue.a ./

netioserver:
	cd $(NET_IO_SERVER_DIR) &&                                                 \
	make clean && make &&                                                      \
	ar r libnetioserver.a *.o &&                                               \
	cd - &&                                                                    \
	cp -f $(NET_IO_SERVER_DIR)/libnetioserver.a ./

inifile:
	cd $(INI_FILE_DIR) &&                                                      \
	make clean && make &&                                                      \
	ar r libinifile.a *.o &&                                                   \
	cd - &&                                                                    \
	cp -f $(INI_FILE_DIR)/libinifile.a ./

c: clean
rebuild: clean all
r: rebuild
