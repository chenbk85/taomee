XX	= g++
CFLAGS	= -Wall -O3 -ggdb -D_GNU_SOURCE
LIBS	= -ldl -lrt -lpthread
INCLUDE	= -I. -I../library/base -I../library/net -I../library/net/http
SOURCES	= $(wildcard *.cpp)
BIN = async_serv
OBJS	= $(patsubst %.cpp, %.o, $(SOURCES))
SUBOBJS	= ../library/base/*.o ../library/net/*.o ../library/net/reactor_impl/*.o ../library/net/http/*.o
NET_LIB = ../library/

all: $(BIN)

$(BIN): $(OBJS) $(SUBOBJS)
	$(XX) $(OBJS) $(SUBOBJS) $(LIBS) -rdynamic -o $(BIN)

%.o: %.cpp
	$(XX) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(SUBOBJS):
	cd $(NET_LIB) && make
clean:
	rm -rf *.o $(BIN)
	cd $(NET_LIB) && make clean

rebuild: clean all

c: clean

r: rebuild


