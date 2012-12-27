// =====================================================================================
// 
//       Filename:  time_stamp_impl.hpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  06/09/2010 11:15:38 AM CST
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  xcwen (xcwen), jim@taomee.com
//        Company:  TAOMEE
// 
// =====================================================================================

#ifndef  TIME_STAMP_IMPL_INC
#define  TIME_STAMP_IMPL_INC
extern "C" {
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/inet/ipaddr_cat.h>

#include <libtaomee/project/utilities.h>

#include <async_serv/net_if.h>
}

#include <libtaomee++/inet/byteswap.hpp>
#include <main_login/common.hpp>
#include <main_login/id_counter.hpp>
#include <main_login/user_manager.hpp>

#include "switch_impl.hpp"

//login-timestamp package header define
typedef struct login_timestamp_pk_header {
	uint32_t	length;
	char		version;
	uint32_t	commandid;
	uint32_t    seq;
	uint32_t	result;
    uint8_t     body[];
}__attribute__((packed)) login_timestamp_pk_header_t;

const uint32_t svr_usr_counter = 60000; 

/*time stamp cmd*/
enum{
	TIMESTAMP_GET_FRIENDTIME = 8,  
};

//part of client-loginServer respond data
typedef struct friend_info {
	uint32_t friend_id ;
	uint32_t record_time ;
}__attribute__((packed)) friend_info_t ;


/** 这个结构定义了客户端到登陆服务器的包头结构 */
typedef struct cli_login_pk_header {
	uint32_t    length;
	uint8_t     version;
	uint32_t    commandid;
	userid_t    userid;
	uint32_t    result;
	uint8_t     body[];
}__attribute__((packed)) cli_login_pk_header_t;

class SwitchService;

class TimeStampService {
	public:

		TimeStampService();

		static int get_timestamp_pkg_len(const void* avail_data);

		static void handle_timestamp_return(login_timestamp_pk_header_t* dbpkg, uint32_t pkglen);

		static void init_timestamp_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd);

		static int send_to_timestamp(usr_info_t *p, const uint8_t *buf, uint32_t len);

	private:		

		static int get_friend_time_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret);

		static int timestamp_timeout(void* owner, void* data);

	public:

		static int fd;

	private:

		static SwitchService mole_swith;
};

#endif   // ----- #ifndef TIME_STAMP_IMPL_INC  ----- 

