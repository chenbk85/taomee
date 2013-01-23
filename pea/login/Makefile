CC		= g++

FLAGS  	= -Wall -DDEBUG -ggdb -fPIC -MMD -DUSE_TLOG


SOFLAGS = -shared

LIBPATH = ../../pea_common/trunk
GEN_PROTO_PATH = ../../gen_proto/

INCLUDE = -I$(LIBPATH) -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 -I/usr/local/include/main_login/


LIBS	= -lxml2 -ltaomee -ltaomee++ -lrt -lpthread -lpea_common -lmainlogin -L$(LIBPATH)

BINS	= liblogin.so

SUBDIRS	 = proto
SRCS 	= $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS	= $(patsubst %.cpp, %.o, $(SRCS))

all: dev



dev: FLAGS += -DENABLE_TRACE_LOG -DDISABLE_NOTI_LOG -DUSE_HEX_LOG_PROTO
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

rebuild: clean all

c: clean

r: rebuild

gen:
	$(GEN_PROTO_PATH)/getxml.sh pea_login
	cp $(GEN_PROTO_PATH)/proto/pea.h ./proto
	cp $(GEN_PROTO_PATH)/proto/pea.cpp ./proto
	cp $(GEN_PROTO_PATH)/proto/pea_login*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/pea_login.cpp ./proto
	
	$(GEN_PROTO_PATH)/getxml.sh pea_db
	cp $(GEN_PROTO_PATH)/proto/pea_db*.h ./proto
	cp $(GEN_PROTO_PATH)/proto/pea_db.cpp ./proto

g: gen

a: g r
