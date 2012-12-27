#include <statistic_agent/msglog.h>

#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "tasks.h"
#include "mole_class.h"

#include "homeinfo.h"

typedef struct msg_item {
	uint32_t 	msgid;
	userid_t	userid;
	char		nick[USER_NICK_LEN];
	uint32_t		submit_time;
	uint32_t		verify_time;
	char		content[200];
	char		reply[100];
}__attribute__((packed)) msg_item_t;

typedef struct msg_list {
	uint32_t 	total_num;
	uint32_t 	msg_num;
	msg_item_t 	msg_itm[];
}__attribute__((packed)) msg_list_t;

#define db_get_mm_tree_info(p_, id) \
		send_request_to_db(SVR_PROTO_GET_MM_TREE_INFO, p_, 0, NULL, id)
#define db_pick_one_mm_tree_fruit(p_) \
		send_request_to_db(SVR_PROTO_PICK_MM_TREE_FRUIT, p_, 0, NULL, (p_)->id)
#define db_get_home_type_list(p_, mapid) \
		send_request_to_db(SVR_PROTO_GET_HOME_TYPE_LIST, p_, 0, NULL, (mapid))


/*
 *@ get home hot
 */
int get_home_hot_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	int j = 0;
	uint32_t mapid;
	UNPKG_UINT32(body, mapid, j);

	return db_get_home_hot(p, NULL, 0, mapid);
}

/*
 *@ get home hot from db, handle db 's reply
 */
int get_home_hot_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(home_hot_info_t));

	int i = sizeof(protocol_t);
	home_hot_info_t* ret = (void*)buf;

	PKG_UINT32(msg, id, i);
	PKG_UINT32(msg, ret->hot, i);
	PKG_UINT32(msg, ret->flower, i);
	PKG_UINT32(msg, ret->mud, i);

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/*
 *@ get the top 10 hot homes
 */
int get_top_hot_homes_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	return db_list_top_hot_homes(p, NULL, 0);
}

/*
 *@ get the top 10 hot homes from db,handle db 's reply
 */
int get_top_hot_homes_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int l = TOP_LIST_NUM * sizeof(top_home_t) + 2;
	CHECK_BODY_LEN(len, l);

	int i = sizeof(protocol_t);
	int loop = 0;
	uint16_t num = *(uint16_t*)buf;
	CHECK_BODY_LEN(num, TOP_LIST_NUM);

	top_home_t* tp = (top_home_t*)(buf + 2);
	PKG_UINT16(msg, num, i);

	for(loop = 0; loop < num; loop++, tp++){
		PKG_UINT32(msg, tp->mapid, i);
		PKG_UINT32(msg, tp->hot, i);
		PKG_STR(msg, tp->nick, i, 16);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/*
 *@ vote flower or mud
 */
int vote_home_flower_mud_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	enum{
		MAX_FLOWER_MUD_NUM	= 200,
		VOTE_EXCLU_ID		= 101
	};
	uint32_t mapid;
	uint32_t vote_flag;
	if (unpkg_uint32_uint32(body, bodylen, &mapid, &vote_flag) == -1)
		return -1;

	if(p->id == mapid){
		ERROR_RETURN(("%u,error can't vote yourself", p->id), -1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session, mapid, p->sess_len);
	PKG_H_UINT32(p->session, vote_flag, p->sess_len);

	return db_set_sth_done(p, VOTE_EXCLU_ID, MAX_FLOWER_MUD_NUM, p->id);
}

/*
 *@ vote someone's home, handle db 's reply
 */
int vote_home_flower_mud_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int j = 0;
	int mapid, vote_flag;

	UNPKG_H_UINT32(p->session, mapid, j);
	UNPKG_H_UINT32(p->session, vote_flag, j);
	p->sess_len = 0;
	db_vote_home(0, vote_flag, mapid);

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/*
 *@ list recent 50 visitors
 */
int get_recent_visitors_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	int j = 0;
	uint32_t mapid = 0;
	UNPKG_UINT32(body, mapid, j);

	return db_list_recent_visitors(p, NULL, 0, mapid);
}

/*
 *@ get recent visitors information from db, handle db's reply
 */
int get_recent_visitors_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 2);

	uint16_t num = *(uint16_t*)buf;
	CHECK_BODY_LEN(len, num*sizeof(visitor_entry_t)+2);
	int loop = 0;
	visitor_entry_t* vp = (visitor_entry_t*)(buf+2);

	int i = sizeof(protocol_t);
	PKG_UINT16(msg, num, i);


	for(loop = 0; loop < num; loop++, vp++){
		PKG_UINT32(msg, vp->userid, i);
		PKG_STR(msg, vp->nick, i, 16);
		PKG_UINT32(msg, vp->color, i);
		PKG_UINT8(msg, vp->isvip, i);
		PKG_UINT32(msg, vp->stamp, i);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/*
 *@ list neighbors,handle client's command
 */
int get_neighbors_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	return db_list_neighbors(p, NULL, 0);
}

/*
 *@ get neighbors information from db
 */
int get_neighbors_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 2);

	uint16_t num = *(uint16_t*)buf;
	CHECK_BODY_LEN(len, num*sizeof(userid_t)+2);
	int loop = 0;
	userid_t* up = (userid_t*)(buf+2);

	int i = sizeof(protocol_t);
	PKG_UINT16(msg, num, i);


	for(loop = 0; loop < num; loop++, up++){
		PKG_UINT32(msg, *up, i);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/*
 *@ add or del several neighbors
 */
int add_neighbors_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 3);

	int i = 1;
	uint16_t count, loop;
	uint8_t flag = -1;
	uint32_t buf[MAX_NUM_NEIGHBOR];

	flag = *(uint8_t*)body;
	UNPKG_UINT16(body, count, i);
	if(0 != flag && 1 != flag){
		ERROR_RETURN(("flag %c is not 0 or not 1", flag), -1);
	}
	CHECK_INT_LE(count, MAX_NUM_NEIGHBOR);
	CHECK_BODY_LEN(bodylen, count*sizeof(userid_t)+2);

	for(loop = 0; loop < count; loop++){
		UNPKG_UINT32(body, buf[loop], i);
	}

	return db_add_neighbors(p, flag, count, buf);
}

/*
 *@ add or del several neighbors from db,handle db 's reply
 */
int add_neighbors_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 0);

	int i = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/*
 *@ leave message for somebody
 */
int submit_message_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 8+2);
	uint32_t mapid;
	uint32_t msglen;
	int i = 0;

	UNPKG_UINT32(body, mapid, i);
	UNPKG_UINT32(body, msglen, i);

	CHECK_BODY_LEN(bodylen - 8, msglen);
	CHECK_INT_LE(msglen, 200);

	uint8_t* content = body + 8;
	content[msglen - 1] = '\0';

	CHECK_DIRTYWORD(p, content);

	DEBUG_LOG("LEAVE MSG\t[%u %u]", p->id, mapid);
	if (IS_CLASS_MAP(p->tiles->id)) {
		return db_class_leave_message(p, mapid, content);
	} else {
		return db_leave_message(p, mapid, content);
	}
}

/*
 *@ leave message for sb, process db's reply
 */
int submit_message_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/*
 *@ verify mesages
 */
int verify_message_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t msgid;
	uint8_t flag;

	if (unpkg_uint32_uint8(body, bodylen, &msgid, &flag) == -1)
		return -1;
	if (flag != 0 && flag != 1)
		ERROR_LOG("verify message flag not correct, userid=%u flag=%c", p->id, flag);

	DEBUG_LOG("VERIFY MSG\t[%u %u %u]", p->id, msgid, flag);

	if (IS_CLASS_MAP(p->tiles->id)) {
		db_class_verify_message(p, msgid, flag);
	} else {
		db_verify_message(p, msgid, flag);
	}

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/*
 *@ leave message for somebody
 */
int reply_message_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 8+2);
	uint32_t msgid;
	uint32_t msglen;
	int i = 0;

	UNPKG_UINT32(body, msgid, i);
	UNPKG_UINT32(body, msglen, i);

	CHECK_BODY_LEN(bodylen - 8, msglen);
	CHECK_INT_LE(msglen, 100);

	uint8_t* content = body + 8;
	content[msglen - 1] = '\0';

	CHECK_DIRTYWORD(p, content);

	DEBUG_LOG("REPLY MSG\t[uid=%u msgid=%u]", p->id, msgid);
	if (IS_CLASS_MAP(p->tiles->id)) {
		db_class_reply_message(p, msgid, content);
	} else {
		db_reply_message(p, msgid, content);
	}

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/*
 *@ get all messages
 */
int get_messages_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t mapid;
	uint32_t pos;

	if (unpkg_uint32_uint32(body, bodylen, &mapid, &pos) == -1)
		return -1;

	//DEBUG_LOG("GET MSG\t[%u %u]", p->id, mapid);
	if (IS_CLASS_MAP(p->tiles->id)) {
		return db_class_get_messages(p, mapid, pos);
	} else {
		return db_get_messages(p, mapid, pos);
	}
}

/*
 *@ get all messages, process db's reply
 */
int get_message_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(msg_list_t));

	msg_list_t* msglist = (void*)buf;

	CHECK_BODY_LEN (len - sizeof(msg_list_t), sizeof(msg_item_t) * msglist->msg_num);
	if (msglist->msg_num > 6)
		ERROR_RETURN (("error id=%u, msg_num=%d", p->id, msglist->msg_num), -1);

	//DEBUG_LOG("DB SEND MSG\t[%u %u]", p->id, msglist->msg_num);
	int loop;
	for (loop = 0; loop < msglist->msg_num; loop++) {
		msglist->msg_itm[loop].msgid        = htonl(msglist->msg_itm[loop].msgid);
		msglist->msg_itm[loop].userid       = htonl(msglist->msg_itm[loop].userid);
		msglist->msg_itm[loop].submit_time  = htonl(msglist->msg_itm[loop].submit_time);
		msglist->msg_itm[loop].verify_time  = htonl(msglist->msg_itm[loop].verify_time);
	}

	msglist->total_num = htonl(msglist->total_num);
	msglist->msg_num   = htonl(msglist->msg_num);

	response_proto_str (p, p->waitcmd, len, buf, 0);
	return 0;
}

/*
 *@ delete some message if you don't like
 */
int del_message_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t msgid;
	uint32_t flag;
	int i = 0;

	UNPKG_UINT32(body, msgid, i);
	UNPKG_UINT32(body, flag, i);
	DEBUG_LOG("DEL MSG\t[%u %u %u]", p->id, msgid, flag);

	if (IS_CLASS_MAP(p->tiles->id)) {
		db_class_del_message(p, msgid, flag);
	} else {
		db_del_message(p, msgid, flag);
	}

	response_proto_head(p, p->waitcmd, 0);
	return 0;

}

/*
 *@ get vip's home information
 */
static void pkg_vip_home_list(void* key, void* spri, void* userdata)
{
	map_t*    map;
	sprite_t* p    = spri;
	int*      idx  = userdata;
	int i = sizeof (protocol_t) + *idx * 24 + 4;
	if (IS_GUEST_ID(p->id) || IS_NPC_ID(p->id)
		|| (p->flag & SPRITE_HOME_PUBLIC_FLAG)
		|| !ISVIP(p->flag))
		return;

	PKG_UINT32(msg, p->id, i);
	PKG_STR(msg, p->nick, i, 16);
	if ( !(map = get_map(p->id)) ) {
		PKG_UINT32(msg, 0, i);
	} else {
		PKG_UINT32(msg, map->sprite_num + get_across_svr_sprite_cnt(p->id), i);
	}

	++(*idx);
}

/*
 *@ get all vip's home information
 */
int get_vip_home_list_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	int i, count = 0;

	traverse_sprites(pkg_vip_home_list, &count);
	i = sizeof(protocol_t);
	PKG_UINT32(msg, count, i);

	i = count * 24 + 4 + sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/*
 *@ get maomao tree detail information
 */
int get_maomao_tree_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t mapid = 0;
	int j = 0;
	UNPKG_UINT32(body, mapid, j); //who's tree  to be check
	CHECK_VALID_ID(mapid);

	//DEBUG_LOG("GET MAOMAO TREE DETAIL\t[%u, %u]", p->id, mapid);

	return db_get_mm_tree_info(p, mapid);
}

/*
 *@ calcute how many fruits to pick
 */
static inline int calc_fruit(uint32_t water, uint32_t mud, uint32_t picked)
{
	int num1, num2, ret;
	if(water < 1000 || mud < 1000)
		return 0;

	num1 = (water - 1000)/200;
	num2 = (mud - 1000)/200;

	num1 -= picked;
	num2 -= picked;

	ret = (num1 > num2)?num1:num2;

	return (ret > 0)?ret:0;
}

/*
 *@ get maomao tree detail information from db
 */
int get_maomao_tree_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);

	uint32_t tree_water, tree_mud, picked_fruit, fruit_num;
	tree_water = *(uint32_t*)buf;
	tree_mud = *(uint32_t*)(buf + 4);
	picked_fruit = *(uint32_t*)(buf +8);
	fruit_num = calc_fruit(tree_water, tree_mud, picked_fruit);

	//DEBUG_LOG("uid=%u water=%u mud=%u picked=u fruit=%u", p->id, tree_water, tree_mud, picked_fruit, fruit_num);
	int j = sizeof(protocol_t);
	switch (p->waitcmd){
	case PROTO_GET_MM_TREE_INFO:
		PKG_UINT32(msg, tree_water, j);
		PKG_UINT32(msg, tree_mud, j);
		PKG_UINT32(msg, fruit_num, j);
		break;
	case PROTO_PICK_FRUIT:
		if (fruit_num >= 1){
			uint32_t itemid = rand()%4 + 180027;
			//uint32_t itemid = 180030;
			db_pick_one_mm_tree_fruit(p);
			db_single_item_op(0, p->id, itemid, 1, 1);
			PKG_UINT32(msg, itemid, j);
		}
		else
			ERROR_RETURN(("no more fruit to pick"), -1);
		break;
	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/*
 *@ fertilize or water maomao tree
 */
int fertilize_water_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);

	uint32_t mapid, type;
	int j = 0;
	UNPKG_UINT32(body, mapid, j); // fertilize who's tree
	UNPKG_UINT32(body, type, j);
	CHECK_VALID_ID(mapid);    //mapid is usrid

	if (type != 0 && type != 1)
		ERROR_RETURN(("type invalid"), -1);
	if (p->id == mapid)
		ERROR_RETURN(("%u,error can't fertilize or water yourself tree", p->id), -1);

	op_mm_tree_t* op_t = (op_mm_tree_t*)(p->session + 4);
	op_t->type = 1;   // for  maomao tree
	op_t->userid = p->id;
	op_t->op_type = type;
	memcpy(op_t->nick, p->nick, USER_NICK_LEN);
	op_t->color = p->color;
	op_t->isvip = ISVIP(p->flag);
	op_t->op_stamp = get_now_tv()->tv_sec;

	DEBUG_LOG("FERTILIZE OR WATER\t[%u, %u]", p->id, mapid);

	return db_set_sth_done(p, 103, 50, p->id);

}

int operate_home_type_obj_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 24);

	switch	(p->waitcmd) {
	case	PROTO_FERTILIZE_WATER_TREE:
	{
		op_mm_tree_t* op_t = (op_mm_tree_t*)(p->session + 4);
		// if fertilize,  decrease mud number
		if (op_t->op_type == 1)
			db_clean_one_mud(0, p->id);
		response_proto_uint32(p, p->waitcmd, op_t->op_type, 0);
		return 0;
	}
	case	PROTO_VOTE_SMC:
	{
		uint32_t tmp = *(uint32_t*)(buf + 4);
		DEBUG_LOG("VOTE SMC CALLBACK\t[%u %u]", p->id, tmp);

		uint32_t buff[2] = {0};
		if (tmp < 100) {
			buff[1] = 2;
			db_set_profession(0, buff, id);
		} else if (tmp >= 100 && tmp < 110) {
			buff[1] = 3;
			db_set_profession(0, buff, id);
		} else if (tmp >= 400 && tmp <= 410) {
			buff[1] = 4;
			db_set_profession(0, buff, id);
		} else if (tmp >= 1000 && tmp < 1100) {
			buff[1] = 5;
			db_set_profession(0, buff, id);
		} else if (tmp >= 2500 && tmp < 2510) {
			buff[1] = 6;
			db_set_profession(0, buff, id);
		} else if (tmp >= 5500 && tmp < 5510){
#ifdef TW_VER
			send_postcard("杰西", 0,  id, 1000050, "恭喜你成為了金蘑菇嚮導，作為你的獎勵你可以在愛心嚮導社領取一個金蘑菇嚮導徽章掛在自己的小屋中！", 0);
#else
			send_postcard("杰西", 0,  id, 1000050, "恭喜你成为了金蘑菇向导，作为你的奖励你可以在爱心向导社领取一个金蘑菇向导徽章挂在自己的小屋中！", 0);
#endif
		}
		response_proto_head(p, p->waitcmd, 0);
		return 0;
	}
	case	PROTO_SEND_FU:
		response_proto_head(p, p->waitcmd, 0);
		return 0;
	default:
		ERROR_RETURN(("bad cmd id\t[uid=%u cmd=%u]", p->id, p->waitcmd), -1);
	}
}

/*
 *@ check who fertilize or water my tree
 */
int list_fertilized_watered_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	uint32_t type = 1; // 1 for maomao shu, 2 for smc, 3 for FU
	return db_list_operated(p, &type);
}

/*
 *@ get all person who fertilize or water my tree from DB
 */
int list_operated_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + count * sizeof(usr_list_t));

	usr_list_t* op_t = (usr_list_t*)(buf + 4);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, count, j);

	int loop;
	for (loop = 0; loop < count; loop++) {
		PKG_UINT32(msg, op_t->userid, j);
		PKG_UINT32(msg, op_t->op_type, j);
		PKG_STR(msg, op_t->nick, j, USER_NICK_LEN);
		PKG_UINT32(msg, op_t->color, j);
		PKG_UINT8(msg, op_t->isvip, j);
		PKG_UINT32(msg, op_t->op_stamp, j);
		op_t++;
	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/*
 *@ pick fruit from maomao tree
 */
int pick_fruit_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	return db_get_mm_tree_info(p, p->id);
}

/*
 *@ get all object in  home
 */
int get_home_type_list_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	if (!p->tiles || (!IS_HOUSE_MAP(p->tiles->id) && !IS_HOUSE1_MAP(p->tiles->id) && !IS_HOUSE2_MAP(p->tiles->id))) {
		ERROR_RETURN(("not in home[%u]", p->id), -1);
	}

	userid_t uid = GET_UID_IN_HOUSE(p->tiles->id);

	//DEBUG_LOG("GET HOME TYPE LIST\t[%u, %u]", p->id, p->tiles->id);
	return db_get_home_type_list(p, uid);
}

/*
 *@ get all object in  home from DB, every type have 6 uint32 data (type, val1,val2,val3,val4,val5)
 */
int get_home_type_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count = *(uint32_t*)buf;
	// every type have 6 uint32 data (type, val1,val2,val3,val4,val5)
	CHECK_BODY_LEN(len, 4 + count * 24);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, count, j);
	int loop;
	uint32_t* tmp = (uint32_t*)(buf + 4);
	for (loop = 0; loop < count * 6; loop++){
		PKG_UINT32(msg, *tmp, j);
		//DEBUG_LOG("%u %u", *tmp, p->id);
		tmp++;
	}

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}


/*
 *@ get smc info
 */
int get_smc_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t uid;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	CHECK_VALID_ID(uid);

	uint32_t type = 2;

	DEBUG_LOG("GET SMC INFO\t[%u, %u]", p->id, uid);
	return db_get_info_by_type(p, &type, uid);
}

/*
 *@ get smc from DB, every type have 6 uint32 data (type, val1,val2,val3,val4,val5)
 */
int get_info_by_type_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 24);

	uint32_t* tmp = (uint32_t*)buf; //the last 16 bytes discard

	int j = sizeof(protocol_t);
	switch	(p->waitcmd){
	case PROTO_GET_SMC_INFO:
		PKG_UINT32(msg, *tmp, j);       // times that have been voted today
		PKG_UINT32(msg, *(tmp + 1), j); // times that have been voted
		DEBUG_LOG("GET SMC CALLBACK\t[%u %u %u]", p->id, *tmp, *(tmp+1));
		break;
	case PROTO_GET_FU_INFO:
		PKG_UINT32(msg, *tmp, j); // times that have been sent fu today
		PKG_UINT32(msg, *(tmp + 1), j); // times that have been sent
		DEBUG_LOG("GET FU CALLBACK\t[%u %u %u]", p->id, *tmp, *(tmp + 1));
		break;
	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/*
 *@ vote for SMC
 */
int vote_smc_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t uid;
	int j = 0;
	UNPKG_UINT32(body, uid, j); // vote for who
	CHECK_VALID_ID(uid);

	if (p->id == uid)
		ERROR_RETURN(("%u,error can't vote yourself", p->id), -1);

	*(uint32_t*)p->session = uid;
	op_mm_tree_t* op_t = (op_mm_tree_t*)(p->session + 4);
	op_t->type = 2; // 2 for  vote smc
	op_t->userid = p->id;
	op_t->op_type = 0; // 0 for the first value
	memcpy(op_t->nick, p->nick, USER_NICK_LEN);
	op_t->color = p->color;
	op_t->isvip = ISVIP(p->flag);
	op_t->op_stamp = get_now_tv()->tv_sec;

	DEBUG_LOG("VOTE SMC\t[%u, %u]", p->id, uid);

	return db_set_sth_done(p, 104, 10, p->id);

}

/* @brief grow profession according exp
 */
int grow_profession(sprite_t *p, uint32_t itemid, uint32_t count)
{
	uint32_t new_stage = 0;
	if (itemid >= 5 &&  itemid <= 20) {
		switch(itemid) {
		    /*architect exp*/
		    case 5:
		    {
			    uint32_t temp = p->architect_exp;
			    p->architect_exp += count;
			    if (temp < 100 && p->architect_exp >= 100) {
			 	    new_stage = 2;
			    } else if (temp < 300 && p->architect_exp >= 300) {
				    new_stage = 3;
			    } else if (temp < 700 && p->architect_exp >= 700) {
				    new_stage = 4;
			    } else if (temp < 1300 && p->architect_exp >= 1300) {
				    new_stage = 5;
			    }
			    if (new_stage != 0) {
				    uint32_t db_buf[] = {7, new_stage};
				    db_set_profession(0, db_buf, p->id);
				    p->profession[7] = new_stage;
			    }
			    break ;
		    }
            /*dietitian exp*/
		    case 11:
		    {
			    p->dietitian_exp += count;
			    new_stage = cal_dietitian_level_by_exp(p->dietitian_exp);
			    if (new_stage > p->profession[10])
			    {
				    uint32_t db_buf[] = {10, new_stage};
				    db_set_profession(0, db_buf, p->id);
				    p->profession[10] = new_stage;
			    }
			    break ;
		    }
		    default:
		    {
		        break;
		    }
		}
	}
	return 0;
}

uint32_t cal_dietitian_level_by_exp(uint32_t dietitian_exp)
{
    if (dietitian_exp  >= 7851)
    {
        return 5;
    }
    else if (dietitian_exp  >= 2563)
    {
        return 4;
    }
    else if (dietitian_exp  >= 793)
    {
        return 3;
    }
    else if (dietitian_exp  >= 200)
    {
        return 2;
    }

    return 1;

}

