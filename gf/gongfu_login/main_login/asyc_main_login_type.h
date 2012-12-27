/**
 *====================================================================
 *  @file        		type.h
 *  @brief     		This file is to define the program specific type.
 *
 *  compiler   		gcc  4.1.2
 *  platform   		Debian GNU/LINUX
 *  author      		Kal, Develop Group 1
 *  create date	2009.2.2
 *  copyright  		TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *====================================================================
*/ 
#ifndef LS_TYPE_H
#define LS_TYPE_H
extern "C" {
#include <stdint.h>
#include <sys/types.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/log.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/dirtyword/dirtyword.h>
}
#include <libtaomee++/inet/pdumanip.hpp>
#include "util.h"

//error number for client
/** 系统错误 */
#define SYSTEM_ERR                      5001                              
/** 密码错误 */
#define PASSWD_ERR                      5003                              
/** 米米号找不到 */
#define USER_NOT_FIND                   5005   
/** 客户端协议字段错误，暂时没用 */
#define CLI_VERSION_ERR                 5008                              
/** 用户在一定时间内输错密码太多 */
#define PASSWD_ERR_TOO_MUCH             5009   
#define INVALID_NICK_NAME               5010

//login by email
#define ULOGIN_EMAIL_USERID             0
#define SVR_USR_COUNTER                 60000
#define MAX_CONNECTION                  60000

#define SESS_LEN                        16
#define NICK_LEN                        16

#define EMAIL_MAX_LEN                   64
//cli protocol cmd
#define PROTO_USER_LOGIN                104

#define MIMI_BASE                       0x06050000
//statistic offset
#define USER_LOGIN_CHANNEL_OFFSET       0
#define USER_BANNED_OFFSET              0x00001000
#define UNDEFINE_LOCATION_OFFSET        0x0000FFFF
#define USER_PASSWD_ERROR_OFFSET        0x00001001

//pkg len limit
#define MIN_LENGTH                      17
#define MAX_LENGTH                      8192 
#define BUFFER_SIZE                     8192
//game flag num
#define MAX_GAME_NR                     32

//define the error num
#define SUCCESS                         0
#define CLOSE_CONN                      -1

#define MAX_FRIEND_NUM                  200

//db return error number
//登入时，检查用户名和密码出错
#define CHECK_PASSWD_ERR                1103
//在select update, delete 时，米米号不存在
#define USER_ID_NOFIND_ERR              1105

typedef uint32_t userid_t;

struct timer_head_t {
    list_head_t timer_list;
}__attribute__((packed));

struct usr_info_t {
    uint32_t uid;
    uint32_t waitcmd;
    uint32_t channel;
    uint32_t counter;
    fdsession_t *session;
    timer_head_t tmr;
    uint8_t  tmpinfo[4096];
} __attribute__((packed));

/** 这个结构定义了客户端到登陆服务器的包头结构 */
typedef struct cli_login_pk_header {
    uint32_t    length;
    //uint8_t     version;
    uint16_t    commandid;
    userid_t    userid;
    uint32_t    seqnum;
    uint32_t    result;
    uint8_t     body[];
} __attribute__((packed)) cli_login_pk_header_t;

typedef struct cli_login_req {
	uint8_t		passwd[32];
	uint32_t  	channel;
	uint32_t    which_game;
	uint32_t    user_ip;
} __attribute__((packed)) cli_login_req_t;

typedef struct svr_login_req {
	uint8_t		passwd[32];
	uint32_t  	channel;
	uint32_t    which_game;
	uint32_t    user_ip;
	uint8_t		email[64];
} __attribute__((packed)) svr_login_req_t;


typedef struct statistic_info {
	int count_unit; //always is 1, used to caculate total number of message log.
	uint32_t user_id;
	statistic_info () {
		count_unit = 1;
	}
}statistic_info_t;

typedef struct _svr_proto_t {
    /*! package length */
    uint32_t    len;
    /*! sequence number ((p->fd << 16) | p->waitcmd) */
    uint32_t    seq;
    /*! command id */
    uint16_t    cmd;
    /*! errno */
    uint32_t    ret;
    /*! user id */
    userid_t    id;
    /*! package body */
    uint8_t     body[];
} __attribute__((packed)) svr_proto_t ;

typedef struct _chk_pwd {
    uint8_t pwd[16];
    uint32_t ip;
    uint32_t channel;
}__attribute__((packed)) chk_pwd_pkg_t;

typedef struct {    
    char    log_dir[256];
    int     log_level;
    uint32_t log_size;
    
    char    mainlogin_dbproxy_ip[16];
    u_short mainlogin_dbproxy_port;

    char    warning_ip[16];
    u_short warning_port;
    
    uint32_t    count_limited;
    uint32_t    passwd_fail_time_limited;
    uint32_t    ban_time;

    uint32_t svr_timeout;
    uint32_t business_type;

    char    statistic_file[128];
    uint32_t channel_total;
    //uint32_t msglog_type_start;
    //int weekday_maintance;
    //uint32_t maintain_ip;
    //uint32_t inner_ip_start;
    //uint32_t inner_ip_end;

}dirser_ini_option_t;

/**
 * @brief 存储IP统计的次数和第一次统计的时间值
 */
struct ip_info_t {
	/**
	 * @brief 构造函数，初始化ip_cnt为零和first_time为当前时间
	 */
	ip_info_t()
	{ 
		ip_cnt = 0; 
		first_time = time(0); 
	}
    
	/*! 存储IP地址的次数 */
	uint32_t ip_cnt;
	/*! 存储IP第一次统计的时间 */
	time_t first_time;
} __attribute__((packed));

struct warning_pkg_t{
    uint32_t   notify_category;//项目类型;mole:1 ,seer:2
    uint32_t   notify_id;// 服务类型:db:1 ,swtich:2...
    uint32_t   mimi_account;//用户ID
    uint32_t   common_id;//cmdid(操作的命令号)
}__attribute__((packed));
#endif


