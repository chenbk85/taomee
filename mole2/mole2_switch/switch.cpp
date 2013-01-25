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

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}

#include "dbproxy.hpp"
#include "dispatcher.hpp"
#include "online.hpp"
#include "proto.hpp"
#include "utils.hpp"

int init_all_timer_type(int rstart)
{
	REGISTER_TIMER_TYPE(n_make_online_dead, Online::make_online_dead, rstart);
	REGISTER_TIMER_TYPE(n_report_usr_num, Online::report_usr_num, rstart);
	REGISTER_TIMER_TYPE(n_try_match_again, Online::try_match_again, rstart);
	REGISTER_TIMER_TYPE(n_get_val_from_db, Online::get_val_from_db, rstart);
	REGISTER_TIMER_TYPE(n_start_refresh, Online::start_refresh, rstart);
	return 0;
}


/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  init_service(int isparent)
{
	if (!isparent) {
		setup_timer();
		init_all_timer_type(0);
		Online::init();
		init_funcs();
		const char *udp_ip = get_server_ip(); 
		uint16_t udp_port = get_server_port();
        bind_udp_socket(udp_ip, udp_port);
        BOOT_LOG(0,"online udp listen:%s:%d",udp_ip, udp_port);
	}

	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  fini_service(int isparent)
{
	if (!isparent) {
		Online::fini();
		destroy_timer();
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
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{	
	if (avail_len < 4) {
		return 0;
	}

	const svr_proto_t* pkg = reinterpret_cast<const svr_proto_t*>(avail_data);
	if ((pkg->len > pkg_size) || (pkg->len < sizeof(svr_proto_t))) {
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
	if (fd == proxysvr_fd) {
		handle_db_return((svr_proto_t*)data, len);
	}
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
	Online::clear_online_info(fd);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_fd_closed(int fd)
{
	if (fd == proxysvr_fd) {
		DEBUG_LOG("DB PROXY CONNECTION CLOSED\t[fd=%d]", proxysvr_fd);
		proxysvr_fd = -1;
	}
}

extern "C" void proc_udp_pkg(int fd,void* data, int len)
{
	if (len < (int)sizeof(svr_proto_t))
		return;
	int ret = dispatch_udp(data, fd);
    DEBUG_LOG("proc_udp_pkg:len:%d,ret=%d", len,ret);
}
