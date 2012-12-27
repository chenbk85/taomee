CC = g++
#FLAGS   = -Wall -pipe -DNDEBUG -g -ggdb -D_GNU_SOURCE  -D_REENTRANT -DDISABLE_DEBUG_LOG
#FLAGS   = -Wall -pipe -O3 -DNDEBUG -DDISABLE_DEBUG_LOG $(SHARE_FLAG)
#FLAGS   = -Wall -pipe -DNDEBUG -O3 -D_GNU_SOURCE  -D_REENTRANT
FLAGS   = -Wall -pipe -DDEBUG -g -D_GNU_SOURCE  -D_REENTRANT -fPIC  $(SHARE_FLAG)
SOFLAGS = -shared -fPIC

INCLUDE = -I../../ -I../../../

OO	= ../mpog.o
TARGETS = libpopgame.so
GAME    = my_errno.h pop.h pop.cpp single_pop_game.h single_pop_game.cpp pop_game_impl.h pop_game_impl.cpp

all: $(TARGETS)

$(TARGETS): $(GAME)
	$(CC) $(FLAGS) $(SOFLAGS) $(INCLUDE) $(OO) $(GAME) -o $@
	mv $(TARGETS) ../

%.o: %.cpp %.hpp
	$(CC) $(FLAGS) $(INCLUDE) -c $<

clean:
	rm -f *.o
	rm -f $(TARGETS)
