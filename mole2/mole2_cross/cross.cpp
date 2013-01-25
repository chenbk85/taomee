/**
 *============================================================
 *  @file      cross.cpp
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
#include "proto.hpp"
#include "utils.hpp"
#include "cross_inc.hpp"
static IJobDispatcher* g_static_dispatcher = NULL;

static int online_timeout_process(void* owner, void* data)
{
	if (g_static_dispatcher){
		return g_static_dispatcher->Dispatch(CROSS_COMMID_ONLINE_TIMEOUT, owner, data);
	}
	return -1;
}

static int online_pk_tryagain(void* owner, void* data)
{
	if (g_static_dispatcher){
		return g_static_dispatcher->Dispatch(CROSS_COMMID_ONLINE_PK_TRYAGAIN, owner, data);
	}
	return -1;
}

int init_all_timer_type(int rstart)
{
	REGISTER_TIMER_TYPE(n_make_online_dead, online_timeout_process, rstart);
	REGISTER_TIMER_TYPE(n_try_match_again, online_pk_tryagain, rstart);
	return 0;
}


/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  init_service(int isparent)
{
	if (!isparent) {
		DEBUG_LOG("CROSS SERVER START");
		setup_timer();
		init_all_timer_type(0);
		g_static_dispatcher = CreateJobDispatcher();
		if (g_static_dispatcher == NULL){
			ERROR_LOG("CROSS SERVER START FAILED, CREATE DISPATCHER FAILED, MEMORY NOT ENOUTH");
			return -1;
		}
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
		DEBUG_LOG("fini service");
		if (g_static_dispatcher){
			g_static_dispatcher->Release();
			delete g_static_dispatcher;
			g_static_dispatcher = NULL;
		}
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

	const mole2cross_proto_t* pkg = reinterpret_cast<const mole2cross_proto_t*>(avail_data);
	if ((pkg->len > pkg_size) || (pkg->len < sizeof(mole2cross_proto_t))) {
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
	if (g_static_dispatcher){
		return dispatch(g_static_dispatcher, data, fdsess);
	}else{
		ERROR_LOG("DISPATCHER IS NULL");
		return -1;
	}
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void proc_pkg_from_serv(int fd, void* data, int len)
{
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
	if (g_static_dispatcher){
		DEBUG_LOG("client conn closed");
		void* param = NULL;
		CONVERT_PTR_TO_PTR(fd, param);
		g_static_dispatcher->Dispatch(CROSS_COMMID_CLEAN_ONLINE_INFO, param, NULL);
	}
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
