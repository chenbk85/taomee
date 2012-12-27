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

#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "mcast_proto.hpp"
#include "reload_conf.hpp"
#include "item.hpp"
#include "trade.hpp"
#include "safe_trade.hpp"

/**
  * @brief Initialize service
  *
  */
extern "C" int init_service(int isparent)
{
	if (!isparent) {
		srand(time(0));
		setup_timer();
		INIT_LIST_HEAD(&(g_events.timer_list));


		if (!init_cli_proto_handles() || !init_db_proto_handles()) {
			return -1;
		}

        //if (init_dbsvr_udp_socket() < 0) {
        //    ERROR_LOG("Failed to init udp socket to db server");
        //    return -1;
        //}

		//items = new GfItemManager("./conf/items.xml");
		//items = new GfItemManager("./conf/items_nor.xml", "./conf/items_attire.xml", "./conf/items_shop.xml");
		//items->load_items();
		init_trade_svr();
		//init_market_num();	
		// statistic
		statistic_logfile = config_get_strval("statistic_log");
		gf_set_static_log_path(statistic_logfile);

        get_safe_trade_mgr()->init();

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
		clear_players();
        //clear_safe_trade_room();
        get_safe_trade_mgr()->final();
		fini_trade_svr();
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
	sSvr.proc_events();
	handle_timer();
	update_all_objects();
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
		if ((len > max_pkg_size) || (len < static_cast<int>(sizeof(tr_proto_t)))) {
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
    clear_safe_trade_room(fd);
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
	} else if  (fd == trade_switch_fd) {
		DEBUG_LOG("TRADE SWITCH CONNECTION CLOSED\t[fd=%d]", trade_switch_fd);
		trade_switch_fd = - 1;
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
			ERROR_LOG("proc_mast_pkg unsurported cmd %u", pkg->main_cmd);
			break;
		}
	}

}

