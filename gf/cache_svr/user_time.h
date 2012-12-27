/**
 * ====================================================================
 *  @file       user_time.h
 *
 *  @brief      定义时间戳命令相关的结构体和错误码
 *
 *  platform   Debian 4.1.1-12 
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ====================================================================
 */

#ifndef  TIME_INC
#define  TIME_INC

extern "C" {
#include <stdint.h>
}

/**
 * @brief 对更新时间的应答
 */
typedef struct update_time_resp {
	uint32_t time;
} __attribute__((packed)) update_time_resp_t;

/*设置时间*/
//nick_jy_lamu
struct pp_nick {
	uint32_t nick_time;
}__attribute__((packed));


struct get_info_out {
	uint32_t jy_time;
	uint32_t lamu_state;
}__attribute__((packed));

/**
 * @brief 对获取时间的应答
 */
typedef struct obtain_time_resp {
	uint32_t user_id;
	uint32_t time;
} __attribute__((packed)) obtain_time_resp_t;

/*更新时间应答结构体的长度*/
#define UPDATE_TIME_RESP_SIZE sizeof(update_time_resp_t)
/*获取时间应答结构体的长度*/
#define OBTAIN_TIME_RESP_SIZE sizeof(obtain_time_resp_t)
/*表示用户数目的变量占用的字节数*/
#define USER_ID_CNT_SIZE 4
/*每个用户ID占用的字节数*/
#define USER_ID_SIZE	4

//
#define REG_SIZE_ERR 20001
#define USER_ID_ERR 20002

/*处理时间请求时的错误码*/
#define CSYS_ERR -10000

#define CCHECK_PASSWD_ERR -10001

#define CUSER_ID_NOACTIVE_ERR -10002

#define CDB_ERR -10003

#define CUSER_ID_NOFIND_ERR -10004

#define CPROTO_LEN_ERR  -10005

#define CUSER_ID_CREATE_ERR -10006


int time_stamp_update(char *recbuf, int rcvlen, char *sendbuf, int *sndlen);

int time_stamp_obtain(char *recbuf, int rcvlen, char *sendbuf, int *sndlen);


#endif   /* ----- #ifndef TIME_INC  ----- */

