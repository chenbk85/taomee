CC		= g++

CFLAGS  = -Wall -fPIC

SOFLAGS = -shared

LIBPATH = ../../itl-common/

GEN_PROTO_PATH = ../../gen_proto/

INCLUDE = -I$(LIBPATH) \
		  `pkg-config --cflags glib-2.0`

LIBS	= -litl_common -L$(LIBPATH) \
		  -ltaomee++ -ltaomee \
		  `pkg-config --libs libcurl glib-2.0`

BINS	= ../bin/libcontrol.so

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

gen: gen_control gen_db gen_node gen_alarm

gen_control:
	$(GEN_PROTO_PATH)/getxml.sh itl_control
	cp $(GEN_PROTO_PATH)/proto/itl.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl.cpp ./proto
	rm -f ./proto/itl_control*.*
	cp $(GEN_PROTO_PATH)/proto/itl_control*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_control.cpp ./proto


gen_db:
	$(GEN_PROTO_PATH)/getxml.sh itl_db
	rm -f ./proto/itl_db*.*
	cp $(GEN_PROTO_PATH)/proto/itl_db*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_db.cpp ./proto

gen_node:
	$(GEN_PROTO_PATH)/getxml.sh itl_node
	rm -f ./proto/itl_node*.*
	cp $(GEN_PROTO_PATH)/proto/itl_node*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_node.cpp ./proto

gen_alarm:
	$(GEN_PROTO_PATH)/getxml.sh itl_alarm
	rm -f ./proto/itl_alarm*.*
	cp $(GEN_PROTO_PATH)/proto/itl_alarm*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_alarm.cpp ./proto


g: gen

a: g r
