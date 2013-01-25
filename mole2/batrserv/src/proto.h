
#ifndef BT_PROTO_H
#define BT_PROTO_H

#include <stdint.h>
#include "onlineapi.h"

enum {
	bt_proto_max_len	= 8 * 1024,
};

enum BroadcasterCmd {
	battle_info_msg			= 61006,
};

//
enum SpriteOpConst {
	/*发起挑战*/
	proto_bt_challenge_battle		= 1300,
	/*接受挑战*/
	proto_bt_accept_battle			= 1301,
	/*挑战npc*/
	proto_bt_challenge_npc_battle	= 1304,
	/*通知客户段对战已经创建*/
	proto_bt_noti_battle_created	= 1305,
	/*客户端上报加载进度*/
	proto_bt_load_battle			= 1306,
	/*通知客户段对战开始*/
	proto_bt_noti_battle_started	= 1307,
	/*客户段发招*/
	proto_bt_battle_attack			= 1308,
	/*使用药瓶*/
	proto_bt_use_chemical			= 1309,
	/*返回客户段打斗行动数据包*/
	proto_bt_battle_attacks_info	= 1310,
	/*同步玩家信息*/
	proto_bt_battle_round_result	= 1311,
	/*广播用户发招完毕*/
	proto_bt_noti_user_fighted		= 1313,
	/*用户被打飞*/
	proto_bt_noti_user_out			= 1315,
	/*广播用户加载状态*/
	proto_bt_noti_load_rate         = 1316,
	/*客户段初始化成功*/
	proto_bt_load_ready  			= 1317,
	/*对战结束*/
	proto_bt_battle_end				= 1318,

	/*拒绝接受pk*/
	proto_bt_reject_challenge		= 1320,
	/*取消对战*/
	proto_bt_cancel_challenge		= 1336,
	/*换位*/
	proto_bt_change_pos             = 1337,
	/*抓宠*/
	proto_bt_catch_pet              = 1338,
	/*换宠*/
	proto_bt_recall_pet             = 1339,

	
	proto_bt_max
};

typedef uint64_t battle_id_t;

// For batrserv
typedef struct batrserv_proto {
	uint32_t	len;
	uint32_t	online_id;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	battle_id_t challenger; 
	uint32_t	opid;
	uint8_t		body[];
} __attribute__((packed)) batrserv_proto_t;


/* Define the communication protocol between the switch and online 
   server(or adminer server, login server) */
typedef struct svr_proto {
        /* package head */
        uint32_t	len; // package length, no more than 4k bytes
        uint32_t	seq; // must be 0
        uint16_t	cmd; // cmd id, >OxEA60
        uint32_t	ret; // Result, must be 0
        uint32_t	id; // Userid
        /* package body, variable length */
        uint8_t		body[];
} __attribute__((packed))svr_proto_t;

typedef struct protocol {
	uint32_t	len;
	uint16_t	cmd;
	userid_t	id;
	uint32_t	seq;
	uint32_t	ret;
	uint8_t		body[];
} __attribute__((packed)) protocol_t;


static inline void
init_proto_head(void* buf, int cmd, int len)
{
#ifdef __cplusplus
	protocol_t* p = reinterpret_cast<protocol_t*>(buf);
#else
	protocol_t* p = buf;
#endif
	p->len = htonl(len);
	p->cmd = htons(cmd);
	p->ret = 0;
}

#endif

