extern "C" 
{
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
}

#include "online.hpp"
#include "switch.hpp"
#include "dispatcher.hpp"
//include "team_room.hpp"
#include "battle_room.hpp"
//#include "room_list_group.hpp"
#include "trade.hpp"
#include "contest.hpp"
//#include "hunter_top.hpp"

#include "ap_toplist.hpp"
#include "limit.hpp"
#include "team_contest.hpp"
#include "cache.hpp"
/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  init_service(int isparent)
{
	if (!isparent) 
	{
		setup_timer();
		Online::init();
		init_funcs();
//		config_init("common.conf");
		//init_room_list_group();
		INIT_LIST_HEAD(&(g_events.timer_list));

		Trade::init();

		init_contest_groups();

		system("rm -f top_list.txt");
		ADD_TIMER_EVENT(&g_events, save_ap_toplist_data,  reinterpret_cast<void*>(1), get_now_tv()->tv_sec + AP_SAVE_TIME);
		ap_list1.init("top_list.txt");
		ap_list2.init("top_list.txt");		
		g_limit_data_mrg.init("limit_data.txt");
		if (g_limit_data_mrg.get_limit_cnt()) {
			g_limit_data_mrg.broad_limit_data(0);

			add_reset_limit_data_timer(0, 0);
			ADD_TIMER_EVENT(&g_events, save_limit_data,  reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 60);
		}

		init_team_contest_server_info();
		get_server_team_info_from_cachesvr();

	}
	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  fini_service(int isparent)
{
	if (!isparent) 
	{
		Online::final();
		final_funcs();
		//final_room_list_group();
		ap_list2.final("top_list.txt");
        //Hunter_top::final();
        //delete g_hunter;
	}
	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void proc_events()
{
	handle_timer();
	update_all_end_contest();
	clear_out_date_team_contest_info();
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{	
	if (avail_len < 4) 
	{
		return 0;
	}

	const svr_proto_t* pkg = reinterpret_cast<const svr_proto_t*>(avail_data);
	if ((pkg->len > pkg_size) || (pkg->len < sizeof(svr_proto_t))) 
	{
		ERROR_LOG("invalid len=%u from fd=%d", pkg->len, fd);
		return -1;
	}

	return pkg->len;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	return dispatch(data, fdsess);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void proc_pkg_from_serv(int fd, void* data, int len)
{
	if (fd == cachesvr_fd) {
		handle_cachesvr_callback(data, len);
	}
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
	int ret = Online::clear_online_info(fd);

	if (ret == -1) {
		ret = Online::clear_battle_info(fd);
	}

	if (ret == -1) {
		ret = Trade::clear_trade_info(fd);
	}
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_fd_closed(int fd)
{

 if (fd == cachesvr_fd) {
		cachesvr_fd = -1;
		DEBUG_LOG("CACHE SVR FD CLOSED");
	}
}

/**
  * @brief Called to process mcast package from the address and port configured in the config file
  */
extern "C" void proc_mcast_pkg(const void* data, int len)
{

}

