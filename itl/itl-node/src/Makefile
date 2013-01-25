CC		= g++

CFLAGS  = -Wall -fPIC -rdynamic \
		  `mysql_config --cflags`

SOFLAGS = -shared

LIBPATH = ../../itl-common/

GEN_PROTO_PATH = ../../gen_proto/

INCLUDE = -I$(LIBPATH) \
		  `pkg-config --cflags glib-2.0 libxml-2.0`

LIBS	= -litl_common -L$(LIBPATH) \
		  -ltaomee++ -ltaomee \
		  `pkg-config --libs libcurl libxml-2.0 glib-2.0`


BINS	= ../bin/libnode.so

SUBDIRS	 = proto collect db_mgr
SRCS 	= $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS	= $(patsubst %.cpp, %.o, $(SRCS))

all: dev



dev: CFLAGS += -ggdb -DENABLE_TRACE_LOG -DMETRIC_LOG -DSHORT_COLLECT_INTERVAL
dev: LIBS += `mysql_config --libs`

rel: CFLAGS += -O2 -DSET_PROC_USER
rel: LIBS += /usr/lib/libmysqlclient.a

centos: LIBS = -litl_common -L$(LIBPATH) -ltaomee++ -ltaomee `pkg-config --libs libcurl libxml-2.0 openssl` /usr/local/lib/libglib-2.0.a /usr/local/lib/mysql/libmysqlclient.a
centos: CFLAGS += -O2 -DSET_PROC_USER -DCENTOS

dev rel centos: $(OBJS) 
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

gen: gen_node gen_db gen_head gen_control

gen_node:
	$(GEN_PROTO_PATH)/getxml.sh itl_node
	cp $(GEN_PROTO_PATH)/proto/itl.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl.cpp ./proto
	rm -f ./proto/itl_node*.*
	cp $(GEN_PROTO_PATH)/proto/itl_node*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_node.cpp ./proto

gen_db:
	$(GEN_PROTO_PATH)/getxml.sh itl_db
	rm -f ./proto/itl_db*.*
	cp $(GEN_PROTO_PATH)/proto/itl_db*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_db.cpp ./proto


gen_head:
	$(GEN_PROTO_PATH)/getxml.sh itl_head
	rm -f ./proto/itl_head*.*
	cp $(GEN_PROTO_PATH)/proto/itl_head*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_head.cpp ./proto

gen_control:
	$(GEN_PROTO_PATH)/getxml.sh itl_control
	rm -f ./proto/itl_control*.*
	cp $(GEN_PROTO_PATH)/proto/itl_control*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_control.cpp ./proto


g: gen

a: g r
