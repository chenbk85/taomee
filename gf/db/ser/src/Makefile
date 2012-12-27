CC = g++ 

MYPATH=`pwd`
#WORKDIR=$(HOME)/DB
WORKDIR=$(MYPATH)/../../
ifdef  NEED_MEMCACHE 
MC_FLAG= -DNEED_MEMCACHE
else
MC_FLAG=
endif

ifdef  MAKE_SEND_EMAIL 
SEND_EMAIL_FLAG= -DMAKE_SEND_EMAIL
else
SEND_EMAIL_FLAG=
endif

ifdef VERSION_TAIWAN
VERSION_FLAG= -DVERSION_TAIWAN
endif

ifdef VERSION_KAIXIN
kfcommonlib=-lkfkxcommon
else
kfcommonlib=-lkfcommon
endif

CFLAGS  = -DGF_PROTO_HEADER  -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared \
		  -D_REENTRANT -D__STDC_FORMAT_MACROS -fPIC $(MC_FLAG) $(SEND_EMAIL_FLAG) $(VERSION_FLAG)

#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT -D__STDC_FORMAT_MACROS -fPIC    
#make for SEND_EMAIL
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT -DMAKE_SEND_EMAIL
LIBDIR =  -L/usr/lib  -L/usr/lib/mysql  -L$(WORKDIR)/lib
LIB 	=  -ldbser -ldl -lmysqlclient  $(kfcommonlib) -ltaomee++ -ltaomee -lxml2 -lrt -lglib-2.0
INCLUDE =  -I/usr/include/dbser/ -I/usr/include/libxml2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/   -I/usr/include/mysql 
#	Cmc.o
  
OO	= Cgongfu.o service.o Croute_func.o Cgf_attire.o Cgf_item.o Cgf_friend.o Cgf_role.o Cgf_task.o Cgf_skill.o Cgf_invite_code.o Cgf_killboss.o Cgf_summon.o Cgf_daily_action.o Cgf_weekly_action.o Cgf_monthly_action.o Cgf_warehouse.o Cgf_clothes_warehouse.o Csend_email.o Cclientproto.o Cgf_amb.o Cgf_secondary_pro.o Cgf_mail.o Cgf_market_log.o Cgf_item_log.o Cgf_buff.o Cgf_strengthen_material.o Cgf_hero_top.o Cgf_get_shopitem_log.o Cgf_shop_log.o Cgf_hunter_top.o Cgf_donate.o Cgf_buy_item_limit.o  Cgf_kill_boss.o Cgf_achievement.o Cgf_title.o Cgf_ap_toplist.o Cgf_ring_task.o Cgf_contest.o Cgf_pvp_game.o Cgf_swap_action.o Cgf_plant.o gf_db.o gf.o Cgf_home.o Cgf_home_log.o Cgf_summon_dragon_list.o Cgf_card.o Cgf_summon_skill.o Cgf_team.o Cgf_team_detail.o Cgf_wuseng_time.o Cgf_other_info.o Cgf_ranker.o Cgf_master.o Cgf_reward.o Cgf_decorate.o Cgf_tmp_info.o Cgf_numen.o Cgf_numen_skill.o Cgf_other_active.o Cgf_stat_info.o

TARGETS = libgfser.so

cur: start_make  $(OO) $(TARGETS) end_make
all: $(OO) $(TARGETS) end_make
start_make:
	mv ./.obj/*.o . || echo "--" 
end_make:
	mv  *.o ./.obj/  

$(TARGETS):
	$(CC) $(CFLAGS) $(INCLUDE) $(OO) -o $@ $(LIBDIR) $(LIB)
	rm ../bin/$(TARGETS); mv $(TARGETS) ../bin/
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

%:%.cpp
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
	echo $@

clean:
	rm -f $(TARGETS)
