/*
 * =====================================================================================
 * 
 *       Filename:  logproto.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月30日 15时47分08秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  LOGPROTO_INC
#define  LOGPROTO_INC
#include "common.h"
#include "sys/types.h"
#include "inttypes.h"
#include "string.h"
#define  PROTOBUF_MAXLEN  500

//define cmd id
#define  LOG_LOGIN_CMD 			0x1101 
#define  LOG_USER_REG_CMD 		0x1102 
#define  LOG_ONLINE_USERCOUNT_CMD 		0x1103 
#define  LOG_XIAOMEE_USE_CMD 		0x1104 
#define  LOG_DAY_COUNT_CMD 		0x1105 
#define  LOG_USER_ON_OFF_LINE_CMD 		0x1106 

#define LOG_PROTO_HEADER_SIZE  (sizeof (LOG_PROTO_HEADER)) 

typedef struct log_proto_header{
	uint32_t proto_length; 
	uint16_t cmd_id; 
} __attribute__((packed)) LOG_PROTO_HEADER;

typedef struct tlogin{
	uint32_t logtime;  
	uint32_t userid; 
	uint8_t  usertype; 
	uint32_t logintime; 
	uint32_t onlinetime; /*second*/
	uint32_t addr_type;  
	uint32_t ip;  
} __attribute__((packed)) TLOGIN;

typedef struct tuserinfo{
	char logtime[20];
	uint8_t usertype;
	uint32_t usercount;
	uint32_t addusercount;
	uint32_t logincount;
	uint32_t logincountindependent;
	uint32_t maxonlinecount ;
	uint32_t onlinetimetotal;
	uint32_t arpu;
	uint32_t logincountonlinetimelevel1;
	uint32_t logincountonlinetimelevel2;
	uint32_t logincountonlinetimelevel3;
	uint32_t logincountonlinetimelevel4;
	uint32_t logincountonlinetimelevel5;
	uint32_t logincountonlinetimelevel6;
} __attribute__((packed))  TUSERINFO;

typedef struct tpetinfo{
	char logtime[20];
	uint32_t apple;
	uint32_t watermelon;
	uint32_t strawberry;
	uint32_t banana;
	uint32_t pear;
	uint32_t peach;
	uint32_t addapple;
	uint32_t addwatermelon;
	uint32_t addstrawberry;
	uint32_t addbanana;
	uint32_t addpear;
	uint32_t addpeach;
} __attribute__((packed))  TPETINFO ;


typedef struct tpetlevelinfo{
	char logtime[20];
	uint8_t level;
	uint32_t apple;
	uint32_t watermelon;
	uint32_t strawberry;
	uint32_t banana;
	uint32_t pear;
	uint32_t peach;
} __attribute__((packed))  TPETLEVELINFO ;

typedef struct tday_count{
	uint32_t type;
	uint32_t date;
	uint32_t count;
} __attribute__((packed))  TDAY_COUNT;



// update grade  
// register : 
// petgrade =0 

typedef struct tuser_reg{
	uint32_t logtime;  
	uint32_t userid;
	uint32_t reg_addr_type;  
	uint32_t ip;  
} __attribute__((packed))  TUSER_REG;

//online usercount                 
typedef struct tonlineusercount{
	uint32_t logtime;  
	uint32_t serverid;  
	uint32_t onlinecount;  
} __attribute__((packed))   TONLINE_USERCOUNT;

typedef struct txiaomee_use{
	uint32_t logtime;  
	uint32_t usecount;  
	uint32_t  reason;  
	uint32_t  reason_ex;  
} __attribute__((packed))   TXIAOMEE_USE;


typedef struct tuser_on_off_line{
	uint32_t flag;  /*3:on ;4:off*/
	uint32_t logtime;  
	uint32_t userid;
} __attribute__((packed))  TUSER_ON_OFF_LINE ;



int write_proto_log (char * buf);

#define f_save_protolog(t_struct) \
inline void SAVE_PROTOLOG_##t_struct (T##t_struct &value ){ \
    char _proto_buf[PROTOBUF_MAXLEN]; \
    ((LOG_PROTO_HEADER*)_proto_buf)->cmd_id=LOG_##t_struct##_CMD; \
    ((LOG_PROTO_HEADER*)_proto_buf)->proto_length=LOG_PROTO_HEADER_SIZE + sizeof(value);\
    memcpy(_proto_buf+LOG_PROTO_HEADER_SIZE,&value,sizeof(value) ); \
    write_proto_log(_proto_buf);\
}

//以下定义相关调用接口
f_save_protolog(LOGIN ); //use it like: SAVE_PROTOLOG_LOGIN 
f_save_protolog(USER_REG);
f_save_protolog(ONLINE_USERCOUNT );
f_save_protolog(XIAOMEE_USE );
f_save_protolog(DAY_COUNT);
f_save_protolog(USER_ON_OFF_LINE);



#endif   /* ----- #ifndef LOGPROTO_INC  ----- */
