#ifndef GF_LIB_TYPE_H
#define GF_LIB_TYPE_H

#include "util.h"



//login by email
enum const_def_t {
	gf_game_flag 	   = 6,
	gf_svr_usr_counter = 60000,
	
//define the error num
	gf_success         = 0,
	gf_close_conn      = -1,
	gf_max_friend_num  = 200,
	gf_max_role_num    = 4,
	gf_min_backup_svr  = 1,
	gf_max_backup_svr  = 100,
	
//svr list range
	gf_min_valid_svr   = 1,
	gf_max_valid_svr   = 2000,

	gf_role_type_min 	= 1,
	gf_role_type_max 	= 4,
	
	gf_timestamp_get_friendtime = 8,

	max_invi_code_len	= 16,
};

enum {
	gf_sess_len = 16,
	gf_nick_len = 16,
};

//pkg len limit
enum len_limit_t {
	gf_max_len 		= 8192,
	gf_buffer_size 	= 8192,
};

//cli protocol cmd
enum cli_cmd_t {
	proto_get_recommend_svr_list 		 = 105,
	proto_get_ranged_svr_list    		 = 106,
	proto_get_role_list          		 = 107,
	proto_create_role           		 = 108,
	proto_gray_delete_role          	 = 110,
	proto_logic_delete_role        		 = 111,
	proto_resume_gray_role     			 = 112,

	proto_check_invite_code				 = 120,
	proto_create_role_with_inv_code		 = 121,
	
	proto_get_version					 = 201,
};

enum db_cmd_t {
	db_gf_login 				= 0x06E4,
	db_gf_get_role_list    		= 0x06E3,
	db_gf_register     			= 0x07E1,
	db_gf_add_game_flag   		= 0x0108,
	db_gf_add_game_flag_ex 		= 0xA123,
	db_gf_add_childid			= 0x0709,

	db_gf_gray_delete_role 			= 0x07F0,
	db_gf_logic_delete_role 		= 0x07F2,
	db_gf_resume_gray_role 			= 0x07F3,
	db_gf_verify_pay_passwd 		= 0x008B,
	
	
	deluser_gf_add_del_role 	= 0x0EA1,

	db_sess_check_session		= 0xA024,

	db_other_check_invite_code  = 0x0E00,
	db_other_check_user_invited = 0x0E02,

	db_userinfo_get_mee_fans    = 0x0060,
	db_gf_get_amb_info			= 0x0707,
	
	db_gf_insert_tmp_info		= 0x0E45,
};

enum switch_cmd_t {
	gf_switch_get_recommend_svr_list = 63001,
	gf_switch_get_ranged_svr_list	 = 63002,
	gf_switch_kick_user_offline		 = 60002,
};

enum outher_cmd_t {
	gf_multicast_cmd = 61003,
};

//error number for client
enum err_client_t {     
	gf_role_not_find					= 6011,
	gf_role_existed_err		       		= 6012,
	gf_role_over_max_err		       	= 6013,
	gf_invalid_nick 					= 6014,

	gf_invalid_invite_code				= 6101,
	gf_invite_code_used					= 6102,
	gf_user_never_input_invite_code		= 6103,
};

//db return error number
enum err_db_t {
//在select update, delete 时，米米号不存在
	gf_user_id_nofind_err           	= 1105,
	gf_role_id_nofind_err			 	= 1106,
//角色已存在
	gf_user_role_existed_err        	= 1107,
	gf_user_role_max_err   				= 3300,

	dberr_session_check  				= 4331,

	gf_invalid_invite_code_err			= 5101,
	gf_invite_code_used_err				= 5102,
};

//statistic start offset 
enum static_log_t {
	//gf_not_normal_reg_channel    	= 0x020D01FF,
	//gf_not_normal_log_channel    	= 0x020D0220,


#ifndef VERSION_KAIXIN
	gf_register_channel_offset		= 0x09010101,//用于统计渠道日新增
	gf_login_offset              	= 0x09010201,//用于统计渠道日登陆人次/人数
	gf_register_mimi_offset     	= 0x09010003,//用于统计双周流失人数
	gf_report_lv_offset				= 0x09010301,//用于统计等级分布
	gf_report_child_lv_num			= 0x09500100,
	gf_report_dragon_exp			= 0x09010031,
	gf_report_user_role_exp			= 0x09011000,
	gf_report_user_vip_xvalue		= 0x09500004,

    gf_register_channel_offset_tw   = 0x0FA00003,
#else
	gf_register_channel_offset		= 0,//用于统计渠道日新增
	gf_login_offset              	= 0,//用于统计渠道日登陆人次/人数
	gf_register_mimi_offset     	= 0x0F500003,
	gf_role_type_distr				= 0x0F50000B,
	gf_report_lv_offset				= 0,//用于统计等级分布
	gf_report_child_lv_num			= 0,
	gf_report_dragon_exp			= 0,
	gf_report_user_role_exp			= 0,
	gf_report_user_vip_xvalue		= 0,

#endif
};

typedef uint32_t userid_t;

struct timer_head_t {
    list_head_t timer_list;
}__attribute__((packed));


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
	uint32_t domain;
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
	char 		sess[gf_sess_len];	
	uint32_t 	role_type;
	char    	nick[gf_nick_len];
	userid_t	parentid;
	userid_t	parent_role_tm;
	//uint32_t	color;
	uint32_t 	channel;
} __attribute__((packed)) role_info_t;

typedef struct clothes_info {
	uint32_t	clothes_id;
	uint32_t	get_time;
	uint32_t	duration;
	uint32_t	level;
} __attribute__((packed)) clothes_info_t;

typedef struct role_list_info {
	uint32_t	role_tm;
	uint32_t	role_type;
	uint32_t	del_tm;
	uint32_t	Ol_last;
	uint32_t	level;
	uint32_t	exp;
	char		nick[gf_nick_len];
	uint32_t	clothes_count;
	clothes_info_t	clothes_item[];
}__attribute__((packed)) role_list_info_t;

typedef struct role_list_header {
	uint32_t			role_tm;//no meaning
	uint32_t			forbit_flag;
	uint32_t	    	deadline;
	uint32_t			vip_is;
	uint32_t			x_value;
	uint32_t			vip_end_tm;
	uint32_t			role_count;
}__attribute__((packed)) role_list_header_t;

typedef struct LoginSession {
	uint32_t uid, ip, tm1, tm2;
} __attribute__((packed)) login_session_t;

typedef struct UserSession {
	uint8_t 	tmp[3000];
	uint32_t 	level;
} __attribute__((packed)) UserSession_t;
typedef struct {    
    char    multicast_interface[16];
    char    multicast_ip[16];   
    u_short multicast_port;

    char    gf_dbproxy_ip[16];
    u_short gf_dbproxy_port;

    char    gf_deluser_ip[16];
    u_short gf_deluser_port;

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
    char 		nick [gf_nick_len];
	userid_t	parentid;
	userid_t	parent_role_tm;
}__attribute__((packed));

typedef struct statistic_info {
	int count_unit; //always is 1, used to caculate total number of message log.
	uint32_t user_id;
	statistic_info () {
		count_unit = 1;
	}
} __attribute__((packed)) statistic_info_t;

struct  gf_gay_del_role_in{
	char 	session[16];
	uint32_t	role_tm;
	char		paypasswd[32];
}__attribute__((packed));

/*
struct warning_info_t {
	char 	 type_str[120];
	uint32_t fail_userid;
	uint32_t fail_cmdid;
	uint32_t is_hex_cmd;
	char 	 fail_ip[16];
	uint32_t recv_count_per_min_to_send;
	uint32_t send_interval;
	char     telephone_list[256];
}__attribute__((packed));
*/

#endif //GF_LIB_TYPE_H

