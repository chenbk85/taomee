XX       = g++
#CFLAGS   = -Wall -ggdb -Wno-non-virtual-dtor 
CFLAGS   = -Wall -ggdb -Wno-non-virtual-dtor -DRELEASE_VERSION
LIBS     = `mysql_config --libs`
TARGET   = ../bin/oa_head_install
INCLUDES = -I ./ 
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES)) 

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(XX) $(OBJS) -o $(TARGET) $(LIBS)
	rm -rf *.o
	
%.o: %.cpp
	$(XX) $(CFLAGS) $(INCLUDES) -c $< -o $@	

clean:
	rm -rf *.o $(TARGET)
	
rebuild: clean all	
	
c: clean

r: rebuild
