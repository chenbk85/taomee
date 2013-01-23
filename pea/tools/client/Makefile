CC = g++

FLAGS  = -Wall -DDEBUG -g -fPIC -MMD

GEN_PROTO = ../..//gen_proto/


SOFLAGS = -shared

LIBPATH = ../../pea_common/trunk 

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 -I$(LIBPATH)


LIBS	= ${KFLIB} -ltaomee -ltaomee++ -lxml2 -lrt -lpthread -lpea_common -L$(LIBPATH)

BINS	= libonline.so

SUBDIRS	 = proto
SRCS 	= $(wildcard *.cpp $(foreach n, $(SUBDIRS), $(n)/*.cpp))
OBJS	= $(patsubst %.cpp, %.o, $(SRCS))

all: gen_online




.PHONY : all clean

clean:
	rm -rf *.pyc

gen: gen_online gen_btlsw gen_btlsvr gen_db gen_login




gen_online:
	$(GEN_PROTO)/getxml.sh pea_online
	cp $(GEN_PROTO)/python/pea_online_proto.py ./

gen_btlsw:
	$(GEN_PROTO)/getxml.sh pea_btlsw
	cp $(GEN_PROTO)/python/pea_btlsw_proto.py ./

gen_btlsvr:
	$(GEN_PROTO)/getxml.sh pea_battle
	cp $(GEN_PROTO)/python/pea_battle_proto.py ./

gen_db:
	$(GEN_PROTO)/getxml.sh pea_db
	cp $(GEN_PROTO)/python/pea_db_proto.py ./

gen_login:
	$(GEN_PROTO)/getxml.sh pea_login
	cp $(GEN_PROTO)/python/pea_login_proto.py ./

rebuild: clean all

c: clean

r: rebuild

g: gen
