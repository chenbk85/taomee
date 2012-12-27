// =====================================================================================
// 
//       Filename:  common.hpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  06/09/2010 05:38:10 PM CST
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  xcwen (xcwen), jim@taomee.com
//        Company:  TAOMEE
// 
// =====================================================================================

#ifndef  MOLE_COMMON_INC
#define  MOLE_COMMON_INC

#include "login_impl.hpp"

const uint32_t sess_len = 16;

const uint32_t nick_len = 16;

const uint32_t max_friend_num = 500;

const uint32_t mole_register_channel_offset = 0x020D0101;

const uint32_t mole_register_mimi_offset = 0x020D0001;

const uint32_t mole_login_offset = 0x020D0201;        

const uint32_t mole_not_normal_log_channel =  0x020D0220;

/*client cmd*/
enum {
	   PROTO_GET_RECOMMEND_SVR_LIST = 105, 
       PROTO_GET_RANGED_SVR_LIST = 106,
	   PROTO_CREATE_ROLE = 107,
	   PROTO_GET_VERSION = 110,
};

/*db cmd*/
enum {
	DB_ADD_GAME_FLAG = 0xA123,	
	DB_MOLE_REGISTER = 0x1109,
	DB_LOGIN_MOLE = 0X1008,
	DB_CHECK_SESSION = 0XA024,
};

/*error num*/
enum {
	SUCCESS = 0,
	SYSTEM_ERR = 5001,
	REGISTERED = 5002,
	NO_ACTIVED = 5004,
	USER_NOT_FIND = 5005,
	USER_BE_BAN = 5006,                              
	USER_BE_24_BAN = 5007,
	INVALID_NICK_NAME = 5010,
	SERVER_MAINTENANCE = 5011,
	USER_BE_7_DAY_BAN = 5012,
	USER_BE_14_DAY_BAN = 5013,
	USER_ID_EXISTED_ERR = 1104,
	USER_ID_NOFIND_ERR	= 1105,
	USER_NOUSED_ERR = 1120,
	USER_OFFLIN24_ERR = 1121,
	USER_NOT_ENABLED_ERR = 1107,
	USER_OFFLINE_SEVEN_DAY_ERR = 2541,
	USER_OFFLINE_FOURTEEN_DAY_ERR = 2542,
	USER_ANXI_SERVICE_OUT_OF = 2571,
	USER_CHECK_SESSION_ERR = 4331,
	USER_OUT_OF_ANXIN = 6001,
};

typedef struct role_info {
	char 		sess[sess_len];	
	char    	nick[nick_len];
	uint32_t	color;
	uint32_t    channel;
}__attribute__((packed)) role_info_t;

typedef struct LoginSession {
	uint32_t uid;
	uint32_t ip;
	uint32_t tm1;
	uint32_t tm2;
}__attribute__((packed)) login_session_t;

struct  mole_register_in {
	uint32_t color;
    char nick [nick_len];
}__attribute__((packed));

typedef struct mole_login_out {
	uint32_t    vip_flag;
	uint32_t    friendcount;
	uint32_t    frienditem[max_friend_num];
}__attribute__((packed)) mole_login_out_t;

typedef struct statistic_info {
	int count_unit; //always is 1, used to caculate total number of message log.
	uint32_t user_id;
	statistic_info () {
		count_unit = 1;
	}
} statistic_info_t;

#define CHECK_BODY_LEN(len_, s_)\
	if((len_)!=(s_)) ERROR_RETURN(("error recvlen=%d expectedlen=%d", int(len_), int(s_)), -1)

#define CHECK_VAL_GE(val_, min_) \
	if ( (val_) < (min_) ) ERROR_RETURN(("val %d is less than min %d", int(val_), int(min_)), -1)

void clear_usr_info(usr_info_t* p);

int unpkg_auth(const uint8_t body[], int len, login_session_t* sess);

int verify_session(const userid_t uid, const login_session_t* sess);

void init_proto_head(void* buf, uint32_t len, uint32_t cmd, userid_t uid, uint32_t result);

int send_to_self(usr_info_t* p, const uint8_t *buf, uint32_t length);
#endif   // ----- #ifndef COMMON_INC  ----- 

