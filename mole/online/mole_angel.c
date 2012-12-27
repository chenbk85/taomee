#include <libtaomee/dataformatter/bin_str.h>

#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "small_require.h"
#include "central_online.h"
#include "mole_homeland.h"
#include "mole_angel.h"
#include "central_online.h"
#include "small_require.h"


#define TD_TASKS_TABLE_SIZE 	(64 + 10 + 6 + 3 +3+1)
static td_task_info_t td_tasks_info[TD_TASKS_TABLE_SIZE];
static td_role_info_t roles_info;
#define MAX_VALUE(v, j) ((v)<(j)?(j):(v))

static angel_honor_bonus_t honors_bonus[2];

#define MAX_ANGEL_COMPOSE_SIZE 		50
static angel_compose_t compose_info[MAX_ANGEL_COMPOSE_SIZE];

//20111223 TD
#define TD_CHALLENGE_GAME_CNT					10					//最大关卡数

static td_game_task_info_t challenge_game_info[TD_CHALLENGE_GAME_CNT][2];


#define db_get_paradise_item(p_, buf_, id_) \
		send_request_to_db(SVR_PROTO_USER_GET_PARADISE, p_, 4, buf_, id_)
#define db_get_paradise_store_items(p_) \
		send_request_to_db(SVR_PROTO_USER_PARADISE_STORE_ITEM, p_, 0, NULL, (p_)->id)
#define db_add_nimsbus(p_, buf_) \
			send_request_to_db(SVR_PROTO_USER_PARADISE_SET_NIMBUS, p_, 8, buf_, (p_)->id)
#define db_free_captured_angel(p_, buf_)\
			send_request_to_db(SVR_PROTO_USER_TERMINATE_CONTRACT, p_, 12, buf_, (p_)->id)
#define db_make_angel_followed(p_, buf_)\
			send_request_to_db(SVR_PROTO_MAKE_ANGEL_FOLLOWED, p_, 8, buf_, (p_)->id)
#define db_change_to_angle(p_, buf_)\
			send_request_to_db(SVR_PROTO_CHANGE_TO_ANGEL, p_, 4, buf_, (p_)->id)
#define db_change_background(p_, buf_)\
			send_request_to_db(SVR_PROTO_USER_CHANGE_PARADISE_BACKGOUND, p_, 8, buf_, (p_)->id)
#define db_get_friend_paradise_info(p_,len_ ,buf_)\
			send_request_to_db(SVR_PROTO_GET_FRIEND_PARAIDSE_INFO, p_, len_, buf_, (p_)->id)
#define db_get_visitor_paradise_list(p_, buf_, id_) \
			send_request_to_db(SVR_PROTO_USER_PARADISE_GET_VISITLIST, p_, 4, buf_, id_)
#define db_get_visitor_paradise_info(p_, len_ , buf_) \
			send_request_to_db(SVR_PROTO_GET_VISTOR_ANGEL_INFO, p_, len_, buf_, (p_)->id)
#define db_get_hot_item_info(p_) \
			send_request_to_db(SVR_PROTO_GET_HOT_ITEM, p_, 0, NULL, (p_)->id)
#define db_use_paradise_prop(p_, buf_) \
				send_request_to_db(SVR_USE_PARADISE_PROP, p_, 8, buf_, (p_)->id)
#define db_get_fight_score(p_, buf_) \
				send_request_to_db(SVR_PROTO_FIGHT_SCORE, p_, 4, buf_, (p_)->id)
#define db_td_game_end(p_,len_ ,buf_)\
				send_request_to_db(SVR_PROTO_FIGHT_END, p_, len_, buf_, (p_)->id)
#define db_clean_black_angel_sucess(p_, buf_) \
					send_request_to_db(SVR_PROTO_USER_CAPTURE_ANGEL_CMD, p_, 8, buf_, (p_)->id)
#define db_get_angel_in_hospital(p_) \
			send_request_to_db(SVR_PROTO_ANGEL_IN_HOSPITAL, p_, 0, NULL, (p_)->id)
#define db_use_tear_of_jack(p_, buf_) \
			send_request_to_db(SVR_PROTO_USE_TRAR, p_, 4, buf_, (p_)->id)
#define db_angel_leave_hospotal(p_, len_ , buf_) \
				send_request_to_db(SVR_PROTO_LEAVE_HOSPITAL, p_, len_, buf_, (p_)->id)


static int notify_paradise_level_up(sprite_t* p, uint32_t level)
{
    uint8_t n_msg[128] = {0};
	int i = sizeof(protocol_t);
	PKG_UINT32(n_msg, level, i);

    init_proto_head(n_msg, PROTO_PARADISE_LEVEL_UP, i);
    return send_to_self(p, n_msg, i, 0);
}


static int notify_user_get_honor(sprite_t* p, uint32_t honor_type, uint32_t honor_id)
{
	uint8_t n_msg[128] = {0};
	int i = sizeof(protocol_t);
	PKG_UINT32(n_msg, honor_type, i);
	PKG_UINT32(n_msg, honor_id, i);

	DEBUG_LOG("notify_user_get_honor userid :%u type:%u, honorid:%u", p->id, honor_type, honor_id);

	init_proto_head(n_msg, PROTO_NOTIFY_USER_GET_HONOR, i);
	return send_to_self(p, n_msg, i, 0);
}


static inline int db_put_animal_home(sprite_t *p, userid_t id, int nbr)
{
	uint32_t buff[2];
	buff[0] = nbr;
	buff[1] = 0;
	return send_request_to_db(SVR_PROTO_ANIMAL_OUTGO, p, 8, buff, id);
}

static inline int db_put_angel_home_to_get_angel(sprite_t *p, userid_t id, int nbr)
{
	uint32_t buff[2];
	buff[0] = nbr;
	buff[1] = 0;
	return send_request_to_db(SVR_PROTO_MAKE_ANGEL_FOLLOWED, p, 8, buff, id);
}

static inline int db_statistics_angel(sprite_t *p, userid_t id, uint32_t exp, uint32_t captured_angel)
{
	uint32_t buff[2];
	buff[0] = exp;
	buff[1] = captured_angel;
	return send_request_to_db(SYSARG_UPDATE_PARADISE_RANK_CMD, p, 8, buff, id);
}


int get_paradise_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t id, high;
	if (unpkg_uint32_uint32(body, len, &id, &high) == -1) return -1;

	CHECK_VALID_ID(id);

	return db_get_paradise_item(p, &high, id);
}

int get_paradise_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 40);

	uint32_t high, nimsbus, exp, attire_count,limit_count,captured_angel,angel_count, paradise_leve, is_vip, show_id;

	int j = 0;
	UNPKG_H_UINT32(buf, is_vip, j);
	UNPKG_H_UINT32(buf, high, j);
	UNPKG_H_UINT32(buf, nimsbus, j);
	UNPKG_H_UINT32(buf, exp, j);
	UNPKG_H_UINT32(buf, paradise_leve, j);
	UNPKG_H_UINT32(buf, limit_count, j);
	UNPKG_H_UINT32(buf, show_id, j);
	UNPKG_H_UINT32(buf, captured_angel, j);
	UNPKG_H_UINT32(buf, attire_count, j);
	UNPKG_H_UINT32(buf, angel_count, j);


    DEBUG_LOG("PARADISE STAUTS [len:%u id:%u is_vip:%u id:%u high:%u nimsbus:%u exp:%u paradise_leve:%u attire_count:%u angel_count:%u captured_angel:%u]",
			   len, p->id, id, is_vip, high, nimsbus, exp, paradise_leve,attire_count, angel_count, captured_angel);
	CHECK_BODY_LEN(len, 40 + angel_count * sizeof(angel_t) + attire_count * sizeof(attire_t));

	uint32_t msgbuff[2] = {id, exp};
	msglog(statistic_logfile, 0x0405CAA1, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	db_statistics_angel(0, id, exp, captured_angel);

	int i = sizeof (protocol_t);
	//添加天使乐园用户id
	PKG_UINT32(msg, id, i);
	PKG_UINT32(msg, is_vip, i);
	PKG_UINT32(msg, high, i);
	PKG_UINT32(msg, nimsbus, i);
	PKG_UINT32(msg, exp, i);
	PKG_UINT32(msg, paradise_leve, i);
	PKG_UINT32(msg, limit_count, i);
	PKG_UINT32(msg, show_id, i);

		//打包装扮
	PKG_UINT32(msg, attire_count, i);
	int loop;
	for (loop = 0; loop < attire_count; loop++) {
		attire_t* attire = (attire_t*)(buf + j);
		PKG_UINT32(msg, attire->attire_id, i);
		j += sizeof(attire_t);

		//DEBUG_LOG("ATTIRE ID=%u", attire->attire_id);
	}

	PKG_UINT32(msg, angel_count, i);
	//打包天使
	for (loop = 0; loop < angel_count; loop++) {
		angel_t* angel = (angel_t*)(buf + j);
		pkg_angel_info(msg, angel, &i);
		j += sizeof(angel_t);

	//	DEBUG_LOG("ANGEL STATE [%u %u %u %u %u %u %u %u %u]",angel->auto_increase_id, angel->angel_id,
			      //angel->growth, angel->pos_id, angel->state, angel->last_caltime, angel->variate_id, angel->rate, angel->diff_mature);
	}

	DEBUG_LOG("get_paradise_callback [uid=%u len=%u]", p->id, i);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int get_paradise_store_items_cmd(sprite_t* p, uint8_t* body, int len)
{

	CHECK_VALID_ID(p->id);

	DEBUG_LOG("GET PARADISE STORE\t[uid=%u]", p->id);
	return db_get_paradise_store_items(p);
}

int get_paradise_store_items_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count_angle = 0;
	int j = 0;
	UNPKG_H_UINT32 (buf, count_angle, j);
	CHECK_BODY_LEN(len, 4 + count_angle*8);

	int loop, i = sizeof (protocol_t);
	PKG_UINT32 (msg, count_angle, i);
	for (loop = 0; loop < count_angle; loop++) {
		uint32_t itm, cnt;
		UNPKG_H_UINT32 (buf, itm, j);
		UNPKG_H_UINT32 (buf, cnt, j);
		PKG_UINT32 (msg, itm, i);
		PKG_UINT32 (msg, cnt, i);
	}

	init_proto_head (msg, p->waitcmd, i);
	DEBUG_LOG ("GET STORE CALLBACK\t[%u %u]", p->id, i);
	return send_to_self (p, msg, i, 1);
}

int add_nimsbus_cmd(sprite_t* p, uint8_t* body, int len)
{

	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);
	/* 判断是否在地图*/
	if (!p->tiles || !IS_PARADISE_MAP(p->tiles->id)){
		ERROR_RETURN(("add_nimsbus_cmd not in PARADISE\t[uid=%u]", p->id), -1);
	}

	uint32_t count, item_id;
	int j = 0;
	UNPKG_UINT32(body, item_id, j);
	UNPKG_UINT32(body, count, j);

	uint32_t dbbuf[] = {item_id, count};

	DEBUG_LOG ("add_nimsbus_cmd NIMSBUS \t[%u %u %u]", p->id, item_id, count);
	return db_add_nimsbus(p, dbbuf);

}

int add_nimsbus_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t nimsbus_value;
	int j = 0;
	UNPKG_H_UINT32(buf, nimsbus_value, j);
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, nimsbus_value, i);

	init_proto_head (msg, p->waitcmd, i);

	DEBUG_LOG ("ADD NIMSBUS CALLBACK\t[%u %u %u]", p->id, nimsbus_value, i);
	return send_to_self (p, msg, i, 1);
}

int feed_angle_seed_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t posid = 0, angleid = 0;

	if (!p->tiles || !IS_PARADISE_MAP(p->tiles->id)){
		ERROR_RETURN(("feed_angle_seed_cmd not in PARADISE\t[uid=%u]", p->id), -1);
	}

	if (unpkg_uint32_uint32(body, len, &posid, &angleid) == -1) return -1;
	CHECK_VALID_ID(p->id);

	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x04041420,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	uint32_t dbbuf[] = {posid, angleid};
	return send_request_to_db(SVR_PROTO_USER_CHANGE_ANGEL_POS, p, sizeof(dbbuf), dbbuf, p->id);
}

int feed_angle_seed_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(angel_t)+4);
	int i = sizeof (protocol_t);
	angel_t* angel = (angel_t*)buf;
	pkg_angel_info(msg, angel, &i);

	friend_count_t* count_in = (friend_count_t*)((char*)buf + sizeof(angel_t));
	paradise_user_id_t* friend_id = (paradise_user_id_t*)((char*)count_in + sizeof(friend_count_t));
	uint32_t count = count_in->count;
	int loop = 0;
	for(loop = 0; loop < count; loop++){
		//DEBUG_LOG("loop:%d, firendid:%u", loop, friend_id->userid);
		//send_postcard(p->nick, p->id, friend_id->userid, 1000211, "", 0);//1000211
		friend_id++;
	}

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int build_angle_contract_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t index = 0;

	CHECK_VALID_ID(p->id);
	DEBUG_LOG("p_id:%u-----index: %u-------len_of_prite: %lu", p->id,  index, sizeof(sprite_t));
	/* 判断是否在地图*/
	if (!p->tiles || !IS_PARADISE_MAP(p->tiles->id)){
		ERROR_RETURN(("build_angle_contract_cmd not in PARADISE\t[uid=%u]", p->id), -1);
	}

	if (unpkg_uint32(body, len, &index) == -1) return -1;

	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x04041421,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	return send_request_to_db(SVR_PROTO_USER_ANGEL_SIGN_CONTRACT, p, 4, &index, p->id);
}

int build_angle_contract_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 20);
	DEBUG_LOG("id: %u -------------------len: %u", id, len);
	uint32_t index = 0, add_exp, paradise_level, honor_type, honor_id;
	int j = 0;
	UNPKG_H_UINT32(buf, index, j);
	UNPKG_H_UINT32(buf, add_exp, j);
	UNPKG_H_UINT32(buf, paradise_level, j);
	UNPKG_H_UINT32(buf, honor_type, j);
	UNPKG_H_UINT32(buf, honor_id, j);

	if(honor_type)
	{
	    notify_user_get_honor(p, honor_type, honor_id);
	}

	if(p->paradise_leve < paradise_level){
		p->paradise_leve = paradise_level;
		notify_paradise_level_up(p, paradise_level);
	}
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, index, i);
	PKG_UINT32 (msg, add_exp, i);
	init_proto_head (msg, p->waitcmd, i);

	DEBUG_LOG ("build_angle_contract_callback CALLBACK\t[%u index:%u add_exp:%u]", p->id, index, add_exp);
	return send_to_self (p, msg, i, 1);
}

int free_angle_in_paradise_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t index = 0;
	CHECK_VALID_ID(p->id);
	/* 判断是否在地图*/
	if (!p->tiles || !IS_PARADISE_MAP(p->tiles->id)){
		ERROR_RETURN(("free_angle_in_paradise_cmd not in PARADISE\t[uid=%u]", p->id), -1);
	}

	if (unpkg_uint32(body, len, &index) == -1) return -1;
	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x04041422, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	return send_request_to_db(SVR_PROTO_USER_ANGEL_FREE, p, 4, &index, p->id);
}

int free_angle_in_paradise_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t index = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, index, j);
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, index, i);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int get_contract_angles_cmd(sprite_t* p, uint8_t* body, int len)
{
    uint32_t userid = 0;
	CHECK_VALID_ID(p->id);
	/* 判断是否在地图*/
	/*
	if (!p->tiles || !IS_PARADISE_MAP(p->tiles->id)){
		ERROR_RETURN(("get_contract_angles_cmd not in PARADISE\t[uid=%u]", p->id), -1);
	}
	*/
	if (unpkg_uint32(body, len, &userid) == -1) return -1;

	CHECK_VALID_ID(userid);
	return send_request_to_db(SVR_PROTO_USER_GET_CONTRACT_ANGLES, p, 0, NULL, userid);
}

int get_contract_angles_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);
	uint32_t count_angle = 0, total_count = 0;
	int j = 0;
	UNPKG_H_UINT32 (buf, total_count, j);
	UNPKG_H_UINT32 (buf, count_angle, j);
	CHECK_BODY_LEN(len, 8 + count_angle*8);

	int loop, i = sizeof (protocol_t);
	PKG_UINT32 (msg, total_count, i);
	PKG_UINT32 (msg, count_angle, i);
	for (loop = 0; loop < count_angle; loop++) {
		uint32_t itm, cnt;
		UNPKG_H_UINT32 (buf, itm, j);
		UNPKG_H_UINT32 (buf, cnt, j);
		PKG_UINT32 (msg, itm, i);
		PKG_UINT32 (msg, cnt, i);
	}

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int save_monster_game_cmd(sprite_t* p, uint8_t* body, int len)
{
    CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

    uint32_t itemid = 0;
	int i = 0;
	UNPKG_UINT32(body, itemid, i);
	if (!ISVIP(p->flag))
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if (itemid != 1354071 && itemid != 1354072 && itemid != 1354093 && itemid != 1354094
	    && itemid != 1354085 && itemid != 1354086 && itemid != 1354100 && itemid != 1354101)
    {
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
    }

    *(uint32_t*)p->session = itemid;
    return db_set_sth_done(p, 40008, 1, p->id);

}

int save_monster_game(sprite_t* p)
{
    uint32_t itemid = 0;
    itemid = *(uint32_t*)p->session;
    uint32_t angel_type = 0;
    uint32_t j = 0;
    for (j = 0; j < 3; j++)
    {
        item_t*  p_item =  NULL;
        p_item = get_item_prop(itemid);
        if (p_item == NULL)
        {
            return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
        }
        if (p_item->u.angel_tag.abilityNum[j] > 0)
        {
            uint32_t nbit = 0;
            if(p_item->u.angel_tag.abilityNum[j] == 1)
            {
                nbit = 2;
            }
            else if (p_item->u.angel_tag.abilityNum[j] == 2)
            {
                nbit = 1;
            }
            else
            {
                nbit = p_item->u.angel_tag.abilityNum[j];
            }
            angel_type = angel_type|(0x01<<(nbit-1));
        }

     }

    db_add_singel_angel(p->id, itemid, 1, angel_type);
    response_proto_uint32_uint32(p, p->waitcmd, itemid, 1, 0);

    uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0406FC7A, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	uint32_t index = 0;
	if (itemid == 1354071)
	{
	    index = 1;
	}
	else if(itemid == 1354072)
	{
	    index = 2;
	}
	else if(itemid == 1354093)
	{
	    index = 3;
	}
	else if(itemid == 1354094)
	{
	    index = 4;
	}
	else if(itemid == 1354085)
	{
	    index = 5;
	}
	else if(itemid == 1354086)
	{
	    index = 6;
	}
	else if(itemid == 1354100)
	{
	    index = 7;
	}
	else if(itemid == 1354101)
	{
	    index = 8;
	}
	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0408B3CD+index-1, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

    return 0;
}


int gain_game_angles_cmd(sprite_t* p, uint8_t* body, int len)
{
    CHECK_BODY_LEN(len, 20);
	CHECK_VALID_ID(p->id);

	uint32_t game_type = 0;
    uint32_t itemid1 = 0, itemid2 = 0, itemid3 = 0, itemid4 = 0;
	int i = 0;
	UNPKG_UINT32(body, game_type, i);
	UNPKG_UINT32(body, itemid1, i);
	UNPKG_UINT32(body, itemid2, i);
	UNPKG_UINT32(body, itemid3, i);
	UNPKG_UINT32(body, itemid4, i);
	*(uint32_t*)p->session = itemid1;
    *(uint32_t*)(p->session + 4) = itemid2;
    *(uint32_t*)(p->session + 8) = itemid3;
    *(uint32_t*)(p->session + 16) = itemid4;

    if (game_type !=1 && game_type != 2)
    {
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
    }

    if (!ISVIP(p->flag))
    {
        if (itemid1 != 0 && itemid2 != 0 && itemid3 != 0 && itemid4 != 0)
        {
            return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
        }
    }

    uint32_t num = 0;
    if (game_type == 1)
    {
        if ((itemid1 != 0 && itemid1 != 1353216 && itemid1 != 1353220 && itemid1 != 1353225)
            || (itemid2 != 0 && itemid2 != 1353216 && itemid2 != 1353220 && itemid2 != 1353225)
            || (itemid3 != 0 && itemid3 != 1353216 && itemid3 != 1353220 && itemid3 != 1353225)
            || (itemid4 != 0 && itemid4 != 1353216 && itemid4 != 1353220 && itemid4 != 1353225))
        {
            return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
        }

        if (itemid1 == 1353225)
        {
            num = num + 1;
        }
        if (itemid2 == 1353225)
        {
            num = num + 1;
        }
        if (itemid3 == 1353225)
        {
            num = num + 1;
        }
        if (itemid4 == 1353225)
        {
            num = num + 1;
        }
    }
    else
    {
        if ((itemid1 != 0 && itemid1 != 1353210 && itemid1 != 1353214 && itemid1 != 1353224)
            || (itemid2 != 0 && itemid2 != 1353210 && itemid2 != 1353214 && itemid2 != 1353224)
            || (itemid3 != 0 && itemid3 != 1353210 && itemid3 != 1353214 && itemid3 != 1353224)
            || (itemid4 != 0 && itemid4 != 1353210 && itemid4 != 1353214 && itemid4 != 1353224))
        {
            return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
        }

        if (itemid1 == 1353224)
        {
            num = num + 1;
        }
        if (itemid2 == 1353224)
        {
            num = num + 1;
        }
        if (itemid3 == 1353224)
        {
            num = num + 1;
        }
        if (itemid4 == 1353224)
        {
            num = num + 1;
        }
    }

    if (num > 1)
    {
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
    }
    else if (num == 1)
    {
        if (game_type == 1)
        {
            db_set_sth_done(p, 40004, 1, p->id);
        }
        else
        {
            db_set_sth_done(p, 40007, 1, p->id);
        }

        *(uint32_t*)(p->session + 12) = 1;
    }
    else
    {
        db_set_sth_done(p, 40005, 5, p->id);
        *(uint32_t*)(p->session + 12) = 0;
    }

	uint32_t msgbuff[] = {p->id, 1};
    msglog(statistic_logfile, 0x04040B07+game_type-1,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	return 0;
}

int gain_game_angles(sprite_t* p)
{
    uint32_t flag = 0;
    flag = *(uint32_t*)(p->session + 12);
    if (flag)
    {
        db_set_sth_done(p, 40005, 5, p->id);
        *(uint32_t*)(p->session + 12) = 0;
    }
    else
    {
        uint32_t msgbuff[7] = {0,0,0,0,0,1,p->id};
        msglog(statistic_logfile, 0x4040B06,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

        uint32_t itemid1 = 0, itemid2 = 0, itemid3 = 0, itemid4 = 0;
        itemid1 = *(uint32_t*)p->session;
        itemid2 = *(uint32_t*)(p->session + 4);
        itemid3 = *(uint32_t*)(p->session + 8);
        itemid4 = *(uint32_t*)(p->session + 16);

        uint32_t len = 0;
        uint32_t count = 0;
        uint8_t buff[1024] = {};
	    int j = 0;
	    PKG_H_UINT32(buff, 1, j);
	    len = j;
	    PKG_H_UINT32(buff, 4, j);
	    PKG_H_UINT32(buff, 0, j);
    	PKG_H_UINT32(buff, 0, j);

        if (!ISVIP(p->flag))
        {
            PKG_H_UINT32(buff, 0, j);
    	    PKG_H_UINT32(buff, 0, j);
    	    PKG_H_UINT32(buff, 100, j);
        }
        else
        {
            PKG_H_UINT32(buff, 0, j);
    	    PKG_H_UINT32(buff, 0, j);
    	    PKG_H_UINT32(buff, 0, j);
        }

    	if (itemid1 != 0)
    	{
    	    item_t* itm = get_item_prop(itemid1);
            if (pkg_item_kind(p, buff, itm->id, &j) == -1)
    	    {
    	        return -1;
    	    }
    	    PKG_H_UINT32(buff, itm->id, j);
    	    PKG_H_UINT32(buff, 1, j);
    	    PKG_H_UINT32(buff, itm->max, j);
    	    count++;
    	}
    	if (itemid2 != 0)
    	{
    	    item_t* itm = get_item_prop(itemid2);
            if (pkg_item_kind(p, buff, itm->id, &j) == -1)
    	    {
    	        return -1;
    	    }
    	    PKG_H_UINT32(buff, itm->id, j);
    	    PKG_H_UINT32(buff, 1, j);
    	    PKG_H_UINT32(buff, itm->max, j);
    	    count++;
    	}
    	if (itemid3 != 0)
    	{
    	    item_t* itm = get_item_prop(itemid3);
            if (pkg_item_kind(p, buff, itm->id, &j) == -1)
    	    {
    	        return -1;
    	    }
    	    PKG_H_UINT32(buff, itm->id, j);
    	    PKG_H_UINT32(buff, 1, j);
    	    PKG_H_UINT32(buff, itm->max, j);
    	    count++;
    	}
    	if (itemid4 != 0)
    	{
    	    item_t* itm = get_item_prop(itemid4);
            if (pkg_item_kind(p, buff, itm->id, &j) == -1)
    	    {
    	        return -1;
    	    }
    	    PKG_H_UINT32(buff, itm->id, j);
    	    PKG_H_UINT32(buff, 1, j);
    	    PKG_H_UINT32(buff, itm->max, j);
    	    count++;
    	}

    	PKG_H_UINT32(buff, count, len);
    	send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);
    }

    return 0;
}

int gain_game_angles_exchange_item(sprite_t* p)
{
    uint32_t itemid1 = 0, itemid2 = 0, itemid3 = 0, itemid4 = 0;
    itemid1 = *(uint32_t*)p->session;
    itemid2 = *(uint32_t*)(p->session + 4);
    itemid3 = *(uint32_t*)(p->session + 8);
    itemid4 = *(uint32_t*)(p->session + 16);

    int i = sizeof (protocol_t);
	PKG_UINT32 (msg, itemid1, i);
	PKG_UINT32 (msg, itemid2, i);
	PKG_UINT32 (msg, itemid3, i);
	PKG_UINT32 (msg, itemid4, i);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}


int free_captured_angle_in_paradise_cmd(sprite_t* p, const uint8_t* body, int len)
{

	uint32_t count, angel_id;
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);

	CHECK_VALID_ID(p->id);
	/* 判断是否在地图*/
	if (!p->tiles || !IS_PARADISE_MAP(p->tiles->id)){
		ERROR_RETURN(("free_captured_angle_in_paradise_cmd not in PARADISE\t[uid=%u]", p->id), -1);
	}

	int j = 0;
	UNPKG_UINT32(body, angel_id, j);
	UNPKG_UINT32(body, count, j);

	//查找奖励
	item_t* itm = get_item_prop(angel_id);
	if (!itm) {
		ERROR_RETURN(("free_captured_angle_in_paradisess Item %u not found", angel_id), -1);
	}

	uint32_t dbbuf[] = {angel_id, count, itm->sell_price};
	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x04041423, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	return db_free_captured_angel(p, dbbuf);

}
int free_captured_angle_in_paradise_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);
	uint32_t xiaomee = 0, count = 0, angel_id = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, angel_id, j);
	UNPKG_H_UINT32(buf, count, j);
	UNPKG_H_UINT32(buf, xiaomee, j);

	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, angel_id, i);
	PKG_UINT32 (msg, count, i);
	PKG_UINT32 (msg, xiaomee, i);

	init_proto_head (msg, p->waitcmd, i);

	DEBUG_LOG ("free_captured_angle_in_paradise_callback CALLBACK\t[%u %u %u %u %u]", p->id, xiaomee, i, angel_id, count);
	return send_to_self (p, msg, i, 1);
}

int make_angle_fallowed_cmd(sprite_t* p, const uint8_t* body, int len)
{

	uint32_t angel_id, type;
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);

	int j = 0;
	UNPKG_UINT32(body, angel_id, j);
	UNPKG_UINT32(body, type, j);

	return set_sth_follow( p, angel_id, type, FAT_ANGEL );

}

int make_angle_fallowed_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t angel_id = 0, type = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, angel_id, j);
	UNPKG_H_UINT32(buf, type, j);
	int i = sizeof (protocol_t);

	PKG_UINT32 (msg, angel_id, i);
	if(type == 1){//带出天使
		p->animal_nbr = angel_id;
		memset(&(p->animal_follow_info), 0, sizeof(p->animal_follow_info));
		p->animal_follow_info.tag_animal.itemid = angel_id;
	}else if(type == 0){//带回家
		p->animal_nbr = 0;
		memset(&(p->animal_follow_info), 0, sizeof(p->animal_follow_info));
	}else{
		ERROR_RETURN(("make_angle_fallowed_callback type err:[%u %u]", p->id, angel_id), -1);
	}

	init_proto_head (msg, p->waitcmd, i);

	DEBUG_LOG ("make_angle_fallowed_callback CALLBACK\t[%u %u %u %u]", p->id, angel_id, i, p->animal_nbr);
	send_to_map(p, msg, i, 1);
	return 0;
}

int change_to_angle_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t angel_id;
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	int j = 0;
	UNPKG_UINT32(body, angel_id, j);
	uint32_t dbbuf[] = {angel_id};

	return db_change_to_angle(p, dbbuf);
}

int change_to_angle_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t angel_id = 0, is_sucess = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, angel_id, j);
	UNPKG_H_UINT32(buf, is_sucess, j);

	p->animal_nbr = 0;
	memset(&(p->animal_follow_info), 0, sizeof(p->animal_follow_info));

	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, is_sucess, i);
	PKG_UINT32 (msg, angel_id, i);

	init_proto_head (msg, p->waitcmd, i);

	DEBUG_LOG ("make_angle_fallowed_callback CALLBACK\t[%u %u %u]", p->id, angel_id, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

int change_background_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t background_id, layer;
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);

	int j = 0;
	UNPKG_UINT32(body, layer, j);
	UNPKG_UINT32(body, background_id, j);
	if(layer < 300){
		ERROR_LOG("===change_background_cmd end alread userid:%u , layer:%u", p->id, layer);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	uint32_t dbbuf[] = {layer, background_id};

	return db_change_background(p, dbbuf);
}

int get_friend_paradise_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	int count = 0;
	uint32_t buff[1024] = {0};

	if (IS_GUEST_ID (p->id) || IS_NPC_ID (p->id))
	{
		ERROR_RETURN(("not a register user\t[%u]", p->id), -1);
	}

	CHECK_BODY_LEN_GE(len, 4);

	int i = 0;
	UNPKG_UINT32(body, count, i);
	buff[0] = count;
	int cnt = 1;

	if (count > 250) {
		ERROR_RETURN(("too many users, count=%d", count), -1);
	}

	CHECK_BODY_LEN(len, count * 4 + 4);
	int j = 0;
	uint32_t uid = 0;
	for (; j != count; ++j)
	{
		UNPKG_UINT32(body, uid, i);
		if(!uid || IS_NPC_ID(uid) || IS_GUEST_ID(uid))
		{
		    continue;
		}
		buff[cnt++] = uid;
	}

	buff[0] = cnt - 1;
	len = (cnt - 1) * 4 + 4;

	return db_get_friend_paradise_info(p, len, buff);
}


int get_friend_paradise_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + 12 * count);

	int loop = 0;
	int cnt = 0;
	int j = sizeof(protocol_t) + 4;
	angel_info_t* p_info = (angel_info_t*)((char*)buf + 4);
	for (loop = 0; loop < count; loop++,p_info++)
	{
		if(p_info->uid != p->id && p_info->level == 0 && p_info->angel_count == 0)
		{
			continue;
		}
		cnt++;
		PKG_UINT32(msg, p_info->uid, j);
		PKG_UINT32(msg, p_info->level, j);
		PKG_UINT32(msg, p_info->angel_count, j);
	}
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, cnt, i);
	j = sizeof(protocol_t) + 4 + 12 * cnt;
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}


int get_visitor_paradise_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t id, high;
	if (unpkg_uint32_uint32(body, len, &id, &high) == -1) return -1;

	CHECK_VALID_ID(id);

	uint32_t dbbuf[] = {high};

	return db_get_visitor_paradise_list(p, dbbuf, id);
}

int get_visitor_paradise_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t buff[1024] = {0};

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	buff[0] = count;
	int cnt = 1;

	CHECK_BODY_LEN(len, 4 + 8 * count);

	if (count > 200) {
		ERROR_RETURN(("too many visitor, count=%d", count), -1);
	}

	int j = 0;
	visitor_list_t* p_info = (visitor_list_t*)((char *)buf + 4);

	for (; j != count; ++j)
	{
		if(!p_info->uid || IS_NPC_ID(p_info->uid) || IS_GUEST_ID(p_info->uid))
		{
		    continue;
		}
		buff[cnt++] = p_info->uid;
		buff[cnt++] = p_info->time;
		p_info++;
	}

	buff[0] = cnt - 1;
	len = (cnt - 1) * 8 + 4;

	return db_get_visitor_paradise_info(p, len, buff);
}

int get_visitor_paradise_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + 16 * count);

	int loop = 0;
	int cnt = 0;
	int j = sizeof(protocol_t) + 4;
	visitor_angel_info_t* p_info = (visitor_angel_info_t*)((char*)buf + 4);
	for (loop = 0; loop < count; loop++,p_info++)
	{
		cnt++;
		PKG_UINT32(msg, p_info->uid, j);
		PKG_UINT32(msg, p_info->time, j);
		PKG_UINT32(msg, p_info->level, j);
		PKG_UINT32(msg, p_info->angel_count, j);
	}
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, cnt, i);
	j = sizeof(protocol_t) + 4 + 16 * cnt;
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int get_hot_item_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	return db_get_hot_item_info(p);
}

int get_hot_item_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + 8 * count);

	int loop = 0;
	int cnt = 0;
	int j = sizeof(protocol_t) + 4;
	hot_info_t* p_info = (hot_info_t*)((char*)buf + 4);
	for (loop = 0; loop < count; loop++,p_info++)
	{
		cnt++;
		PKG_UINT32(msg, p_info->id, j);
		PKG_UINT32(msg, p_info->count, j);
	}
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, cnt, i);
	j = sizeof(protocol_t) + 4 + 8 * cnt;
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int use_prop_info_cmd(sprite_t* p, const uint8_t* body, int len){
	uint32_t prop_id, position_id;
	if (unpkg_uint32_uint32(body, len, &prop_id, &position_id) == -1) return -1;

	CHECK_VALID_ID(p->id);

	uint32_t dbbuf[] = {prop_id, position_id};

	return db_use_paradise_prop(p, dbbuf);
}

int use_prop_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(angel_t) + 4);
	int i = sizeof (protocol_t);

	paradise_prop_id_t *p_prop_id = (paradise_prop_id_t*)buf;
	PKG_UINT32(msg, p_prop_id->prop_id, i);

	uint32_t msgbuff[2] = {p->id, 1};
	uint32_t add_index = p_prop_id->prop_id - 1353100;
	msglog(statistic_logfile, 0x0405CAA3 + add_index, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	angel_t* angel = (angel_t*)((char *)buf + 4);
	pkg_angel_info(msg, angel, &i);
	init_proto_head (msg, p->waitcmd, i);
	DEBUG_LOG("use_prop_info_callback userid:%u prop_id:%u angel_id:%u", p->id, p_prop_id->prop_id, angel->angel_id);
	return send_to_self (p, msg, i, 1);
}

int get_angel_info_for_td_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	return send_request_to_db(SVR_PROTO_GET_FIGHT_ANGEL, p, 0, NULL, p->id);
}

int get_angel_info_for_td_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count_angle = 0;
	int j = 0;
	UNPKG_H_UINT32 (buf, count_angle, j);
	CHECK_BODY_LEN(len, 4 + count_angle*8);

	int loop, i = sizeof (protocol_t);
	PKG_UINT32 (msg, count_angle, i);
	for (loop = 0; loop < count_angle; loop++) {
		uint32_t itm, cnt;
		UNPKG_H_UINT32 (buf, itm, j);
		UNPKG_H_UINT32 (buf, cnt, j);
		PKG_UINT32 (msg, itm, i);
		PKG_UINT32 (msg, cnt, i);
	}

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int td_game_start_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t task_id;
	db_exchange_single_item_op(p, 202, 1351342, 1, 0);
	if (unpkg_uint32(body, len, &task_id) == -1) return -1;

	uint32_t cur_task_id	= task_id % 10000;
	uint32_t cur_task_mode	= task_id / 10000;
	//圣诞天使挑战赛
	if( cur_task_id > 64 && cur_task_id <= 64 + TD_CHALLENGE_GAME_CNT )
	{
		if( cur_task_id - 64 > 1 &&					//不是第一关
			( p->td_task_mode != cur_task_mode ||	//本次模式和上次不同
			(uint32_t)(cur_task_id - p->td_task_id) > 1 )		//当前关卡和上一关不连续
			)
		{
			DEBUG_LOG( "td_game_start_cmd Invalid TaskID:[%u], Last Task ID:[%u], CurMode:[%u], LastMode[%u]",
				cur_task_id, p->td_task_id, cur_task_mode, p->td_task_mode );
			return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
		}
	}
	p->td_task_id = cur_task_id;
	p->td_task_mode = cur_task_mode;
	if( p->td_task_mode > 1 )
	{
		DEBUG_LOG( "td_game_start_cmd Invalid Task Mode TaskID:[%u] Mode:[%u]", p->td_task_id, p->td_task_mode );
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	//圣诞天使挑战赛参与统计
	if( p->td_task_id == 64 + 1 )
	{
		uint32_t msglog_id;
		if( 0 == p->td_task_mode )
		{
			msglog_id = 0x0409BF42;
		}
		else
		{
			msglog_id = 0x0409BF43;
		}
		uint32_t msgbuff[2] = {p->id, 1};
		msglog(statistic_logfile, msglog_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	//clear award
	p->td_award_left = 0;

	CHECK_VALID_ID(p->id);
	uint32_t now = time(NULL);
	p->td_game_start_time = now;
	DEBUG_LOG("td_game_start_cmd id:%u, time:%u taskid:%u", p->id, p->td_game_start_time, task_id);

	{
		uint32_t msgbuff[2] = {p->id, 1};
		//msglog(statistic_logfile, 0x0406FAAF, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		if(task_id >= 75 && task_id <= 87){
			msglog(statistic_logfile, 0x0406BAEF + (task_id - 75), get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}

	int i = sizeof (protocol_t);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int td_game_kill_monster_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t count = 0, monster_id = 0, monster_count = 0,
			item_id = 0, item_count = 0, item_kind_count = 0, get_item_count = 0;
	int j = 0;
	UNPKG_UINT32(body, count, j);
	CHECK_BODY_LEN(len, 4 + count * 8);
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, count, i);

	int loop = 0, loop_monster = 0;
	for(loop = 0; loop < count; loop ++){
		UNPKG_UINT32(body, monster_id, j);
		UNPKG_UINT32(body, monster_count, j);

		if(monster_id < 10000 || monster_id > 10506){
			ERROR_LOG("td_game_kill_monster_cmd un define monster_id:%u user:%u", monster_id, p->id);
			continue;
		}
		if(p->td_task_id < 1 || p->td_task_id > TD_TASKS_TABLE_SIZE){
			ERROR_LOG("td_game_kill_monster_cmd taskid err taskid:[%u %u]", p->id, p->td_task_id);
			continue;
		}
		//概率计算
		for(loop_monster = 0; loop_monster < monster_count; loop_monster ++){
			uint32_t drop_rate = roles_info.roles[monster_id - 10000].rate *
								td_tasks_info[p->td_task_id - 1].rate_scale;

			if((rand() % 100) < drop_rate){
				get_item_count ++;
			}
		}

		PKG_UINT32 (msg, monster_id, i);
		//如果产生战利品
		if(get_item_count != 0){
			item_id = 1353433 + monster_id - 10000;
			item_count = roles_info.roles[monster_id - 10000].count * get_item_count;
			item_kind_count = 1;
			p->drop_award_count += item_count;
			PKG_UINT32 (msg, item_kind_count, i);
			PKG_UINT32 (msg, item_id, i);
			PKG_UINT32 (msg, item_count, i);
		}else {
			item_kind_count = 0;
			PKG_UINT32 (msg, item_kind_count, i);
		}

		item_kind_count = 0;
		p->killed_monster_count += 1;

	}

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}


int td_game_end_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN_GE(len, 28);
	CHECK_VALID_ID(p->id);
	uint32_t now = time(NULL);
	uint32_t old_time = p->td_game_start_time,
		     killed_monster_count = p->killed_monster_count,
		     drop_award_count = p->drop_award_count;
	p->td_game_start_time = 0;
	p->killed_monster_count = 0;
	p->drop_award_count = 0;

	uint32_t task_id = 0, is_pass = 0, score = 0, count_monster = 0, count_award_kind = 0, angel_count = 0,
			 award_id = 0, award_count = 0, enemy_id = 0, enemy_count = 0, barrier_level = 0;
	int j = 0;
	UNPKG_UINT32(body, task_id, j);
	task_id %= 10000;
	UNPKG_UINT32(body, is_pass, j);   //1过关 否则不过关
	UNPKG_UINT32(body, barrier_level, j);//轮数
	UNPKG_UINT32(body, score, j);
	UNPKG_UINT32(body, count_monster, j);
	UNPKG_UINT32(body, count_award_kind, j);
	UNPKG_UINT32(body, angel_count, j);

	DEBUG_LOG("===id:%u task_id:%u is_pass:%u barrier_level:%u award_count:%u score:%u count_monster:%u count_award_kind:%u angel_count:%u",
				p->id, task_id, is_pass, barrier_level, award_count, score, count_monster, count_award_kind, angel_count);
	CHECK_BODY_LEN(len, 28 + count_monster * 8 + count_award_kind * 8 + angel_count * 8);

	if(task_id < 1 || task_id > TD_TASKS_TABLE_SIZE || barrier_level < 0 || barrier_level > ROUND_COUNT ){
		ERROR_LOG("===td_end data_err task_id:%u, is_pass:%u, award_count:%u, score:%u, count_monster:%u, count_award:%u",
					task_id, is_pass, award_count, score, count_monster, count_award_kind);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint32_t tmp_time = 0;
	if(barrier_level == 0){
		tmp_time = 0 ;
	}else{
		tmp_time = td_tasks_info[task_id - 1].round_monster[barrier_level - 1].time_limit / 1000;
	}

	DEBUG_LOG("===id:%u now:%u old_time:%u time_limit:%u exp_limit;%u, monster_max:%u, drop_award:%u",
		p->id, now, old_time,
		tmp_time,
		td_tasks_info[task_id - 1].round_monster[barrier_level].exp_limit,
		td_tasks_info[task_id - 1].round_monster[barrier_level].total_count,
		drop_award_count);

	//时间是否合法
	// (old_time == 0) ||
	if( (old_time == 0) ||(now < old_time) ||( (now - old_time) < tmp_time)){
		ERROR_LOG("===td_end time_err userid:%u time_limit:%u real_time:%u now:%u",
					  p->id, tmp_time, (now - old_time), now);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	int loop;
	uint32_t total_exp = 0;
	for(loop = 0; loop < count_monster; loop++){
		UNPKG_UINT32(body, enemy_id, j);
		UNPKG_UINT32(body, enemy_count, j);
		//计算经验
		if(enemy_id < 10000 || enemy_id > 10506){
			ERROR_LOG("===td_end data_err, enemy_id=%u", enemy_id);
			return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);

		}

		total_exp += roles_info.roles[enemy_id - 10000].exp * enemy_count * td_tasks_info[task_id - 1].exp_scale;
	}

	//经验是否合法
	if(total_exp > td_tasks_info[task_id - 1].round_monster[barrier_level].exp_limit){
		total_exp = td_tasks_info[task_id - 1].round_monster[barrier_level].exp_limit;
	}
	//战胜则标记相应boss被击败
	if( is_pass == 1 )
	{
		if( p->dungeon_area_id_for_boss >= 0 && p->dungeon_on_explore_screen_id > 0 )
		{
			p->dungeon_area_explore_counter[p->dungeon_on_explore_screen_id - 1][(uint8_t)p->dungeon_area_id_for_boss] = 1;
			p->dungeon_area_id_for_boss = -1;
		}
	}

	//获取战利品数量
	uint32_t db_award_count = 0;//战利品数量
	for(loop = 0; loop < count_award_kind; loop++){
		UNPKG_UINT32(body, award_id, j);
		UNPKG_UINT32(body, award_count, j);

		if(award_id == 1353433){//
			//判断战利品是否合法
			if((killed_monster_count > td_tasks_info[task_id - 1].round_monster[barrier_level].total_count) ||
				(drop_award_count < award_count)){
				ERROR_LOG("===td_end kill err userid:%u, killed:%u, max:%u, drop:%u, max:%u",
							p->id, killed_monster_count,
							td_tasks_info[task_id - 1].round_monster[barrier_level].total_count,
							drop_award_count, award_count);
				return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
			}
			db_award_count = award_count;
		}
		DEBUG_LOG("---award_count:%u %u %u", award_id,award_count, db_award_count);
	}

	*(uint32_t *)p->session = task_id;
	uint32_t dbbuf[1024];
	int cnt = 0;
	dbbuf[cnt ++] = td_tasks_info[task_id - 1].mapid;
	dbbuf[cnt ++] = td_tasks_info[task_id - 1].mode;
	dbbuf[cnt ++] = score;
	dbbuf[cnt ++] = total_exp;
	dbbuf[cnt ++] = is_pass;
	dbbuf[cnt ++] = db_award_count;

	//61-64号塔防从6月3号到17号在晚上8-9点有不同的次数限制
	const struct tm* cur_time = get_now_tm();
	uint32_t limit_flag = 0;
	if (task_id >= 61 && task_id <= 64 && cur_time->tm_mon == 5
#ifndef TW_VER
			&& cur_time->tm_mday >= 3 && cur_time->tm_mday < 17
#else
			&& cur_time->tm_mday >= 3 && cur_time->tm_mday < 24
#endif
			&& cur_time->tm_hour >= 20 && cur_time->tm_hour < 21) {
		limit_flag = 1;
	}
	dbbuf[cnt ++] = limit_flag;
	//圣诞天使挑战赛
	if( p->td_task_id > 64 && p->td_task_id <= 64 + TD_CHALLENGE_GAME_CNT )
	{
		if( 0 == p->td_task_mode )
		{
			angel_count = 0;
		}
	}
	dbbuf[cnt ++] = angel_count;
	uint32_t tmp_angel_id = 0, tmp_angel_cout = 0, tmp_count_heart = 0;
	for(loop = 0; loop < angel_count; loop ++){
		UNPKG_UINT32(body, tmp_angel_id, j);
		UNPKG_UINT32(body, tmp_angel_cout, j);
		dbbuf[cnt ++] = tmp_angel_id;
		dbbuf[cnt ++] = tmp_angel_cout;
		tmp_count_heart ++;
	}
	if(tmp_count_heart > 0){
		uint32_t msgbuff[2] = {p->id, tmp_count_heart};
		msglog(statistic_logfile, 0x0406FAB0, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	{
		uint32_t msgbuff[2] = {p->id, drop_award_count};
		msglog(statistic_logfile, 0x0406DAA9 + task_id - 5, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		uint32_t msgbuff_2[2] = {p->id, db_award_count};
		msglog(statistic_logfile, 0x0406EAA9 + task_id - 5, get_now_tv()->tv_sec, msgbuff_2, sizeof(msgbuff_2));
	}
	if(is_pass == 1){
		if(task_id >= 75 && task_id <= 86){
			{
				uint32_t msgbuff[2] = {p->id, 1};
				msglog(statistic_logfile, 0x0406CAEF + (task_id - 75), get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
			}
		}
	}
	uint32_t dblen = cnt * 4;
	DEBUG_LOG("td_game_end_cmd id:%u len;%u", p->id, cnt * 4);
	return db_td_game_end(p, dblen, dbbuf);

}

int td_game_end_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 28);

	uint32_t task_id = *(uint32_t *)p->session;
	td_game_end_t *p_game_end = (td_game_end_t*)buf;
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, p_game_end->barrier_id, i);
	PKG_UINT32 (msg, p_game_end->experience, i);
	PKG_UINT32 (msg, p_game_end->level, i);

	if(p_game_end->honor_type)
	{
		notify_user_get_honor(p, p_game_end->honor_type, p_game_end->honor_id);
	}

	if(task_id < 1 || task_id > TD_TASKS_TABLE_SIZE || p_game_end->mode <0 || p_game_end->mode > 3){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	uint32_t item_count			= 0;
	uint32_t item_id			= 0;
	uint32_t rarity_item		= 0;
	uint32_t rarity_count		= 0; 
	uint32_t item_kind_count	= 0;
	if(p_game_end->is_pass == 1 && p_game_end->mode != 0){
		if(p_game_end->mode == 3){
			item_count = 2;
		}else {
			item_count = 1;
		}
	}
	uint32_t tmp_count_index = i;
	i += 4;

	//set award for 20111223 challenge game
	if( p->td_task_id > 64 && p->td_task_id <= 64 + TD_CHALLENGE_GAME_CNT )
	{
		if( 0 == p->td_task_mode )
		{
			if( ISVIP(p->flag) )
			{
				p->td_award_left = 2;
			}
			else
			{
				p->td_award_left = 1;
			}
		}
		else
		{
			if( ISVIP(p->flag) )
			{
				p->td_award_left = 3;
			}
			else
			{
				p->td_award_left = 2;
			}
		}
	}
	if(p_game_end->is_pass == 1 && p_game_end->mode != 0){
		uint32_t award[TD_TASKS_TABLE_SIZE][3][2] = {
												  {{1353100, 1}, {0, 0}},
												  {{1353209, 1}, {0, 0}},
												  {{1353209, 2}, {0, 0}},
												  {{0, 0}, {0, 0}},
												  {{1353100, 1}, {0, 0}},
												  {{1353209, 1}, {0, 0}},
												  {{1353209, 2}, {0, 0}},
												  {{1353100, 1}, {0, 0}},
												  {{1353211, 1}, {0, 0}},
												  {{1353211, 2}, {1353210, 20}},
												  {{1353100, 1}, {0, 0}},
												  {{1353215, 1}, {0, 0}},
												  {{1353215, 2}, {1353214, 20}},
												  {{1353103, 1}, {0, 0}},
												  {{1353216, 1}, {0, 0}},
												  {{1353216, 2}, {1353217, 20}},
												  {{0, 0}, {0, 0}},
												  {{1353103, 1}, {0, 0}},
												  {{1353221, 1}, {0, 0}},
												  {{1353221, 2}, {1353220, 20}},
												  {{1351053, 5}, {0, 0}},//21
												  {{1351053, 10}, {0, 0}},//22
												  {{1351053, 20}, {0, 0}},//23
												  {{1351053, 10}, {0, 0}},//24
												  {{1351053, 10}, {0, 0}},//25
												  {{1351053, 5}, {0, 0}},//26
												  {{1351053, 10}, {0, 0}}, //27
												  {{1351053, 10}, {0, 0}}, //28
												  {{1351053, 40}, {0, 0}}, //29
												  {{1351053, 20}, {0, 0}}, //30
												  {{1453000, 2}, {0, 0}}, //31
												  {{1453001, 2}, {0, 0}}, //32
												  {{1453002, 2}, {0, 0}}, //33
												  {{1453005, 1}, {0, 0}}, //34
												  {{0, 0}, {1353107, 35}}, //35
												  {{1353106, 1}, {0, 0}}, //36
												  {{1453008, 1}, {0, 0}}, //37
												  {{1353231, 1}, {0, 0}}, //38
												  {{1453014, 1}, {0, 0}}, //39
												  {{1453015, 1}, {0, 0}}, //40
												  {{1453016, 1}, {0, 0}}, //41
												  {{1453002, 3}, {0, 0}}, //42
												  {{1453017, 1}, {0, 0}}, //43
												  {{1353234, 1}, {1453013, 30}}, //44
												  {{1453018, 1}, {0, 0}}, //45
												  {{1453000, 3}, {0, 0}}, //46
												  {{1453001, 5}, {0, 0}}, //47
												  {{1453003, 1}, {1353241, 30}}, //48
												  {{1453020, 1}, {0, 0}}, //49
												  {{1453021, 1}, {0, 0}}, //50
												  {{1453022, 1}, {0, 0}}, //51
												  {{1453023, 1}, {0, 0}}, //52
												  {{1453059, 1}, {0, 0}}, //53
												  {{1453056, 1}, {0, 0}}, //54
												  {{1453057, 1}, {0, 0}}, //55
												  {{1453058, 1}, {1353246, 50}}, //56
												  {{1453004, 2}, {0, 0}}, //57
												  {{1453000, 10}, {0, 0}}, //58
												  {{1453013, 2}, {0, 0}}, //59
												  {{1453055, 1}, {1353249, 50}}, //60
												  {{1351058, 15}, {0, 0}}, //61
												  {{1351058, 20}, {0, 0}}, //62
												  {{1351057, 20}, {0, 0}}, //63
												  {{1351057, 20}, {0, 0}}, //64
												  {{0, 0}, {0, 0}},
												  {{0, 0}, {0, 0}},
												  {{0, 0}, {0, 0}},
												  {{0, 0}, {0, 0}},
												  {{0, 0}, {0, 0}},
												  {{0, 0}, {0, 0}},
												  {{0, 0}, {0, 0}},
												  {{0, 0}, {0, 0}},
												  {{0, 0}, {0, 0}},
												  {{0, 0}, {0, 0}},
												  {{1351328, 1}, {0, 0}},//75
												  {{1351328, 2}, {0, 0}},//76
												  {{1351328, 3}, {0, 0}},//77
												  {{1351328, 1}, {0, 0}},//78
												  {{1351328, 2}, {0, 0}},//79
												  {{1351328, 3}, {0, 0}},//80
												  {{1351328, 1}, {0, 0}},//81
												  {{1351328, 2}, {0, 0}},//82
												  {{1351328, 3}, {0, 0}},//83
												  {{1351328, 3}, {0, 0}},//84
												  {{1351328, 5}, {0, 0}},//85
												  {{1351328, 10}, {0, 0}},//86
		};

		if (task_id >= 1 && task_id <= TD_TASKS_TABLE_SIZE){

			item_id = award[task_id - 1][0][0];
			item_count = award[task_id - 1][0][1];

			//taskid为61时，在6.03-6.17日之间的20-21点送的东西不一样
			const struct tm* cur_time = get_now_tm();
			if (task_id >= 61 && task_id <= 64 && cur_time->tm_mon == 5
#ifndef TW_VER
					&& cur_time->tm_mday >= 3 && cur_time->tm_mday < 17
#else
					&& cur_time->tm_mday >= 3 && cur_time->tm_mday < 24
#endif
					&& cur_time->tm_hour >= 20 && cur_time->tm_hour < 21)
			{
				item_id = 1351059;
				if (task_id == 61 || task_id == 62) {
					item_count = 20;
				} else if (task_id == 63 || task_id == 64) {
					item_count = 25;
				}
			}

			if(item_id != 0 && item_count != 0){
				PKG_UINT32 (msg, item_id, i);
				PKG_UINT32 (msg, item_count, i);

				item_kind_count ++;
				db_exchange_single_item_op(p, 202, item_id, item_count, 0);
			}
			if(task_id >= 75 && task_id <= 83){
				rarity_item = award[task_id - 1][1][0];
				rarity_count = award[task_id - 1][1][1];
				PKG_UINT32 (msg, rarity_item, i);
				PKG_UINT32 (msg, rarity_count, i);
				item_kind_count ++;
				db_exchange_single_item_op(p, 202, rarity_item, rarity_count, 0);
			}
			else if(task_id >= 84 && task_id <= 86){
				;
			}
			else if(task_id == 87){
				;
			}
			else{
				if((rand() % 100) < award[task_id - 1][1][1]){
					rarity_item = award[task_id - 1][1][0];
					rarity_count = 1;
					PKG_UINT32 (msg, rarity_item, i);
					PKG_UINT32 (msg, rarity_count, i);
					item_kind_count ++;
					db_exchange_single_item_op(p, 202, rarity_item, rarity_count, 0);
				}
			}
		}
	}

	PKG_UINT32 (msg, item_kind_count, tmp_count_index);


	DEBUG_LOG("td_game_end_callback id%u item_id:%u item_count:%u barrier_id:%u experience:%u***",
		      p->id, item_id, item_count, p_game_end->barrier_id, p_game_end->experience);
	{
		uint32_t msgbuff[2]= {p->id, 1};

		msglog(statistic_logfile, 0x0406BAA9 + task_id - 5, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

		if(p_game_end->is_pass == 1){
			msglog(statistic_logfile, 0x0406CAA9 + task_id - 5, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}

		if(task_id > 20 && task_id < 31){
			msglog(statistic_logfile, 0x0406FACE, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}

		if(task_id > 30){
			msglog(statistic_logfile, 0x04088334, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}

	//圣诞天使挑战赛，通过与否统计
	if( p->td_task_id == 64 + 1 )
	{
		uint32_t msglog_id;
		if( 0 == p->td_task_mode )
		{
			if( 1 == p_game_end->is_pass )
			{
				msglog_id = 0x0409C2C9;
			}
			else
			{
				msglog_id = 0x0409C2CA;
			}
		}
		else
		{
			if( 1 == p_game_end->is_pass )
			{
				msglog_id = 0x0409C2CB;
			}
			else
			{
				msglog_id = 0x0409C2CC;
			}
		}
		uint32_t msgbuff[2] = {p->id, 1};
		msglog(statistic_logfile, msglog_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);

}


int get_fight_level_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	return send_request_to_db(SVR_PROTO_FIGHT_LEVEL, p, 0, NULL, p->id);
}

int get_fight_level_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);
	uint32_t level = 0, exp = 0, victory_count = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, level, j);
	UNPKG_H_UINT32(buf, exp, j);
	UNPKG_H_UINT32(buf, victory_count, j);

	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, level, i);
	PKG_UINT32 (msg, exp, i);
	PKG_UINT32 (msg, victory_count, i);

	init_proto_head (msg, p->waitcmd, i);

	{
		uint32_t msgbuff[2]= {p->id, exp};
		msglog(statistic_logfile, 0x0405CAA6, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	return send_to_self (p, msg, i, 1);
}

int get_fight_score_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t barrier_id;
	if (unpkg_uint32(body, len, &barrier_id) == -1) return -1;

	CHECK_VALID_ID(p->id);

	uint32_t dbbuf[] = {barrier_id};
	return db_get_fight_score(p, dbbuf);
}

int get_fight_score_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 16);
	uint32_t barrier_id = 0, easy_score = 0, nomal_score = 0, hard_score = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, barrier_id, j);
	UNPKG_H_UINT32(buf, easy_score, j);
	UNPKG_H_UINT32(buf, nomal_score, j);
	UNPKG_H_UINT32(buf, hard_score, j);
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, barrier_id, i);
	PKG_UINT32 (msg, easy_score, i);
	PKG_UINT32 (msg, nomal_score, i);
	PKG_UINT32 (msg, hard_score, i);

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}


static int parse_single_monster_info(monster_info_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"monster")))
		{

			DECODE_XML_PROP_INT (iut[j].id, cur, "id");
			DECODE_XML_PROP_INT_DEFAULT (iut[j].time, cur, "time", 0);
			DECODE_XML_PROP_INT_DEFAULT (iut[j].count, cur, "count", 0);
			DECODE_XML_PROP_INT_DEFAULT (iut[j].stime, cur, "stime", 0);
			j++;
		}
		cur = cur->next;
	}

	*cnt = j;
	return 0;
}



int load_tdtask_all( )
{
	char file_name[64];
	uint32_t i = 0;
	for (i = 0; i < TD_TASKS_TABLE_SIZE; i++)
	{
		memset(file_name, 0, sizeof(file_name));
		snprintf(file_name, sizeof(file_name), "./conf/TDTask%u.xml", i+1);
		//DEBUG_LOG("===file:%s ===", file_name);
		if (load_tdtask_info(file_name, &td_tasks_info[i]) == -1)
		{
			return -1;
		}
	}

	BOOT_LOG (0, "Load all td task file ok");

	if( angel_load_game_award_info( "./conf/TD20111223AwardInfo.xml" ) == -1 )
	{
		return -1;
	}
	return 0;
}


int load_tdtask_info(const char *file, td_task_info_t* p_td_task_info)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int err = -1;
	int ex_count;

	ex_count = 0;
	memset(p_td_task_info, 0, sizeof (td_task_info_t));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	DECODE_XML_PROP_INT(p_td_task_info->mapid, cur, "mapID");
	DECODE_XML_PROP_INT_DEFAULT(p_td_task_info->mode, cur, "mode", 0);
	DECODE_XML_PROP_INT_DEFAULT(p_td_task_info->start_time, cur, "starTime", 0);
	decode_xml_prop_float_default(&(p_td_task_info->exp_scale), cur, "expScale", 1.0);
	decode_xml_prop_float_default(&(p_td_task_info->rate_scale), cur, "rateScale", 1.0);
	//DEBUG_LOG("===file:%s mapID:%u mode:%u starTime:%u exp_scale %f rateScale:%f", file,
	//		p_td_task_info->mapid, p_td_task_info->mode, p_td_task_info->start_time,
	//		p_td_task_info->exp_scale, p_td_task_info->rate_scale);

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"round"))) {
			p_td_task_info->round_monster[ex_count].round_id = ex_count + 1;
			chl = cur->xmlChildrenNode;
			if ( (parse_single_monster_info(p_td_task_info->round_monster[ex_count].monsters,
				&(p_td_task_info->round_monster[ex_count].count), chl) != 0) )
			{
				goto exit;
			}
			count_time_exp_limit(p_td_task_info, ex_count);
			ex_count++;
		}
		cur = cur->next;
	}

	p_td_task_info->round_count = ex_count;
	err = 0;
exit:
	xmlFreeDoc (doc);
    BOOT_LOG (err, "Load td task file %s", file);

}


static int parse_single_role_info(role_info_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	int id = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"role")))
		{
		    DECODE_XML_PROP_INT (id, cur, "id");
		    if (id >= 10000 && id < 10500)
		    {
			    iut[id - 10000].id = id;
			    DECODE_XML_PROP_INT_DEFAULT (iut[id - 10000].exp, cur, "exp", 0);
			    DECODE_XML_PROP_INT_DEFAULT (iut[id - 10000].rate, cur, "rate", 0);
			    DECODE_XML_PROP_INT_DEFAULT (iut[id - 10000].count, cur, "count", 0);
			    j++;
			}
		}
		cur = cur->next;
	}

	*cnt = j;
	return 0;
}

int load_td_role_info(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int err = -1;
	int ex_count;

	ex_count = 0;
	memset(&roles_info, 0, sizeof (roles_info));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"roles"))) {

			chl = cur->xmlChildrenNode;
			if ( (parse_single_role_info(roles_info.roles, &(roles_info.role_count), chl) != 0) )
			{
				goto exit;
			}

			ex_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load td task file %s", file);

}


int count_time_exp_limit(td_task_info_t* p_td_task_info, int round)
{
	float exp_scale = p_td_task_info->exp_scale;

	uint32_t loop = 0, max_time = 0, max_exp = 0, total_count = 0;
	//DEBUG_LOG("===mapid:%u===round:%u==========", p_td_task_info->mapid, round);
	for(loop = 0; loop < p_td_task_info->round_monster[round].count; loop++){
		//每批次怪物数
		uint32_t count = p_td_task_info->round_monster[round].monsters[loop].count;
		//每批次怪间隔时间
		uint32_t interval_time = p_td_task_info->round_monster[round].monsters[loop].time;
		//怪出现时间
		uint32_t start_time = p_td_task_info->round_monster[round].monsters[loop].stime;
		//怪物id
		uint32_t monster_id = p_td_task_info->round_monster[round].monsters[loop].id;

		//获取经验
		if(monster_id >= 10000 && monster_id <= 10500){
			max_exp += roles_info.roles[monster_id - 10000].exp * count * exp_scale;
		}

		uint32_t tmp_time = ((count-1) * interval_time + start_time) / 3;//改批次怪所需要的时间
		max_time = MAX_VALUE(max_time, tmp_time);
		total_count += count;

	}


	if(round >= 1){
		p_td_task_info->round_monster[round].time_limit = max_time +
						p_td_task_info->round_monster[round-1].time_limit;

		p_td_task_info->round_monster[round].exp_limit = max_exp +
						p_td_task_info->round_monster[round-1].exp_limit;
		p_td_task_info->round_monster[round].total_count =total_count + p_td_task_info->round_monster[round-1].total_count;

	}else if (round == 0){
		//p_td_task_info->round_monster[round].time_limit = max_time + p_td_task_info->start_time / 3;
		p_td_task_info->round_monster[round].time_limit = max_time;
		p_td_task_info->round_monster[round].exp_limit = max_exp;
		p_td_task_info->round_monster[round].total_count = 0 + total_count;
	}

	return 0;
}

int clean_black_angel_start_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t clothes_id;
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);
	int j = 0;
	UNPKG_UINT32(body, clothes_id, j);
	{
		if(clothes_id == 190850){
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0406FACC, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}
	p->clean_angel_game_start_time = time(NULL);
	uint32_t db_buf[] = {1, 0, 0, 0, 0, clothes_id, 1};
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, sizeof(db_buf), db_buf, p->id);
}

int clean_black_angel_sucess_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t angel_id, ball_id;
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);
	int j = 0;
	UNPKG_UINT32(body, angel_id, j);
	UNPKG_UINT32(body, ball_id, j);
	uint32_t now = time(NULL);
	if(now  - p->clean_angel_game_start_time > 200){
		ERROR_LOG("===clean_black_angel_sucess_cmd end alread userid:%u , now:%u, start_time:%u",
			p->id, now, p->clean_angel_game_start_time);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint32_t dbbuf[] = {angel_id, ball_id};
	DEBUG_LOG("clean_black_angel_sucess_cmd p->id :%u angel_id:%u ball_id:%u", p->id, angel_id, ball_id);
	return db_clean_black_angel_sucess(p, dbbuf);
}

int clean_black_angel_sucess_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t angel_id = 0, ball_id;
	int j = 0;
	UNPKG_H_UINT32(buf, angel_id, j);
	UNPKG_H_UINT32(buf, ball_id, j);

	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, angel_id, i);
	PKG_UINT32 (msg, ball_id, i);

	DEBUG_LOG ("clean_black_angel_sucess_callback CALLBACK\t[%u %u %u]", p->id, angel_id, i);

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int clean_black_angel_end_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	DEBUG_LOG("clean_black_angel_end_cmd id:%u, time:%u", p->id, p->clean_angel_game_start_time);
	p->clean_angel_game_start_time = 0;

	int i = sizeof (protocol_t);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int get_angel_in_hospital_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);

	return db_get_angel_in_hospital(p);
}

int get_angel_in_hospital_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t count = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, 4 + count * 16);
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, count, i);
	int loop = 0;
	for(loop = 0; loop < count; loop++){
		uint32_t angel_id, angel_index, time, state;
		UNPKG_H_UINT32 (buf, angel_index, j);
		UNPKG_H_UINT32 (buf, angel_id, j);
		UNPKG_H_UINT32 (buf, time, j);
		UNPKG_H_UINT32 (buf, state, j);
		PKG_UINT32 (msg, angel_id, i);
		PKG_UINT32 (msg, angel_index, i);
		PKG_UINT32 (msg, time, i);
	}
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);

}

int use_tear_of_jack_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t angel_index = 0;
	int j = 0;
	UNPKG_UINT32(body, angel_index, j);

	uint32_t dbbuf[] = {angel_index};
	return db_use_tear_of_jack(p, dbbuf);
}
int use_tear_of_jack_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t angel_index = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, angel_index, j);

	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, angel_index, i);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int angel_leave_hospotal_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t count = 0, dbbuf[1024] = {0};
	int cnt = 1;
	int j = 0;
	UNPKG_UINT32(body, count, j);
	if(count > 1000){
		ERROR_LOG("===angel_leave_hospotal_cmd angel too much userid:%u , count:%u",
			p->id, count);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	CHECK_BODY_LEN(len, 4 + count * 4);
	dbbuf[0] = count;

	int loop = 0;
	uint32_t angel_index;
	for(loop = 0; loop < count; loop++){
		UNPKG_UINT32 (body, angel_index, j);
		dbbuf[cnt++] = angel_index;
	}
	uint32_t buflen = 4 + dbbuf[0] * 4;
	DEBUG_LOG("angel_leave_hospotal_cmd:[%u %u]", p->id, buflen);
	return db_angel_leave_hospotal(p, buflen, dbbuf);
}

int angel_leave_hospotal_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = sizeof (protocol_t);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int use_prop_for_td_cmd(sprite_t* p, const uint8_t* body, int len)
{
    DEBUG_LOG("use_prop_for_td_cmd:[%u]", p->id);

	uint32_t item_id;
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);
	int j = 0;
	UNPKG_UINT32(body, item_id, j);
	if(item_id < 1353381 || item_id > 1353400){
		ERROR_LOG("===use_prop_for_td_cmd item_id id:%u , item_id:%u", p->id, item_id);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	uint32_t db_buf[] = {1, 0, 0, 0, 0, item_id, 1};
	{
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0406FAB8 + (item_id - 1353381), get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, sizeof(db_buf), db_buf, p->id);
}

int get_prop_for_td_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	int j = 0;
	uint8_t buff[13];
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 1353381, j);
	PKG_H_UINT32(buff, 1353400, j);
	PKG_UINT8(buff, 2, j);
	return send_request_to_db( SVR_PROTO_GET_ITEM, p, j, buff, p->id);
}

int add_angel_cmd(sprite_t * p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);

	CHECK_BODY_LEN(len,4);
	uint32_t type = 0;
	int i = 0;
	UNPKG_UINT32(body, type, i);
	if (type != 1 && type != 2)
	{
	   return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if (type == 1)
	{
	    if(!set_only_one_bit(p, 108))
	    {
		    return send_to_self_error(p, p->waitcmd, -ERR_have_add_angle, 1);
	    }
	    uint32_t kind_count = 1;
	    uint32_t angel_id = 1354069;
	    uint32_t angel_count = 1;
	    uint32_t angel_type = 1;	// 1 as fight, 2 as ride, 3 for two, 0 as none
	    uint8_t db_buf[128] = {};
	    int j = 0;
	    PKG_H_UINT32(db_buf,kind_count,j);
	    PKG_H_UINT32(db_buf,angel_id,j);
	    PKG_H_UINT32(db_buf,angel_type,j);
	    PKG_H_UINT32(db_buf,angel_count,j);
	    return send_request_to_db(SVR_PROTO_ADD_ANGEL, p, j, db_buf, p->id);
	}
	else
	{
	    if(!set_only_one_bit(p, 109))
	    {
		    return send_to_self_error(p, p->waitcmd, -ERR_have_add_angle, 1);
	    }
	    uint32_t kind_count = 1;
	    uint32_t angel_id = 1353404;
	    uint32_t angel_count = 1;
	    uint32_t angel_type = 0;	// 1 as fight, 2 as ride, 3 for two, 0 as none
	    uint8_t db_buf[128] = {};
	    int j = 0;
	    PKG_H_UINT32(db_buf,kind_count,j);
	    PKG_H_UINT32(db_buf,angel_id,j);
	    PKG_H_UINT32(db_buf,angel_type,j);
	    PKG_H_UINT32(db_buf,angel_count,j);
	    return send_request_to_db(SVR_PROTO_ADD_ANGEL, p, j, db_buf, p->id);
	}

}

int add_angel_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t kind_count;
	uint32_t angel_id;
	uint32_t angel_count;
	int j = 0;
	int k = sizeof (protocol_t);
	UNPKG_H_UINT32(buf, kind_count, j);
	PKG_UINT32 (msg, kind_count, k);
	DEBUG_LOG ("add_angel_callback CALLBACK\t[%u kind count:%u]", p->id, kind_count);
	uint32_t i;
	for( i = 0; i < kind_count; ++i)
	{
		UNPKG_H_UINT32(buf, angel_id, j);
		UNPKG_H_UINT32(buf, angel_count, j);

		PKG_UINT32 (msg, angel_id, k);
		PKG_UINT32 (msg, angel_count, k);
		DEBUG_LOG ("add_angel_callback CALLBACK\t[%u AngelID:%u Count:%u]", p->id, angel_id, angel_count);
	}

	init_proto_head (msg, p->waitcmd, k);

	return send_to_self (p, msg, k, 1);
}

int angel_show_cmd(sprite_t * p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len,4);
	uint32_t angel_id;
	int j = 0;
	UNPKG_UINT32(body, angel_id, j);
	uint32_t db_buf[]={angel_id, 1};
	DEBUG_LOG ("angel_show_cmd\t[%u AngelID:%u", p->id, angel_id);
	return send_request_to_db(SVR_PROTO_ANGEL_SHOW, p, sizeof(db_buf), db_buf, p->id);
}

int angel_show_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	DEBUG_LOG ("angel_show_callback");

	CHECK_BODY_LEN(len, 4);
	uint32_t angel_id;
	int j = 0;
	UNPKG_H_UINT32(buf, angel_id, j);
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, angel_id, k);
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int angel_regain_cmd(sprite_t * p, const uint8_t* body, int len)
{
	DEBUG_LOG ("angel_regain_cmd");

	CHECK_VALID_ID(p->id);
	uint32_t db_buf[]={0, 0};
	return send_request_to_db(SVR_PROTO_ANGEL_SHOW, p, sizeof(db_buf), db_buf, p->id);
}

int angel_get_favorite_info_cmd(sprite_t * p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t user_id;
	int j = 0;
	UNPKG_UINT32(body, user_id, j);
	DEBUG_LOG ("angel_get_favorite_info_cmd");

	CHECK_VALID_ID(p->id);
	return send_request_to_db(SVR_PROTO_ANGEL_GET_FAVORITE_INFO, p, 0, NULL, user_id);
}

int angel_get_favorite_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	DEBUG_LOG ("angel_get_favorite_info_callback");
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t kind_count;

	int j = 0;
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, id, k);
	UNPKG_H_UINT32(buf, kind_count, j);
	PKG_UINT32(msg, kind_count, k);
	int i;
	for(i = 0; i< kind_count; ++i)
	{
		uint32_t angel_id;
		UNPKG_H_UINT32(buf, angel_id, j);
		PKG_UINT32(msg, angel_id, k);
	}
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int get_angel_paradise_background_cmd(sprite_t * p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	return send_request_to_db(SVR_PROTO_GET_BACKGROUND_ID, p, 0, NULL, p->id);
}

int get_angel_paradise_background_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count_item = 0;
	int j = 0;
	UNPKG_H_UINT32 (buf, count_item, j);
	CHECK_BODY_LEN(len, 4 + count_item*8);
	int loop, i = sizeof (protocol_t);
	PKG_UINT32 (msg, count_item, i);
	for (loop = 0; loop < count_item; loop++)
	{
		uint32_t itm, cnt;
		UNPKG_H_UINT32 (buf, itm, j);
		UNPKG_H_UINT32 (buf, cnt, j);
		PKG_UINT32 (msg, itm, i);
		PKG_UINT32 (msg, cnt, i);
	}

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int exchange_davis_bean_cmd(sprite_t * p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 0);
	return send_request_to_db(SVR_PROTO_EXCHANGE_DAVIS_BEAN, p, 0, NULL, p->id);
}

int exchange_davis_bean_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t itemid = 0, count = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, itemid, j);
	UNPKG_H_UINT32(buf, count, j);
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, k);
	PKG_UINT32(msg, count, k);
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int get_angel_paradise_honors_cmd(sprite_t * p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t userid = 0;
	int j = 0;
	UNPKG_UINT32(body, userid, j);
	return send_request_to_db(SVR_PROTO_GET_ANGEL_HONORS, p, 0, NULL, userid);
}

int get_angel_paradise_honors_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);
	uint32_t count_angel = 0;
	uint32_t count_war = 0;
	int j = 0;
	UNPKG_H_UINT32 (buf, count_angel, j);
	UNPKG_H_UINT32 (buf, count_war, j);
	CHECK_BODY_LEN(len, 8 + count_angel*8 + count_war*8);
	int loop, i = sizeof (protocol_t);
	PKG_UINT32 (msg, count_angel, i);
	for (loop = 0; loop < count_angel; loop++)
	{
		uint32_t honorid = 0, flag = 0;
		UNPKG_H_UINT32 (buf, honorid, j);
		UNPKG_H_UINT32 (buf, flag, j);
		PKG_UINT32 (msg, honorid, i);
		PKG_UINT32 (msg, flag, i);
	}

	PKG_UINT32 (msg, count_war, i);
	for (loop = 0; loop < count_war; loop++)
	{
		uint32_t honorid = 0, flag = 0;
		UNPKG_H_UINT32 (buf, honorid, j);
		UNPKG_H_UINT32 (buf, flag, j);
		PKG_UINT32 (msg, honorid, i);
		PKG_UINT32 (msg, flag, i);
	}

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int angel_get_honor_bonus_cmd(sprite_t * p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t type = 0, honor_id = 0;
	int j = 0;
	UNPKG_UINT32(body, type, j);
	UNPKG_UINT32(body, honor_id, j);

    if (type < 1 || type > 2 || honor_id < 1 || honor_id > 32)
    {
        ERROR_LOG("===angel_get_honor_bonus_cmd userid:%u,type:%u, honor:%u",
			p->id, type, honor_id);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
    }

	*(uint32_t*)p->session = type;
	*(uint32_t*)(p->session + 4) = honor_id;
	uint32_t dbbuf[] = {type, honor_id};
	return send_request_to_db(SVR_PROTO_SET_ANGEL_HONORS, p, sizeof(dbbuf), dbbuf, p->id);

}

int angel_get_honor_bonus_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t type = 0, honor_id = 0;
	type =  *(uint32_t*)p->session;
	honor_id =  *(uint32_t*)(p->session + 4);
	honor_item_t * p_item = &(honors_bonus[type-1].item_honor[honor_id-1]);

    if (type == 1)
    {
        uint32_t msgbuff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0406FB94+honor_id-1, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
    }
    else
    {
        uint32_t msgbuff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0406FBF3+honor_id-1, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
    }

    int i = sizeof(protocol_t);
    PKG_UINT32 (msg, p_item->item_count, i);
    int loop = 0;
    for (loop = 0; loop < p_item->item_count; loop++)
    {
        PKG_UINT32 (msg, p_item->item[loop].itm->id, i);
        PKG_UINT32 (msg, p_item->item[loop].count, i);

        if ((p_item->item[loop].itm->id >= 1353401 && p_item->item[loop].itm->id <= 1353432)
            || (p_item->item[loop].itm->id > 1354033))
        {
            uint32_t angel_type = 0;
            uint32_t j = 0;
            for (j = 0; j < 3; j++)
            {
                if (p_item->item[loop].itm->u.angel_tag.abilityNum[j] > 0)
                {
                    uint32_t nbit = 0;
                    if(p_item->item[loop].itm->u.angel_tag.abilityNum[j] == 1)
                    {
                        nbit = 2;
                    }
                    else if (p_item->item[loop].itm->u.angel_tag.abilityNum[j] == 2)
                    {
                        nbit = 1;
                    }
                    else
                    {
                        nbit = p_item->item[loop].itm->u.angel_tag.abilityNum[j];
                    }

                    angel_type = angel_type|(0x01<<(nbit-1));
                }
            }

            db_add_singel_angel(p->id, p_item->item[loop].itm->id, p_item->item[loop].count, angel_type);

        }
        else
        {
            db_exchange_single_item_op(p, 202, p_item->item[loop].itm->id, p_item->item[loop].count, 0);
        }
    }

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);

}

static int parse_honor_single_item(item_unit_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	int id = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item")))
		{
			if (j == 20)
			{
				ERROR_RETURN(("too many items"), -1);
			}
			DECODE_XML_PROP_INT (id, cur, "ID");
			if (!(iut[j].itm = get_item_prop(id)))
			{
				ERROR_RETURN(("can't find item=%d", id), -1);
			}

			DECODE_XML_PROP_INT (iut[j].count, cur, "Count");
			j++;
		}
		cur = cur->next;
	}

	*cnt = j;
	return 0;
}

static int parse_angel_honors(angel_honor_bonus_t *git, uint32_t* cnt, xmlNodePtr chl)
{
	xmlNodePtr cur;
	int i = 0;
	int honorid = 0;

	while (chl)
	{
		if (!xmlStrcmp(chl->name, (const xmlChar *)"Honor"))
		{
			DECODE_XML_PROP_INT(honorid, chl, "HonorID");
			git->item_honor[honorid-1].honor= honorid;

			cur = chl->xmlChildrenNode;
			if (parse_honor_single_item(git->item_honor[honorid-1].item, &(git->item_honor[honorid-1].item_count), cur) != 0)
			{
				return -1;
			}
			i++;
		}
		chl = chl->next;
	}

	*cnt = i;
	return 0;

}

int load_angel_honor_bonus(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int ex_count;

	ex_count = 0;
	memset(honors_bonus, 0, sizeof (honors_bonus));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"HonorType"))) {
			DECODE_XML_PROP_INT(i, cur, "Type");
			if (ex_count >= 2 || i <= 0) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}
			honors_bonus[i-1].angel_type = i;
			if (honors_bonus[i-1].angel_type > 2) {
				ERROR_LOG ("parse %s failed, Count=%d, honor_count=%d", file, ex_count, honors_bonus[i-1].angel_type);
				goto exit;
			}

			chl = cur->xmlChildrenNode;
			if ( (parse_angel_honors(&honors_bonus[i-1], &(honors_bonus[i-1].honor_count), chl) != 0) )
				goto exit;

			ex_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load angle honor bonus item file %s", file);

}

int db_add_singel_angel(uint32_t userid, uint32_t angel_id, uint32_t angel_count, uint32_t angel_type)
{
	uint32_t kind_count = 1;
	uint8_t db_buf[128] = {};
	int j = 0;
	PKG_H_UINT32(db_buf,kind_count,j);
	PKG_H_UINT32(db_buf,angel_id,j);
	PKG_H_UINT32(db_buf,angel_type,j);
	PKG_H_UINT32(db_buf,angel_count,j);
	return send_request_to_db(SVR_PROTO_ADD_ANGEL, NULL, j, db_buf, userid);

}

int db_add_singel_angel_ex(uint32_t userid, item_unit_t * iut)
{
	if ((iut->itm->id >= 1353401 && iut->itm->id <= 1353432)
            || (iut->itm->id > 1354033))
    {
		uint32_t angel_type = 0;
		uint32_t j = 0;
		for (j = 0; j < 3; j++)
		{
			if (iut->itm->u.angel_tag.abilityNum[j] > 0)
			{
				uint32_t nbit = 0;
				if(iut->itm->u.angel_tag.abilityNum[j] == 1)
				{
					nbit = 2;
				}
				else if (iut->itm->u.angel_tag.abilityNum[j] == 2)
				{
					nbit = 1;
				}
				else
				{
					nbit = iut->itm->u.angel_tag.abilityNum[j];
				}

				angel_type = angel_type|(0x01<<(nbit-1));
			}
		}

		return db_add_singel_angel(userid, iut->itm->id, iut->count, angel_type);
    }
	else		//如果是天使蛋，则用114D协议增加
	{
		uint32_t db_buf[] = {0, 1, 0, 0, 0, iut->itm->id, iut->count, 99999};
		return send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, userid);
	}

	return 0;
}

static int parse_compose_items(item_unit_t * iut, uint32_t * cnt, xmlNodePtr cur)
{
	int j = 0;
	int id = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item")))
		{
			if (j >= 4)	//材料不能多于4种
			{
				ERROR_RETURN(("too many items"), -1);
			}
			DECODE_XML_PROP_INT (id, cur, "ID");
			if (!(iut[j].itm = get_item_prop(id)))
			{
				ERROR_RETURN(("can't find item=%d", id), -1);
			}

			DECODE_XML_PROP_INT (iut[j].count, cur, "Count");
			j++;
		}
		cur = cur->next;
	}

	*cnt = j;
	return 0;
}

static int parse_single_compose_material(uint32_t index, xmlNodePtr cur)
{
	int err = -1;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Angel")))
		{
			err = parse_compose_items(compose_info[index].angel_iut, &(compose_info[index].angel_cnt), cur->xmlChildrenNode);
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"Material")))
		{
			err = parse_compose_items(compose_info[index].material_iut, &(compose_info[index].material_cnt), cur->xmlChildrenNode);
		}
		cur = cur->next;
	}

	return err;
}


int load_angel_compose_material_conf(const char * file)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	int i, err = -1;
	int ex_count;

	ex_count = 0;
	memset(compose_info, 0, sizeof (compose_info));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load angel compose config failed"), -1);

    cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Compose"))) {
			DECODE_XML_PROP_INT(i, cur, "ID");
			if (ex_count >= MAX_ANGEL_COMPOSE_SIZE - 1 || i >= MAX_ANGEL_COMPOSE_SIZE || i <= 0) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}
			DECODE_XML_PROP_INT(compose_info[i-1].goal_angel_id, cur, "Goal_id");
			DECODE_XML_PROP_INT(compose_info[i-1].goal_angel_cnt, cur, "Goal_count");
			DECODE_XML_PROP_INT(compose_info[i-1].fail_angel_id, cur, "Fail_id");
			DECODE_XML_PROP_INT(compose_info[i-1].fail_angel_cnt, cur, "Fail_count");
			DECODE_XML_PROP_INT(compose_info[i-1].rate, cur, "Rate");
			DECODE_XML_PROP_INT(compose_info[i-1].level, cur, "Level");
			DECODE_XML_PROP_INT(compose_info[i-1].exp, cur, "Exp");
			if (parse_single_compose_material(i-1, cur->xmlChildrenNode)) {
				return -1;
			}
			ex_count++;
		}
		cur = cur->next;
	}
	err = 0;

exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load angel compose file %s", file);
}

int get_angel_compose_material_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 0);
	return send_request_to_db(SVR_PROTO_GET_ANGEL_COMPOSE, p, 0, NULL, p->id);
}

int get_angel_compose_material_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = 0;
	int l = 0;
	l = sizeof(protocol_t);

	CHECK_BODY_LEN_GE(len, 16);
	uint32_t level = 0;
	uint32_t exp = 0;
	uint32_t count1 = 0;
	uint32_t count2 = 0;
	uint32_t item_id = 0;
	uint32_t item_cnt = 0;
	UNPKG_H_UINT32(buf, level, i);
	UNPKG_H_UINT32(buf, exp, i);
	UNPKG_H_UINT32(buf, count1, i);
	UNPKG_H_UINT32(buf, count2, i);
	PKG_UINT32(msg, level, l);
	PKG_UINT32(msg, exp, l);
	PKG_UINT32(msg, count1, l);
	DEBUG_LOG("level = %u, count1 = %u, count2 = %u", level, count1, count2);

	CHECK_BODY_LEN(len, 16 + (count1 + count2) * 8);

	while (count1)
	{
		UNPKG_H_UINT32(buf, item_id, i);
		UNPKG_H_UINT32(buf, item_cnt, i);
		PKG_UINT32(msg, item_id, l);
		PKG_UINT32(msg, item_cnt, l);
		count1--;
	}
	PKG_UINT32(msg, count2, l);
	while (count2)
	{
		UNPKG_H_UINT32(buf, item_id, i);
		UNPKG_H_UINT32(buf, item_cnt, i);
		PKG_UINT32(msg, item_id, l);
		PKG_UINT32(msg, item_cnt, l);
		count2--;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int user_compose_new_angel_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN_GE(len, 8);
	int i = 0;
	uint32_t angel_id = 0;//待合成的天使id
	uint32_t rate_cnt = 0;//增加合成率的材料种类
	uint32_t buff[1024] = {0};
	uint32_t cnt = 4;

	UNPKG_UINT32(body, angel_id, i);
	UNPKG_UINT32(body, rate_cnt, i);
	CHECK_BODY_LEN(len, 8 + rate_cnt * 4);

	if (rate_cnt > 2)	//成功率的材料必须少于2种
	{
		return send_to_self_error(p, p->waitcmd, -ERR_compose_angel_id_wrong, 1);
	}

	*(uint32_t*)(p->session + 4) = rate_cnt;
	uint32_t rate_id;
	int k;
	for (k = 0; k < rate_cnt; k++)
	{
		UNPKG_UINT32(body, rate_id, i);
		*(uint32_t*)(p->session + 8 + k * 4) = rate_id;
		buff[cnt++] = rate_id;
		buff[cnt++] = 1;
	}

	//判断合成的天使的index
	for (i = 0; i < MAX_ANGEL_COMPOSE_SIZE; i++)
	{
		if (compose_info[i].goal_angel_id == angel_id || compose_info[i].goal_angel_id == 0)
		{
			break;
		}
	}
	if (angel_id == 0 || i == MAX_ANGEL_COMPOSE_SIZE || compose_info[i].goal_angel_id == 0)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_compose_angel_id_wrong, 1);
	}

	buff[0] = compose_info[i].level;
	buff[1] = compose_info[i].exp;
	buff[2] = compose_info[i].material_cnt + rate_cnt;
	buff[3] = compose_info[i].angel_cnt;
	//DEBUG_LOG("---- material_cnt = %u, angel_cnt = %u ----", buff[2], buff[3]);
	for (k = 0; k < compose_info[i].material_cnt; k++)
	{
		buff[cnt++] = compose_info[i].material_iut[k].itm->id;
		buff[cnt++] = compose_info[i].material_iut[k].count;
	}
	for (k = 0; k < compose_info[i].angel_cnt; k++)
	{
		buff[cnt++] = compose_info[i].angel_iut[k].itm->id;
		buff[cnt++] = compose_info[i].angel_iut[k].count;
	}
	len = cnt * 4;

	*(uint32_t*)p->session = i;//合成新的天使在xml中的index
	return send_request_to_db(SVR_PROTO_USER_COMPOSE_NEW_ANGEL, p, len, buff, p->id);
}

static int get_rate_from_material_id(uint32_t id)
{
	int rate_val = 0;
	if (id == 1353106)
	{
		rate_val = 5;
	}
	else if (id == 1353107)
	{
		rate_val = 15;
	}
	else if (id == 1353108)
	{
		rate_val = 100;
	}

	return rate_val;
}

int user_compose_new_angel_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t level = 0;
	uint32_t fail_times = 0;
	int i = 0;
	UNPKG_H_UINT32(buf, level, i);
	UNPKG_H_UINT32(buf, fail_times, i);

	int rate = 0;
	int idx = *(uint32_t*)p->session;
	rate = compose_info[idx].rate;
	if (fail_times > 1) {//如果连续几次不成功，合成几率要加(fail_times-1)*(原几率)*0.7
		rate += (int)((fail_times - 1) * compose_info[idx].rate * 0.7);
	}

	uint32_t db_buff[1024] = {0};
	db_buff[0] = *(uint32_t*)(p->session + 4) + compose_info[idx].material_cnt;
	//db_buff[2] = 210;//reason 210融合天使
	int cnt = 4;
	for (i = 0; i < *(uint32_t*)(p->session + 4); i++)//得到合成成功率
	{
		uint32_t rate_id = *(uint32_t*)(p->session + 8 + (i * 4));
		rate += get_rate_from_material_id(rate_id);
		db_buff[cnt++] = 0;
		db_buff[cnt++] = rate_id;
		db_buff[cnt++] = 1;
	}
	DEBUG_LOG("---- compose angel rate = %d, material_cnt = %d ----", rate, compose_info[idx].material_cnt);
	for (i = 0; i < compose_info[idx].material_cnt; i++)
	{
		db_buff[cnt++] = 0;
		db_buff[cnt++] = compose_info[idx].material_iut[i].itm->id;
		db_buff[cnt++] = compose_info[idx].material_iut[i].count;
	}
	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, cnt * 4, db_buff, p->id);

	uint32_t new_angel_id;
	uint32_t new_angel_cnt;
	uint32_t success = 0;
	if (rand() % 100 < rate) //成功
	{
		success = 1;
		new_angel_id = compose_info[idx].goal_angel_id;
		new_angel_cnt = compose_info[idx].goal_angel_cnt;
	}
	else
	{
		new_angel_id = compose_info[idx].fail_angel_id;
		new_angel_cnt = compose_info[idx].fail_angel_cnt;
	}
	item_unit_t iut;
	iut.itm = get_item_prop(new_angel_id);
	iut.count = new_angel_cnt;
	db_add_singel_angel_ex(p->id, &iut);

	for (i = 0; i < compose_info[idx].angel_cnt; i++)
	{
		uint32_t dbbuf[] = {compose_info[idx].angel_iut[i].itm->id, compose_info[idx].angel_iut[i].count, 0};
		send_request_to_db(SVR_PROTO_USER_TERMINATE_CONTRACT, NULL, 12, dbbuf, p->id);
	}

	//增加成功率材料使用统计
	uint32_t msgbuff[2] = {p->id, 1};
	for (i = 0; i < *(uint32_t*)(p->session + 4); i++)
	{
		uint32_t msgid = 0x04088331 + *(uint32_t*)(p->session + 8 + 4 * i) - 1353106;
		msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	//合成一次统计，统计顺序与配表顺序一致
	if (success == 1)
	{
		msglog(statistic_logfile, 0x04088337 + idx, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	else
	{
		msglog(statistic_logfile, 0x04089337 + idx, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	//每天进行天使强化的人数和人次
	msglog(statistic_logfile, 0x0408A337, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	//强化等级情况
	msgbuff[1] = level;
	msglog(statistic_logfile, 0x04088330, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	db_buff[0] = idx + 1;
	db_buff[1] = success;
	send_request_to_db(SVR_USER_SYNTHESIS_HISTORY, NULL, 8, db_buff, p->id);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, level, l);
	PKG_UINT32(msg, success, l);
	PKG_UINT32(msg, new_angel_id, l);
	PKG_UINT32(msg, new_angel_cnt, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int angel_get_game_award_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	if( p->td_award_left > 0 &&												//还有抽奖机会
		p->td_task_id > 64 && p->td_task_id <= 64 + TD_CHALLENGE_GAME_CNT	//当前任务可以抽奖
		)
	{
		//圣诞天使挑战赛，领奖统计
		if( p->td_task_id == 64 + 1 )
		{
			uint32_t msglog_id;
			if( 0 == p->td_task_mode )
			{
				msglog_id = 0x0409C2CE;
			}
			else
			{
				msglog_id = 0x0409C2C8;
			}
			uint32_t msgbuff[2] = {p->id, 1};
			msglog(statistic_logfile, msglog_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}

		uint32_t award_type;
		if( 0 == p->td_task_mode )
		{
			if( ISVIP(p->flag) )
			{
				award_type = 2 + ( 2 - p->td_award_left );
			}
			else
			{
				award_type = 0 + ( 1 - p->td_award_left );
			}
		}
		else
		{
			if( ISVIP(p->flag) )
			{
				award_type = 2 + ( 3 - p->td_award_left );
			}
			else
			{
				award_type = 0 + ( 2 - p->td_award_left );
			}
		}

		uint32_t item_id = 0;
		uint32_t item_cnt = 0;
		int i;
		int rand_val = rand() % challenge_game_info[p->td_task_id - 64 - 1][p->td_task_mode].total_rate[award_type];
		int cur_val = 0;
		for( i = 0; i < challenge_game_info[p->td_task_id - 64 - 1][p->td_task_mode].award_cnt; ++i )
		{
			cur_val += challenge_game_info[p->td_task_id - 64 - 1][p->td_task_mode].award_info[i].rate[award_type];
			if( rand_val < cur_val )
			{
				item_id = challenge_game_info[p->td_task_id - 64 - 1][p->td_task_mode].award_info[i].item_id;
				item_cnt = challenge_game_info[p->td_task_id - 64 - 1][p->td_task_mode].award_info[i].item_cnt;
				break;
			}
		}
		//go to next stage directly
		if( item_cnt == 0 )
		{
			p->td_award_left = 0;
		}
		else
		{
			p->td_award_left--;
		}

		//notice db
		{
			int sendlen = 0;
			uint8_t buff[128];
			PKG_H_UINT32( buff, 0, sendlen );
			PKG_H_UINT32( buff, 1, sendlen );
			PKG_H_UINT32( buff, 203, sendlen );
			PKG_H_UINT32( buff, 0, sendlen );
			item_kind_t* ik = find_kind_of_item(item_id);
			item_t* it = get_item( ik, item_id );

			pkg_item_kind( p, buff, item_id, &sendlen );
			PKG_H_UINT32( buff, item_id, sendlen );
			PKG_H_UINT32( buff, item_cnt, sendlen );
			PKG_H_UINT32( buff, it->max, sendlen );
			send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );
		}

		uint32_t send_len = sizeof(protocol_t);
		PKG_UINT32( msg, item_cnt, send_len );
		PKG_UINT32( msg, item_id, send_len );
		PKG_UINT32( msg, p->td_award_left, send_len );
		init_proto_head( msg, p->waitcmd, send_len );
		return send_to_self( p, msg, send_len, 1 );
	}
	else
	{
		DEBUG_LOG( "angel_get_game_award_cmd Invalid TaskID:[%u] Mode:[%u]", p->td_task_id, p->td_task_mode );
		return send_to_self_error( p, p->waitcmd, ERR_td_can_not_get_award, 1 );
	}
}

int angel_load_game_award_info( char* file )
{
	xmlDocPtr doc;
	xmlNodePtr root;
	int err = -1;

	doc = xmlParseFile (file);
	if (!doc)
	{
		ERROR_RETURN (("angel_load_game_award_info failed"), -1);
	}

	root = xmlDocGetRootElement(doc);
	if (!root)
	{
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}
	if( root )
	{
		xmlNodePtr chl_element_group = root->children;
		uint32_t task_id = 0;
		while( chl_element_group )
		{
			if( 0 == xmlStrcmp( chl_element_group->name, (const xmlChar *)"Task" ) )
			{
				uint32_t mode;
				uint32_t map_id;
				DECODE_XML_PROP_UINT32( mode, chl_element_group, "Mode");
				DECODE_XML_PROP_UINT32( map_id, chl_element_group, "ID");
				task_id = map_id - 64 - 1;
				if( task_id >= TD_CHALLENGE_GAME_CNT )
				{
					ERROR_LOG( "angel_load_game_award_info Invalid MapID:[%u]", map_id );
					continue;
				}
				if( mode > 1 )
				{
					ERROR_LOG( "angel_load_game_award_info Invalid Mode:[%u], MapID:[%u]", mode, map_id );
					continue;
				}

				uint32_t* award_cnt = &challenge_game_info[task_id][mode].award_cnt;
				*award_cnt = 0;
				xmlNodePtr child = chl_element_group->children;
				while( child )
				{
					if( 0 == xmlStrcmp( child->name, (const xmlChar *)"Award" ) )
					{
						DECODE_XML_PROP_UINT32( challenge_game_info[task_id][mode].award_info[*award_cnt].item_id, child, "ItemID");
						DECODE_XML_PROP_UINT32( challenge_game_info[task_id][mode].award_info[*award_cnt].item_cnt, child, "Cnt");
						decode_xml_prop_uint32_default( &challenge_game_info[task_id][mode].award_info[*award_cnt].rate[TAT_PL1], child, "RatPL1", 0);
						decode_xml_prop_uint32_default( &challenge_game_info[task_id][mode].award_info[*award_cnt].rate[TAT_PL2], child, "RatPL2", 0);
						decode_xml_prop_uint32_default( &challenge_game_info[task_id][mode].award_info[*award_cnt].rate[TAT_SL1], child, "RatSL1", 0);
						decode_xml_prop_uint32_default( &challenge_game_info[task_id][mode].award_info[*award_cnt].rate[TAT_SL2], child, "RatSL2", 0);
						decode_xml_prop_uint32_default( &challenge_game_info[task_id][mode].award_info[*award_cnt].rate[TAT_SL3], child, "RatSL3", 0);
						++(*award_cnt);
					}
					child = child->next;
				}
				int i;
				for( i = TAT_PL1; i < TAT_MAX; ++i )
				{
					challenge_game_info[task_id][mode].total_rate[i] = 0;
					int j;
					for( j = 0; j < challenge_game_info[task_id][mode].award_cnt; ++j )
					{
						challenge_game_info[task_id][mode].total_rate[i] += challenge_game_info[task_id][mode].award_info[j].rate[i];
					}
				}
			}
			chl_element_group = chl_element_group->next;
		}
	}
	err = 0;
	DEBUG_LOG( "angel_load_game_award_info OK" );
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "angel_load_game_award_info %s", file);
}

/*
 * 获取超拉天使兑换祥云九尾信息 
 */
int get_vip_angel_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 4);
	int i = 0;
	uint32_t type = 0;
	UNPKG_UINT32(body, type, i);

	return send_request_to_db(SVR_PROTO_GET_VIP_ANGEL_INFO, p, 4, &type, p->id);
}

int get_vip_angel_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	
	CHECK_BODY_LEN(len, sizeof(uint32_t));
	int offset = sizeof(protocol_t);
	PKG_UINT32(msg, *((uint32_t*)buf), offset);

	init_proto_head(msg, p->waitcmd, offset);
	return send_to_self(p, msg, offset, 1);
}

/*
 * 满足超拉天使兑换条件，兑换祥云九尾
 */
int exchange_vip_angel_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);

	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 4);
	int i = 0;
	uint32_t type = 0;
	UNPKG_UINT32(body, type, i);

	return send_request_to_db(SVR_PROTO_EXCHANGE_VIP_ANGEL, p, 4, &type, p->id);
}

int exchange_vip_angel_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(uint32_t));
	int offset = sizeof(protocol_t);
	PKG_UINT32(msg, *((uint32_t*)buf), offset);

	init_proto_head(msg, p->waitcmd, offset);
	return send_to_self(p, msg, offset, 1);

}

void reload_angel_td_conf()
{
	if( load_td_role_info( "./conf/TDInfo.xml" ) )
	{
		ERROR_LOG( "reload ./conf/TDInfo.xml error" );
	}
	DEBUG_LOG( "reload ./conf/TDInfo.xml" );

	if( load_tdtask_all() )
	{
		ERROR_LOG( "reload all td task info error" );
	}
	DEBUG_LOG( "reload all td task info" );

	if( load_angel_honor_bonus("./conf/angle_honor_bonus.xml") )
	{
		ERROR_LOG( "reload ./conf/angle_honor_bonus.xml error" );
	}
	DEBUG_LOG( "reload ./conf/angle_honor_bonus.xml" );
}


