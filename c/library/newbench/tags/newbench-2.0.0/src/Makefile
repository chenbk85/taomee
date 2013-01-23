# vim: set noexpandtab tabstop=4 softtabstop=4 shiftwidth=4:

# 用户配置自定义部分

# ring-queue的目录
RING_QUEUE_DIR = ../../../../ring-queue/tags/ring-queue-1.2.2/

# net-io-server的目录
NET_IO_SERVER_DIR = ./net-io-server/

# ini-file的目录
INI_FILE_DIR = ../../../../ini-file/tags/ini-file-1.1.1/

CC       = g++
BINS     = ../bin/newbench
INCLUDES = -I$(RING_QUEUE_DIR) -I$(NET_IO_SERVER_DIR) -I$(INI_FILE_DIR)
PROFILER = -DENABLE_PROFILER
DEBUG    = -ggdb -DDEBUG -DENABLE_TRACE_LOG
# OPT      = -O3 -funroll-loops -D__USE_STRING_INLINES
OPT      = -funroll-loops -D__USE_STRING_INLINES
CFLAGS   = -W -Wall -D_REENTRANT -D_GNU_SOURCE -Wno-non-virtual-dtor $(DEBUG) $(OPT)

LIBS = -L$(RING_QUEUE_DIR) -lringqueue -L$(NET_IO_SERVER_DIR) -lnetioserver -L$(INI_FILE_DIR) -linifile -ldl -rdynamic -lpthread
LIBS_DYNAMIC = -L$(LIBS_DYNAMIC_DIR) -lnetioserver -lringqueue -lbrq -linifile
LIBS_DIR := ./lib/$(shell getconf LONG_BIT)/
LIBS_STATIC = $(LIBS_DIR)libbrq.a

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

.PHONY: ringqueue netioserver inifile all clean test rebuild a c r

# 默认编译静态链接版本
all: static

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# 编译动态链接版本
dynamic: $(OBJS) ringqueue netioserver inifile
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) $(LIBS) $(LIBS_DYNAMIC) -o $(BINS)

# 编译静态链接版本
static: $(OBJS) balanced ringqueue netioserver inifile
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LIBS_STATIC) -o $(BINS)

pkg-filter-dynamic:
	cd pkg-filter && make clean && make so

pkg-filter-static:
	cd pkg-filter && make clean && make a

clean:
	cd pkg-filter/balanced/ && make clean && cd ../dmux/ && make clean
	@-rm -f *.o *.so $(BINS)
	@rm -f ../bin/libringqueue* ../bin/libnetioserver*

balanced: $(OBJS) ringqueue netioserver inifile
	cd ./pkg-filter/balanced/ && make clean && make static && cp ./bin/libbrq.a ../../lib/$(shell getconf LONG_BIT)/
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LIBS_STATIC) -o $(BINS)

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

dmux: $(OBJS)
	cd ./pkg-filter/dmux/ && make clean && make static && cp ./bin/libbrq.a ../../lib/$(shell getconf LONG_BIT)/
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LIBS_STATIC) -o $(BINS)

c: clean
rebuild: clean all
r: rebuild
