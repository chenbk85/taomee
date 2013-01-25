CC		= g++

CFLAGS 	= -Wall -fPIC \
		  `mysql_config --cflags`

SOFLAGS = -shared

LIBPATH = ../../itl-common/

GEN_PROTO_PATH = ../../gen_proto/

INCLUDE = -I$(LIBPATH) \
		  `pkg-config --cflags glib-2.0`

LIBS	= -litl_common -L$(LIBPATH) \
		  -ltaomee++ -ltaomee \
		  `pkg-config --libs glib-2.0` \
		  `mysql_config --libs`

BINS	= ../bin/libdb.so

SUBDIRS	 = proto
SRCS 	= $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS	= $(patsubst %.cpp, %.o, $(SRCS))

all: dev



dev: FLAGS += -ggdb -DENABLE_TRACE_LOG -DUSE_NEW_DB_ITL

rel: FLAGS += -O2



dev rel: $(OBJS) 
	$(CC) $(CFLAGS) $(SOFLAGS) $(OBJS) $(LIBS) -o $(BINS).$@
	@-ln -sf $(BINS).$@ $(BINS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@



.PHONY : all clean

clean:
	@-rm -f $(OBJS) $(BINS) ../core

rebuild: clean all

c: clean

r: rebuild

gen:
	$(GEN_PROTO_PATH)/getxml.sh itl_db
	cp $(GEN_PROTO_PATH)/proto/itl.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl.cpp ./proto
	rm -f ./proto/itl_db*.*
	cp $(GEN_PROTO_PATH)/proto/itl_db*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_db.cpp ./proto

g: gen

a: g r
