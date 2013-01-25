CC		= g++

FLAGS  	= -Wall -DDEBUG -ggdb -fPIC -MMD -rdynamic


SOFLAGS = -shared

LIBPATH = ../../itl-common/

GEN_PROTO_PATH = ../../gen_proto/

INCLUDE = -I$(LIBPATH) -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2/


LIBS	= -litl_common -L$(LIBPATH) -ltaomee++ -ltaomee -lcurl -lglib-2.0

BINS	= ../bin/libswitch.so

SUBDIRS	 = proto
SRCS 	= $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS	= $(patsubst %.cpp, %.o, $(SRCS))

all: dev



dev: FLAGS += -DENABLE_TRACE_LOG

rel: FLAGS += -O2




dev rel: $(OBJS) 
	$(CC) $(FLAGS) $(SOFLAGS) $(OBJS) $(LIBS) -o $(BINS).$@
	@-ln -sf $(BINS).$@ $(BINS)
	@-rm -rf $(OBJS)

%.o: %.cpp
	$(CC) $(FLAGS) $(INCLUDE) -c $< -o $@
	@-rm -f $*.d



.PHONY : all clean

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) ../core

rebuild: clean all

c: clean

r: rebuild

gen: gen_switch gen_head gen_db


gen_switch:
	$(GEN_PROTO_PATH)/getxml.sh itl_switch
	cp $(GEN_PROTO_PATH)/proto/itl.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl.cpp ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_switch*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_switch.cpp ./proto


gen_head:
	$(GEN_PROTO_PATH)/getxml.sh itl_head
	cp $(GEN_PROTO_PATH)/proto/itl_head*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_head.cpp ./proto


gen_db:
	$(GEN_PROTO_PATH)/getxml.sh itl_db
	cp $(GEN_PROTO_PATH)/proto/itl_db*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/itl_db.cpp ./proto

g: gen

a: g r
