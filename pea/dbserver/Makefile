CC = g++ 


LIBPATH = ../../pea_common/trunk

GEN_PROTO_PATH = ../../gen_proto/

CFLAGS  = -DGF_PROTO_HEADER  -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT -D__STDC_FORMAT_MACROS -fPIC

LIBDIR  = -L/usr/lib/mysql -L $(LIBPATH)

LIB 	= -ldbser -ldl -lmysqlclient -ltaomee++ -ltaomee -lxml2 -lrt -lglib-2.0 -lpea_common

INCLUDE = -I/usr/include/libxml2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/dbser/ -I/usr/include/mysql/ -I$(LIBPATH)

SUBDIRS	 = proto
SRCS 	= $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS	= $(patsubst %.cpp, %.o, $(SRCS))

TARGETS = libdbser.so

all: dev


dev: CFLAGS += -DCREATE_ROLE_WITH_DATA
rel: CFLAGS +=

dev rel: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) -o $(TARGETS) $(LIBDIR) $(LIB)
	@-rm -rf $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<


.PHONY : all clean

clean:
	@-rm -f $(TARGETS) $(OBJS)

gen:
	$(GEN_PROTO_PATH)/getxml.sh pea_db
	cp $(GEN_PROTO_PATH)/proto/pea.h  ./proto
	cp $(GEN_PROTO_PATH)/proto/pea.cpp  ./proto
	cp $(GEN_PROTO_PATH)/proto/pea_db*.h  ./proto
	cp $(GEN_PROTO_PATH)/proto/pea_db.cpp ./proto



rebuild: clean all

c: clean

r: rebuild


g: gen

a: g r
