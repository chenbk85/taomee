/**
 *============================================================
 *  @file      login.cpp
 *  @brief     通用的登录服务器框架代码，不许更改，具体的登录细节实现通过login_impl.hpp和login_impl.cpp来完成
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

extern "C" {
// #include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>

#include <async_serv/net_if.h>
}

#include <main_login/user_manager.hpp>

#include "login_impl.hpp"

/**
* @brief AsyncServer框架要求实现的接口之一。 
*
*/
extern "C" int  init_service(int isparent)
{
	if(!isparent) {
		setup_timer();
		// initiate service
		if (login.init() == false) {
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
		destroy_timer();
		// finalize service
		login.fini();
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
	return login.get_pkg_len(fd, avail_data, avail_len, isparent);
}

/**
* @brief AsyncServer框架要求实现的接口之一。 
*
*/
extern "C" int  proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	return login.dispatch(data, fdsess);
}

/**
* @brief AsyncServer框架要求实现的接口之一。 
*
*/
extern "C" void proc_pkg_from_serv(int fd, void* data, int len)
{
	login.proc_pkg_from_serv(fd, data, len);
}

/**
* @brief AsyncServer框架要求实现的接口之一。 
*
*/
extern "C" void on_client_conn_closed(int fd)
{
	user_mng.dealloc_user(fd);
}

/**
* @brief AsyncServer框架要求实现的接口之一。 
*
*/
extern "C" void on_fd_closed(int fd)
{
	login.on_fd_closed(fd);
}

