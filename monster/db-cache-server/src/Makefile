TARGET   = ../bin/libdbcacheserver.so
XX       = g++
LIB_PATH = ../../lib
LIBS 	 = -ltaomee -lsafemysql -lhiredis -L$(LIB_PATH)/so/
INCLUDE  =  -I$(LIB_PATH)/include/ -I../../common/ -I../../common/pack/
CPPFLAGS = -g3 -fPIC -Wall -Wno-non-virtual-dtor -export-dynamic -ggdb
SUBDIRS	 = work-proc  ../../common/pack ../../common/
SRCS 	 = $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS 	 = $(patsubst %.cpp, %.o, $(SRCS))

# 默认编译debug版本
all: bins

bins: $(OBJS)
	$(XX) -shared -o $(TARGET) $(OBJS) $(LIBS) $(INCLUDE)
	rm -rf $(OBJS)

%.o : %.cpp
	$(XX) $(CPPFLAGS) $(INCLUDE) -c $< -o $@



.PHONY : all clean


clean:
	rm -f $(TARGET) $(OBJS)

rebuild: clean all

c: clean

r: rebuild
