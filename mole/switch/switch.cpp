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

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  init_service(int isparent)
{
	if (!isparent) {
		setup_timer();
		Online::init();
		init_funcs();
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
svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(data);

#define CALLBACK(CODE, FUNC) \
	case (CODE): (FUNC)(pkg,len - sizeof(svr_proto_t), fd); break

	if(fd == proxysvr_fd) {
		switch(pkg->cmd) {
			CALLBACK(dbcmd_guess_end,		Online::broadcast_guess_pkt);
			CALLBACK(dbcmd_guess_start,	Online::broadcast_guess_pkt);
			CALLBACK(dbcmd_add_item,	Online::user_add_item_pkt);
			default:
				ERROR_LOG("Not Supported Command = %x", pkg->cmd);
				break;
		}
	}
	
#undef CALLBACK
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

