XX	= g++
CFLAGS	= -Wall -O3 -ggdb
LIBS	= -ldl -lrt -lpthread
INCLUDE	= -I. -I../../library/base -I../../library/net
SOURCES	= $(wildcard *.cpp)
ALL_BIN = $(patsubst %.o, %, $(OBJS))
OBJS	= $(patsubst %.cpp, %.o, $(SOURCES))
SUBOBJS	= ../../library/base/*.o ../../library/net/*.o ../../library/net/reactor_impl/*.o
NET_LIB = ../../library/

ERL_SRCS = $(wildcard *.erl)
ERL_XX = erlc

all: $(ALL_BIN)

$(ALL_BIN): $(OBJS) $(SUBOBJS)
	$(XX) -o $@ $(patsubst %, %.o, $(notdir $@)) $(SUBOBJS) $(LIBS)
	$(ERL_XX) $(ERL_SRCS)

%.o: %.cpp
	$(XX) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(SUBOBJS):
	cd $(NET_LIB) && make
clean:
	rm -rf *.o $(ALL_BIN) *.beam
	cd $(NET_LIB) && make clean

rebuild: clean all

c: clean

r: rebuild


