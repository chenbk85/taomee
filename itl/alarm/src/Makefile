CC		= g++

CFLAGS  = -Wall -fPIC

SOFLAGS = -shared

LIBPATH = ../../itl-common/

GEN_PROTO_PATH = ../../gen_proto/

INCLUDE = -I$(LIBPATH) \
		  `pkg-config --cflags glib-2.0 libxml-2.0`

LIBS	= -litl_common -L$(LIBPATH) \
		  -ltaomee++ -ltaomee \
		  `pkg-config --libs glib-2.0 libcurl libxml-2.0`

BINS	= ../bin/libalarm.so

SUBDIRS	 = proto
SRCS 	= $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS	= $(patsubst %.cpp, %.o, $(SRCS))

all: dev



dev: CFLAGS += -ggdb -DENABLE_TRACE_LOG

rel: CFLAGS += -O2



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

gen: gen_alarm gen_db

gen_alarm:
	$(GEN_PROTO_PATH)/getxml.sh itl_alarm
	cp $(GEN_PROTO_PATH)/proto/itl.h ./proto
	rm -f ./proto/itl_alarm*.*
	cp $(GEN_PROTO_PATH)/proto/itl.cpp ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_alarm*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_alarm.cpp ./proto


gen_db:
	$(GEN_PROTO_PATH)/getxml.sh itl_db
	rm -f ./proto/itl_db*.*
	cp $(GEN_PROTO_PATH)/proto/itl_db*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_db.cpp ./proto

g: gen

a: g r
