# vim: set noexpandtab tabstop=4 softtabstop=4 shiftwidth=4:
# 用户配置自定义部分

# ring-queue的目录
RING_QUEUE_DIR = ../../ring-queue/tags/ring-queue-1.2.1/

# net-io-server的目录
NET_IO_SERVER_DIR = ../../net-io-server/tags/net-io-server-2.1.0/

CC = g++
BINS = newbench

PROFILER = -DENABLE_PROFILER
DEBUG = -g -DDEBUG -DENABLE_TRACE_LOG
OPT = -O3 -funroll-loops -D__USE_STRING_INLINES

CFLAGS = -W -Wall -D_REENTRANT -D_GNU_SOURCE -Wno-non-virtual-dtor $(DEBUG) $(OPT)

LIBS = -L$(RING_QUEUE_DIR) -lringqueue -L$(NET_IO_SERVER_DIR)/bin/ -lnetioserver -ldl -rdynamic -lpthread
LIBS_DYNAMIC = -L$(LIBS_DYNAMIC_DIR) -lnetioserver -lringqueue -lbrq -linifile
LIBS_DIR := ./lib/$(shell getconf LONG_BIT)/
LIBS_STATIC = $(LIBS_DIR)libinifile.a $(LIBS_DIR)libbrq.a

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

.PHONY: ringqueue netioserver all clean test rebuild a c r

# 默认编译静态链接版本
all: static

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

# 编译动态链接版本
dynamic: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LIBS_DYNAMIC) -o $(BINS)

# 编译静态链接版本
static: $(OBJS) balanced ringqueue netioserver
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LIBS_STATIC) -o $(BINS)

pkg-filter-dynamic:
	cd pkg-filter && make clean && make so

pkg-filter-static:
	cd pkg-filter && make clean && make a

clean:
	cd pkg-filter/balanced/ && make clean && cd ../dmux/ && make clean
	@-rm -f *.o *.so $(BINS)
	@rm -f libringqueue* libnetioserver*

balanced: $(OBJS) ringqueue
	cd ./pkg-filter/balanced/ && make clean && make static && cp ./bin/libbrq.a ../../lib/$(shell getconf LONG_BIT)/
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LIBS_STATIC) -o $(BINS)

ringqueue: 
	cd $(RING_QUEUE_DIR) && make clean all && cd -
	cp -f $(RING_QUEUE_DIR)/libringqueue* ./

netioserver:
	cd $(NET_IO_SERVER_DIR) &&                                                 \
	sed -i "16s/4096/8192/" ./src/header/c_net_io_server.h  &&                 \
	sed -i "18s/(4/(1024/" ./src/header/c_net_io_server.h  &&                  \
	sed -i "19s/(4/(1024/" ./src/header/c_net_io_server.h  &&                  \
	make clean && make &&                                                      \
	cd - &&                                                                    \
	cp -f $(NET_IO_SERVER_DIR)/bin/libnetioserver* ./

dmux: $(OBJS)
	cd ./pkg-filter/dmux/ && make clean && make static && cp ./bin/libbrq.a ../../lib/$(shell getconf LONG_BIT)/
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LIBS_STATIC) -o $(BINS)

c: clean
rebuild: clean all
r: rebuild
