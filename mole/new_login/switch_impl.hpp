// =====================================================================================
// 
//       Filename:  switch_impl.hpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  06/09/2010 11:14:31 AM CST
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  xcwen (xcwen), jim@taomee.com
//        Company:  TAOMEE
// 
// =====================================================================================

#ifndef  SWITCH_IMPL_INC
#define  SWITCH_IMPL_INC

extern "C" {
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/inet/ipaddr_cat.h>
#include <libtaomee/project/utilities.h>

#include <async_serv/net_if.h>
}

#include <main_login/common.hpp>
#include <main_login/id_counter.hpp>
#include <main_login/user_manager.hpp>

typedef struct switch_head {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t id;
	uint8_t body[];
}__attribute__((packed)) switch_head_t;

struct timer_head_t {
	list_head_t timer_list;
}__attribute__((packed));

//online server infomation
typedef struct svr_info {
	uint32_t id;
	uint32_t users ;
	char ip[16] ;
	short int port ;
	uint32_t friends;
}__attribute__((packed)) svr_info_t ;

/*保存switch返回的服务器信息*/
typedef struct svr_list {
	uint16_t   domain_id;
	uint32_t   online_cnt;
	svr_info_t  svrs[];
}__attribute__((packed)) svr_list_t;

/*switch cmd*/
enum {
	SWITCH_GET_RECOMMEND_SVR_LIST = 62005, 
	SWITCH_GET_RANGED_SVR_LIST = 64001,     
};

/*定时备份最小的online的ID号*/
const uint32_t min_backup_svr = 1;

/*定时备份是最大的online的ID号*/
const uint32_t max_backup_svr = 100;

/*最小的有效online的ID号*/
const uint32_t min_valid_svr = 1;

/*最大的有效的online的ID号*/
const uint32_t max_valid_svr = 2000; 

const uint32_t switch_max_size = 1024 * 1024 * 2;

class SwitchService {

public:

	SwitchService();

	static int get_switch_pkg_len(const void *avail_data);

	static void init_switch();

	static void handle_switch_return(switch_head_t *dbpkg, uint32_t pkglen);

	static int get_recommended_svr_list(usr_info_t *p);

	static int get_ranged_svr_list(usr_info_t *p, int start_id, int end_id, int friendcount, userid_t allfriendid[]);

private:

	static int get_recommeded_svr_list_callback(usr_info_t *p, uint8_t *body, uint32_t bodylen, int ret);

	static int send_recommeded_svr_list(usr_info_t *p, svr_info_t *svrs, uint32_t online_cnt, int max_online_id);

	static int switch_timeout(void *owner, void *data);

	static int update_backup_svrlist_callback(uint8_t *body, uint32_t bodylen, int ret);

	static int update_backup_svrlist(void *owner, void *data);

	static int get_backup_svr_list(usr_info_t *p);

	static int send_to_switch_without_init_head(usr_info_t *p, const uint8_t *buf, uint32_t length);

	static int get_ranged_svr_list_callback(usr_info_t *p, uint8_t *body, uint32_t bodylen, int ret);

	static int send_ranged_svrlist(usr_info_t *p, svr_info_t *svrs, uint32_t online_cnt);

	static int send_to_switch(usr_info_t *p, uint16_t cmd, const void *dbpkgbuf, uint32_t body_len);

	static void init_switch_head(usr_info_t *p, void *buf, uint32_t len, uint32_t cmd);

	static void send_login_num_to_stat(usr_info_t *p);

	static int check_switch_return(switch_head_t *dbpkg, uint32_t pkglen);
public:

	static int fd;

private:

	static uint8_t switch_buf[switch_max_size];
};

#endif   // ----- #ifndef SWITCH_IMPL_INC  ----- 

