#ifndef GF_LIB_TYPE_H
#define GF_LIB_TYPE_H

#include "main_login/asyc_main_login_lib.h"
//#include <main_login/asyc_main_login_lib.h>
//svr protocol cmd
#define MULTICAST_CMD                   61003
#define SWITCH_GET_RECOMMEND_SVR_LIST   63001
#define SWITCH_GET_RANGED_SVR_LIST      63002
#define TIMESTAMP_GET_FRIENDTIME        8

//cli protocol cmd
#define PROTO_GET_RECOMMEND_SVR_LIST    105
#define PROTO_GET_RANGED_SVR_LIST       106
#define PROTO_GET_ROLE_LIST             107
#define PROTO_CREATE_ROLE               108
#define PROTO_DELETE_ROLE               110

//statistic start offset 
#define MOLE_LOGIN_OFFSET               0x020D0201
#define MOLE_REGISTER_MIMI_OFFSET       0x020D0001
#define MOLE_REGISTER_CHANNEL_OFFSET    0x020D0101
#define MOLE_NOT_NORMAL_REG_CHANNEL    	0x020D01FF
#define MOLE_NOT_NORMAL_LOG_CHANNEL    	0x020D0220

//ipcounter cmd
#define ADD_UIDCOUNT                    1
#define CLEAR_UID_INFO                  2
#define GET_UID_INFO                    3
//svr list range
#define MIN_VALID_SVR                   1
#define MAX_VALID_SVR                   2000                              
//在insert，米米号已存在
#define USER_ID_EXISTED_ERR             1104
//角色不存在
#define USER_ROLE_NOFIND_ERR            1106
//角色已存在
#define USER_ROLE_EXISTED_ERR           1107
//封号
#define USER_NOUSED_ERR                 1120
//24小时离线
#define USER_OFFLIN24_ERR               1121
#define USER_NOT_ENABLED_ERR            1107
#define USER_OFFLINE_SEVEN_DAY_ERR      2541
#define USER_OFFLINE_FOURTEEN_DAY_ERR   2542
#define USER_ANXI_SERVICE_OUT_OF   		2571
#define USER_ROLE_MAX_ERR   			3300

#define ROLE_EXISTED_ERR		        6012
#define ROLE_NOT_FIND		            6013
#define ROLE_OVER_MAX_ERR		        6014

#define MAX_ROLE_NUM                    4 


//part of client-loginServer respond data
typedef struct friend_info {
	uint32_t friend_id ;
	uint32_t record_time ;
}__attribute__((packed)) friend_info_t ;

//online server infomation
typedef struct svr_info {
	uint32_t id;
	uint32_t users ;
	char ip[16] ;
	short int port ;
	uint32_t friends;
} __attribute__((packed)) svr_info_t ;

//switch server respond data body
typedef struct svr_list {
	uint32_t   max_online_id;
	uint16_t   domain_id;
	uint32_t   online_cnt;
	svr_info_t  svrs[];
} __attribute__((packed)) svr_list_t;

//login-timestamp package header define
typedef struct login_timestamp_pk_header {
	uint32_t	length;
	char		version;
	uint32_t	commandid;
	uint32_t    seq;
	uint32_t	result;
    uint8_t     body[];
} __attribute__((packed)) login_timestamp_pk_header_t;

//login-onlineServer multicast package header define
typedef struct login_online_pk_header {
	uint32_t	len;
	uint32_t	online_id;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	uint64_t	mapid;
	uint32_t	opid;
} __attribute__((packed)) login_online_pk_header_t;

typedef struct role_info {
	char 		sess[SESS_LEN];	
	uint32_t 	role_type;
	char    	nick[NICK_LEN];
	//uint32_t	color;
	//uint32_t 	channel;
} __attribute__((packed)) role_info_t;

typedef struct clothes_info {
	uint32_t	clothes_id;
	uint32_t	get_time;
} __attribute__((packed)) clothes_info_t;

typedef struct role_list_info {
	uint32_t	role_tm;
	uint32_t	role_type;
	uint32_t	level;
	char		nick[NICK_LEN];
	uint32_t	clothes_count;
	clothes_info_t	clothes_item[];
}__attribute__((packed)) role_list_info_t;

typedef struct role_list_header {
	uint32_t			role_tm;//no meaning
	uint32_t			role_count;
}__attribute__((packed)) role_list_header_t;

/**
 * 该结构在调用util中的tcp服务时会用到，主要描述了ip和port 
 * @see tcp_service_query
 */
typedef struct local_service {
	char *ip ;
	u_short port ;
} local_service_t;

typedef struct LoginSession {
	uint32_t uid, ip, tm1, tm2;
} __attribute__((packed)) login_session_t;

typedef struct main_login_conf {
	char 		dbproxy_ip[16];
	u_short 	dbproxy_port;
	
	char		uidcount_ip[16];
	u_short 	uidcount_port;
	
	uint32_t	count_limited;
	uint32_t	passwd_fail_time_limited;
	uint32_t	ban_time;
	
	uint32_t 	channel_total;
	char    	statistic_file[128];
} __attribute__((packed)) main_login_conf_t;

typedef struct {    
    char    multicast_interface[16];
    char    multicast_ip[16];   
    u_short multicast_port;

    char    gf_dbproxy_ip[16];
    u_short gf_dbproxy_port;

    char    gf_deluser_ip[16];
    u_short gf_deluser_port;

    char    time_ip[16];
    u_short time_port;

    char    master_ip[16];
    u_short master_port;
	char    master_ser[32];

	u_short dx_or_wt;
}my_option_t;

struct gf_login_out{
	uint32_t	role_tm;
    uint32_t    vip_flag;
    uint32_t    friendcount;
    uint32_t    blackcount;
    uint8_t     frienditem[];
}__attribute__((packed));

struct  gf_register_in{
	uint32_t 	role_tm;
	uint32_t 	role_type;
    uint32_t 	level;
    uint32_t 	hp;
    uint32_t 	mp;
    char 		nick [NICK_LEN];
}__attribute__((packed));

#endif //GF_LIB_TYPE_H

