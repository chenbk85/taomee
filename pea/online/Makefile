CC = g++

FLAGS  = -Wall -DDEBUG  -g -fPIC -MMD -DUSE_TLOG


SOFLAGS = -shared

LIBPATH = ../../pea_common/trunk 
GEN_PROTO_PATH = ../../gen_proto/

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 -I$(LIBPATH)


LIBS	= -ltaomee -ltaomee++ -lxml2 -lrt -lpthread -lssl -lpea_common -L$(LIBPATH)

BINS	= libonline.so

SUBDIRS	= proto
SRCS 	= $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS	= $(patsubst %.cpp, %.o, $(SRCS))

all: dev


dev: FLAGS += -DENABLE_TRACE_LOG -DUSE_HEX_LOG_PROTO
rel: FLAGS += -DCHECK_SESSION


dev rel: $(OBJS) 
	$(CC) $(FLAGS) $(SOFLAGS) -o $(BINS) $(OBJS) $(LIBS)
	@-rm -rf $(OBJS)

%.o: %.cpp
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-rm -f $*.d


.PHONY : all clean

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core

gen: gen_online gen_btlsw gen_btlsvr gen_db
	
	
new:
	cp ../../xml/online_server/*.xml ./conf/
	cp ../../xml/share/*.xml ./conf/

gen_online:
	$(GEN_PROTO_PATH)/getxml.sh pea_online
	cp $(GEN_PROTO_PATH)/proto/pea.h      ./proto
	cp $(GEN_PROTO_PATH)/proto/pea.cpp      ./proto
	cp $(GEN_PROTO_PATH)/proto/pea_online*.h      ./proto
	cp $(GEN_PROTO_PATH)/proto/pea_online.cpp    ./proto

gen_btlsw:
	$(GEN_PROTO_PATH)/getxml.sh pea_btlsw
	cp $(GEN_PROTO_PATH)/proto/pea_btlsw*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/pea_btlsw.cpp ./proto

gen_btlsvr:
	$(GEN_PROTO_PATH)/getxml.sh pea_battle
	cp $(GEN_PROTO_PATH)/proto/pea_battle_enum.h ./proto

gen_db:
	$(GEN_PROTO_PATH)/getxml.sh pea_db
	cp $(GEN_PROTO_PATH)/proto/pea_db*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/pea_db.cpp ./proto


rebuild: clean all

c: clean

r: rebuild

g: gen

a: g r
