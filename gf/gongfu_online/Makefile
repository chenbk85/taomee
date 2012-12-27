CC = g++

FLAGS  = -Wall -DDEBUG -g -fPIC -MMD  
ifndef RELEASE
FLAGS += -DENABLE_TRACE_LOG  -DENABLE_GONGFU_LOG -DMOCK_TEST
else
FLAGS += -DRELEASE_VER -DENABLE_GONGFU_LOG 
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

SOFLAGS = -shared -fPIC

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 -I/usr/local/include/lua5.1
LIBPATH =
LIBS	= -ltaomee -ltaomee++ -lxml2 ${KFLIB} liblua.a -lrt -lpthread -lssl

BINS	= libkf.so
E_BIN = libdata.so

SRCS 	= dll_interface.cpp fwd_decl.cpp reload_conf.cpp dbproxy.cpp switch.cpp \
			switch.cpp player.cpp map.cpp message.cpp item.cpp\
			cli_proto.cpp contest.cpp common_op.cpp login.cpp battle.cpp\
			cachesvr.cpp home.cpp task.cpp skill.cpp user_feedback.cpp restriction.cpp\
			summon_monster.cpp ambassador.cpp warehouse.cpp vip_impl.cpp apothecary.cpp\
			secondary_profession.cpp magic_number.cpp mail.cpp battle_switch.cpp\
			store_sales.cpp trade.cpp cache_switch.cpp stage_config.cpp gf_cryptogram.cpp \
			buy_item_limit.cpp coupons.cpp achievement.cpp kill_boss.cpp \
			lua_engine.cpp app_log.cpp log.cpp title_system.cpp ap_toplist.cpp use_item_history.cpp \
			indoor_test.cpp ring_task.cpp check_shop.cpp limit.cpp home.cpp home_impl.cpp  \
			card.cpp fight_team.cpp active_config.cpp rank_top.cpp random_event.cpp player_attribute.cpp \
			master_prentice.cpp task_logic.cpp other_active.cpp svc_ac_api.cpp chat.cpp \
			numen.cpp god_guard.cpp temporary_team.cpp

E_SRCS = global_data.cpp

all: $(BINS) $(E_BIN)
	



$(BINS): $(SRCS:.cpp=.o)
	$(CC)  $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)
	@-rm ./bin/$(BINS)
	@-cp $(BINS) ./bin
$(E_BIN):$(E_SRCS:.cpp=.o)
	$(CC) $(FLAGS) $(SOFLAGS) -o $@ $^  $(LIBPATH) $(LIBS)
	@-rm -f ./bin/$(E_BIN)
	@-cp $(E_BIN) ./bin

%.o: %.cpp Makefile  
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-mv -f $*.d .dep.$@

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) $(E_BIN) 
	@-rm ./bin/*.so

-include /dev/null $(wildcard .dep.*)
