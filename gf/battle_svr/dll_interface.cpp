/**
 *============================================================
 *  @file      dll_interface.cpp
 *  @brief    Implement interfaces required by 'AsynServer'
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#include <libtaomee++/inet/byteswap.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <kf/pet_attr.hpp>

using namespace taomee;

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/stat_agent/msglog.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}

#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "player.hpp"
#include "battle.hpp"
#include "item.hpp"
#include "monster.hpp"
#include "stage.hpp"
#include "skill.hpp"
#include "mcast_proto.hpp"
#include "reload_conf.hpp"
#include "base_effect.hpp"
#include "buff.hpp"
#include "aura.hpp"
#include "skill_imme_effect.hpp"
#include "battle_switch.hpp"
#include "roll.hpp"
#include "map_summon_object.hpp"
//#include "ai/lua_engine.hpp"
#include "cache_switch.hpp"
#include "skill_additional_damage.hpp"
//#include "app_log.hpp"
#include "battle_logic.hpp"
#include "mechanism_drop.hpp"
#include "title_attribute.hpp"
/**
  * @brief Initialize service
  *
  */
extern "C" int init_service(int isparent)
{
	if (!isparent) {
		TRACE_LOG("start init servive");
		ERROR_LOG("start init servive");
		DEBUG_LOG("start init servive");
		srand(time(0));
		setup_timer();
		INIT_LIST_HEAD(&(g_events.timer_list));
		init_watch_list();
		connect_to_battle_switch();
		if(battle_switch_fd == -1)
		{
			connect_to_battle_switch_timely(0, 0);				        
		}


		if (!init_cli_proto_handles() || !init_db_proto_handles()) {
			return -1;
		}

        if (init_dbsvr_udp_socket() < 0) {
            ERROR_LOG("Failed to init udp socket to db server");
            return -1;
        }

//		char name[ 1024 ] = {0};
//		sprintf(name, "%s%d_%s", "file://day@gf_log//", get_server_id(), "log");
//		GF_LOG_OPEN(name);


		//init skill add damage xml data
		//this function should be called before skills load
		skill_add_damage_mgr::getInstance()->init();

		//skills
		g_skills = new GobalSkill("./conf/skills.xml");
		g_skills->load_skills();

		//items = new GfItemManager("./conf/items.xml");
		items = new GfItemManager("./conf/items_nor.xml", "./conf/items_attire.xml", "./conf/items_shop.xml");
		items->load_items();


		load_xmlconf("./conf/unique_item.xml", load_unique_items);
	
		mechanism_drop_mgr::getInstance()->init("./conf/mechanism_drop.xml");

		load_xmlconf("./conf/stage_easy/monsters.xml", load_easy_monsters);
		load_xmlconf("./conf/stage_normal/monsters.xml", load_nor_monsters);
		load_xmlconf("./conf/stage_hard/monsters.xml", load_hard_monsters);
		load_xmlconf("./conf/stage_hell/monsters.xml", load_hell_monsters);
		load_xmlconf("./conf/stage_tower/monsters.xml", load_tower_monsters);
		load_xmlconf("./conf/stage_epic/monsters.xml", load_epic_monsters);
		load_xmlconf("./conf/stage_legend/monsters.xml", load_legend_monsters);

		load_all_stages();

		load_xmlconf("./conf/stage_easy/quality.xml", load_easy_quality);
		load_xmlconf("./conf/stage_normal/quality.xml", load_nor_quality);
		load_xmlconf("./conf/stage_hard/quality.xml", load_hard_quality);
		load_xmlconf("./conf/stage_hell/quality.xml", load_hell_quality);
		load_xmlconf("./conf/stage_tower/quality.xml", load_tower_quality);
		load_xmlconf("./conf/stage_epic/quality.xml", load_epic_quality);
		load_xmlconf("./conf/stage_legend/quality.xml", load_legend_quality);

		load_xmlconf("./conf/suit.xml", load_suit);
		load_xmlconf("./conf/treasure_box.xml", load_treasure_box);
		load_xmlconf("./conf/treasure_box.xml", load_stage_boxs);
		load_xmlconf("./conf/treasure_box.xml", load_vip_boxs);
		load_xmlconf("./conf/holidaydrops.xml", load_holiday_drop);

#ifdef  VERSION_KAIXIN
		load_xmlconf("./conf/kaixin/dead_punish.xml", load_punish_stage);
#else
#endif

		load_xmlconf("./conf/challenge.xml", load_challenges);

		add_holiday_timer();
		items->for_each_item(check_item_suit, 0);
		special_drop_mrg.init();

		//load all base effect xml data
		effect_data_mgr::getInstance()->init("./conf/effect.xml");	
		//load all buff xml data
		buff_data_mgr::getInstance()->init("./conf/buff.xml");
		//load all aura xml data
		aura_data_mgr::getInstance()->init("./conf/aura.xml");
		//load map summon object xml
		map_summon_data_mgr::getInstance()->init("./conf/skill_summon.xml");
		//load pet attribute data xml
		//get_pet_attr_data_mgr()->init("./conf/pet_attribute.xml");
		load_pet_attribute("./conf/pet_attribute.xml");
		get_title_attr_data_mgr()->init("./conf/glory.xml");
		//init skill effect  type func
		init_effect_func_table();
		// statistic
		statistic_logfile = config_get_strval("statistic_log");
		gf_set_static_log_path(statistic_logfile);


		init_call_monsters_order_data();
		//load lua
//		g_lua_engine.export_core_interfaces_to_lua();
//		g_lua_engine.load_scripts();
		g_special_double_time_mrg.init();

	}

	return 0;
}

/**
  * @brief Finalize service
  *
  */
extern "C" int fini_service(int isparent)
{
	if (!isparent) {
		delete items;
		delete g_skills;

		unload_stages();
		clear_players();
		effect_data_mgr::getInstance()->final();
		buff_data_mgr::getInstance()->final();
		aura_data_mgr::getInstance()->final();
		map_summon_data_mgr::getInstance()->final();
		skill_add_damage_mgr::getInstance()->final();
		mechanism_drop_mgr::getInstance()->final();
		//get_pet_attr_data_mgr()->final();
		get_title_attr_data_mgr()->final();
		final_effect_func_table();
//		GF_LOG_CLOSE();
	}
	return 0;
}

/**
  * @brief Process events such as timers and signals
  *
  */
extern "C" void proc_events()
{
	proc_cached_pkgs();
	ev_mgr.process_events();
	handle_timer();
	update_all_objects();

	battle_statistics();
}

/**
  * @brief Return length of the receiving package
  *
  */
extern "C" int get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
	if (avail_len < 4) {
		return 0;
	}

	int len = *reinterpret_cast<const uint32_t*>(avail_data);
	if (fd == proxysvr_fd) {
		if ((len > dbproto_max_len) || (len < static_cast<int>(sizeof(db_proto_t)))) {
			if (len != 18) {
				ERROR_LOG("c: invalid len=%d from fd=%d", len, fd);
				return -1;
			}
		}
	} else {
		if ((len > btlproto_max_len) || (len < static_cast<int>(sizeof(btl_proto_t)))) {
			ERROR_LOG("c: invalid len=%d from fd=%d", len, fd);
			return -1;
		}
	}

	return len;
}

/**
  * @brief Process packages from clients
  *
  */
extern "C" int proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	return dispatch(data, fdsess);
}

/**
  * @brief Process packages from servers
  *
  */
extern "C" void proc_pkg_from_serv(int fd, void* data, int len)
{
	if (fd == proxysvr_fd) {
		handle_db_return(reinterpret_cast<db_proto_t*>(data), len);
	}
}

/**
  * @brief Called each time on client connection closed
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
	DEBUG_LOG("ONLINE CRASHED\t[fd=%u]", fd);
	clear_players(fd);
}

/**
  * @brief Called each time on close of the fds created by the child process
  *
  */
extern "C" void on_fd_closed(int fd)
{
	if (fd == proxysvr_fd) {
		DEBUG_LOG("DB PROXY CONNECTION CLOSED\t[fd=%d]", proxysvr_fd);
		proxysvr_fd = -1;
	}
	if (fd == battle_switch_fd){
		DEBUG_LOG("BATTLE SW CONNECTION CLOSED\t[fd=%d]", battle_switch_fd);
		battle_switch_fd = -1;
	}
	if ( fd == cache_switch_fd) {
		cache_switch_fd = -1;
	} 
}

/**
  * @brief Called to process mcast package from the address and port configured in the config file
  */
extern "C" void proc_mcast_pkg(const void* data, int len)
{
	// TODO: reload configs
	const mcast_pkg_t* pkg = reinterpret_cast<const mcast_pkg_t*>(data);
	if ( pkg->server_id != get_server_id() ) {
		switch (pkg->main_cmd) {
		case mcast_reload_conf:
			reload_conf_op(pkg->minor_cmd, pkg->body, len - sizeof(mcast_pkg_t));
			break;
		default:
		//	ERROR_LOG("proc_mast_pkg unsurported cmd %u", pkg->main_cmd);
			break;
		}
	}

}

