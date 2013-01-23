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
#include <libtaomee/project/stat_agent/msglog.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}

#include "cli_proto.hpp"
#include "map_data.hpp"
#include "bullet.hpp"
#include "skill.hpp"
#include "stage.hpp"
#include "battle_manager.hpp"
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
		TRACE_TLOG("start init servive");
		DEBUG_TLOG("start init servive");
		srand(time(0));
		setup_timer();
		init_cli_proto_handles();
		map_data_mgr::getInstance()->init_all("./maps/");
		map_data_mgr::getInstance()->init_all("./bullet_craters/");
		bullet_mgr::get_instance()->init_xml("./conf/bullet.xml");	
		stage_data_mgr::get_instance()->init_all_xmls("./stages/");
		load_xmlconf("./conf/player_attribute.xml", init_player_attr_config_data);
        load_xmlconf("./conf/pet.xml", init_pet_config_data);	
		skill_data_mgr::get_instance()->init_xml("./conf/skill.xml"); 
		load_xmlconf("./conf/buff.xml", init_buff_data);
		load_xmlconf("./conf/effect.xml", init_effect_data);

        init_pet_data();
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
		final_cli_proto_handles();
		map_data_mgr::getInstance()->final();
		bullet_mgr::get_instance()->final();
		stage_data_mgr::get_instance()->final();
		skill_data_mgr::get_instance()->final();
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
	update_all_objects();
}

/**
  * @brief Return length of the receiving package
  *
  */
extern "C" int get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
	if (avail_len < 4) 
	{
		return 0;
	}

	int len = *reinterpret_cast<const uint32_t*>(avail_data);
	/*
	if (fd == proxysvr_fd) 
	{
		if ((len > dbproto_max_len) || (len < static_cast<int>(sizeof(db_proto_t)))) {
			if (len != 18) {
				ERROR_TLOG("c: invalid len=%d from fd=%d", len, fd);
				return -1;
			}
		}
	} 
	*/
	//else 
	{
		if ((len > btl_proto_max_len) || (len < static_cast<int>(sizeof(btl_proto_t)))) 
		{
			ERROR_TLOG("c: invalid len=%d from fd=%d", len, fd);
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

#ifdef USE_HEX_LOG_PROTO
    if (g_log_send_buf_hex_flag)
    {
        char outbuf[13000];
        bin2hex(outbuf, (char *)data, len, 2000);
        TRACE_TLOG("[IN] %s", outbuf);
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
	/*
	if (fd == proxysvr_fd) {
		handle_db_return(reinterpret_cast<db_proto_t*>(data), len);
	}
	*/
}

/**
  * @brief Called each time on client connection closed
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
	clear_players(fd);
}

/**
  * @brief Called each time on close of the fds created by the child process
  *
  */
extern "C" void on_fd_closed(int fd)
{
	/*
	if (fd == proxysvr_fd) {
		DEBUG_TLOG("DB PROXY CONNECTION CLOSED\t[fd=%d]", proxysvr_fd);
		proxysvr_fd = -1;
	}
	if (fd == battle_switch_fd){
		DEBUG_TLOG("BATTLE SW CONNECTION CLOSED\t[fd=%d]", battle_switch_fd);
		battle_switch_fd = -1;
	}
	if ( fd == cache_switch_fd) {
		cache_switch_fd = -1;
	} 
	*/
}

/**
  * @brief Called to process mcast package from the address and port configured in the config file
  */
extern "C" void proc_mcast_pkg(const void* data, int len)
{
	/*
	// TODO: reload configs
	const mcast_pkg_t* pkg = reinterpret_cast<const mcast_pkg_t*>(data);
	if ( pkg->server_id != get_server_id() ) {
		switch (pkg->main_cmd) {
		case mcast_reload_conf:
			reload_conf_op(pkg->minor_cmd, pkg->body, len - sizeof(mcast_pkg_t));
			break;
		default:
		//	ERROR_TLOG("proc_mast_pkg unsurported cmd %u", pkg->main_cmd);
			break;
		}
	}
	*/
}

