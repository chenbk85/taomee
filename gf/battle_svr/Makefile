
ifndef RELEASE_O2
FLAGS  = -Wall -DDEBUG -g -fPIC -MMD
else
FLAGS  = -Wall -DDEBUG -O2 -fPIC -MMD
endif

ifndef RELEASE
FLAGS += -DENABLE_TRACE_LOG  -DENABLE_GONGFU_LOG
CC = clang++
else
FLAGS += -DRELEASE_VER	-DENABLE_GONGFU_LOG
CC = g++
endif


ifdef VERSION_TAIWAN
	FLAGS += -DVERSION_TAIWAN
endif

ifdef VERSION_KAIXIN          
    FLAGS += -DVERSION_KAIXIN 
    KFLIB = -lkfkxcommon      
else   
    KFLIB = -lkfcommon        
endif



SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 -I/usr/local/include/
#INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2
LIBPATH = 
LIBS	= ${KFLIB} -ltaomee -ltaomee++ -lxml2 -lrt -lpthread
#LIBS	= ${KFLIB} -ltaomee -ltaomee++ -lxml2 -llua5.1 -lluabind #-lkfcommon
#LIBS	= ${KFLIB} -ltaomee -ltaomee++ -lxml2

BINS	= libbattle.so
SRCS 	= object.cpp \
			dll_interface.cpp fwd_decl.cpp dbproxy.cpp \
			player.cpp item.cpp monster.cpp skill.cpp stage.cpp \
			cli_proto.cpp battle.cpp battle_impl.cpp reload_conf.cpp \
		    base_effect.cpp	buff.cpp aura.cpp player_status.cpp skill_imme_effect.cpp \
			battle_switch.cpp roll.cpp battle_manager.cpp map_summon_object.cpp player_attribute.cpp \
			cache_switch.cpp skill_additional_damage.cpp battle_logic.cpp indoor_test.cpp \
			mechanism_drop.cpp  pet_attribute.cpp title_attribute.cpp detective.cpp

all: $(BINS)

$(BINS): $(SRCS:.cpp=.o) subdir
# $(CC)  $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)
	$(CC)  $(FLAGS) $(SOFLAGS) -o $(BINS) *.o liblua.a libluabind.a $(LIBPATH) $(LIBS)
	-rm ./bin/$(BINS)
	cp $(BINS) ./bin

%.o: %.cpp  Makefile
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-mv -f $*.d .dep.$@
	
subdir:
	make -C ai

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core
	make -C ai clean

-include /dev/null $(wildcard .dep.*)
