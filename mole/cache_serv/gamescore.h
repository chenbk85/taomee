/**
 * ====================================================================
 *  @file       gmscore.h
 *
 *  @brief      定义游戏分数命令相关的结构体，错误码与函数声明
 *
 *  platform   Debian 4.1.1-12 
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ====================================================================
 */


#ifndef  GMSCORE_INC
#define  GMSCORE_INC

extern "C"{
#include <stdint.h> 
}

#include "./pkg/Cclientproto.h"

/*排行榜中最大的用户数目*/
#define GMSC_FIRST_NUM 10 
/*最大的游戏号*/
//#define GMID_CLASS_NUM 50 
/*最大的游戏分数*/
#define GMID_MAX_SCOR 10000000 
/*昵称的最大长度*/
#define NICK_SIZE 16
/*session的长度*/
#define SESSION_SIZE 24


/*处理分数时的错误码*/
#define REG_SYS_ERR 2003

#define CMDID_NODIFINE_ERR 20015 

#define REG_SIZE_ERR 20001

#define USER_ID_ERR 20002

#define GAME_ID_ERR 20003

#define GAME_NOT_EXIST 20004

#define GET_USER_NICK_ERR 20005

#define SUBMIT_SCORE_ERR 20006

#define GAME_SCORE_ERR 20007

#define OUT_LAMU_STATE 20010

#define GS_CLIENT_IP_ERR 20016

#define GAME_SCORE_NONEED_SET_ERR 1402

#define GAME_SCORE_NOFIND_ERR 1401

/*从session中，得到游戏号*/
#define GAME_ID(session_id) (session_id&0x0000FFFF)
/*从session中，得到游戏的次序号*/
//#define GAME_INDEX(session_id) ((session_id&0xFFFF0000)>>16)

/**
 * @brief 存储游戏的ID号和表示游戏是按升序还是降序排序的flag
 */
typedef struct gmid_list {
	/*indicate the game score is sort ascending or not*/
	uint32_t flag; 
	uint32_t game_id;
} __attribute__((packed)) gmid_list, *gmid_ptr_t;


/**
 * @brief 存储用户ID，用户昵称和用户的游戏分数
 */
typedef struct gmsc {
	uint32_t user_id;
	char nick[NICK_SIZE];
	uint32_t score;
} __attribute__((packed)) gmsc_t, *gmsc_ptr_t, gmsc_list_t[GMSC_FIRST_NUM];


/**
 * @brief 每个游戏的存储结构 
 */
typedef struct gs_mem {
	uint32_t flag;
	uint32_t game_id;
	uint32_t playernum;
	gmsc_list_t game_score;
} __attribute__((packed)) gs_mem_t, *gs_mem_ptr_t;


/**
 * @brief 提交分数的请求结构
 */
typedef struct gmsc_submit_req {
	uint32_t game_id;
	uint32_t score;
	/*encrypted information*/
	char session[SESSION_SIZE];
} __attribute__((packed)) gmsc_submit_req_t, *gmsc_submit_req_ptr_t;


/**
 * @brief 获取分数的请求结构
 */
typedef struct gmsc_obtain_req {
	uint32_t game_id;
} __attribute__((packed)) gmsc_obtain_req_t, *gmsc_obtain_req_ptr_t;


/**
 * @brief 对获取分数的应答结构体
 */
typedef struct gmsc_obtain_resp {
	uint32_t game_id;
	uint32_t playernum;
	gmsc_list_t game_score;
} __attribute__((packed)) gmsc_obtain_resp_t, *gmsc_obtain_resp_ptr_t;

/**
 * @brief session相关的结构体
 */
typedef struct stu_gmsc_key {
	uint32_t tm1;
	uint32_t ip;
	uint32_t tm2;
	uint32_t score;
	uint32_t user_id;
	uint32_t game_id;
} __attribute__((packed)) stru_gmsc_key_t, *stru_gmsc_key_ptr_t;

/*提交分数的请求结构体长度*/
#define GMSC_SUBMIT_REQ_BODY_SIZE (sizeof(gmsc_submit_req_t))
/*提交分数的应答结构体的长度*/
#define GMSC_SUBMIT_RESP_BODY_SIZE 0
/*获取分数应答结构体的长度*/
#define GMSC_OBTAIN_RESP_SIZE (sizeof(gmsc_obtain_resp_t))
/*获取分数请求结构体的长度*/
#define GMSC_OBTAIN_REQ_SIZE (sizeof(gmsc_obtain_req_t))
/*存储单个分数结构体的长度*/
#define GMSC_LEN (sizeof(gmsc_t))
/*存储每个游戏的结构体的长度*/
#define GMMEM_LEN (sizeof(gs_mem_t))

int get_game_id(gmid_ptr_t gmid_array, char *gmid_file);

int init_game_score(Cclientproto *cpo, gmid_ptr_t gmid_array);

int gmsc_submit(char *recvbuf, int rcvlen, char *sendbuf, int *sndlen);

int gmsc_obtain(char *recvbuf, int rcvlen, char *sendbuf, int *sndlen);

int insert_game_score(uint32_t game_id, gmsc_ptr_t gmsc_ptr);

int get_insert_pos(uint32_t game_id, uint32_t score, int *pos);

int get_game_score_list(uint32_t game_id, gs_mem_ptr_t gmscptr);

#endif   /* ----- #ifndef GMSCORE_INC  ----- */

