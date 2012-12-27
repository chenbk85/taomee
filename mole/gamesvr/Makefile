#DEBUG	= -g -DDEBUG -DGAME_TEST
DEBUG	= -g -DDEBUG
OPT	= -funroll-loops -D__USE_STRING_INLINES -fhosted
OPT	+= -DEPOLL_MODE -g -I./

SHARE_FLAG = # -DTW_VER
export SHARE_FLAG

CC	= gcc
CPP	= g++
BINS	= gameserver
SRCS 	= main.c config.c daemon.c util.c shmq.c net.c tcp.c sprite.c proto.c service.c dll.c \
			dbproxy.c game.c timer.c mcast.c gamexx.oxx

CFLAGS:= -Wall -g -I/usr/include/libxml2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I./ -MMD $(DEBUG) -D_REENTRANT -D_GNU_SOURCE $(OPT) $(SHARE_FLAG)
cpp_flag:= -Wall -I/usr/include/libxml2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I./ -MMD $(DEBUG) -D_REENTRANT -D_GNU_SOURCE $(SHARE_FLAG)
LIBS	+= -lxml2 -ldl -rdynamic -lm -lglib-2.0 -ltaomee

.PHONY: all clean doc

all: base game_base cardgame chinesechess gobang kart pop sled surf_riding tetris tug_of_war paopaoball flight_game rushques new_card angel_fight linker blow_candles link_link
#all: base

base: $(BINS)

game_base:
	cd games && make clean all
cardgame:
	cd games/cardgame && make clean all
chinesechess:
	cd games/chinesechess && make clean all
gobang:
	cd games/gobang && make clean all
kart:
	cd games/kart && make clean all
pop:
	cd games/pop && make clean all && make -f makefile-m clean all
sled:
	cd games/sled && make clean all
surf_riding:
	cd games/surf_riding && make clean all
tetris:
	cd games/tetris && make clean all
tug_of_war:
	cd games/tug_of_war && make clean all
paopaoball:
	cd games/paopaoball && make clean all
flight_game:
	cd games/flight_game && make clean all
rushques:
	cd games/rushques && make clean all
new_card:
	cd games/new_card && make clean all
angel_fight:
	cd games/angel_fight && make clean all
linker:
	cd games/linker && make clean all
blow_candles:
	cd games/blow_candles && make clean all
link_link:
	cd games/link_link && make clean all
%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<
	@-mv -f $*.d .dep.$@

%.oxx: %.cpp Makefile
	$(CPP)  $(cpp_flag) -c -o $@ $<

$(BINS): $(SRCS:.c=.o)
	$(CPP)  $(LDFLAGS) -o $@ $^ $(LIBS)
#	rm ../bin/$(BINS); mv $(BINS) ../bin/
clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core

doc:
	@doxygen

-include $(wildcard .dep.*)
