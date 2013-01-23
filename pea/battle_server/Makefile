CC = g++

FLAGS  = -Wall -DDEBUG -g -fPIC -MMD -DUSE_TLOG


SOFLAGS = -shared

LIBPATH = ../../pea_common/trunk 
GEN_PROTO_PATH = ../../gen_proto/

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 -I/usr/include/lua5.1 -I$(LIBPATH)

LIBS	= -ltaomee -ltaomee++ -lxml2 -lpea_common  -lssl -L$(LIBPATH)

BINS	= libbattle.so

SUBDIRS	= proto
SRCS 	= $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS	= $(patsubst %.cpp, %.o, $(SRCS))


all: dev


dev: FLAGS += -DENABLE_TRACE_LOG -DUSE_HEX_LOG_PROTO
rel: FLAGS += 


dev rel: $(OBJS) 
	$(CC) $(FLAGS) $(SOFLAGS) -o $(BINS) $(OBJS) $(LIBS)
	@-rm -rf $(OBJS)

%.o: %.cpp
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-rm -f $*.d


.PHONY : all clean

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core

new: 
	cp ../../xml/battle_server/*.xml ./conf/
	cp ../../xml/share/*.xml ./conf/
	rm -rf ./conf/stages/*.*
	rm -rf ./conf/maps/*.*
	cp ../../xml/battle_server/stages/*.* ./stages/
	cp ../../xml/battle_server/maps/*.* ./maps/
	    

gen:
	$(GEN_PROTO_PATH)/getxml.sh pea_battle
	cp $(GEN_PROTO_PATH)/proto/pea.h      		./proto/
	cp $(GEN_PROTO_PATH)/proto/pea.cpp     		./proto
	cp $(GEN_PROTO_PATH)/proto/pea_battle*.h    ./proto
	cp $(GEN_PROTO_PATH)/proto/pea_battle.cpp   ./proto
	
	$(GEN_PROTO_PATH)/getxml.sh pea_online
	cp $(GEN_PROTO_PATH)/proto/pea_online_enum.h ./proto
	
	$(GEN_PROTO_PATH)/getxml.sh pea_db
	cp $(GEN_PROTO_PATH)/proto/pea_db*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/pea_db.cpp ./proto


rebuild: clean all

c: clean

r: rebuild


g: gen

a: g r
