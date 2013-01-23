# 如果动态链接库不在主目录，请配置下面的路径变量
LIBS_DYNAMIC_DIR := $(HOME)/lib/

CC := g++
BINS := newbench

PROFILER := -DENABLE_PROFILER
DEBUG := -g -DDEBUG -DENABLE_TRACE_LOG
OPT := -O3 -funroll-loops -D__USE_STRING_INLINES

CFLAGS := -W -Wall -D_REENTRANT -D_GNU_SOURCE -Wno-non-virtual-dtor $(DEBUG) $(OPT)

LIBS += -ldl -rdynamic -lpthread
LIBS_DYNAMIC_DIR := ./lib/$(shell getconf LONG_BIT)/
LIBS_DYNAMIC = -L$(LIBS_DYNAMIC_DIR) -lnetioserver -lringqueue -lbrq -linifile
LIBS_DIR := ./lib/$(shell getconf LONG_BIT)/
LIBS_STATIC = $(LIBS_DIR)libnetioserver.a $(LIBS_DIR)libringqueue.a $(LIBS_DIR)libinifile.a $(LIBS_DIR)libbrq.a

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

.PHONY: all clean test

# 默认编译静态链接版本
all: static

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

# 编译动态链接版本
dynamic: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LIBS_DYNAMIC) -o $(BINS)

# 编译静态链接版本
static: $(OBJS) balanced
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LIBS_STATIC) -o $(BINS)

pkg-filter-dynamic:
	cd pkg-filter && make clean && make so

pkg-filter-static:
	cd pkg-filter && make clean && make a

clean:
	cd pkg-filter/balanced/ && make clean && cd ../dmux/ && make clean
	@-rm -f *.o *.so $(BINS)

balanced: $(OBJS)
	cd ./pkg-filter/balanced/ && make clean && make static && cp ./bin/libbrq.a ../../lib/$(shell getconf LONG_BIT)/
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LIBS_STATIC) -o $(BINS)

dmux: $(OBJS)
	cd ./pkg-filter/dmux/ && make clean && make static && cp ./bin/libbrq.a ../../lib/$(shell getconf LONG_BIT)/
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LIBS_STATIC) -o $(BINS)
