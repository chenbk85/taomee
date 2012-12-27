CC = g++

#FLAGS   = -Wall -pipe -DDEBUG -g -D_GNU_SOURCE  -D_REENTRANT
FLAGS   = -Wall -pipe -DNDEBUG -O3 -D_GNU_SOURCE  -D_REENTRANT
SOFLAGS = -shared -fPIC

INCLUDE = -I../../ -I../../../

OO	= racer.o #racetrack.o
LINKOBJ = ../mpog.o
TARGETS = libkart.so
GAME    = kartgame.cpp

all: $(OO) $(TARGETS)

$(TARGETS): $(OO) $(GAME)
	$(CC) $(FLAGS) $(SOFLAGS) $(INCLUDE) $(OO) $(LINKOBJ) $(GAME) -o $@

%.o: %.cpp %.hpp
	$(CC) $(FLAGS) $(INCLUDE) -c $<

clean:
	rm -f *.o
	rm -f $(TARGETS)
