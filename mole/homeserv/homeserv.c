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

#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>

#include "hs_dispatcher.h"
#include "hs_online.h"
#include "proto.h"

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
int  init_service(int isparent)
{
	if (!isparent) {
		setup_timer();
		init_communicator();
		init_funcs();
	}

	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
int  fini_service(int isparent)
{
	if (!isparent) {
		destroy_timer();
	}
	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
void proc_events()
{
	handle_timer();
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
int  get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{	
	if (avail_len < 4) {
		return 0;
	}

	const svr_proto_t* pkg = avail_data;
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
int  proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	return dispatch(data, fdsess);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
void proc_pkg_from_serv(int fd, void* data, int len)
{
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
void on_client_conn_closed(int fd)
{
	DEBUG_LOG("connection break\t[fd=%u]", fd);
	clean_all_users_onlinex_down(fd);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
void on_fd_closed(int fd)
{/*
	if (fd == proxysvr_fd) {
		DEBUG_LOG("DB PROXY CONNECTION CLOSED\t[fd=%d]", proxysvr_fd);
		proxysvr_fd = -1;
	}
	*/
}

