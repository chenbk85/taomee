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

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}


#include "dispatcher.h"
#include "proto.h"
#include "onlinehome.h"
#include "home.h"
#include  <libtaomee++/utils/tcpip.h>

#include "sprite.h"


typedef struct TmrTest {
    list_head_t timer_list;
} tmr_test_t;
tmr_test_t tmr;
//处理超时报文
int do_time_event(void* owner, void* data)
{

	//DEBUG_LOG("do_time_event");
	g_pvp[0].check_time_outs();
	g_pvp[1].check_time_outs();
    //追加定时操作
    ADD_TIMER_EVENT(&tmr, do_time_event, NULL, get_now_tv()->tv_sec + 1);
    return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */


bool g_is_test_env;
int g_log_send_buf_hex_flag;
extern "C" 
int  init_service(int isparent)
{
	if (!isparent) {
     	DEBUG_LOG("INIT_SERVICE");
	    const char *ip= get_ip_ex(0x01);
	    if ( strncmp( ip,"10.",3 )==0 ) {
	        g_is_test_env=true;
			DEBUG_LOG("1111=============TEST ENV TRUE =============");
	    }else{
	        g_is_test_env=false;
			DEBUG_LOG("1111=============TEST ENV FALSE =============");
	    }


		g_log_send_buf_hex_flag=g_is_test_env?1:0;
		g_pvp[0].game_user_count= config_get_intval("PVP_GAME_USER_COUNT" ,4);
		g_pvp[1].game_user_count= config_get_intval("PVP_GAME_USER_COUNT" ,4);
		DEBUG_LOG("PVP_GAME_USER_COUNT game1:%u",g_pvp[0].game_user_count  );
		DEBUG_LOG("PVP_GAME_USER_COUNT game2:%u",g_pvp[1].game_user_count  );
	


		srand(time(0));
		setup_timer();
        INIT_LIST_HEAD(&tmr.timer_list);
        ADD_TIMER_EVENT(&tmr, do_time_event, NULL, get_now_tv()->tv_sec + 1);

		init_hero_cup();
		init_home_maps();
		init_cli_handle_funs();
		memset(all_fds,0,sizeof(all_fds));
	}
	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" 
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
extern "C" 
void proc_events()
{
	handle_timer();
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" 
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
extern "C" 
int  proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	return dispatcher(data, fdsess);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" 
void proc_pkg_from_serv(int fd, void* data, int len)
{

}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" 
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
extern "C" 
void on_fd_closed(int fd)
{

}



