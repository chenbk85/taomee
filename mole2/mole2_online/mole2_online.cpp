/**
 *============================================================
 *  @file      switch.cpp
 *  @brief
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifdef __cplusplus
extern "C"
{
#endif
#include <async_serv/async_serv.h>
#include <libtaomee/conf_parser/config.h>
#ifdef __cplusplus
}
#endif


#include "util.h"

#include "cli_proto.h"
#include "dbproxy.h"
#include "cli_login.h"
#include "center.h"
#include "code.h"
#include "battle.h"
#include "pet.h"
#include "beast.h"
#include "items.h"
#include "skill.h"
#include "honor.h"
#include "homemap.h"
#include "box.h"
#include "mail.h"
#include "picsrv.h"
#include "mc_proto.h"
#include "task_new.h"

#include "maze.h"
#include "npc.h"
#include "global.h"
#include "activity.h"
#include "viponly.h"
#include "mall.h"

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */

bool g_is_test_env;
#ifdef __cplusplus
extern "C"
#endif
int  init_service(int isparent)
{

	if (!isparent) {

        DEBUG_LOG("INIT_SERVICE");
	    const char *ip= get_ip_ex(0x01);
	    if ( strncmp( ip,"10.",3 )==0 ) {
	        g_is_test_env=true;
			DEBUG_LOG("=============TEST ENV TRUE =============");
	    }else{
	        g_is_test_env=false;
			DEBUG_LOG("=============TEST ENV FALSE =============");
	    }

		g_log_send_buf_hex_flag=g_is_test_env?1:0;
	



		if (sizeof(sprite_t) >= SPRITE_STRUCT_LEN - 800 || sizeof(grp_loop_t) != VIP_BUFF_LEN) {
			ERROR_RETURN(("sprite struct not big enough\t[%lu %u]", sizeof(sprite_t), SPRITE_STRUCT_LEN), -1);
		}
		srand(time(0) * get_server_id());
		setup_timer();
		INIT_LIST_HEAD(&(g_events.timer_list));
		INIT_LIST_HEAD(&active_box_list);
		if ( config_get_strval("_use_lua_config") ==NULL  ){
			config_init("./conf/common.conf");
		}

		statistic_logfile = config_get_strval("statistic_file");
		if (!statistic_logfile)
			return -1;
		if ((init_cli_proto_handles(0) == -1)	|| 
			(init_db_proto_handles(0) == -1)		||
			(init_home_handle_funs() == -1)		||
			(init_switch_handle_funs() == -1)		||
			(init_all_timer_type() == -1)			||
			(init_magic_code_proto_handles(0) == -1) ||
			(init_spacetime_code_proto_handles(0) == -1) ||
			(init_mall_proto_handles(0) == -1)
			) {
			return -1;
		}
		init_sprites();
		init_exp_lv();
		init_home_maps();
		init_all_items();
		init_beast_grp();
		init_rand_infos();
		init_npcs();
		init_all_skills();
		init_all_clothes();
		init_sys_info();
		init_all_tasks();
		init_mail();
		init_shops();
		init_vip_items();
		init_products();

		idc_type = config_get_intval("idc_type" ,1);
		KDEBUG_LOG(0, "ONLINE START\t[%lu %d]", sizeof(sprite_t), idc_type);

		if (
			//11
			(load_xmlconf("./conf/items.xml", load_items) == -1)
			|| (load_xmlconf("./conf/clothes.xml", load_clothes) == -1)
			|| (load_xmlconf("./conf/beasts.xml", load_beasts) == -1)
			//
			|| (load_xmlconf("./conf/pet_exchange.xml", load_pet_exchange) == -1)//3M
			|| (load_xmlconf("./conf/pet_exchange_egg.xml", load_pet_exchange_egg) == -1)
			//62
			|| (load_xmlconf("./conf/gplan.xml", load_rare_beasts) == -1)
			|| (load_xmlconf("./conf/titles.xml", load_honor_titles) == -1)
			|| (load_xmlconf("./conf/picsrv.xml", load_picsrv_config) == -1)
			) {
			return -1;
		}
		if (
				load_xmlconf("./conf/beastgrp.xml", load_beast_grp) == -1 
				|| (load_xmlconf("./conf/handbook.xml", load_handbook) == -1)
				|| (load_xmlconf("./conf/suits.xml", load_suit) == -1)
				|| ( load_xmlconf("./conf/maps.xml", load_maps) == -1)
				|| (load_xmlconf("./conf/rand_item.xml", load_rand_item) == -1)
				|| (load_xmlconf("./conf/vip_item.xml", load_vip_item) == -1)
				|| (load_xmlconf("./conf/commodity.xml", load_products) == -1)
				|| (load_xmlconf("./conf/skills_price.xml", load_all_skills) == -1)
				|| (load_xmlconf("./conf/tasks_new.xml", load_tasks) == -1)//task:12M
				|| (load_xmlconf("./conf/tasks_new.xml", load_task_loops) == -1)
				|| (load_xmlconf("./conf/holiday.xml", load_holiday_factor) == -1)
				|| (load_xmlconf("./conf/box.xml", load_box) == -1)
				|| (load_xmlconf("./conf/exchanges.xml", load_exchange_info) == -1)
				|| (load_xmlconf("./conf/npc.xml", load_npc) == -1)
				|| (load_xmlconf("./conf/npcSkills.xml", load_shop_skill) == -1)
				|| (load_xmlconf("./conf/npcShop.xml", load_shop_item) == -1)
				|| (load_xmlconf("./conf/mail.xml", load_sys_mail) == -1)
				|| (load_xmlconf("./conf/sysinfo.xml", load_sys_info) == -1)
				|| (load_xmlconf("./conf/fishGame.xml", load_fish_info) == -1)
				|| (load_xmlconf("./conf/professtion.xml", load_init_prof_info) == -1)
				|| (load_xmlconf("./conf/maze.xml", load_maze_xml) == -1)
				|| (load_xmlconf("./conf/mapcopy.xml", load_map_copy) == -1)
			)

			return -1;

	//	sleep(1000);
		activate_boxes();
		start_maze_timer();

		/*
		if(tm_load_dirty("./data/tm_dirty.dat") < 0){
			KERROR_LOG(0, "Failed to load drity word file!");
			return -1;
		}
		*/
		
		init_batter_teams();
		init_batter_infos();
		connect_to_switch_timely(0, 0);
		regist_timers();
		udp_report_fd = create_udp_socket(&udp_report_addr, config_get_strval("report_svr_ip"), config_get_intval("report_svr_port", 0));
		udp_post_fd = create_udp_socket(&udp_post_addr, config_get_strval("post_svr_ip"), config_get_intval("post_svr_port", 0));
		switch (idc_type) {
		case idc_type_dx:
			chat_svr_fd = create_udp_socket(&udp_chat_svr_addr, config_get_strval("dx_chat_svr_ip"), config_get_intval("chat_svr_port", 0));
			break;
		case idc_type_wt:
			chat_svr_fd = create_udp_socket(&udp_chat_svr_addr, config_get_strval("wt_chat_svr_ip"), config_get_intval("chat_svr_port", 0));
			break;
		case idc_type_internal:
		case idc_type_internal + 1:
			chat_svr_fd = create_udp_socket(&udp_chat_svr_addr, config_get_strval("in_chat_svr_ip"), config_get_intval("chat_svr_port", 0));
			break;
		default:
			return -1;
		}
	}

	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
#ifdef __cplusplus
extern "C"
#endif
int  fini_service(int isparent)
{
	if (!isparent) {
		KDEBUG_LOG(0, "FINI SERVICE");
		destroy_timer();
		fini_sprites();
		fini_home_maps();
		unload_maps();
		fini_mail();
		fini_all_items();
		fini_beast_grp();
		fini_rand_infos();
		fini_npcs(1);
		fini_all_skills();
		fini_all_clothes();
		fini_sys_info();
		fini_all_tasks();
		fini_shops();
	}
	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
#ifdef __cplusplus
extern "C"
#endif
void proc_events()
{
	handle_cmd_busy_sprite();
	handle_timer();
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
#ifdef __cplusplus
extern "C"
#endif
int  get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
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
	//DEBUG_LOG("avail_data");

	uint32_t len = 0;
	if (isparent) {
		// the client requests for a socket policy file
		if ((avail_len == sizeof(request)) && !memcmp(avail_data, request, sizeof(request))) {
			net_send(fd, response, sizeof(response));

			TRACE_LOG("Policy Req [%s] Received, Rsp [%s] Sent", request, response);
			return 0;
		}

		const protocol_t* pkg = (const protocol_t *)avail_data;

		len = ntohl(pkg->len);
		if ((len > cli_proto_max_len) || (len < sizeof(protocol_t))) {
			KERROR_LOG(0, "[p] invalid len=%d from fd=%d", len, fd);
			return -1;
		}
	} else {
		len = *(uint32_t*)avail_data;
		if ((len > db_proto_max_len) || (len < sizeof(db_proto_t))) {
			KERROR_LOG(0, "[c] invalid len=%d from fd=%d", len, fd);
			return -1;
		}
	}

	return len;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
#ifdef __cplusplus
extern "C"
#endif
int  proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	return dispatch(data, fdsess, 1);
}

#ifdef __cplusplus
extern "C"
#endif
int proc_mcast_pkg(void* data, int len) 
{
	return handle_mc_return((reload_proto_t *)data, len);
}


/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
#ifdef __cplusplus
extern "C"
#endif
void proc_pkg_from_serv(int fd, void* data, int len)
{
	if (fd == proxysvr_fd || fd == vipserv_fd) {		
		handle_db_return((db_proto_t*)data, len);
	} else if (fd == switch_fd) {
		handle_switch_return((switch_proto_t*)data, len);
	} else if (fd == magic_fd) {
		handle_magic_return((code_proto_t*)data, len);
	} else if (fd == mall_fd) {
		handle_mall_return((db_proto_t*)data, len);
	} else if (fd == spacetime_fd ) {
		handle_spacetime_return((code_proto_t*)data, len);
	} else if (batrserv_fd_idx(fd) != -1) {
		handle_batrserv_pkg((batrserv_proto_t*)data, len);
	} else if (homeserv_fd_idx(fd) != -1) {
		handle_homeserv_pkg((homeserv_proto_t*)data, len);
	} 
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
#ifdef __cplusplus
extern "C"
#endif
void on_client_conn_closed(int fd)
{
	sprite_t* p = get_sprite_by_fd(fd);
	if (p) {
		KDEBUG_LOG(p->id, "CLIENT CLOSE");
		del_sprite(p, 1);
	}
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
#ifdef __cplusplus
extern "C"
#endif
void on_fd_closed(int fd)
{
	int idx = 0;
	if (fd == proxysvr_fd) {
		KDEBUG_LOG(0, "DB PROXY CONNECTION CLOSED\t[fd=%d]", fd);
		proxysvr_fd = -1;
	} else if (fd == switch_fd) { 
		KDEBUG_LOG(0, "SWITCH CONNECTION CLOSED\t[fd=%d]", fd);
		switch_fd = -1;
	} else if (fd == magic_fd) {
		KDEBUG_LOG(0, "MAGIC CONNECTION CLOSED\t[fd=%d]", fd);
		magic_fd = -1;
	} else if (fd == spacetime_fd) {
		KDEBUG_LOG(0, "MAGIC CONNECTION CLOSED\t[fd=%d]", fd);
		spacetime_fd = -1;
	} else if (fd == mall_fd) {
		KDEBUG_LOG(0, "MALL CONNECTION CLOSED\t[fd=%d]", fd);
		mall_fd = -1;
	} else if ((idx = batrserv_fd_idx(fd)) != -1) {
		KDEBUG_LOG(0, "BATRSERV CONNECTION CLOSED\t[fd=%d]", fd);
		batrserv_fd[idx] = -1;
		kick_users_out_battle(idx);
	} else if ((idx = homeserv_fd_idx(fd)) != -1) {
		KDEBUG_LOG(0, "HOMESERV CONNECTION CLOSED\t[fd=%d]", fd);
		home_serv_fd[idx] = -1;
		clean_home_by_idx(idx);
	} 
}

#ifdef __cplusplus
extern "C"
#endif
int  reload_global_data()
{
	DEBUG_LOG("XXXXXXXXXXXXXX  reload_global_data");
	/*unregister all timer callbacks*/
	unregister_timers_callback();
	if ((init_cli_proto_handles(1) == -1) 
		|| (init_db_proto_handles(1) == -1)
		|| (init_home_handle_funs() == -1)
		|| (init_switch_handle_funs() == -1)
		|| (init_all_timer_type() == -1)
		|| (init_spacetime_code_proto_handles(1) == -1)
		|| (init_magic_code_proto_handles(1) == -1)
		|| (init_mall_proto_handles(1) == -1)
		) {
		return -1;
	}
	
	refresh_timers_callback();
	return 0;
}

extern "C" int get_channel_id(int fd, const void*pkg, int pkglen, int max_channel_id)
{

	protocol_t* p_pkg = (protocol_t*)pkg;
	int login_pkg_len=sizeof(*p_pkg)+20 ;

	uint32_t cmdid=ntohs(p_pkg->cmd);
	//DEBUG_LOG("xxxx:opt_pkg:%u ,%u,%u,%u",cmdid, proto_cli_login  , pkglen,login_pkg_len );
	int id=-1;

	if (cmdid== proto_cli_login  && pkglen== login_pkg_len){
		id=ntohl( *((int*)(p_pkg +1)));  
	}

	if (cmdid== proto_cli_version && pkglen==sizeof(*p_pkg)+4 ){
		id= *((int*)(p_pkg +1));  
	}

    if (id >= 1 ){
        return get_channel_id_by_svr_id(id);
	} else {
		//DEBUG_LOG("get_channel_id return onlineid:-1");
        return -1;
	}
}

