CC = g++

FLAGS   = -Wall -pipe -DDEBUG -g -D_GNU_SOURCE  -D_REENTRANT $(SHARE_FLAG) -fPIC
#FLAGS   = -Wall -pipe -DNDEBUG -O3 -D_GNU_SOURCE  -D_REENTRANT
SOFLAGS = -shared -fPIC

INCLUDE = -I../ -I../../ -I../../../ -I/usr/include/libxml2

all_srcs:=$(wildcard *.cpp)
all_objs:=$(patsubst %.cpp, %.o, $(all_srcs))

TARGETS	= liblink_link.so
LINKOBJ	= ../mpog.o

all: $(all_objs) $(TARGETS)

$(TARGETS): $(all_objs)
	$(CC) $(FLAGS) $(SOFLAGS) $(INCLUDE) $(LINKOBJ) $+ -o $@ -ltaomee
	mv $(TARGETS) ../

%.o: %.cpp %.h
	$(CC) $(FLAGS) $(INCLUDE) -c $<

clean:
	rm -f $(TARGETS) $(all_objs)
