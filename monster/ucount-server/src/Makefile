TARGET   = ../bin/libucountserver.so
XX       = g++ -g
LIBS 	 = -ltaomee
CPPFLAGS = -fPIC -Wall -Wno-non-virtual-dtor -export-dynamic -ggdb
SUBDIRS	 = work-proc ucount ../../common/pack ../../common/
SRCS 	 = $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS 	 = $(patsubst %.cpp, %.o, $(SRCS))


all: $(OBJS)
	$(XX) -shared -o $(TARGET) $(OBJS) $(LIBS) $(INCLUDE)
	rm -rf $(OBJS)

%.o : %.cpp
	$(XX) $(CPPFLAGS) $(DEBUG_FLAGS) $(INCLUDE) -c $< -o $@


.PHONY : all clean


clean:
	rm -f $(TARGET) $(OBJS)

rebuild: clean all

c: clean

r: rebuild
