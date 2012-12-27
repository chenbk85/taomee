CC = g++

#FLAGS   = -Wall -pipe -DDEBUG -g -D_GNU_SOURCE  -D_REENTRANT
FLAGS   = -Wall -pipe -DNDEBUG -O3 -D_GNU_SOURCE  -D_REENTRANT -fPIC  $(SHARE_FLAG)
SOFLAGS = -shared -fPIC

INCLUDE = -I../../ -I../../../ -I../

OO	= kart_rule.o
TARGETS	= libkart.so
GAME	= kart.cpp
LINKOBJ	= ../mpog.o

all: $(OO) $(TARGETS)

$(TARGETS): $(GAME)
	$(CC) $(FLAGS) $(SOFLAGS) $(INCLUDE) $(LINKOBJ) $(OO) $(GAME) -o $@
	mv $(TARGETS) ../

%.o: %.cpp %.hpp
	$(CC) $(FLAGS) $(INCLUDE) -c $<

clean:
	rm -f *.o
	rm -f $(TARGETS) $(OO)
