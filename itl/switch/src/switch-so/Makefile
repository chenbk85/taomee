XX       = g++
CFLAGS	= -Wall -Wno-non-virtual-dtor -O -ggdb -fPIC -shared -export-dynamic 
LIBPATH = ../../../itl-common/
LIBS	= -litl_common -L$(LIBPATH) `net-snmp-config --libs` `net-snmp-config --agent-libs`
INCLUDES = -I$(LIBPATH) -I/usr/include/libxml2/
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET	= ../../so/libswitchproto.so

all: $(TARGET)

$(TARGET): $(OBJS)
	$(XX) $(CFLAGS) $(OBJS) $(LIBS) $(INCLUDES) -o $(TARGET)
	@-rm *.o
	
%.o: %.cpp
	$(XX) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@-rm -rf *.o $(TARGET)
	
c: clean

r: clean all


