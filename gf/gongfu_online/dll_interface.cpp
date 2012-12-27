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

using namespace taomee;

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}
#include <kf/player_attr.hpp>
#include <kf/home.hpp>
#include <kf/pet_attr.hpp>
#include <kf/title_attr.hpp>

#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "switch.hpp"
#include "battle_switch.hpp"
#include "map.hpp"
#include "player.hpp"
#include "battle.hpp"
#include "item.hpp"
#include "mcast_proto.hpp"
#include "reload_conf.hpp"
#include "home.hpp"
#include "task.hpp"
#include "skill.hpp"
#include "login.hpp"
#include "item.hpp"
#include "restriction.hpp"
#include "ambassador.hpp"
#include "vip_impl.hpp"
#include "summon_monster.hpp"
#include "apothecary.hpp"
#include "secondary_profession.hpp"
#include "magic_number.hpp"
#include "gf_cryptogram.hpp"
#include "store_sales.hpp"
#include "common_op.hpp"
#include "cache_switch.hpp"
#include "trade.hpp"
#include "stage_config.hpp"
#include "buy_item_limit.hpp"
#include "coupons.hpp"
#include "global_data.hpp"
#include "achievement.hpp"
#include "lua_engine.hpp"
#include "app_log.hpp"
#include "check_shop.hpp"
#include "home.hpp"
#include "card.hpp"
#include "active_config.hpp"
#include "random_event.hpp"
#include "rank_top.hpp"
#include "chat.hpp"
#include "numen.hpp"
#include "vip_impl.hpp"
#include "other_active.hpp"
#include "fight_team.hpp"
#include "cachesvr.hpp"

const char *version_str="2010-09-02 22:00";

/**
  * @brief Initialize service
  *
  */
extern "C" int init_service(int isparent)
{
	if (!isparent) {
		srand(time(0));
		setup_timer();

		config_init("./conf/common.conf");

		g_version_number = config_get_intval("version_number", 0);
		init_timer_callback_type();

		if (!init_cli_proto_handles() || !init_db_proto_handles()
				|| !init_btl_proto_handles() || !init_btl_switch_proto_handles() 
				|| !init_chat_proto_handles()){
			return -1;
		}

		INIT_LIST_HEAD(&(g_events.timer_list));

		/* connect to switch server and send an initilization package of online information */
		connect_to_switch();
		if (switch_fd == -1) { //If connectting failed, reconnect every 30 seconds
			connect_to_switch_timely(0, 0);
		}
		init_sw_keepalive_timer();

		//connect to battle switch server and send an init package of online infomation	
		connect_to_battle_switch();
		if(battle_switch_fd == -1) {
			connect_to_battle_switch_timely(0, 0);
		}
		init_bsw_keepalive_timer();
		init_server_config_timer();
		
 		if (init_dbsvr_udp_socket() < 0) {
			ERROR_LOG("Failed to init udp socket to db server");
			return -1;
		}
		if (init_dbsvr_onoff_socket() < 0) {
			ERROR_LOG("Failed to init onoff udp socket to db server");
			return -1;
		}

		char name[ 1024 ] = { 0 };
		sprintf(name, "%s%d_%s", "file://day@gf_log//", get_server_id(), "log");
		GF_LOG_OPEN(name);

		init_players();
		INIT_LIST_HEAD(&(trade_players.player_list));

//		items = new GfItemManager("./conf/items.xml");
//		items = new GfItemManager("./conf/nor_items.xml", "./conf/attire_items.xml", "./conf/shop_items.xml");
		init_home_exp2level_data();

		items->load_items();
		init_trsvr_fds();
        if ( !init_tr_proto_handles()) {
			ERROR_LOG("init trade proto error!");
			return -1;
		}
		if( !init_home_proto_handles( )){
			ERROR_LOG("init home proto error!");
			return -1;
		}
		load_xmlconf("./conf/unique_item.xml", load_unique_items);
		load_xmlconf("./conf/maps.xml", load_maps);
		load_xmlconf("./conf/btlsvr.xml", load_battle_servers);
		load_xmlconf("./conf/trsvr.xml", load_trade_servers);
		load_xmlconf("./conf/homesvr.xml", load_home_servers);
		load_xmlconf("./conf/tasks.xml", load_tasks_config);
		//load_xmlconf("./conf/skills.xml", load_skills_config);
		load_xmlconf("./conf/skills.xml", load_skills);
		//added by cws0530
		load_xmlconf("./conf/qa.xml", load_quesAns_config);
		load_xmlconf("./conf/buff.xml", load_buff_skills);
		load_xmlconf("./conf/specialstages.xml", load_special_stages);
		load_xmlconf("./conf/compose.xml", load_compose);
		load_xmlconf("./conf/dailyActivity.xml", load_restrictions);
		load_xmlconf("./conf/suit.xml", load_suit);
		items->for_each_item(check_item_suit, 0);
		load_xmlconf("./conf/ambassador.xml", load_ambassador);
		load_xmlconf("./conf/summon_monster.xml", load_summons);
		load_xmlconf("./conf/summon_skill_scroll.xml", load_summons_skills_scroll);
		load_xmlconf("./conf/numen.xml", load_numens);
#ifdef VERSION_KAIXIN
		load_xmlconf("./conf/kaixin/strengthenMaterial.xml", load_strengthen_material);
#else
		load_xmlconf("./conf/strengthenMaterial.xml", load_strengthen_material);
#endif
		load_xmlconf("./conf/coupons.xml", load_coupons_exchange);
		load_xmlconf("./conf/storeSales.xml", load_store_sales);
		load_xmlconf("./conf/storeFeedback.xml", load_store_feedback);
		load_xmlconf("./conf/title.xml", load_title_config);

		load_xmlconf("./conf/puzzle.xml", load_all_intelligence_test);
		//init_intelligence_test_room();

        get_title_attr_data_mgr()->init("./conf/glory.xml");

		load_xmlconf("./conf/char_content.xml", load_char_content);
		reload_home_svr_config();	

		g_items_upgrade_mrg.init("./conf/items_upgrade.xml");
		g_swap_action_mrg.init("./conf/swapAction.xml");
		//init() and init(const char* xml)   you can load data from code and xml file 
		//vip_config_data_mgr::getInstance()->init();	
		get_vip_config_data_mgr()->init();
		//init all calculators of professions
		init_all_exp2level_calculator();
        //init all profession formulation datas from xml
		get_formulation_data_mgr()->init("./conf/formulation.xml");
		// statistic
		statistic_logfile = config_get_strval("statistic_log");
		gf_set_static_log_path(statistic_logfile);
		// init stage data xml
		//stage_data_mgr::getInstance()->init("./conf/stage.xml");
		get_stage_data_mgr()->init("./conf/stage.xml");
		get_active_data_mgr()->init("./conf/active_time_limit.xml");
		get_god_guard_mgr()->init("./conf/god_guard.xml");
		// init buy item limit xml
		//buy_item_limit_mgr::getInstance()->init("./conf/buy_item_limit.xml");	
  		get_buy_item_limit_mgr()->init("./conf/buy_item_limit.xml"); 
		// init glory.xml 
		get_achievement_data_mgr()->init_ex("./conf/glory.xml"); 	
		// init ring_task.xml
		get_ring_task_mgr()->init("./conf/ring_task.xml");	
		// init achievement logic functions
//		init_achievement_callback_handles();
		// check npc shop items price
		check_npc_shop("./conf/shop.xml");
		// load npc shop items
		get_shop_mgr()->init("./conf/shop.xml");

		//get_pet_attr_data_mgr()->init("./conf/pet_attribute.xml");
		//load_xmlconf("./conf/pet_attribute", load_pet_attribute);
		load_pet_attribute("./conf/pet_attribute.xml");

		load_xmlconf("./conf/cards.xml", load_cards);

		load_xmlconf("./conf/active_out_date.xml", load_active_out_date);

        get_sum_event_mgr()->init("./conf/random_event.xml");
        get_event_trigger_mgr()->init("./conf/event_trigger.xml");
		//dirtyword load
		if (tm_load_dirty("./data/tm_dirty.dat") < 0) {
			ERROR_RETURN(("Failed to tm_load_dirty!"), -1);
		}	

		// day limit for players' online time of one day for the incoming week
		if (init_time_limit_day(config_get_strval("time_limit"))) {
			ERROR_LOG("init time limit day error");	
			return -1;
		}
        // double experience time coefficient
		if (init_time_limit_day(config_get_strval("time_double_limit"), 1)) {
			ERROR_LOG("init time double day error");	
			return -1;
		}

		init_fight_team_contest_info();

		/*
		lua_engine::getInstance()->regist_to_lua();
		lua_engine::getInstance()->init("./lua/achieve.lua");
		*/

		init_lua();
		regist_to_lua();
		bool ret = load_lua_file("./lua/achieve.lua");
		if(ret == false){
			exit(0);
		}
#ifdef VERSION_KAIXIN
		printf("VERSION_KAIXIN %u %u\n", calc_exp(5), calc_exp(5, 1));
#else
		printf("VERSION_TAOMEE %u %u\n", calc_exp(5), calc_exp(5, 1));
#endif
		// add a timer to kick all users offline at 24 o'clock
		kick_all_users_offline(0, 0);	
		
		// run keep_players_alive every 50 secs and send a keepalive pkg to all the players
		// that didn't receive any pkg from server in the past 30 secs
		//ADD_TIMER_EVENT(&g_events, keep_players_alive, 0, get_now_tv()->tv_sec + 50);
		//
		strncpy(branch_sys_ip, config_get_strval("branch_sys_ip"), sizeof(branch_sys_ip));
		char tmp_str[128] = "";
		strncpy(tmp_str, config_get_strval("branch_sys_port"), sizeof(tmp_str));
		branch_sys_port = atoi(tmp_str);
	} else {
		INFO_LOG("ol cur  VERSION:%s", version_str);
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
		fini_players();
		unload_maps();
		get_vip_config_data_mgr()->final();
		get_stage_data_mgr()->final();
		final_all_exp2level_calculator();
		get_formulation_data_mgr()->final();
		unload_summons();
        unload_summons_skills_scroll();
        unload_numens();
		final_btl_switch_proto_handles();
		get_buy_item_limit_mgr()->final();
		get_achievement_data_mgr()->final();
//		final_achievement_callback_handles();
		//get_pet_attr_data_mgr()->final();
		get_title_attr_data_mgr()->final();
		get_ring_task_mgr()->final();
		get_shop_mgr()->final();
        get_sum_event_mgr()->final();
		get_active_data_mgr()->final();
		get_god_guard_mgr()->final();
        get_event_trigger_mgr()->final();
		final_lua( );
		GF_LOG_CLOSE();
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
	handle_timer();

	proc_auto_ranker_event();
	proc_intelligence_test_event();

 	update_contest_room_event();

    process_active_broadcast();
}

/**
  * @brief Return length of the receiving package
  *
  */
extern "C" int get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
	static char request[]  = "<policy-file-request/>";
	static char response[] = "<?xml version=\"1.0\"?>"
								"<!DOCTYPE cross-domain-policy>"
								"<cross-domain-policy>"
								"<allow-access-from domain=\"*\" to-ports=\"*\" />"
								"</cross-domain-policy>";

	if (avail_len < 4) {
		return 0;
	}

	int len = -1;
	if (isparent) {
		// the client requests for a socket policy file
		if ((avail_len == sizeof(request)) && !memcmp(avail_data, request, sizeof(request))) {
			net_send(fd, response, sizeof(response));

			TRACE_LOG("Policy Req [%s] Received, Rsp [%s] Sent", request, response);
			return 0;
		}

		const cli_proto_t* pkg = reinterpret_cast<const cli_proto_t*>(avail_data);
		

		len = taomee::bswap(pkg->len);
		int pkgcmd = taomee::bswap(pkg->cmd);
		int pkgid = taomee::bswap(pkg->id);
		int pkgno = taomee::bswap(pkg->seqno);
		//KDEBUG_LOG(p->id, "len=[%d],comid=[%d],userid=[%d],seqnum=[%d],ret=[%d]",
		//		len,pkgcmd,pkgid,pkgno,pkg->ret);
		if ((len > cli_proto_max_len) || (len < static_cast<int>(sizeof(cli_proto_t)))) {
			ERROR_LOG("p: invalid len=%d from fd=%d cmd=%d uid=%d pkgno=%d", len, fd, pkgcmd, pkgid, pkgno);
			return -1;
		}
	} else {
		len = *reinterpret_cast<const uint32_t*>(avail_data);
		if (fd == proxysvr_fd) {
			if ((len > dbproto_max_len) || (len < static_cast<int>(sizeof(db_proto_t)))) {
				if (len != 18) {
					ERROR_LOG("c: invalid len=%d from(db) fd=%d", len, fd);
					return -1;
				}
			}
		} else if (fd == switch_fd) {
			if ((len > player_guess_len) || (len < static_cast<int>(sizeof(switch_proto_t)))) {
				ERROR_LOG("c: invalid len=%d from(switch) fd=%d", len, fd);
				return -1;
			}
        }else {
			if ((len > btlproto_max_len) || (len < static_cast<int>(sizeof(btl_proto_t)))) {
				ERROR_LOG("c: invalid len=%d from(btl) fd=%d", len, fd);
				return -1;
			}
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
	} else if (fd == switch_fd) {
		handle_switch_return(reinterpret_cast<switch_proto_t*>(data), len);
	} else if (get_btlsvr_fd_idx(fd) != -1) {
		handle_battle_svr_return(reinterpret_cast<btl_proto_t*>(data), len, fd);
	} else if (fd == magic_number_svr_fd) {
        handle_magic_number_svr_return(reinterpret_cast<magic_number_proto_t*>(data), len);
    } else if (fd == gf_cryptogram_svr_fd) {
        handle_gf_cryptogram_svr_return(reinterpret_cast<gf_cryptogram_proto_t*>(data), len);
    } else if (fd == storesvr_fd) {
        handle_storesvr_return(reinterpret_cast<store_proto_t*>(data), len);
    } else if (fd == battle_switch_fd){
		handle_battle_switch_return( reinterpret_cast<battle_switch_proto_t*>(data), len);
	} else if (fd == cache_switch_fd) {
		handle_cache_switch_return( reinterpret_cast<cache_switch_proto_t*>(data), len);
	} else if (get_trsvr_fd_idx(fd) != -1) {
		handle_trade_svr_return(reinterpret_cast<tr_proto_t *>(data), len, fd);
	} else if (fd == trade_switch_fd) {
		handle_battle_switch_return( reinterpret_cast<battle_switch_proto_t*>(data), len);
	} else if ( get_homesvr_fd_idx(fd) != -1 ){
		handle_home_svr_return(reinterpret_cast<home_proto_t*>(data), len, fd);
	} else if ( fd == chat_svr_fd ){
		handle_chat_svr_return(reinterpret_cast<chat_proto_t*>(data), len, fd);
	} else if ( fd == vipsvr_fd) {
		handle_vipsvr_return(reinterpret_cast<vip_proto_t*>(data), len);
    } else if ( fd == cachesvr_fd) {
		handle_cachesvr_return(reinterpret_cast<cachesvr_proto_t*>(data), len);
	}
}

/**
  * @brief Called each time on client connection closed
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
	player_t* p = get_player_by_fd(fd);
	if (p) {
		del_player(p);
	}
}

/**
  * @brief Called each time on close of the fds created by the child process
  *
  */
extern "C" void on_fd_closed(int fd)
{
	int idx;
	if (fd == proxysvr_fd) {
		KDEBUG_LOG(0, "DB PROXY CONNECTION CLOSED\t[fd=%d]", proxysvr_fd);
		proxysvr_fd = -1;
	} else if (fd == switch_fd) {
		KDEBUG_LOG(0, "SWITCH CONNECTION CLOSED\t[fd=%d]", switch_fd);
		switch_fd = -1;
	
	} else if ( (idx = get_homesvr_fd_idx(fd)) != -1){
		KDEBUG_LOG(0, "HOME SERVER CONNECTION CLOSED\t[fd=%d index=%u]", fd, idx);
		do_while_home_svr_crashed(fd);
		home_fds[idx] = -1;
	} else if ((idx = get_btlsvr_fd_idx(fd)) != -1) {
		KDEBUG_LOG(0, "BATTLE SERVER CONNECTION CLOSED\t[fd=%d index=%u]", fd, idx);
		do_while_battle_svr_crashed(fd);
		battle_fds[idx] = -1;
	} else if (fd == magic_number_svr_fd) {
        KDEBUG_LOG(0, "MAGIC NUMBER SERVER CONNECTION CLOSED\t[fd=%d index=%u]", fd, idx);
        magic_number_svr_fd = -1;
    } else if (fd == gf_cryptogram_svr_fd) {
        KDEBUG_LOG(0, "CRYPTO NUMBER SERVER CONNECTION CLOSED\t[fd=%d index=%u]", fd, idx);
        gf_cryptogram_svr_fd = -1;
    } else if (fd == storesvr_fd) {
        KDEBUG_LOG(0, "STORE SALES SERVER CONNECTION CLOSED\t[fd=%d index=%u]", fd, idx);
        storesvr_fd = -1;
    } else if ( fd == battle_switch_fd) {
		 KDEBUG_LOG(0, "BTLSWITCH CONNECTION CLOSED\t[fd=%d]", battle_switch_fd);
		 battle_switch_fd = -1;
	} else if ( fd == cache_switch_fd) {
		cache_switch_fd = -1;
	} else if ((idx = get_trsvr_fd_idx(fd)) != -1) {
		KDEBUG_LOG(0, "TRADE _SVR CONNECTION CLOSED");
		trade_fds[idx] = -1;
	} else if (fd == trade_switch_fd) {
		KDEBUG_LOG(0, "TRADE SWITCH SERVR CONNECTION CLOSED");
		trade_switch_fd = -1;
	} else if (fd == chat_svr_fd) {
		KDEBUG_LOG(0, "CHAT SERVR CONNECTION CLOSED");
		chat_svr_fd = -1;
	} else if (fd == cachesvr_fd) {
		KDEBUG_LOG(0, "CACHE SERVR CONNECTION CLOSED");
		cachesvr_fd = -1;
	}

}

/**
  * @brief Called to process mcast package from the address and port configured in the config file
  */
extern "C" void proc_mcast_pkg(const void* data, int len)
{
	const mcast_pkg_t* pkg = reinterpret_cast<const mcast_pkg_t*>(data);
	KDEBUG_LOG(0, "proc_mast_pkg cmd=%u,minorcmd=%u,serverid=%u", pkg->main_cmd, pkg->minor_cmd,pkg->server_id);
	if ( pkg->server_id != get_server_id() ) {
		switch (pkg->main_cmd) {
		case mcast_reload_conf:
			reload_conf_op(pkg->minor_cmd, pkg->body, len - sizeof(mcast_pkg_t));
			break;
		case mcast_trade_message:
			mcast_trade(pkg->body, len - sizeof(mcast_pkg_t));
			break;
        case mcast_world_notice:
			world_public_notice(pkg->body, len - sizeof(mcast_pkg_t));
			break;
		case mcast_friend_login:
			//friend_login_op(pkg->body, len - sizeof(mcast_pkg_t));
			break;
		case mcast_limit_data:
			update_limit_data(pkg->body, len - sizeof(mcast_pkg_t));
			break;

		case mcast_achieve_mcast:
			proc_achieve_mcast(pkg->body, len - sizeof(mcast_pkg_t));
			break;
		default:
			ERROR_LOG("proc_mast_pkg unsurported cmd %u", pkg->main_cmd);
			break;
		}
	}
}


extern "C" int reload_global_data()
{
	KDEBUG_LOG(0, "RELOAD GLOBAL DATA!");
	unregister_timers_callback();

	if (!init_cli_proto_handles() || !init_db_proto_handles() || !init_tr_proto_handles()
		|| !init_btl_proto_handles() || !init_btl_switch_proto_handles() || !init_timer_callback_type()){
		ERROR_LOG("reload global data error!");
		return -1;
	}
	load_lua_file("./lua/achieve.lua");
	refresh_timers_callback();
	KDEBUG_LOG(0, "LOAD GLOBAL DATA OWNER!");

	

	return 0;
}



extern "C" int before_reload(int is_parent)
{
	if(!is_parent)
	{
		KDEBUG_LOG(0, "before_read");
		final_lua();
		init_lua();
		regist_to_lua();
		load_lua_file("./lua/achieve.lua");	
	}
	return 0;
}

