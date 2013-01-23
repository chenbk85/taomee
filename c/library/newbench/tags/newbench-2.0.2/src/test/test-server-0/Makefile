# vim: set noexpandtab tabstop=4 softtabstop=4 shiftwidth=4:

NEWBENCH_SRC_DIR = ../../

XX       = g++
CFLAGS   = -Wall -fPIC -ggdb
INCLUDES = -I../../
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET   = ./bin/test_server.so

.PHONY: all clean c rebuild r test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(XX) -shared -o $(TARGET) $(OBJS)
	rm -f $(OBJS)

%.o: %.cpp
	$(XX) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
	rm -f ./bin/*

rebuild: clean all
c: clean
r: rebuild

test: all
	@current_dir=`pwd` &&                                  \
	cd $(NEWBENCH_SRC_DIR) &&                              \
	make rebuild &&                                        \
	cp ../bin/newbench $$current_dir/bin/ &&               \
	cd $$current_dir
