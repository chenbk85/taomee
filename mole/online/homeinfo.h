/*
 *@ home information, and protocal between Online and Home server
 */

#ifndef HOMEINFO_H
#define HOMEINFO_H

#include "benchapi.h"
#include "dbproxy.h"

#define MAX_NUM_NEIGHBOR 	200
#define TOP_LIST_NUM	 	10
#define RECENT_VISITOR_NUM	50

typedef struct home_hot_info_res
{
    uint32_t hot;
    uint32_t flower;
    uint32_t mud;
}__attribute__(( packed ))home_hot_info_t;

typedef struct top_home
{
    uint32_t mapid;
    uint32_t hot;
    uint8_t  nick[16];
}__attribute__((packed))top_home_t;

typedef struct top_list_res
{
    uint32_t 		cnt;
    top_home_t 	tophome[TOP_LIST_NUM];
}__attribute__((packed))top_list_res_t;

typedef struct vote_res
{
    uint32_t vote_ret; // 0 failure otherwise return the number of flower or mud
}__attribute__((packed))vote_res_t;

typedef struct visitor_entry
{
    uint32_t userid;
    uint8_t  nick[16];
	uint32_t color;
	uint8_t	 isvip;
    uint32_t stamp;
}__attribute__((packed))visitor_entry_t;

typedef struct recent_visitor_list_res
{
    uint32_t cnt;
    visitor_entry_t visitor[RECENT_VISITOR_NUM];
}__attribute__((packed))recent_visitor_list_res_t;

typedef struct op_mm_tree {
	uint32_t	type;  // 1 for maomao tree,  2 for smc desk, 3 for fu
	userid_t   	userid;
	uint32_t 	op_type;
	char	 	nick[USER_NICK_LEN];
	uint32_t  	color;
	uint8_t 	isvip;
	uint32_t	op_stamp;
}__attribute__((packed)) op_mm_tree_t;

typedef struct usr_list {
	userid_t   	userid;
	uint32_t 	op_type;
	char	 	nick[USER_NICK_LEN];
	uint32_t  	color;
	uint8_t 	isvip;
	uint32_t	op_stamp;
}__attribute__((packed)) usr_list_t;

int get_home_hot_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_top_hot_homes_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int vote_home_flower_mud_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_recent_visitors_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_neighbors_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int add_neighbors_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_home_hot_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_top_hot_homes_callback(sprite_t* p, uint32_t id, char* buf, int len);
int vote_home_flower_mud_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_recent_visitors_callback(sprite_t* p, uint32_t id, char* buf, int len);
int add_neighbors_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_neighbors_callback(sprite_t* p, uint32_t id, char* buf, int len);
int submit_message_cmd(sprite_t *p, uint8_t *body, int bodylen);
int verify_message_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_messages_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int del_message_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int submit_message_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_message_callback(sprite_t* p, uint32_t id, char* buf, int len);
int reply_message_cmd(sprite_t *p, uint8_t *body, int bodylen);
int get_vip_home_list_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_maomao_tree_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int operate_home_type_obj_callback(sprite_t* p, uint32_t id, char* buf, int len);
int list_operated_callback(sprite_t* p, uint32_t id, char* buf, int len);
int fertilize_water_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_maomao_tree_info_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int list_fertilized_watered_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int pick_fruit_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_home_type_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_home_type_list_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int vote_smc_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_smc_info_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_info_by_type_callback(sprite_t* p, uint32_t id, char* buf, int len);
int grow_profession(sprite_t *p, uint32_t itemid, uint32_t count);
uint32_t cal_dietitian_level_by_exp(uint32_t dietitian_exp);


#define db_get_home_hot(p_, body_, l, mapid) \
		send_request_to_db(SVR_PROTO_GET_HOT, (p_), l, (body_), mapid)
#define db_list_top_hot_homes(p_, body_, l) \
		send_request_to_db(SVR_PROTO_LIST_TOP_HOT, (p_), l, (body_), (p_)->id)
#define db_list_recent_visitors(p_, body_, l, mapid) \
		send_request_to_db(SVR_PROTO_LIST_RECENT_VISITOR, (p_), l, (body_), mapid)
#define db_list_neighbors(p_, body_, l) \
		send_request_to_db(SVR_PROTO_LIST_NEIGHBOR, (p_), l, (body_), (p_)->id)
#define db_clean_mud(p_, body_, l, mapid) \
		send_request_to_db(SVR_PROTO_CLEAN_MUD, (p_), l, (body_), mapid)
#define db_clean_one_mud(p_, mapid) \
		send_request_to_db(SVR_PROTO_CLEAN_ONE_MUD, (p_), 0, NULL, mapid)

#define db_operate_home_type_obj(p_, buf, id) \
		send_request_to_db(SVR_PROTO_OPERATE_HOME_TYPE_OBJ, p_, sizeof(op_mm_tree_t), buf, id)
#define db_get_info_by_type(p_, buf, uid) \
		send_request_to_db(SVR_PROTO_GET_INFO_BY_TYPE, p_, 4, buf, (uid))
#define db_list_operated(p_, buf) \
		send_request_to_db(SVR_PROTO_LIST_OPERATED, p_, 4, buf, (p_)->id)

static inline int db_vote_home(sprite_t* p, uint32_t vote_flag, uint32_t usrid)
{
	return send_request_to_db(SVR_PROTO_VOTE_HOME, p, 4, &vote_flag, usrid);
}

static inline int db_add_neighbors(sprite_t *p, uint8_t flag, uint16_t cnt, uint32_t *userid)
{
	uint8_t buf[MAX_NUM_NEIGHBOR*4+3];

	uint16_t* num = (uint16_t*)(buf+1);
	uint32_t* id = (uint32_t*)(buf+3);
	buf[0] = flag;
	*num = cnt;
	memcpy(id, userid, cnt*4);
	return send_request_to_db(SVR_PROTO_ADD_NEIGHBOR, p, 3+cnt*4, &buf, p->id);
}

static inline int db_add_hot(sprite_t *p, uint32_t usrid)
{
	return send_request_to_db(SVR_PROTO_ADD_HOT, 0, 4, &(p->id), usrid);
}

static inline int db_leave_message(sprite_t *p, uint32_t mapid, const uint8_t* content)
{
	char buf[200 + 24];
	int i = 0;

	PKG_H_UINT32 (buf, p->id, i);
	PKG_STR (buf, p->nick, i, 16);
	PKG_H_UINT32 (buf, get_now_tv()->tv_sec, i);
	strcpy(buf + i, (const char*)content);
//	PKG_STR (buf, content, i, 200);

	return send_request_to_db(SVR_PROTO_LEAVE_HOME_MSG, p, sizeof(buf), buf, mapid);
}

static inline int db_verify_message(sprite_t *p, uint32_t msgid, uint8_t flag)
{
	char buf[5];
	int i = 0;

	PKG_H_UINT32 (buf, msgid, i);
	buf[4] = flag;
	return send_request_to_db(SVR_PROTO_VERIFY_HOME_MSG, 0, sizeof(buf), &buf, p->id);
}

static inline int db_reply_message(sprite_t *p, uint32_t msgid, const uint8_t* content)
{
	char buf[100 + 4];
	int i = 0;

	PKG_H_UINT32 (buf, msgid, i);
	strcpy(buf + i, (const char*)content);
//	PKG_STR (buf, content, i, 200);

	return send_request_to_db(SVR_PROTO_REPLY_HOME_MSG, 0, sizeof(buf), buf, p->id);
}

static inline int db_get_messages(sprite_t *p, uint32_t mapid, uint32_t pos)
{
	char buf[8];
	int i = 0;

	PKG_H_UINT32 (buf, p->id, i);
	PKG_H_UINT32 (buf, pos, i);

	return send_request_to_db(SVR_PROTO_GET_HOME_MSG, p, i, buf, mapid);
}

static inline int db_del_message(sprite_t *p, uint32_t msgid, uint32_t flag)
{
	char buf[8];
	int i = 0;

	PKG_H_UINT32 (buf, msgid, i);
	PKG_H_UINT32 (buf, flag, i);
	return send_request_to_db(SVR_PROTO_DEL_HOME_MSG, 0, i, buf, p->id);
}

#endif

