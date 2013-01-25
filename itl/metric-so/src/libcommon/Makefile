XX       = g++
CFLAGS	= -Wall -Wno-non-virtual-dtor -O2 -fPIC -shared -export-dynamic -DREMOVE_BOGUS_SPIKES
LIBPATH = ../../../itl-common/
LIBS	= -litl_common -L$(LIBPATH) -ltaomee++ -ltaomee
INCLUDES = -I$(LIBPATH)
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET	= ../../so/libcommonproto.so

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


