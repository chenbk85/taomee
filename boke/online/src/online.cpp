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

#include "load_data.h"
#include "global.h"
#include "timer_func.h"
#include "cli_proto.h"
#include "switch.h"
#include "Cgame.h"

void init_user_reg_config_map();
bool init_data()
{
	const char *ip= get_ip_ex(0x01);
	if ( strncmp( ip,"10.",3 )==0 ) {
		g_is_test_env=true;
		g_log_send_buf_hex_flag=1;
		DEBUG_LOG("EVN: TEST ");
	}else{
		g_is_test_env=false;
		DEBUG_LOG("EVN: PRODUCTION ");
	}

 	init_user_reg_config_map();

	init_switch_handle_funs();
	init_cli_handle_funs();
	init_db_handle_funs();
	init_timer_funcs();
	load_xmlconf("../conf/TaskData.xml", load_task_map );
	load_xmlconf("../conf/ItemXmlData.xml", load_item_conf_map);
	load_xmlconf("../conf/jf_shop.xml", load_jf_shop);
	load_xmlconf("../conf/Map_name.xml", load_island_map);
	load_xmlconf("../conf/npcxp_new.xml", load_npc_exp);
	g_game_conf_map.init();
	load_xmlconf("../conf/gamesid.xml", load_game_map);
	load_xmlconf("../conf/movement_new.xml", load_movement);
	load_xmlconf("../conf/RandGroup.xml", load_rand_group);
	//g_task_map.show();

	snprintf(statistic_file,sizeof(statistic_file ), config_get_strval("statistic_file"));
	DEBUG_LOG("ONLINE START SUCC[stat=%s]", statistic_file);
	return true;
}

void init_user_reg_config_map()
{
	//头发
	uint32_t v_list_0[]	={
		100404,100405,100406,100407,100408,100409,100410,
		100411,100412,100413,100414,100415,100416,100417,
		100418,100419,100420,100421,100422,100423 
	};
	user_reg_config_map[0].insert(v_list_0, v_list_0 + sizeof(v_list_0)/sizeof(v_list_0[0]));

	//嘴巴：
	uint32_t v_list_1[]	={
		100117,100147,100153,100216,100363,100117,100147,
		100153,100216,100363,100025,100185,100238,100291,
		100396,100025,100185,100238,100291,100396
	};
	user_reg_config_map[1].insert(v_list_1, v_list_1 + sizeof(v_list_1)/sizeof(v_list_1[0]));
	//上衣：
	uint32_t v_list_2[]	={
		100424,100425,100426,100427,100428,100429,100430,
		100431,100432,100433,100434,100435,100436,100437,
		100438,100439,100440,100441,100442,100443
	};
	user_reg_config_map[2].insert(v_list_2, v_list_2 + sizeof(v_list_2)/sizeof(v_list_2[0]));
	//裤子：
	uint32_t v_list_3[]	={
		100444,100445,100446,100447,100448,100449,100450,
		100451,100452,100453,100454,100455,100456,100457,
		100458,100459,100460,100461,100462,100463
	};
	user_reg_config_map[3].insert(v_list_3, v_list_3 + sizeof(v_list_3)/sizeof(v_list_3[0]));

	//眼睛
	uint32_t v_list_4[]	={ 101330,101331 };
	user_reg_config_map[4].insert(v_list_4, v_list_4 + sizeof(v_list_4)/sizeof(v_list_4[0]));
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
#ifdef __cplusplus
extern "C"
#endif
int  init_service(int isparent)
{
	if (!isparent) {
		srand(time(NULL) * get_server_id());
		config_init("../conf/common.conf");
	 	g_sprite_map=new Csprite_map();
	 	g_timer_map=new Ctimer();
		init_data();
		
		//处理定时器
		deal_all_off_line(0,NULL,0);
		//上报switch
		connect_to_switch();	
		udp_post_fd = create_udp_socket(&udp_post_addr, config_get_strval("post_svr_ip"), config_get_intval("post_svr_port", 0));
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
		g_sprite_map=new Csprite_map() ;
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
	g_timer_map->deal_timer(TimeVal(now));
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

	uint32_t len = 0;
	if (isparent) {
		// the client requests for a socket policy file
		if ((avail_len == sizeof(request)) && 
				!memcmp(avail_data, request, sizeof(request))) {
			net_send(fd, response, sizeof(response));
			return 0;
		}

		const protocol_t* pkg = (const protocol_t *)avail_data;

		len = pkg->len;
		if ( (len > db_proto_max_len) || (len < sizeof(protocol_t))) {
			KDEBUG_LOG(0, "[p] invalid len=%d from fd=%d", len, fd);
			return -1;
		}
	
	} else {
		len = *(uint32_t*)avail_data;
		if ((len > db_proto_max_len) || (len < sizeof(db_proto_t))) {
			KDEBUG_LOG(0, "[c] invalid len=%d from fd=%d", len, fd);
			return -1;
		}
	}

	return len;
}

#ifdef __cplusplus
extern "C"
#endif
int get_channel_id(int fd, const void*pkg, int pkglen, int max_channel_id)
{
	if (pkglen < 22)
		return -1;

	int svrid = *(uint32_t *)((char*)pkg + 18);
    int id = get_channel_id_by_svr_id(svrid);
    if (id >= 0 && id <= max_channel_id)
        return id;
    else 
        return -1;
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
	return dispatch(data, fdsess, true);
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

	if (fd == proxysvr_fd) {		
		handle_db_return((db_proto_t*)data, len);
	} else if (fd == switch_fd) {
		handle_switch_return((db_proto_t*)data, len);
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
	DEBUG_LOG("ON_CLIENT_CONN_CLOSED: fd=%d",fd);
	g_sprite_map->del_sprite_by_fd(fd);
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
	if (fd == proxysvr_fd) {
		KDEBUG_LOG(0, "DB PROXY CONNECTION CLOSED\t[fd=%d]", fd);
		proxysvr_fd = -1;
	} else if (fd == switch_fd) { 
		KDEBUG_LOG(0, "SWITCH CONNECTION CLOSED\t[fd=%d]", fd);
		switch_fd = -1;
		connect_to_switch();
	}else{
		KDEBUG_LOG(0, "NODEFINE SERVER CONNECTION CLOSED\t[fd=%d]", fd);
	}	
}

#ifdef __cplusplus
extern "C"
#endif
int  reload_global_data()
{
	DEBUG_LOG("===RELOAD SO===");
	init_data();
	return 0;
}

