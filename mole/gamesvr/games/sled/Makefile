CC = g++

#FLAGS   = -Wall -pipe -DDEBUG -g -D_GNU_SOURCE  -D_REENTRANT
FLAGS   = -Wall -pipe -DNDEBUG -O3 -D_GNU_SOURCE  -D_REENTRANT -fPIC $(SHARE_FLAG)   
SOFLAGS = -shared -fPIC

INCLUDE = -I../../ -I../../../

OO	= sledmap.o
LINKOBJ = ../mpog.o
TARGETS = libsled.so
GAME    = sledgame.cpp

all: $(OO) $(TARGETS)

$(TARGETS): $(OO) $(GAME)
	$(CC) $(FLAGS) $(SOFLAGS) $(INCLUDE) $(OO) $(LINKOBJ) $(GAME) -o $@
	mv $(TARGETS) ../

sledmap.o: sledmap.cpp sledmap.hpp coordinate.hpp
	$(CC) $(FLAGS) $(INCLUDE) -c $<

%.o: %.cpp %.hpp
	$(CC) $(FLAGS) $(INCLUDE) -c $<

clean:
	rm -f *.o
	rm -f $(TARGETS)
