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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>


#include "dispatcher.h"
#include "proto.h"
#include "onlinehome.h"
#include "home.h"

#include "sprite.h"



/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
int  init_service(int isparent)
{
	if (!isparent) {
		srand(time(0));
		setup_timer();

		init_hero_cup();
		init_home_maps();
		memset(all_fds,0,sizeof(all_fds));
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

	home_proto_t *pkg = (home_proto_t*)avail_data;

	if(pkg->len > proto_max_len || pkg->len < sizeof(home_proto_t)) {
		KERROR_LOG(pkg->id,"invalid len=%u from online(%u):%d",pkg->len,pkg->onlineid,fd);
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
	return dispatcher(data, fdsess);
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
	ERROR_LOG("connection break [fd=%u]", fd);

	free_hero_cup();
	clean_all_users_from_online(fd);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
void on_fd_closed(int fd)
{

}



