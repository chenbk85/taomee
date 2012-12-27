DEBUG	= -g -DDEBUG
OPT	= -funroll-loops -D__USE_STRING_INLINES -fhosted -DUSE_CMD_QUEUE 
OPT	+= -DEPOLL_MODE -g #-O3


CC	= gcc
BINS	= OnlineNew
SRCS 	= main.c daemon.c util.c shmq.c net.c tcp.c central_online.c mole_messenger.c \
	 sprite.c proto.c map.c item.c service.c dbproxy.c dll.c npc.c logic.c event.c pet.c bindconf.c exclu_things.c \
	 message.c communicator.c market.c poll.c tasks.c wish_pool.c picsrv.c homeinfo.c login.c mole_group.c birthday.c \
	 mibiserver.c party.c mole_card.c spring.c small_require.c pet_item.c lahm_sport.c mole_bank.c mole_homeland.c \
	 magic_code.c mole_question.c cache_svr.c gift.c mole_class.c mole_pasture.c mole_happy_card.c mole_doctor.c \
	 game_pk.c lahm_diary.c mole_work.c mole_car.c race_car.c mole_candy.c hallowmas.c npc_event.c thanksgiving.c \
	 fashion_data.c rand_itm.c mcast.c lamu_skill.c profession_work.c street.c delicous_food.c mole_dragon.c\
	 add_sth_rand.c fire_cup.c swap.c game_bonus.c friendship_box.c race_bonus.c im_message.c lahm_classroom.c\
	 charity_drive.c mail.c give_item_rank.c snowball_war.c christmas_sock.c chris_cake.c foot_print.c mole_angel.c \
	 mole_dungeon.c gold_bean_serv.c angry_lahm.c hide_and_seek.c tv_question.c pasture.c angel_fight.c fight_prize.c\
	 sand_pool.c play_drum.c final_boss_2011.c moon_cake.c lucky_lottery.c mole_cutepig.c week_activity.c xhx_server.c\
	 mole_avatar.c super_guider.c svc_ac_api.c charitable_party.c winter_vs_spring.c spring_couplets.c year_feast.c\
	 lanterns_day.c flower_party.c advanced_class.c mole_cake.c mole_ocean.c


INCL	+= -I/usr/include/libxml2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ 
#CFLAGS	= -Wall $(INCL) -MMD $(DEBUG) -D_REENTRANT -D_GNU_SOURCE -DENABLE_TRACE_LOG $(OPT) 
CFLAGS	= -Wall $(INCL) -MMD $(DEBUG) -D_REENTRANT -D_GNU_SOURCE $(OPT) 

ifdef LUA_TEST
CFLAGS += -DLUA_TEST
endif

LIBS	+= --warn-common -lxml2 -ldl -rdynamic -lm -lglib-2.0 -lssl -lmsglog -ltaomee -lpthread

all: $(BINS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<
	@-mv -f $*.d .dep.$@ 

$(BINS): $(SRCS:.c=.o)
	$(CC)  $(LDFLAGS) -o $@ $^ $(LINKDIR) $(LIBS)

#	rm ../bin/$(BINS); mv $(BINS) ../bin/
clean:
	@-rm -f *.o *.po *.so *.d .dep.* $(BINS) core

-include /dev/null $(wildcard .dep.*)

