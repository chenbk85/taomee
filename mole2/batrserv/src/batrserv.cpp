/**
 *============================================================
 *  @file     batrserc.c
 *  @brief
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */


#ifdef __cplusplus
extern "C"
{
#endif

#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
#ifdef __cplusplus
}
#endif
#include  <libtaomee++/utils/tcpip.h>
#include "version.h"
#include "bt_dispatcher.h"
#include "bt_online.h"
#include "bt_skill.h"
#include "bt_bat_check.h"
#include "proto.h"
#include "bt_beastai.h"
#include "global.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
bool g_is_test_env;
int  init_service(int isparent)
{
	if (!isparent) {
		DEBUG_LOG("==INIT_SERVICE BEGIN== ");
        DEBUG_LOG("INIT_SERVICE");
        const char *ip= get_ip_ex(0x01);
        if ( strncmp( ip,"10.",3 )==0 ) {
            g_is_test_env=true;
            DEBUG_LOG("=============TEST ENV TRUE =============");
        }else{
            g_is_test_env=false;
            DEBUG_LOG("=============TEST ENV FALSE =============");
        }

        g_log_send_buf_hex_flag=g_is_test_env?1:0;

		setup_timer();
		if (init_battles() != 0){
			ERROR_LOG("init battserv failed");
			return -1;
		}
		
		init_funcs();
		init_attck_proc_func();
		init_attck_proc_pet_func();
		if (load_lua_script() != 0){
			ERROR_LOG("load ai script failed");
			return -1;
		}
		
		init_attck_check_func();
		if ((load_xmlconf("../conf/skills_price.xml", load_skill_use_mp) == -1)
			|| (load_xmlconf("../conf/BattleActionList.xml", load_skill_attr) == -1) 
			|| (load_xmlconf("../conf/BattleMonsterTalk.xml", load_beast_topic) == -1) 
			|| init_all_timer_type(0) == -1
			){
			return -1;
		}
		init_cli_handle_funs();
		DEBUG_LOG("==INIT_SERVICE END== ");
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
	//DEBUG_LOG("get_pkg_len FD:%d,len %d ",fd,avail_len );
	if (avail_len < 4) {
		return 0;
	}

	if (isparent) {
		const batrserv_proto_t* pkg = (const batrserv_proto_t*)avail_data;
		if ((pkg->len > bt_proto_max_len) || (pkg->len < sizeof(batrserv_proto_t))) {
			if ( pkg->len>=18 && pkg->cmd ){
				char version_buf[256];
		        memset(version_buf ,0,sizeof(version_buf ) );
		        *((uint32_t*)version_buf)=sizeof(version_buf )  ;
		        std::vector<std::string> file_name_list;
		        file_name_list.push_back("../conf/BattleActionList.xml");
              	strncpy(version_buf+18 ,
                get_version_str(g_version_str,file_name_list ).c_str(),
               	sizeof(version_buf )- 18);
        		return net_send(fd,version_buf , sizeof(version_buf ) );
 
			}
			
			ERROR_LOG("invalid len-1=%u from fd=%d", pkg->len, fd);
			return -1;
		}

		return pkg->len;
	} else {
		const batrserv_proto_t* pkg = (const batrserv_proto_t*)avail_data;
		if ((pkg->len > bt_proto_max_len) || (pkg->len < sizeof(batrserv_proto_t))) {
			ERROR_LOG("invalid len-2=%u from fd=%d", pkg->len, fd);
			return -1;
		}

		return pkg->len;
	}
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
int  proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	int ret=dispatch(data, fdsess);
	return ret;
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
	clean_fdsess_onlinex_down(fd);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
void on_fd_closed(int fd)
{
}
int  reload_global_data()
{
    /*unregister all timer callbacks*/
    unregister_timers_callback();

		init_funcs();
		init_attck_proc_func();
		init_attck_proc_pet_func();
		if (load_lua_script() != 0){
			ERROR_LOG("load ai script failed");
			return -1;
		}
		
		init_attck_check_func();
		if ((load_xmlconf("../conf/skills_price.xml", load_skill_use_mp) == -1)
			|| (load_xmlconf("../conf/BattleActionList.xml", load_skill_attr) == -1) 
			|| (load_xmlconf("../conf/BattleMonsterTalk.xml", load_beast_topic) == -1) 
			|| init_all_timer_type(0) == -1
			){
			return -1;
		}
		init_cli_handle_funs();


    refresh_timers_callback();
    return 0;
}


#ifdef __cplusplus
}
#endif

