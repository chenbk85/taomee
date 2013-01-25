#ifndef GF_LIB_TYPE_H
#define GF_LIB_TYPE_H

#include "util.h"

//default user info
#define MOLE2_USER_DEFAULT_NICK			""		
#define MOLE2_USER_DEFAULT_COLOR		6737792
#define MOLE2_USER_DEFAULT_PROFESSION	0
#define	MOLE2_USER_DEFAULT_SIGNATURE	""

//login by email
enum const_def_t {
	mole2_svr_usr_counter 	= 60000,
	
//define the error num
	mole2_success         	= 0,
	mole2_close_conn      	= -1,

	
	mole2_max_friend_num  	= 200,
	mole2_max_role_num    	= 1,
	mole2_min_backup_svr  	= 1,
	mole2_max_backup_svr  	= 100,
	
//svr list range
	mole2_min_valid_svr   	= 1,
	mole2_max_valid_svr   	= 2000,
//user attribute argument
/*
	mole2_attribute_value_min 	= 0,
	mole2_attribute_value_max 	= 15,
	mole2_attribute_num			= 5,
	mole2_attribute_sum			= 30,
//user_crystal argument
	mole2_crystal_type_min		= 120001,
	mole2_crystal_type_max		= 120040,
*/
	mole2_game_flag			= 7,

	mole2_del_flag			= 0,

	mole2_timestamp_get_friendtime = 8,
};

enum {
	mole2_sess_len		= 16,
	mole2_nick_len		= 16,
	mole2_activation_len	= 16,
};

//pkg len limit
enum len_limit_t {
	mole2_max_len 		= 8192,
	mole2_buffer_size 	= 8192,
};

//cli protocol cmd
enum cli_cmd_t {
	proto_get_recommend_svr_list = 105,
	proto_get_ranged_svr_list    = 106,	
	proto_create_role            = 107,
	proto_activation			 = 108,
	proto_version				= 4096,
};

enum db_cmd_t {
	db_base_session_check			= 0xA024,
	db_base_check_fan				= 0x0060,//not used  replace by 009B
	db_base_check_key				= 0x0097,//not used  replace by 009B
	db_base_set_game_flag_v1		= 0x0108,//not used  replace by 019A
	db_base_set_game_flag_v2 	= 	0xA123,
	db_base_get_create_able_flags = 0x009B,

	db_mole2_login					= 0x081A,
	db_mole2_role_register		= 0x0902,
	db_mole2_get_mole_info		= 0x30AA,
	db_mole2_activation			= 0xD524,
};

enum switch_cmd_t {
	mole2_switch_get_recommend_svr_list		= 62006,
	mole2_switch_get_ranged_svr_list		= 64002,
};

enum outher_cmd_t {
	mole2_multicast_cmd		= 61003,
	 	
};

//error number for client
enum err_client_t { 
	mole2_system_err					= 5001,    
	mole2_user_no_actived				= 5004,
	mole2_user_nofind					= 5005,
	mole2_user_be_ban					= 5013,
	mole2_user_be_24_ban				= 5014,
	mole2_user_be_7_day_ban			= 5015,
	mole2_user_be_14_day_ban			= 5016,
	mole2_user_out_of_anxi			= 6001,

	mole2_invalid_crystal				= 6012,
	mole2_invalid_attribute				= 6013,
	mole2_invalid_nick 					= 6014,
	mole2_user_existed					= 6015,
	mole2_nokey_err						= 6016,
	mole2_activation_err				= 6017,
	mole2_mfan_err						= 6018,
	mole2_normal_err					= 100000,
};

//db return error number
enum err_db_t {
//在select update, delete 时，米米号不存在
	mole2_key_existed_err						= 1014,
	mole2_user_id_nofind_err           		= 1105,
	mole2_user_noused_err						= 1120,
	mole2_user_offlin24_err					= 1121,
	mole2_user_not_enabled_err				= 1107,
	mole2_user_offline_seven_day_err		= 2542,
	mole2_user_offline_fourteen_day_err		= 2541,
	mole2_user_anxi_service_out_of_err		= 2571,

	mole2_user_role_existed_err				= 1104,

	dberr_session_check						= 4331,

};

//statistic start offset 
enum static_log_t {
	mole2_login_offset              	= 0x020D0201,
	mole2_register_mimi_offset     		= 0x020D0001,
	mole2_register_channel_offset   	= 0x020D0101,
	mole2_not_normal_reg_channel    	= 0x020D01FF,
	mole2_not_normal_log_channel    	= 0x020D0220,
};

typedef uint32_t userid_t;



struct timer_head_t {
    list_head_t timer_list;
} __attribute__((packed));


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

typedef struct clothes_info {
	uint32_t	clothes_id;
	uint32_t	get_time;
} __attribute__((packed)) clothes_info_t;

typedef struct role_list_info {
	uint32_t	role_tm;
	uint32_t	role_type;
	uint32_t	level;
	char		nick[mole2_nick_len];
	uint32_t	clothes_count;
	clothes_info_t	clothes_item[];
}__attribute__((packed)) role_list_info_t;

typedef struct role_list_header {
	uint32_t			role_tm;//no meaning
	uint32_t			role_count;
}__attribute__((packed)) role_list_header_t;

typedef struct LoginSession {
	uint32_t uid, ip, tm1, tm2;
} __attribute__((packed)) login_session_t;

typedef struct {    
	char    master_ser[32];
	u_short dx_or_wt;
}my_option_t;

struct mole2_login_out{
	uint32_t	vip_flag;
	uint32_t	last_online;
	uint32_t	friendcount;
	uint32_t 	blackcount;
	uint32_t	frienditem[];
}__attribute__((packed));

struct  mole2_register_in{
	userid_t	parent;
	char		nick[mole2_nick_len];
	char		signature[2 * mole2_nick_len];
	uint32_t	color;
	uint8_t	profession;
}__attribute__((packed));

typedef struct statistic_info {
	int count_unit; //always is 1, used to caculate total number of message log.
	uint32_t user_id;
	statistic_info () {
		count_unit = 1;
	}
} __attribute__((packed)) statistic_info_t;

#define UID_DESKEY "HtoE@$#&"

typedef struct{
	userid_t parent;
	char	 magic[4];
} __attribute__((packed)) des_data_t;

#endif //GF_LIB_TYPE_H

