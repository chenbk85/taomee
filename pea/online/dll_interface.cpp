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
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}

#include "pea_common.hpp"
#include "cli_proto.hpp"
#include "battle.hpp"
#include "player.hpp"
#include "map.hpp"
#include "map_data.hpp"
#include "room.hpp"
#include "battle_switch.hpp"
#include "dbproxy.hpp"
#include "proto.hpp"
#include "db_player.hpp"
#include "npc_shop.hpp"
#include "task.hpp"
#include "task_funcs.hpp"
#include "prize.hpp"

#ifdef USE_HEX_LOG_PROTO

int g_log_send_buf_hex_flag = 1;

#else

int g_log_send_buf_hex_flag = 0;

#endif




/**
  * @brief Initialize service
  *
  */
extern "C" int init_service(int isparent)
{
	if (isparent) 
    {
    }
    else
	{
/* MYTEST(zog): 测试 不分时log 和 TRACE_TLOG */
#if 1
		SET_TIME_SLICE_SECS(0);
		SET_LOG_LEVEL(tlog_lvl_trace);
#endif

		TRACE_TLOG("start init servive");

		srand(time(0));
		setup_timer();
		
		INIT_LIST_HEAD(&(g_events.timer_list));
		INIT_LIST_HEAD(&awaiting_playerlist);
		
		init_btl_proto_handles();
		init_players();
		init_battle_server_config("./conf/btlsvr.xml");
		map_data_mgr::get_instance()->init_xml("./conf/maps.xml");
		init_maps();
		item_data_mgr::get_instance()->init_xml("./conf/item.xml");
        item_data_mgr::get_instance()->init_xml("./conf/equip.xml");
        load_xmlconf("./conf/npc_shop.xml", init_npc_shop_config_data);
		load_xmlconf("./conf/player_attribute.xml", init_player_attr_config_data);

        load_xmlconf("./conf/pet_iq.xml", init_pet_iq_config_data);
        load_xmlconf("./conf/pet_culture.xml", init_pet_train_config_data);
        load_xmlconf("./conf/pet_innate.xml", init_pet_gift_config_data);
        load_xmlconf("./conf/pet_innate_consumption.xml", init_pet_gift_consume_config_data);
        load_xmlconf("./conf/pet_unreal.xml", init_pet_merge_config_data);
        load_xmlconf("./conf/pet.xml", init_pet_config_data);
        load_xmlconf("./conf/prize.xml", init_prize_data);
        load_xmlconf("./conf/task.xml", init_task_config_data);
        init_pet_data();

		// task-fn-system
		init_task_fn_map();

		// connect to battle switch server and send an init package of online infomation 
		init_connect_to_battle_switch();
        // connect to db
        init_connect_to_dbproxy();

	}
	return 0;
}

/**
  * @brief Finalize service
  *
  */
extern "C" int fini_service(int isparent)
{
	if (isparent) 
    {

    }
    else
    {
		list_del_init(&awaiting_playerlist);
		list_del_init(&(g_events.timer_list));

        destroy_timer();

		final_btl_proto_handles();
		final_battle_server_config();
		final_players();
		map_data_mgr::get_instance()->final();
		final_maps();
		item_data_mgr::get_instance()->final();
        final_prize_data();
        final_task_config_data();
		final_task_fn_map();
        
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
		if ((avail_len == sizeof(request))
			&& !memcmp(avail_data, request, sizeof(request))) {
			net_send(fd, response, sizeof(response));
			TRACE_TLOG("Policy Req [%s] Received, Rsp [%s] Sent", request, response);
			return 0;
		}
		
		const cli_proto_t* pkg = reinterpret_cast<const cli_proto_t*>(avail_data);
		len = taomee::bswap(pkg->len);
		if ((len > cli_proto_max_len)
			|| (len < static_cast<int>(sizeof(cli_proto_t)))) {
			ERROR_TLOG("p: invalid len=%d from fd=%d", len, fd);
			return -1;
		}
	} else {
		len = *reinterpret_cast<const uint32_t*>(avail_data);
		
		if (is_dbproxy_fd(fd)) {
			if ((len > dbproto_max_len)
				|| (len < static_cast<int>(sizeof(db_proto_t)))) {
				if (len != 18) {
					ERROR_TLOG("c: invalid len=%d from(db) fd=%d", len, fd);
					return -1;
				}
			}
		} else {
			if ((len > btl_proto_max_len)
				|| (len < static_cast<int>(sizeof(btl_proto_t)))) {
				ERROR_TLOG("c: invalid len=%d from(btl) fd=%d", len, fd);
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

#ifdef USE_HEX_LOG_PROTO
    if (g_log_send_buf_hex_flag)
    {
        char outbuf[13000];
        bin2hex(outbuf, (char *)data, len, 2000);
        DEBUG_TLOG("[CI] %s", outbuf);
    }
#endif

	return dispatch(data, fdsess);
}

/**
  * @brief Process packages from servers
  *
  */
extern "C" void proc_pkg_from_serv(int fd, void* data, int len)
{
#ifdef USE_HEX_LOG_PROTO
    if (g_log_send_buf_hex_flag) {
        char outbuf[13000];
        bin2hex(outbuf, (char *)data, len, 2000);
        DEBUG_TLOG("[SI] %s", outbuf);
    }
#endif

	if(get_btlsvr_fd_idx(fd) != -1) {
		handle_battle_svr_return(reinterpret_cast<btl_proto_t*>(data), len, fd);
	} else if(is_battle_switch_fd(fd)) {
		handle_battle_switch_return(reinterpret_cast<btlsw_proto_t*>(data), len);
	} else if (is_dbproxy_fd(fd)) {
		handle_db_return(reinterpret_cast<db_proto_t*>(data), len);
	}
}

/**
  * @brief Called each time on client connection closed
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
	player_t* p = get_player_by_fd(fd);
	if (p)
    {
        p->last_off_line_tm = get_now_tv()->tv_sec;
        //db_save_player(p);
		del_player(p);
	}
}

/**
  * @brief Called each time on close of the fds created by the child process
  *
  */
extern "C" void on_fd_closed(int fd)
{
	int idx = 0;
	if( (idx = get_btlsvr_fd_idx(fd)) != -1)
	{
		do_while_battle_svr_crashed(fd);
		battle_fds[idx] = -1;
		DEBUG_TLOG("BATTLE SERVER CONNECTION CLOSED\t [fd=%d index=%u]", fd, idx);	
	}
	else if(is_battle_switch_fd(fd))
	{
        close_battle_switch_fd();
		DEBUG_TLOG("BTLSWITCH CONNECTION CLOSED\t[fd=%d]", fd);
	}
	else if(is_dbproxy_fd(fd))
	{
        close_dbproxy_fd();
		DEBUG_TLOG("DB PROXY CONNECTION CLOSED\t[fd=%d]", fd);
	}
	
}

/**
  * @brief Called to process mcast package from the address and port configured in the config file
  */
extern "C" void proc_mcast_pkg(const void* data, int len)
{
	/*
	const mcast_pkg_t* pkg = reinterpret_cast<const mcast_pkg_t*>(data);
	DEBUG_TLOG("user: %u, proc_mast_pkg cmd=%u,minorcmd=%u,serverid=%u", 0, pkg->main_cmd, pkg->minor_cmd,pkg->server_id);
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
		default:
			ERROR_TLOG("proc_mast_pkg unsurported cmd %u", pkg->main_cmd);
			break;
		}
	}
	*/
}


extern "C" int reload_global_data()
{
	return 0;
}



extern "C" int before_reload(int is_parent)
{
	return 0;
}

