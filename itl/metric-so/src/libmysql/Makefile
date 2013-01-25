CC       = g++
CFLAGS	= -Wall -Wno-non-virtual-dtor -O2 -fPIC -shared -export-dynamic
LIBS	= -litl_common -L$(LIBPATH) -ltaomee++ -ltaomee -ldl -lglib-2.0 `mysql_config --libs`
LIBPATH = ../../../itl-common/
INCLUDES = -I$(LIBPATH) -I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET	= ../../so/libmysql.so

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(INCLUDES) -o $(TARGET)
	@-rm *.o
	
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf *.o $(TARGET)
	
c: clean

r: clean all


