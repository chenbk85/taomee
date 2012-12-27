#include "central_online.h"
#include "dbproxy.h"
#include "proto.h"
#include "util.h"
#include "week_activity.h"
#include "exclu_things.h"
#include "small_require.h"


uint32_t explor_time_begin  = 0;

static uint32_t mine_timer_init_flag = 0;
static uint32_t mine_timer_flag = 0;
static mine_explor_timer_t  mine_timer;
#define MINE_EXPLOR_TIME  5*60

#define DECORATE_BUF_SIZE  500
#define NICK_LEN 16


static uint32_t item_timer_init_flag = 0;
static uint32_t item_timer_flag = 0;
static guess_item_timer_t  item_timer;

#ifndef TW_VER
#define GUESS_ITEM_PRICE_DATE		20120120
#define GUESS_ITEM_PRICE_DATE_END	20120131
static guess_item_price_t item_price[] = {
	{20120120, 1353288, 2370},
	{20120121, 1353244, 12541},
	{20120122, 1353266, 30001},
	{20120123, 1353308, 40601},
	{20120124, 1270075, 50041},
	{20120125, 1353292, 78013},
	{20120126, 1593069, 40023},
	{20120127, 1353204, 41102},
	{20120128, 1353222, 32145},
	{20120129, 1350011, 10014},
	{20120130, 190716, 120414},
	{20120131, 1353236, 70891},
};
#else
#define GUESS_ITEM_PRICE_DATE		20120203
#define GUESS_ITEM_PRICE_DATE_END	20120209
static guess_item_price_t item_price[] = {
	{20120203, 1353301, 2200},
	{20120204, 1353271, 12000},
	{20120205, 1350011, 30001},
	{20120206, 1353223, 41000},
	{20120207, 1353305, 50000},
	{20120208, 1353306, 78000},
	{20120209, 1353312, 40001},
};
#endif

//mole great produce
static greate_produce_t greate_items[] = {
	{1, {{1613401, 5},{0, 0},{0, 0}},{{1614000, 5}, {0, 0}, {0, 0}}},
	{1, {{1613501, 5},{0, 0},{0, 0}},{{1613604, 3}, {0, 0}, {0, 0}}},
	{1, {{1613600, 3},{0, 0},{0, 0}},{{1614001, 3}, {0, 0}, {0, 0}}},
	{1, {{1613601, 3},{0, 0},{0, 0}},{{1614006, 3}, {0, 0}, {0, 0}}},
	{1, {{1613602, 3},{0, 0},{0, 0}},{{1614017, 3}, {0, 0}, {0, 0}}},
	{1, {{1613603, 2},{0, 0},{0, 0}},{{1613600, 30}, {1613601, 30}, {0, 0}}},
	{2, {{1613501, 10},{0, 0},{0, 0}},{{1613923, 1}, {1613910, 1}, {1613913, 1}}},
};
static uint32_t champion_piglet[] = {1593036, 1593038, 1593038, 1593037, 1593041, 1593041, 1593041, 
	1593036,  1593037, 1593037, 1593038, 1593034};

static cos_item_score_info_t cos_star_items[] = {
    {{{13850, 40}, {13851, 40}, {13852, 10}, {13853, 10}}},
    {{{12014, 50}, {12040, 50}, {0, 0}, {0, 0}}},
    {{{14136, 40}, {14137, 40}, {14138, 10}, {14142, 10}}},
    {{{14281, 50}, {14282, 50}, {0, 0}, {0, 0}}},
    {{{14015, 40}, {14016, 50}, {14017, 10}, {0, 0}}},
    {{{14018, 40}, {14019, 50}, {14020, 10}, {0, 0}}}, 
};
	
static int guess_item_cnt = sizeof(item_price) / sizeof(guess_item_price_t);
static guess_right_users_t right_users = {0};

static speech_user_info_t speech_user = {0};
static uint32_t speech_info_init_flag = 0;
static speech_info_timer_t  speech_timer;

static cheer_team_info_t cheer_team = {0};

int cheer_team_init()
{
    cheer_team.teacher_act = 1;
    return 0;
}

int cheer_team_clear_user_info(sprite_t *p)
{
    int j = 0;
	for (j = 0; j < 6; j++){
	    if(cheer_team.userid[j] == p->id){
	        cheer_team.user_act[j] = 0;
	        cheer_team.userid[j] = 0;
	        uint32_t pos = j + 1;
	        notify_cheer_team_user_leave(p, p->id, pos, 0);
	        break ;
	    }
	}
    
    return 0;
}

int check_user_in_pos(uint32_t userid)
{
    int j = 0;
	for (j = 0; j < 6; j++){
	    if(cheer_team.userid[j] == userid){
	        return 1;
	    }
	}
    
    return 0;
}

int cheer_team_user_act_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t pos = 0;
	uint32_t act = 0;
	unpack(buf, sizeof(pos)+sizeof(act), "LL", &pos, &act);
	
	DEBUG_LOG("userid %u pos %u act %u", p->id, pos, act);

	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C34F,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	if (pos < 1){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	pos = pos - 1;
	
	if (pos > 5 || act > 3){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if (act == 0)
	{
	    cheer_team.user_act[pos] = 0;
	    cheer_team.userid[pos] = 0;
	    response_proto_uint32_uint32_uint32(p, p->waitcmd, p->id, pos+1, act, 1);
	    return 0;
	}

	if (check_user_in_pos(p->id)){	    
	    return send_to_self_error(p, p->waitcmd, -ERR_cheer_team_user_in_other_pos, 1);
	}
	
	if (cheer_team.user_act[pos] != 0){
	    return send_to_self_error(p, p->waitcmd, -ERR_cheer_team_pos_user_exist, 1);
	}

	if (act != cheer_team.teacher_act){
	    return send_to_self_error(p, p->waitcmd, -ERR_cheer_team_user_act_error, 1);
	}
	
	cheer_team.user_act[pos] = act;
	cheer_team.userid[pos] = p->id;

    uint32_t win_team = 0;
	if (pos >= 3){
	    if (cheer_team.user_act[3] != 0 && cheer_team.user_act[4] != 0 
	        && cheer_team.user_act[5] != 0){
	        win_team = 2;
	    }
	}
	else{
	   if (cheer_team.user_act[0] != 0 && cheer_team.user_act[1] != 0 
	        && cheer_team.user_act[2] != 0){
	        win_team = 1;
	    }
	}

	response_proto_uint32_uint32_uint32(p, p->waitcmd, p->id, pos+1, act, 1);

	if (win_team){
	     cheer_team.team_rate[win_team - 1] += 25;
	     int j = 0;
	     for (j = 0; j < 6; j++){
	        cheer_team.userid[j]  = 0;
	        cheer_team.user_act[j]  = 0;
	     }

	     cheer_team.teacher_act = rand()%3 + 1;
	     notify_cheer_team_turn_end(p, win_team, cheer_team.teacher_act, 
	        cheer_team.team_rate[0], cheer_team.team_rate[1]);

	     if (cheer_team.team_rate[win_team - 1] == 100){
	        cheer_team.team_score[win_team - 1] += 1;
	        cheer_team.team_rate[0] = 0;
	        cheer_team.team_rate[1] = 0;
	     }
	}

	return 0;
	
}

int notify_cheer_team_turn_end(sprite_t* p, uint32_t win_team, uint32_t teacher_act, 
    uint32_t team_rate1, uint32_t team_rate2)
{
    uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, win_team, l);
	PKG_UINT32(n_msg, teacher_act, l);
	PKG_UINT32(n_msg, team_rate1, l);
	PKG_UINT32(n_msg, team_rate2, l);
    init_proto_head(n_msg, PROTO_USER_CHEER_TEAM_TURN_END, l);
    send_to_map3(47, n_msg, l);
    return 0;
}

int notify_cheer_team_user_leave(sprite_t* p, uint32_t userid, uint32_t pos, uint32_t act)
{
    uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, userid, l);
	PKG_UINT32(n_msg, pos, l);
	PKG_UINT32(n_msg, act, l);
    init_proto_head(n_msg, PROTO_USER_CHEER_TEAM_ACT, l);
    send_to_map3(47, n_msg, l);
    return 0;
}


int query_cheer_team_info_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, cheer_team.teacher_act, i);
	PKG_UINT32(msg, cheer_team.team_score[0], i);
	PKG_UINT32(msg, cheer_team.team_score[1], i);
	PKG_UINT32(msg, cheer_team.team_rate[0], i);
	PKG_UINT32(msg, cheer_team.team_rate[1], i);
	int j = 0;
	for (j = 0; j < 6; j++){
	    PKG_UINT32(msg, cheer_team.user_act[j], i);
	}

    j = 0;
	for (j = 0; j < 6; j++){
	    PKG_UINT32(msg, cheer_team.userid[j], i);
	}
	
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}


int get_candy_friend_cnt_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	return  send_request_to_db(SVR_PROTO_GET_CANDY_FREIND_CNT, p, 0, NULL, p->id);
}

int get_candy_friend_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	int j = 0;
	uint32_t friend_cnt = 0;
	UNPKG_H_UINT32(buf,friend_cnt,j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, friend_cnt, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_candy_share_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	return  send_request_to_db(SVR_PROTO_GET_CANDY_SHARE_BONUS, p, 0, NULL, p->id);
}

int get_candy_share_bonus_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int rate = rand()%100;
	uint32_t itemid = 0;
	uint32_t cnt = 0;
	if (rate < 50){
		itemid = 1351123;
		cnt = 10;
	}
	else{
		itemid = 160990;
		cnt = 1;
	}

	db_exchange_single_item_op(p, 202, itemid, cnt, 0);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, 1, i);
	PKG_UINT32(msg, itemid, i);
	PKG_UINT32(msg, cnt, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_candy_from_friend_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	int j = 0;
	uint32_t userid = 0;
	UNPKG_UINT32(body,userid,j);

	return  send_request_to_db(SVR_PROTO_GET_CANDY_FROM_FREIND, p, 4, &(p->id), userid);
}

int get_candy_from_friend_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t itemid = 0;
	uint32_t item_cnt = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 8);
	UNPKG_H_UINT32(buf,itemid,j);
	UNPKG_H_UINT32(buf,item_cnt,j);

	db_exchange_single_item_op(p, 202, itemid, item_cnt, 0);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, i);
	PKG_UINT32(msg, item_cnt, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int add_as_type_int_data_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	uint32_t type = 0;
	uint32_t data = 0;
	UNPKG_UINT32(body,type,j);
	UNPKG_UINT32(body,data,j);

	uint32_t db_buff[2] = {type, data};
	return  send_request_to_db(SVR_PROTO_USER_AS_ADD_TYPE_INT_DATA, p, 8, db_buff, p->id);
}

int add_as_type_int_data_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int query_as_type_int_data_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	int j = 0;
	uint32_t type = 0;
	UNPKG_UINT32(body,type,j);

	return  send_request_to_db(SVR_PROTO_USER_AS_QUERY_TYPE_INT_DATA, p, 4, &type, p->id);
}

int query_as_type_int_data_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t type = 0;
	uint32_t data = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 8);
	UNPKG_H_UINT32(buf,type,j);
	UNPKG_H_UINT32(buf,data,j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, type, i);
	PKG_UINT32(msg, data, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int mine_explor__timeout()
{
	DEBUG_LOG("mine explor timeout !");

	send_request_to_db(SVR_PROTO_SYSARG_DROP_MAP, NULL, 0, NULL, 0);
	send_request_to_db(SVR_PROTO_SYSARG_DEL_PLAYER_NUM, NULL, 0, NULL, 0);
	mine_timer_flag = 0;
	explor_time_begin = 0;
	return 0;
}


int add_mine_map_piece_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	int j = 0;
	uint32_t itemid = 0;
	UNPKG_UINT32(body,itemid,j);
	if (itemid < 1351182 || itemid > 1351241){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if( ISVIP(p->flag)){
		uint32_t msg_buff[4] = {p->id,1,0,0};
		msglog(statistic_logfile, 0x0409C2D0,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else {
		uint32_t msg_buff[4] = {0,0,p->id,1};
		msglog(statistic_logfile, 0x0409C2D0,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}

	*(uint32_t*)p->session = itemid;
	uint32_t db_buff[] = {1, 0, 0, 0, 99, itemid, 1};
	return  send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, sizeof(db_buff), db_buff, p->id);

}

int add_mine_map_piece_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t flag = 0;
	uint32_t map_flag = 0;
	uint32_t item_cnt = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 8);
	UNPKG_H_UINT32(buf,flag,j);
	UNPKG_H_UINT32(buf,map_flag,j);
	if (flag){
		item_cnt = 3;
	}
	else{
		item_cnt = 10;
	}

	uint32_t db_buff[] = {0, 1, 0, 0, 99, 1351242, item_cnt, 99999};
	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buff), db_buff, p->id);

	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C2D5,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	if (map_flag && !mine_timer_flag){
		char txt[256] = {0};
#ifndef TW_VER
		int msg_len = sprintf(txt, "5分钟之后，开启龙族宝藏大探险。");
#else
		int msg_len = sprintf(txt, "5分鐘之後，開啟龍族寶藏大探險。");
#endif
		tell_flash_some_msg_across_svr(p, CBMT_LONGZU_EXPLORE, msg_len, txt);
		if(!mine_timer_init_flag){
			INIT_LIST_HEAD(&mine_timer.timer_list);
			mine_timer_init_flag = 1;
		}

		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C2CF,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

		DEBUG_LOG("userid %u map_flag %u", p->id, map_flag);
		mine_timer_flag = 1;
		ADD_TIMER_EVENT(&mine_timer, mine_explor__timeout, NULL, get_now_tv()->tv_sec + 2*MINE_EXPLOR_TIME);
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, 1351242, i);
	PKG_UINT32(msg, item_cnt, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_mine_map_pieces_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return  send_request_to_db(SVR_PROTO_SYSARG_GET_MAP, p, 0, NULL, p->id);
}

int get_mine_map_pieces_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t count = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, 4 + count * 4);
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, count, i);
	int loop = 0;
	for(loop = 0; loop < count; loop++){
		uint32_t itemid = 0;
		UNPKG_H_UINT32 (buf, itemid, j);
		PKG_UINT32 (msg, itemid, i);
	}
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int get_mine_key_chance_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	uint32_t now_time_diff = get_now_tv()->tv_sec - explor_time_begin;
	if(now_time_diff < MINE_EXPLOR_TIME || now_time_diff > 2*MINE_EXPLOR_TIME){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if( ISVIP(p->flag)){
		uint32_t msg_buff[4] = {p->id,1,0,0};
		msglog(statistic_logfile, 0x0409C2D1,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else {
		uint32_t msg_buff[4] = {0,0,p->id,1};
		msglog(statistic_logfile, 0x0409C2D1,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}

	return  send_request_to_db(SVR_PROTO_SYSARG_CHECK_PLAYER_NUM, p, 0, NULL, p->id);
}

int get_all_chance_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t state = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, state, j);

	DEBUG_LOG("userid %u get chance %u", p->id, state);

	if(!state && !ISVIP(p->flag)){
		return send_to_self_error(p, p->waitcmd, -ERR_mine_map_chance_limit_max, 1);
	}
	return  send_request_to_db(SVR_PROTO_USER_GET_KEY_CHANCE, p, 0, NULL, p->id);
}

int get_mine_key_chance_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t flag = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 4);
	UNPKG_H_UINT32(buf,flag,j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, flag, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}


int query_mine_key_chance_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return  send_request_to_db(SVR_PROTO_USER_EXPLORE_QUERY_STATE, p, 0, NULL, p->id);
}

int query_mine_key_chance_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t flag = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 4);
	UNPKG_H_UINT32(buf,flag,j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, flag, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_mine_map_key_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return  send_request_to_db(SVR_PROTO_USER_EXPLORE_CLEAR_STATE, p, 0, NULL, p->id);
}

int get_mine_map_key_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t itemid = 0;
	uint32_t item_cnt = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 8);
	UNPKG_H_UINT32(buf,itemid,j);
	UNPKG_H_UINT32(buf,item_cnt,j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, i);
	PKG_UINT32(msg, item_cnt, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int query_mine_map_explor_time_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	uint32_t now_time_diff = get_now_tv()->tv_sec - explor_time_begin;

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, now_time_diff, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_gold_compass_users_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 0);
	return send_request_to_db(SVR_PROTO_SYSARG_GET_GOLD_USERS_INFO, p, 0, NULL, p->id);
}

int get_gold_compass_users_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	struct gold_info_hdr {
		uint32_t    userid;
		char        nick[16];
		uint32_t    datetime;
		uint32_t	count;
	}__attribute__((packed));

	struct gold_info {
		uint32_t    userid;
		char        nick[16];
		uint32_t    datetime;
	}__attribute__((packed));

	struct gold_info_hdr *p_hdr_info = (struct gold_info_hdr*)buf;

	CHECK_BODY_LEN_GE(len,sizeof(struct gold_info_hdr));
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, p_hdr_info->userid, l);
	PKG_STR(msg, p_hdr_info->nick, l, sizeof p_hdr_info->nick);
	PKG_UINT32(msg, p_hdr_info->datetime, l);
	PKG_UINT32(msg, p_hdr_info->count, l);

	CHECK_BODY_LEN(len,sizeof(struct gold_info_hdr) + p_hdr_info->count*sizeof(struct gold_info));
	struct gold_info *p_info = (struct gold_info*)(buf + sizeof(struct gold_info_hdr));

	int i = 0;
	for (i = 0; i < p_hdr_info->count; i++)
	{
		PKG_UINT32(msg, p_info->userid, l);
		PKG_STR(msg, p_info->nick, l, sizeof p_info->nick);
		PKG_UINT32(msg, p_info->datetime, l);
		p_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int guess_date_item_price_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	int j = 0;
	uint32_t price = 0;
	UNPKG_UINT32(body, price, j);
	*(uint32_t*)p->session = price;
	uint32_t t_day = get_today();

	if (t_day < GUESS_ITEM_PRICE_DATE || t_day > GUESS_ITEM_PRICE_DATE_END){
		return send_to_self_error(p, p->waitcmd, -ERR_guess_item_price_time_wrong, 1);
	}

#ifndef TW_VER
	if ((get_now_tm()->tm_hour != 14) || (get_now_tm()->tm_hour == 14 && get_now_tm()->tm_min < 30)
		|| (get_now_tm()->tm_hour == 14 && get_now_tm()->tm_min > 40))
#else
	if ((get_now_tm()->tm_hour != 14) || (get_now_tm()->tm_hour == 14 && get_now_tm()->tm_min < 30))
#endif
	{
		return send_to_self_error(p, p->waitcmd, -ERR_guess_item_price_time_wrong, 1);
	}

	return db_set_sth_done(p, 40010+(t_day - GUESS_ITEM_PRICE_DATE), 1, p->id);
}

int guess_item_price_timeout()
{
	user_item_info_t user_item = {0};
	uint32_t index = get_today() - GUESS_ITEM_PRICE_DATE;

	if (index > guess_item_cnt - 1){
		return 0;
	}

	uint32_t i = 0;
	for (i = 0; i < right_users.count; i++)
	{
		if (right_users.users[i].userid != 0){
			user_item.itemid = item_price[index].itmid;
			memcpy(user_item.nick, right_users.users[i].nick, USER_NICK_LEN);
			user_item.price = item_price[index].price;
			send_request_to_db(SVR_PROTO_SYSARG_ADD_GUESS_PRICE_INFO, NULL, sizeof(user_item_info_t),
				&user_item, right_users.users[i].userid);
			item_t *itm = get_item_prop(item_price[index].itmid);
			if(!itm) {
				return 0;
			}

			db_add_single_item_op(right_users.users[i].userid, 202, item_price[index].itmid, 1);

			char txt[256] = {0};
#ifndef TW_VER
			int msg_len = sprintf(txt, "恭喜你在我猜我猜我猜猜猜活动中猜中物品价格，特送上你猜中价格的%s作为奖励！",itm->name);
			send_postcard("伊莲", 0, right_users.users[i].userid, 1000305, txt, msg_len);
#else
			int msg_len = sprintf(txt, "恭喜你在我猜我猜我猜猜猜活動中猜中物品價格，特送上你猜中價格的%s作為獎勵！",itm->name);
			send_postcard("伊蓮", 0, right_users.users[i].userid, 1000305, txt, msg_len);
#endif
		}
	}

	item_timer_flag = 0;
	memset (&right_users, 0, sizeof(right_users));
	if(config_cache.bc_elem->online_id == 5){
		char txt[256] = {0};
#ifndef TW_VER
		int msg_len = sprintf(txt, "米勒大道我猜猜猜活动已经结束，小摩尔们快去查看吧！");
#else
		int msg_len = sprintf(txt, "米勒大道我猜猜猜活動已經結束，小摩爾們快去查看吧！");
#endif
		tell_flash_some_msg_across_svr(NULL, CBMT_GUESS_ITEM_PRICE, msg_len, txt);

	}
	return 0;
}


int guess_item_price_day_limit_callback(sprite_t *p)
{
	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C314,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	uint32_t price = *(uint32_t *)p->session;
	uint32_t t_day = get_today();
	if (t_day < GUESS_ITEM_PRICE_DATE || t_day > GUESS_ITEM_PRICE_DATE_END){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	uint32_t index = t_day - GUESS_ITEM_PRICE_DATE;
	if (index > guess_item_cnt - 1){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	if (item_price[index].price == price){
		if (right_users.count < GUESS_RIGHT_MAX){
			right_users.users[right_users.count].userid = p->id;
			memcpy(right_users.users[right_users.count].nick, p->nick, USER_NICK_LEN);
			right_users.count = right_users.count + 1;
		}
	}

	if(!item_timer_init_flag){
		INIT_LIST_HEAD(&item_timer.timer_list);
		item_timer_init_flag = 1;
	}
	if(!item_timer_flag){
		uint32_t m_time = 0;
		if (get_now_tm()->tm_min > 30 && get_now_tm()->tm_min < 40){
			m_time = 40 - get_now_tm()->tm_min;
		}
		else{
			m_time = 10;
		}
		ADD_TIMER_EVENT(&item_timer, guess_item_price_timeout, NULL, get_now_tv()->tv_sec + m_time*60+2);
		item_timer_flag = 1;
	}

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_guess_right_user_info_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	int j = 0;
	uint32_t date = 0;
	UNPKG_UINT32(body, date, j);
	return  send_request_to_db(SVR_PROTO_SYSARG_GUESS_RIGHT_USER_INFO, p, 4, &date, p->id);
}

int get_guess_right_user_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t count = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, 4 + count * sizeof(user_item_db_info_t));

	user_item_db_info_t *p_info = (user_item_db_info_t*)(buf + 4);;
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, count, i);
	int loop = 0;
	for(loop = 0; loop < count; loop++){
		PKG_UINT32(msg, p_info->userid, i);
		PKG_STR(msg, p_info->nick, i, sizeof p_info->nick);
		PKG_UINT32(msg, p_info->itemid, i);
		PKG_UINT32(msg, p_info->price, i);
		p_info++;
	}
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int get_sprint_festival_login_info_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return  send_request_to_db(SVR_PROTO_USER_GET_SPRINT_FESTIVAL_LOGIN_INFO, p, 0, NULL, p->id);
}

int get_sprint_festival_login_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t day_cnt = 0;
	uint32_t flag = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 8);
	UNPKG_H_UINT32(buf,day_cnt,j);
	UNPKG_H_UINT32(buf,flag,j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, day_cnt, i);
	PKG_UINT32(msg, flag, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_sprint_festival_login_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	uint32_t day_cnt = 0;
	int j = 0;
	CHECK_BODY_LEN(bodylen, 4);
	UNPKG_UINT32(body, day_cnt, j);

	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C30A+day_cnt-1,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	return  send_request_to_db(SVR_PROTO_USER_GET_SPRINT_FESTIVAL_LOGIN_BONUS, p, 4, &day_cnt, p->id);
}

int get_sprint_festival_login_bonus_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t count = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, 4 + count * 8);
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, count, i);
	int loop = 0;
	for(loop = 0; loop < count; loop++){
		uint32_t itemid = 0;
		uint32_t cnt = 0;
		UNPKG_H_UINT32 (buf, itemid, j);
		UNPKG_H_UINT32 (buf, cnt, j);
		PKG_UINT32 (msg, itemid, i);
		PKG_UINT32 (msg, cnt, i);
	}
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

/*
 * vip连续周登陆信息
 */
int get_vip_continue_week_login_info_cmd(sprite_t *p, uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id); 
	uint32_t type = 6;
	return send_request_to_db(SVR_PROTO_USER_GET_VIP_CONTINUE_WEEK_LOGIN_INFO, p, 4, &type, p->id);

}
int get_vip_continue_week_login_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct ret_week_login_info{
		uint32_t count;
		uint32_t flags;
	}ret_info_t;

	 CHECK_BODY_LEN(len, sizeof(ret_info_t)); 
	 ret_info_t* content = (ret_info_t*)buf;
	 uint32_t offset =  sizeof(protocol_t);
	 PKG_UINT32(msg, content->count, offset);
	 PKG_UINT32(msg, content->flags, offset);

	init_proto_head(msg, p->waitcmd, offset);
	DEBUG_LOG("count: %u, flags: %u", content->count, content->flags);
	return send_to_self(p, msg, offset, 1);
}

/*
 * vip连续周登陆领取礼物
 */
int get_vip_week_login_award_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id); 
	CHECK_BODY_LEN(bodylen, 0);

	return send_request_to_db(SVR_PROTO_USER_GET_VIP_WEEK_LOGIN_AWARD, p, 0, NULL, p->id);
}
int get_vip_week_login_award_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct ret_vip_login_award{
		uint32_t state;
		uint32_t count;
	}ret_award_head_t;
	typedef struct ret_vip_login_content{
		uint32_t itemid;
		uint32_t count;
	}ret_login_award_t;

	CHECK_BODY_LEN_GE(len, sizeof(ret_award_head_t));
	ret_award_head_t *head = (ret_award_head_t*)buf;
	uint32_t offset =  sizeof(protocol_t);
	PKG_UINT32(msg, head->state, offset);  
	PKG_UINT32(msg, head->count, offset);  

	CHECK_BODY_LEN(len, sizeof(ret_login_award_t) * head->count+sizeof(ret_award_head_t));
	ret_login_award_t* pointer = (ret_login_award_t*)(buf+sizeof(ret_award_head_t));
	uint32_t k = 0;
	for(; k < head->count; ++k){
		 PKG_UINT32(msg, pointer->itemid, offset);	
		 PKG_UINT32(msg, pointer->count, offset);	
		 ++pointer;
	}

	uint32_t msg_buff[2] = {1, p->id};
	msglog(statistic_logfile, 0x0409C328,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	init_proto_head(msg, p->waitcmd, offset);
	return send_to_self(p, msg, offset, 1);
}

int add_user_item_score_cmd(sprite_t * p,const uint8_t * body,int len)
{
    uint32_t item_cnt = 0;
    uint32_t itemid = 0;
    uint32_t score = 0;
    int i = 0;
    CHECK_BODY_LEN_GE(len, 4);
	UNPKG_UINT32(body, item_cnt, i);
	CHECK_BODY_LEN(len, 4+item_cnt*8);

	if ((item_cnt > 100) || (!item_cnt))
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint8_t dbbuf[1024] = {};
	int l = 0;
	PKG_H_UINT32(dbbuf,item_cnt,l);

	int j = 0;
	for (j = 0; j < item_cnt; j++)
	{
	    UNPKG_UINT32(body, itemid, i);
	    UNPKG_UINT32(body, score, i);

	    if(score > 5){
	        score = 5;
	    }
	    
	    PKG_H_UINT32(dbbuf, itemid, l);
	    PKG_H_UINT32(dbbuf, score, l);
	}

    send_request_to_db(SVR_PROTO_SYSARG_SET_ITEMID_SCORE, NULL, l, dbbuf, p->id);
    return send_request_to_db(SVR_PROTO_USER_SET_ITEMID_SCORE, p, l, dbbuf, p->id);

}


int add_user_item_score_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C31D, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	int i = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_user_items_score_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return  send_request_to_db(SVR_PROTO_USER_GET_ITEMID_SCORE, p, 0, NULL, p->id);
}

int get_user_items_score_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t count = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, 4 + count * 8);
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, count, i);
	int loop = 0;
	for(loop = 0; loop < count; loop++){
		uint32_t itemid = 0;
		uint32_t score = 0;
		UNPKG_H_UINT32 (buf, itemid, j);
		UNPKG_H_UINT32 (buf, score, j);
		PKG_UINT32 (msg, itemid, i);
		PKG_UINT32 (msg, score, i);
	}
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int get_user_cutepig_gold_key_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	int j = 0;
	uint32_t itemid = 0;
	UNPKG_UINT32(body,itemid,j);

	return  send_request_to_db(SVR_PROTO_USER_GET_CUTEPIG_GOLD_KEY, p, 4, &itemid, p->id);
}

int get_user_cutepig_gold_key_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t itemid = 0;
	uint32_t count = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 8);
	UNPKG_H_UINT32(buf,itemid,j);
	UNPKG_H_UINT32(buf,count,j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, i);
	PKG_UINT32(msg, count, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int speech_user_timeout()
{
    if ((get_now_tv()->tv_sec - speech_user.time > 120) && (speech_user.time > 0))
    {
        speech_user_clean();
    }
    
    return 0;
}

int speech_user_clean( )
{
    speech_user.time = 0;
    speech_user.last_time = get_now_tv()->tv_sec;
    speech_user.last_userid = speech_user.userid;
    speech_user.userid = 0;    
    return 0;
}

int speech_user_leave(sprite_t* p)
{
    if (p->id == speech_user.userid)
    {
        speech_user.time = 0;
        speech_user.last_time = get_now_tv()->tv_sec;
        speech_user.last_userid = speech_user.userid;
        speech_user.userid = 0;

        int j = sizeof(protocol_t);
	    uint8_t n_msg[1024] = {0};
	    PKG_UINT32(n_msg, p->id, j);
        PKG_UINT32(n_msg, 0, j);
        PKG_UINT32(n_msg, 0, j);
	    init_proto_head(n_msg, PROTO_SPEECH_USER_ENTER_OR_LEAVE, j);
    	send_to_map3(47, n_msg,  j);    	
    }
    
    return 0;
}

int get_speech_time_limit( )
{

#ifndef TW_VER
	uint32_t day1 = 20120302;
#else
	uint32_t day1 = 20120309;
#endif
    uint32_t cur_hour = get_now_tm()->tm_hour;
	uint32_t cur_min = get_now_tm()->tm_min;
	int today = get_today();
	if (today == day1){
	    if(( cur_hour == 20 && cur_min <= 30))
		{
	        return 1;
	    }
	}
	
	return 0;
}

int speech_user_enter_or_leave_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	uint32_t flag = 0;
	uint32_t state = 0;
	UNPKG_UINT32(body,flag,j);
	UNPKG_UINT32(body,state,j);

	if (!get_speech_time_limit()){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
    }
    
	if (!flag && speech_user.time == 0){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

    if (flag && speech_user.time != 0){
	    return send_to_self_error(p, p->waitcmd, -ERR_week_activity_speech_aready_begin, 1);
	}

	if (flag){
	    if ((speech_user.last_userid == p->id) && (get_now_tv()->tv_sec < speech_user.last_time + 60))
	    {
	        return send_to_self_error(p, p->waitcmd, -ERR_week_activity_speech_canmot_continuous, 1);
	    }

	    speech_user.time = get_now_tv()->tv_sec;
	    speech_user.userid = p->id;

	    if(!speech_info_init_flag){
		    INIT_LIST_HEAD(&speech_timer.timer_list);
		    speech_info_init_flag = 1;
	    }

		ADD_TIMER_EVENT(&speech_timer, speech_user_timeout, NULL, get_now_tv()->tv_sec + 120);
	}
	else {
	    if (state){
	        if ((get_now_tv()->tv_sec - speech_user.time) < 20){
	            return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	        }
	    }
	}

	if (!flag){
        speech_user_clean();
	}

	response_proto_uint32_uint32_uint32(p, p->waitcmd, p->id, flag, state, 1);
	return 0;	
}

int speech_user_get_user_info_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
	response_proto_uint32(p, p->waitcmd, speech_user.userid, 0);
	return 0;	
}

int get_majesty_or_urgent_order_cmd(sprite_t*p, uint8_t* body, int len)
{
	uint32_t type = 0;
	CHECK_BODY_LEN(len, sizeof(type));
	int j = 0;
	UNPKG_UINT32(body,type,j);
	*(uint32_t*)(p->session) = type;
	return  send_request_to_db(SVR_PROTO_USER_GET_GREAT_PRODUCE_ORDER, p, 4, &type, p->id);

}

int get_majesty_or_urgen_order_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct order_info{
		uint32_t exist;
		uint32_t order_id;//0, 1,2...
		uint32_t finish_flag;
		uint32_t week_cnt;
		uint32_t week_id;
	}order_info_t;

#ifndef TW_VER
	uint32_t DAY = 20120601;
#else
	uint32_t DAY = 20120608;
#endif
	CHECK_BODY_LEN(len, sizeof(order_info_t));
	order_info_t* head = (order_info_t* )buf;
	uint32_t type = *(uint32_t*)(p->session);
	
	switch(p->waitcmd)
	{
		case PROTO_GET_GREAT_PRODUCE_ORDER:
			{
				uint32_t offset =  sizeof(protocol_t);
				PKG_UINT32(msg, type, offset); 
				if(type == 1){//majesty order
					uint32_t recv_flag = 0;
					if(head->exist == 1){
						if(head->finish_flag == 2 ){//finished
							recv_flag = 1;
						}
						else if(head->finish_flag == 1){//producing
							recv_flag = 0;
						}
					}
					else{
						recv_flag = 1;
					}
					PKG_UINT32(msg, recv_flag, offset); 
				}
				else if(type == 2){//urgent order
					uint32_t recv_flag = 0;
					DEBUG_LOG("exist: %u", head->exist);
					if(head->exist == 0){
						uint32_t cur_hour = get_now_tm()->tm_hour;
						//uint32_t cur_min = get_now_tm()->tm_min;
						int today = get_today();
						DEBUG_LOG("today: %u, cur_hour: %u", today, cur_hour);
						if(today ==  DAY && cur_hour>= 19 && cur_hour < 20 ){
							recv_flag = 1;
						}
						else if(today < DAY || (today == DAY && (cur_hour < 19))){
							recv_flag = 0;
						}
						else{
							recv_flag = 2;
						}
						//recv_flag = 1;
					}
					else{
						recv_flag = 2;
					}

					PKG_UINT32(msg, recv_flag, offset); 

				}
				
				uint32_t orderid = 0;
				if(head->exist != 0){
					orderid = head->order_id;
				}
				//DEBUG_LOG("exist: %u = order_id: %u", head->exist, orderid);
				uint32_t finish_flag = head->finish_flag;
				if(type == 1){
					if(head->finish_flag == 2 && orderid < 5){
						orderid = orderid+1;
						finish_flag = 0;
					}
				}
				else{
					orderid = 6;
				}

				PKG_UINT32(msg, finish_flag, offset);
				PKG_UINT32(msg, head->week_cnt, offset);
				uint32_t k = 0;
				for(; k < 3; ++k){
					PKG_UINT32(msg, greate_items[orderid].order[k].itemid, offset);
					PKG_UINT32(msg, greate_items[orderid].order[k].count, offset);
				}
				for(k = 0; k < 3; ++k){
					PKG_UINT32(msg, greate_items[orderid].award[k].itemid, offset);
					PKG_UINT32(msg, greate_items[orderid].award[k].count, offset);
				}
				PKG_UINT32(msg, head->week_cnt, offset);

				init_proto_head(msg, p->waitcmd, offset);
				return  send_to_self(p, msg, offset, 1);
			}
		case PROTO_PUT_GREAT_PRODUCE_ORDER:
			{

				DEBUG_LOG("[type: %u, order_id: %u, finish_flag:%u, week_id: %u, now: %u]", type, head->order_id, 
						head->finish_flag, head->week_id, get_week_id(time(NULL)));
				if(type == 1){
					uint32_t week_day = get_now_tm()->tm_wday;
					if(head->exist == 1 && week_day != 5 && head->finish_flag != 2){
						 return send_to_self_error(p, p->waitcmd, -ERR_order_cannot_receive_now, 1);
					}
				}
				else if(type == 2){
					uint32_t cur_hour = get_now_tm()->tm_hour;
					int today = get_today();
					if(today != DAY  || ( today == DAY && !(cur_hour>= 19 && cur_hour < 20))){
					//if(today != 20120308){
						 return send_to_self_error(p, p->waitcmd, -ERR_order_cannot_receive_now, 1);
					}

				}
				DEBUG_LOG("[type: %u, order_id: %u, finish_flag:%u]", type, head->order_id, head->finish_flag);
				
				uint32_t order_id = 0;
				if(type == 1){
					if(head->order_id >= 5){
						return send_to_self_error(p, p->waitcmd, -ERR_order_cannot_receive_now, 1);
					}
					if(head->exist == 0){
						order_id = 0;
					}
					else{
						order_id = head->order_id+1;
					}
				}
				else{
					if(head->order_id >= 6){
						return send_to_self_error(p, p->waitcmd, -ERR_order_cannot_receive_now, 1);
					}
					order_id = 6;
				}

				uint32_t db_buff[] = {type, order_id};
				send_request_to_db(SVR_PROTO_USER_PUT_GREAT_PRODUCE_ORDER, NULL, sizeof(db_buff), db_buff, p->id);

				uint32_t msg_buff[2] = {p->id, 1};
				uint32_t msgid = 0;
				if(order_id == 0){
					msgid = 0x0409C340;
				}
				else{
					msgid = 0x0409C32E + (order_id - 1)*2;
				}
				msglog(statistic_logfile, msgid,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

				uint32_t offset =  sizeof(protocol_t);
				init_proto_head(msg, p->waitcmd, offset);
				return  send_to_self(p, msg, offset, 1);
				
			}
		case PROTO_FINISH_GREAT_PRODUCE_ORDER:
			{
				if(type == 1){
					if(head->exist == 0 || (head->exist == 1 && head->finish_flag != 1) || head->order_id > 5){
						return send_to_self_error(p, p->waitcmd, -ERR_order_cannot_finish_now, 1);
					}
				}
				else if(type == 2){
					uint32_t cur_hour = get_now_tm()->tm_hour;
					int today = get_today();
					if(today != DAY  || ( today == DAY && !(cur_hour>= 19 && cur_hour < 20))){
					//if(today != 20120308){
						return send_to_self_error(p, p->waitcmd, -ERR_order_cannot_finish_now, 1);
					}
					else{
						if(head->exist == 0 || (head->exist == 1 && head->finish_flag != 1) || head->order_id != 6){
							return send_to_self_error(p, p->waitcmd, -ERR_order_cannot_finish_now, 1);
						}
						
					}
				}

				int sendlen = 0;
				uint8_t buff[128];
				uint32_t count= 0;
				uint32_t k = 0;
				int32_t off = sendlen + 16;
				for(; k < sizeof(greate_items[head->order_id].order)/sizeof(items_pro_t);++k){
					if(greate_items[head->order_id].order[k].itemid != 0){
						uint32_t item_id =  greate_items[head->order_id].order[k].itemid;
						pkg_item_kind(p, buff, item_id, &off);
						PKG_H_UINT32( buff, greate_items[head->order_id].order[k].itemid, off);
						PKG_H_UINT32( buff, greate_items[head->order_id].order[k].count, off);
						++count;
					}
				}
				PKG_H_UINT32( buff, count, sendlen );
				
				count = 0;
				k = 0;
				for(; k < sizeof(greate_items[head->order_id].award)/sizeof(items_pro_t);++k){
					if(greate_items[head->order_id].award[k].itemid != 0 || greate_items[head->order_id].award[k].count != 0){
						item_kind_t* ik = find_kind_of_item(greate_items[head->order_id].award[k].itemid);
						item_t* it = get_item( ik, greate_items[head->order_id].award[k].itemid );
						pkg_item_kind( p, buff, greate_items[head->order_id].award[k].itemid, &off);
						PKG_H_UINT32( buff, greate_items[head->order_id].award[k].itemid, off);
						PKG_H_UINT32( buff, greate_items[head->order_id].award[k].count, off);
						PKG_H_UINT32( buff, it->max, off);
						++count;
					}
				}
			   PKG_H_UINT32( buff, count, sendlen );
			   PKG_H_UINT32( buff, 202, sendlen );
			   PKG_H_UINT32( buff, 0, sendlen );

				*(uint32_t*) (p->session+4) = head->order_id;
				return send_request_to_db( SVR_PROTO_EXCHG_ITEM, p, off, buff, p->id );
			}

	}//switch
	return 0;
}


int put_majesty_or_urgent_order_cmd(sprite_t* p, uint8_t* body, int len)
{
	uint32_t type = 0;
	CHECK_BODY_LEN(len, sizeof(type));
	int j = 0;
	UNPKG_UINT32(body,type,j);
	*(uint32_t*) (p->session) = type;
	return  send_request_to_db(SVR_PROTO_USER_GET_GREAT_PRODUCE_ORDER, p, 4, &type, p->id);
}


int finish_majesty_or_urgent_order_cmd(sprite_t* p, uint8_t* body, int len)
{
	uint32_t type = 0;
	CHECK_BODY_LEN(len, sizeof(type));
	int j = 0;
	UNPKG_UINT32(body,type,j);
	*(uint32_t*)(p->session) = type;

	 return send_request_to_db(SVR_PROTO_USER_GET_GREAT_PRODUCE_ORDER, p, 4, &type, p->id );
}
int finish_majesty_or_urgent_order_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t offset =  sizeof(protocol_t);
	uint32_t type = *(uint32_t*)(p->session);
	uint32_t order_id = *(uint32_t*)(p->session+4);
	DEBUG_LOG("type: %u, order_id: %u, if 2 is ok", type, order_id);
	if(type == 1){
		if(order_id > 5){
			return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
		}
	}
	else{
		if(order_id != 6){
			return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
		}
	}
	uint32_t db_buff[] = {type, order_id};
	send_request_to_db(SVR_PROTO_USER_FINISH_GREAT_PRODUCE_ORDER, NULL, sizeof(db_buff), db_buff, p->id );

	uint32_t k = 0, l = offset+4, count = 0;
	for(; k < sizeof(greate_items[order_id].award)/sizeof(items_pro_t);++k){
		if(greate_items[order_id].award[k].itemid != 0 || greate_items[order_id].award[k].count != 0){
			PKG_UINT32(msg, greate_items[order_id].award[k].itemid, l); 
			PKG_UINT32(msg, greate_items[order_id].award[k].count, l);
			++count;
		}
		
	}
	PKG_UINT32(msg, count, offset); 
	uint32_t total = *(uint32_t*)(buf);
	if(type == 1){
		if(total < 1000 && order_id == 5){
			    db_exchange_single_item_op(p, 202, 1613501, 100, 0);
				char txt[256] = {0};
#ifndef TW_VER
				int msg_len = sprintf(txt, "%s小摩尔第%u个完成了本周所有的皇家订单，获得100铁锭的额外奖励!", p->nick, total+1);
#else
				int msg_len = sprintf(txt, "%s小摩爾第%u個完成了本周所有的皇家訂單,獲得100鐵錠的額外獎勵！",p->nick, total+1);
#endif
				tell_flash_some_msg_across_svr(NULL, CBMT_MOLE_GREAT_PRODUCE_TASK, msg_len, txt);
				send_request_to_db(SVR_PROTO_USER_SET_GREAT_PRODUCE_NUMBER , NULL, 0, NULL, p->id );
		}
		
	}

	uint32_t msg_buff[2] = {p->id, 1};
	uint32_t msgid = 0;
	if(order_id == 0){
		msgid = 0x0409C32D;
	}
	else{
		msgid = 0x0409C32F + (order_id - 1)*2;
	}
	msglog(statistic_logfile, msgid,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	init_proto_head(msg, p->waitcmd, l);
	return  send_to_self(p, msg, l, 1);

}

int get_greate_produce_fisrt_thousand_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t count = 0;
	CHECK_BODY_LEN(len, sizeof(count));

	return finish_majesty_or_urgent_order_callback(p, id, buf, len);
}

//KFC party
int put_user_KFC_decoration_cmd(sprite_t *p, uint8_t *body, int len)
{

	CHECK_BODY_LEN(len, DECORATE_BUF_SIZE);

	char db_buf[500];
	uint32_t k = 0, offset = 0;
	for(; k < DECORATE_BUF_SIZE; ++k){
		UNPKG_UINT8(body, db_buf[k], offset);
	}
	send_request_to_db(SVR_PROTO_USER_PUT_KFC_DECORATION, NULL, sizeof(db_buf), db_buf, p->id );
	send_request_to_db(SVR_PROTO_USER_PUT_DECORATION_POLL, NULL, 16, p->nick, p->id );
	uint32_t msg_buff[2] = {1, p->id};
	msglog(statistic_logfile, 0x0409C344,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	int l = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}


int get_user_KFC_decoration_cmd(sprite_t *p, uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID( p->id );

	uint32_t uid = 0, j = 0;
	UNPKG_UINT32(body, uid, j);

	return send_request_to_db(SVR_PROTO_GET_KFC_DECORATION, p, 0, NULL, uid);
}
int get_user_KFC_decoration_callback(sprite_t *p, uint32_t id, char* buf, int len)
{ 
	CHECK_BODY_LEN(len, DECORATE_BUF_SIZE);
	uint32_t i = 0;
	int j = sizeof(protocol_t);
	for(;i < DECORATE_BUF_SIZE; ++i){
		PKG_UINT8(msg, *(buf+i), j);
	}
	
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int vote_KFC_friend_decoration_cmd(sprite_t *p, uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID( p->id );

	uint32_t userid = 0, j = 0;
	UNPKG_UINT32(body, userid, j);
	*(uint32_t*)p->session = userid;
 
	uint32_t type = 50018;
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &type, p->id);

}
int vote_KFC_friend_decorate_callback(sprite_t *p, uint32_t id, char* buf, int len)
{

	uint32_t count = *(uint32_t*)buf;
	if(count == 0){
		uint32_t day_buf[] = {50018, 1, 1};
		send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
		uint32_t uid = *(uint32_t*)p->session;
		send_request_to_db(SVR_PROTO_USER_PUT_DECORATION_POLL, NULL, 16, p->nick,  uid);
		{
			uint32_t msg_buff[2] = {p->id, 1};
			msglog(statistic_logfile, 0x0409C345,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}

		int l = sizeof(protocol_t);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}
	else{
		return send_to_self_error(p, p->waitcmd, -ERR_day_limit_vote, 1);
	}
}

int get_poll_friend_decoration_cmd(sprite_t *p, uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t index = 1, j = 0;

	UNPKG_UINT32(body, index, j);
	return send_request_to_db(SVR_PROTO_USER_GET_POLL_DECORATION, p, 4, &index, p->id);
}
int get_poll_friend_decoration_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct poll_head{
		uint32_t total_page;
		uint32_t cur_page;
		uint32_t count;
	}poll_head_t;

	typedef struct poll_decorate{
		uint32_t uid;
		char nick[16];
		uint32_t poll;
	}poll_decorate_t;

	CHECK_BODY_LEN_GE(len, sizeof(poll_head_t));
	
	poll_head_t *head = (poll_head_t*)buf;
	CHECK_BODY_LEN(len, sizeof(poll_head_t)+head->count* sizeof(poll_decorate_t));

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, head->total_page, j);
	PKG_UINT32(msg, head->cur_page, j);
	PKG_UINT32(msg, head->count, j);

	poll_decorate_t* cur_pos = (poll_decorate_t*)(buf+sizeof(poll_head_t));
	uint32_t k = 0;
	for(; k < head->count;++k){
		PKG_UINT32(msg, cur_pos->uid, j);
		PKG_STR(msg, cur_pos->nick, j, sizeof(p->nick));
		PKG_UINT32(msg, cur_pos->poll, j);
		++cur_pos;
	}

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
	
}

int user_catch_butterfly_angel_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	uint32_t today = get_today();
#ifndef TW_VER
	uint32_t activity_day = 20120401;
#else
	uint32_t activity_day = 20120408;
#endif
	if(today != activity_day){
		return send_to_self_error(p, p->waitcmd, -ERR_day_limit_catch_butterfly, 1);
	}
	return  send_request_to_db(SVR_PROTO_USER_CATCH_BUTTERFLY_ANGEL, p, 0, NULL, p->id);
}

int user_catch_butterfly_angel_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t itemid = 0;
	int32_t count = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 8);
	UNPKG_H_UINT32(buf,itemid,j);
	UNPKG_H_UINT32(buf,count,j);
	if(count == -1){
		return send_to_self_error(p, p->waitcmd, -ERR_day_limit_catch_butterfly, 1);
	}
	db_exchange_single_item_op(p, 202, itemid, count, 0);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, i);
	PKG_UINT32(msg, count, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}



int create_lahm_sport_mvp_team_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(bodylen, sizeof(struct mvp_team_info) - 4);
	struct mvp_team_info info;
	unpack(buf, sizeof(info.logo), "L", &info.logo);
	memcpy(info.name, buf + sizeof(info.logo), 16);
	memcpy(p->session, buf + sizeof(info.logo), 16);

	DEBUG_LOG("create team now, who's name is: %s   logo: %u", info.name, info.logo);
	if(tm_dirty_check(7, (char*)(buf + sizeof(info.logo))) > 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_dirty_word, 1);
	}
	info.teamid = p->team_id;

	typedef struct sysarg_mvp_team {
		uint32_t mvp_team;
		uint8_t name[16];
		uint32_t logo;
		uint32_t teamid;
	}sysarg_mvp_team_t;

	sysarg_mvp_team_t *db_buf = (sysarg_mvp_team_t *)p->session;
	memset(db_buf, 0, sizeof(sysarg_mvp_team_t));
	db_buf->logo = info.logo;
	db_buf->mvp_team = p->id;
	db_buf->teamid = info.teamid;
	memcpy(db_buf->name, info.name, sizeof(info.name));
	
	return send_request_to_db(SVR_PROTO_USER_CREATE_MVP_TEAM, p, sizeof(struct mvp_team_info), &info, p->id);
}

int create_lahm_sport_mvp_team_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
    uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C34A, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	        
	CHECK_BODY_LEN_GE(len, 8);
	int i = 0, state = 0, count = 0;
	UNPKG_H_UINT32(buf, state, i);
	UNPKG_H_UINT32(buf, count, i);
	if(state != 0){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	
	typedef struct sysarg_mvp_team {
		uint32_t mvp_team;
		uint8_t name[16];
		uint32_t logo;
		uint32_t teamid;
	}sysarg_mvp_team_t;
	DEBUG_LOG("state: [%u]", state);
	send_request_to_db(SVR_PROTO_SYSARG_BUILD_MVP_TEAM, NULL, sizeof(sysarg_mvp_team_t), p->session, p->id);
	p->mvp_team = p->id;
	uint32_t *f_pos = (uint32_t*)(buf+8); 
	char txt[256];
	uint32_t k = 0;
	for(; k < count; ++k){
#ifdef TW_VER
		snprintf(txt, sizeof txt, "你的好友%s已經開設了MVP小隊，小對號：%u，快去加入他的小隊吧！", p->nick, p->id);
		send_postcard("公民管理處", 0, *f_pos, 1000322, txt, 0);
#else
		snprintf(txt, sizeof txt, "你的好友%s已经开设了MVP小队，小对号：%u, 快去加入他的小队吧！", p->nick, p->id);
		send_postcard("公民管理处", 0, *f_pos, 1000322, txt, 0);
#endif
		++f_pos;
	}
	int j = sizeof(protocol_t);
	PKG_STR(msg,p->session+4, j, 16);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);

}


int enter_lahm_sport_mvp_team_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t userid = 0;
	unpack(buf, sizeof(userid), "L", &userid);
	
	*(uint32_t*)p->session = userid;
	if (userid != 0){
		CHECK_VALID_ID(userid);
	    uint32_t dbbuff[2] = {p->id, p->team_id};
		DEBUG_LOG("mvp_team_id: [%u], ", p->team_id);
		if(p->mvp_team == 0){
			 return send_request_to_db(SVR_PROTO_USER_ENTER_MVP_TEAM, p, sizeof(dbbuff), dbbuff, userid);
		}
		else{
			return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1); 
		}
	   
	}
	else
	{
	    return send_request_to_db(SVR_PROTO_SYSARG_MVP_GET_TEAMID, p, 4, &(p->team_id), p->id);
	}

	return 0;
}

int enter_lahm_sport_mvp_team_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	int i = 0, state = 0, mem_count = 0;
	UNPKG_H_UINT32(buf, state, i);
	UNPKG_H_UINT32(buf, mem_count, i);
	DEBUG_LOG("------state: [%u]", state);
	if(state != 1){
		if(state == 3 || state == 0){
			 return send_to_self_error(p, p->waitcmd, -ERR_too_many_member, 1);
		}
		else if(state == 2){
			 return send_to_self_error(p, p->waitcmd, -ERR_mvp_team_not_exist, 1);
		}
	}
	
	uint32_t mvp_teamid = *(uint32_t*)p->session;
	send_request_to_db(SVR_PROTO_USER_MVP_SET_TEAMID, NULL, 4, &mvp_teamid, p->id);
	//int32_t cnt = 1;
	send_request_to_db(SVR_PROTO_CHANGE_MVP_TEAM_MEMBER_CNT, NULL, 4, &mem_count, mvp_teamid);
	p->mvp_team = mvp_teamid;
	
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}
int user_get_sport_mvp_team_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t mvp_team = 0;
	mvp_team = *(uint32_t*)buf;
	switch(p->waitcmd)
	{
		case PROTO_USER_LAHM_SPORT_LEAVE_MVP_TEAM:
			{
				if(mvp_team == 0){
					return send_to_self_error(p, p->waitcmd, -ERR_mvp_team_user_not_exist_yet, 1);
				}
				else{
					uint32_t leave_user = *(uint32_t*)(p->session+8);
					return send_request_to_db(SVR_PROTO_USER_LEAVE_MVP_TEAM, p, 4, &leave_user, p->mvp_team);
				}
				break;
			}
		case PROTO_USER_LAHM_SPORT_GET_MVP_TEAM_INFO:
			{
				return send_request_to_db(SVR_PROTO_USER_GET_MVP_TEAM_INFO, p, 0, NULL, mvp_team);
				break;
			}
		case PROTO_USER_LAHM_SPORT_GET_SYSARG_MVP_TEAM:
			{
				return send_request_to_db(SVR_PROTO_SYSARG_GET_MVP_TEAM_INFO, p, 0, NULL, mvp_team);
				break;
			}
	}
	return 0;
}
int leave_lahm_sport_mvp_team_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t userid = 0;
	unpack(buf, sizeof(userid), "L", &userid);
	
    if (p->mvp_team == 0){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1); 
	}

    uint32_t leave_user = 0;
	if (userid == 0){
	    leave_user = p->id;	    
	}
	else{
	    if (p->mvp_team != p->id){
	        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1); 
	    }
	    leave_user = userid;
	}

	*(uint32_t*)p->session = p->mvp_team;
	*(uint32_t*)(p->session + 4) = userid;
	*(uint32_t*)(p->session + 8) = leave_user;
	 return send_request_to_db(SVR_PROTO_USER_MVP_GET_TEAMID, p, 0, NULL, leave_user);
   // return send_request_to_db(SVR_PROTO_USER_LEAVE_MVP_TEAM, p, 4, &leave_user, p->mvp_team);
        
}

int get_sysarg_mvp_teamid_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
    CHECK_BODY_LEN(4, len);
	uint32_t mvp_teamid = 0;
	unpkg_host_uint32((uint8_t *)buf, &mvp_teamid);
	
	*(uint32_t*)p->session = mvp_teamid;
	uint32_t dbbuff[2] = {p->id, p->team_id};
    DEBUG_LOG("mvp_team_id: [%u], ", p->team_id);
	return send_request_to_db(SVR_PROTO_USER_ENTER_MVP_TEAM, p, sizeof(dbbuff), dbbuff, mvp_teamid);
  
}


int leave_lahm_sport_mvp_team_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	int i = 0, state = 0, mem_count = 0;
	UNPKG_H_UINT32(buf, state, i);
	UNPKG_H_UINT32(buf, mem_count, i);
	if(state != 1){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1); 
	}
	response_proto_head(p, p->waitcmd, 0);
	uint32_t mvp_teamid = 0;
	uint32_t uid = *(uint32_t*)(p->session + 4);
	if(uid == 0){
		send_request_to_db(SVR_PROTO_USER_MVP_SET_TEAMID, NULL, 4, &mvp_teamid, p->id);
		p->mvp_team = 0;
	}
	else{
		send_request_to_db(SVR_PROTO_USER_MVP_SET_TEAMID, NULL, 4, &mvp_teamid, uid);
	}
	
	//int32_t cnt = -1;
	send_request_to_db(SVR_PROTO_CHANGE_MVP_TEAM_MEMBER_CNT, NULL, 4, &mem_count, *(uint32_t*)p->session);
	
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int dissolve_lahm_sport_mvp_team_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_USER_DISSOLVE_MVP_TEAM, p, 0, NULL, p->id);
}

int dissolve_lahm_sport_mvp_team_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	CHECK_VALID_ID(p->id);
	uint32_t count = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, 4 + count * 4);

	int loop = 0;
	for(loop = 0; loop < count; loop++){
		uint32_t userid = 0;
		UNPKG_H_UINT32 (buf, userid, j);
		uint32_t mvp_teamid = 0;
	    send_request_to_db(SVR_PROTO_USER_MVP_SET_TEAMID, NULL, 4, &mvp_teamid, userid);
#ifndef TW_VER
		send_postcard("公民管理处", 0, userid, 1000324, "", 0);
#else
		send_postcard("公民管理處", 0, userid, 1000324, "", 0);
#endif
	}
	send_request_to_db(SVR_PROTO_SYSARG_DROP_MVP_TEAM, NULL, 4, &p->id, p->id);
	p->mvp_team = 0;

    response_proto_head(p, p->waitcmd, 0);
    return 0;
}

int get_lahm_sport_mvp_team_info_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t i = 0, mvp_team = 0;
	UNPKG_UINT32(buf, mvp_team, i);
	*(uint32_t*)p->session = mvp_team;
	//DEBUG_LOG("===mvp_team:[ %u], userid: %u, p->team: %u", mvp_team, p->id, p->mvp_team);

	if(mvp_team == 0){
		//mvp_team = p->mvp_team;
		return send_request_to_db(SVR_PROTO_USER_MVP_GET_TEAMID, p, 0, NULL, p->id);
	}
	else{
		return send_request_to_db(SVR_PROTO_USER_GET_MVP_TEAM_INFO, p, 0, NULL, mvp_team);
	}
}

int get_lahm_sport_mvp_team_info_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct team_info_head{
		uint32_t userid;
		uint8_t name[16];
		uint32_t logo;
		uint32_t teamid;
		uint32_t badge;
		uint32_t count;
	}team_info_head_t;

	typedef struct team_info{
		uint32_t userid;
	}team_info_t;

	CHECK_BODY_LEN_GE(len, sizeof(team_info_head_t));
	
	team_info_head_t *head = (team_info_head_t*)buf;
	CHECK_BODY_LEN(len, sizeof(team_info_head_t)+head->count* sizeof(team_info_t));
	DEBUG_LOG("mvp_team:[ %u] name: [%s] logo:[%u] sport_team:[%u] badge:[%u] count:[%u]", head->userid,
		head->name, head->logo, head->teamid, head->badge, head->count);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, head->userid, j);
	PKG_STR(msg, head->name, j, sizeof(head->name));
	PKG_UINT32(msg, head->logo, j);
	if(*(uint32_t*)p->session == 0){
		PKG_UINT32(msg, p->team_id, j);
	}
	else{
		PKG_UINT32(msg, head->teamid, j);
	}
	PKG_UINT32(msg, head->badge, j);
	PKG_UINT32(msg, head->count, j);

	team_info_t* cur_pos = (team_info_t*)(buf+sizeof(team_info_head_t));
	uint32_t k = 0;
	for(; k < head->count;++k){
		PKG_UINT32(msg, cur_pos->userid, j);
		++cur_pos;
	}

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);	
}

int user_change_lahm_sport_team_cmd(sprite_t *p, uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len , 0 );
	CHECK_VALID_ID(p->id);
	
	if(p->team_id == 0){
		return send_to_self_error(p, p->waitcmd, -ERR_use_not_enter_lahm_sport_team, 1); 
	}

	return db_delete_single_item_op(p, 202, 1351308, 1, 1);

}

int user_change_lahm_sport_team_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t team_id = p->team_id;
	if(p->team_id == 1){
		team_id = 2;
	}
	else{
		team_id = 1;
	}
	
	*(uint32_t* )p->session = team_id;
	 //return send_to_self_error(p, p->waitcmd, -ERR_change_lahm_sport_need_bean, 1);
	return send_request_to_db(SVR_PROTO_SPORT_SIGN, p, sizeof(team_id), &team_id, p->id);
}

int user_get_mvp_team_ranking_cmd(sprite_t *p, uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t i = 0, page = 1;
	UNPKG_UINT32(buf, page, i);
	DEBUG_LOG("=====page: %u =====", page);
	return send_request_to_db(SVR_PROTO_GET_MVP_TEAM_RANKING, p, sizeof(page), &page, p->id);
}


int user_get_mvp_team_ranking_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct mvp_rank_head{
		uint32_t total_page;
		uint32_t cur_page;
		uint32_t count;
	}mvp_rank_head_t;

	typedef struct mvp_ranking{
		uint32_t mvp_team;
		uint32_t sport_team;
		char mvp_team_name[16];
		uint32_t logo;
		uint32_t badge;
		uint32_t mem_count;
	}mvp_ranking_t;
	CHECK_BODY_LEN_GE(len, sizeof(mvp_rank_head_t));

	
	mvp_rank_head_t *head = (mvp_rank_head_t*)buf;
	CHECK_BODY_LEN(len, sizeof(mvp_ranking_t)*head->count + sizeof(mvp_rank_head_t));
	DEBUG_LOG("total_page: %u, cur_page: %u, count: %u",
		head->total_page, head->cur_page, head->count);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, head->total_page, j);
	PKG_UINT32(msg, head->cur_page, j);
	PKG_UINT32(msg, head->count, j);
	mvp_ranking_t *cur_pos = (mvp_ranking_t*)(buf+ sizeof(mvp_rank_head_t));
	uint32_t k = 0;
	for(; k < head->count; ++k){
		PKG_UINT32(msg, cur_pos->mvp_team, j);
		PKG_UINT32(msg, cur_pos->sport_team, j);
		PKG_STR(msg,cur_pos->mvp_team_name, j, sizeof(cur_pos->mvp_team_name));
		PKG_UINT32(msg, cur_pos->badge, j);
		PKG_UINT32(msg, cur_pos->logo, j);
		PKG_UINT32(msg, cur_pos->mem_count, j);
		DEBUG_LOG("mvp_team: %u, sport_team: %u, logo: %u, badge: %u, memcount: %u",
			cur_pos->mvp_team, cur_pos->sport_team, cur_pos->logo, cur_pos->badge, cur_pos->mem_count);
		++cur_pos;
     
	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);

}

int get_two_team_medal_cnt_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return  send_request_to_db(SVR_PROTO_SYSARG_GET_TWO_TEAM_MEDAL_CNT, p, 0, NULL, p->id);
}

int get_two_team_medal_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t team1_cnt = 0;
	uint32_t team2_cnt = 0;
    uint32_t team1_score = 0;
	uint32_t team2_score = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 16);
	UNPKG_H_UINT32(buf,team1_cnt,j);
	UNPKG_H_UINT32(buf,team2_cnt,j);
	UNPKG_H_UINT32(buf,team1_score,j);
	UNPKG_H_UINT32(buf,team2_score,j);
	
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, team1_cnt, i);
	PKG_UINT32(msg, team2_cnt, i);
	PKG_UINT32(msg, team1_score, i);
	PKG_UINT32(msg, team2_score, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_lahm_sport_sysarg_mvp_team_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t i = 0, mvp_team = 0;
	UNPKG_UINT32(buf, mvp_team, i);
	*(uint32_t*)p->session = mvp_team;
	//DEBUG_LOG("===mvp_team:[ %u], userid: %u, p->team: %u", mvp_team, p->id, p->mvp_team);

	if(mvp_team == 0){
		//mvp_team = p->mvp_team;
		return send_request_to_db(SVR_PROTO_USER_MVP_GET_TEAMID, p, 0, NULL, p->id);
	}
	else{
		return send_request_to_db(SVR_PROTO_SYSARG_GET_MVP_TEAM_INFO, p, 0, NULL, mvp_team);
	}
}


int get_lahm_sport_sysarg_mvp_team_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct sys_team_info_head{
		uint32_t userid;
		uint8_t name[16];
		uint32_t logo;
		uint32_t teamid;
		uint32_t badge;
		uint32_t count;
	}sys_team_info_head_t;

	CHECK_BODY_LEN(len, sizeof(sys_team_info_head_t));
	
	sys_team_info_head_t *head = (sys_team_info_head_t*)buf;

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, head->userid, j);
	PKG_STR(msg, head->name, j, sizeof(head->name));
	PKG_UINT32(msg, head->logo, j);
	PKG_UINT32(msg, head->teamid, j);
	PKG_UINT32(msg, head->badge, j);
	PKG_UINT32(msg, head->count, j);

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);	
}

int choice_cute_piglet_rush_activity_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 8);
#ifndef TW_VER
 uint32_t activity_day = 20120401;
#else
 uint32_t activity_day = 20120408;
#endif
	 uint32_t today = get_today();
	if (!(today == activity_day && get_now_tm()->tm_hour >= 19 && get_now_tm()->tm_hour <= 20)){
		return send_to_self_error(p, p->waitcmd, -ERR_piglet_rush_acitvity_day, 1 );
	}

	typedef struct{
		uint32_t round;
		uint32_t pigid;
	} choice_champ_t;
	choice_champ_t option = {0};
	int j = 0;
	UNPKG_UINT32(buf, option.round, j);
	if( option.round <= 0 || option.round > 12){ //|| !(get_now_tm()->tm_min < option.round*5 && get_now_tm()->tm_min >= (option.round -1)*5)){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1 );
	}
	UNPKG_UINT32(buf, option.pigid, j);
	
	send_request_to_db(SVR_PROTO_CHOICE_WHICH_PIGLET_CHAMPOIN, NULL, sizeof(option), &option, p->id);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, option.round, l);
	PKG_UINT32(msg, option.pigid, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);	

}

int check_cute_piglet_champion_activity_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	 return send_request_to_db(SVR_PROTO_QUERY_WHICH_PIGLET_CHAMPOIN, p, 0, NULL, p->id);

}
int check_cute_piglet_champion_activity_callback(sprite_t *p, uint32_t id, char* buf, int len)
{

	typedef struct{
		uint32_t round;
		uint32_t is_get_award;
		uint32_t pigid;
	} choice_champ_t;

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = 0;
	int j = 0;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, sizeof(count)+ count*sizeof(choice_champ_t));
	
	choice_champ_t *head = (choice_champ_t*)(buf+4);
	int l = sizeof(protocol_t);
	uint32_t k = 0;
	PKG_UINT32(msg, count, l);
	for(; k < count; ++k){
		//DEBUG_LOG("round: %u, pigletid: %u", head->round, head->pigid);
		PKG_UINT32(msg, head->round, l);
		PKG_UINT32(msg, head->pigid, l);
		if(head->round <= 0 || head->round > 12){
			return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1 );
		}
		if(champion_piglet[head->round -1] == head->pigid){
			PKG_UINT32(msg, 1, l);
		}
		else{
			PKG_UINT32(msg, 0, l);
		}
		PKG_UINT32(msg, head->is_get_award, l);
		++head;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int get_piglet_champion_activity_award_cmd(sprite_t *p, const uint8_t* buf, int len)
{
	CHECK_BODY_LEN(len ,4);

	uint32_t db_buf[2] = {0};
	int j = 0;
	UNPKG_UINT32(buf, db_buf[0], j);
	if(db_buf[0] <= 0 || db_buf[0] > 12){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1 );
	}

	db_buf[1] = champion_piglet[db_buf[0]-1];

	return send_request_to_db(SVR_PROTO_GET_PIGLET_CHAMPOIN_AWARD, p, sizeof(db_buf), db_buf, p->id);
}
int get_piglet_champion_activity_award_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct{
		int32_t state;
		uint32_t round;
		uint32_t pigletid;
	} champ_award_t;

	CHECK_BODY_LEN(len, sizeof(champ_award_t));
	champ_award_t *cur = (champ_award_t*)buf;

	int l = sizeof(protocol_t);
	if(cur->state == -1){
		cur->state = 2;
	}
	PKG_UINT32(msg, cur->state, l);
	PKG_UINT32(msg, cur->round, l);
	PKG_UINT32(msg, cur->pigletid, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int user_water_jack_and_modou_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);


	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3A7,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	uint32_t db_type = 50024;
	
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &db_type, p->id);
	
}

int user_water_jack_and_modou_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C3A7,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
			
	uint32_t count = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 4);
	UNPKG_H_UINT32(buf,count,j);

	int i = sizeof(protocol_t);
	uint32_t day_limit = *(uint32_t*)p->session;
	uint32_t itemid = 1351328;
	if(day_limit != 0 && day_limit % 5 == 0 && day_limit <= 100){
		PKG_UINT32(msg,  itemid , i);
		db_exchange_single_item_op(p, 202, itemid, 1, 0);
	}
	else{
		PKG_UINT32(msg, 0 , i);
	}
	db_exchange_single_item_op(p, 202, 1351340, 1, 0);

	uint32_t day_buf[] = {50024, 999999999, 1};
	send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_water_time_jack_and_modou_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return  send_request_to_db(SVR_PROTO_SYSARG_GET_WATER_TIME, p, 0, NULL, p->id);
}

int get_water_time_jack_and_modou_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t count = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 4);
	UNPKG_H_UINT32(buf,count,j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, count, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int user_shake_dice_for_vip_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return  send_request_to_db(SVR_PROTO_USER_SHAKE_DICE, p, 0, NULL, p->id);
}

int user_shake_dice_for_vip_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t count = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 4);
	UNPKG_H_UINT32(buf,count,j);

	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C3BC,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	if (count == 6){
	    msglog(statistic_logfile, 0x0409C3BD,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, count, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int user_get_signal_flare_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	if( p->tiles->id != 240 )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_not_valid_map, 1 );
	}
	int flag = 0;
	CHECK_BODY_LEN( len, sizeof(flag) );
	int j = 0;
	UNPKG_UINT32( body, flag, j );
	if( flag > 1)
	{
		return send_to_self_error( p, p->waitcmd, -ERR_invalid_session, 1 );
	}

	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C3B8,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, p->id, msg_len );
	PKG_UINT32( msg, flag, msg_len );
	init_proto_head( msg, p->waitcmd, msg_len );
	send_to_map( p, msg, msg_len, 1 );
	return 0;
}

int user_get_shake_dice_left_time_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return  send_request_to_db(SVR_PROTO_USER_GET_SHAKE_DICE_LEFT_TIME, p, 0, NULL, p->id);
}

int user_get_shake_dice_left_time_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t left_time = 0;
	uint32_t six_flag = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 8);
	UNPKG_H_UINT32(buf,left_time,j);
	UNPKG_H_UINT32(buf,six_flag,j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, left_time, i);
	PKG_UINT32(msg, six_flag, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int  user_guess_stone_scissors_cloth_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	//stone 1, scissors 2, cloth 3
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t type = 0;
	int j = 0;
	UNPKG_UINT32(body, type, j);
	return  send_request_to_db(SVR_PROTO_GUESS_SSCLOTH, p, sizeof(type), &type, p->id);
}

int user_guess_stone_scissors_cloth_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t state;
		uint32_t itemid;
		uint32_t self_type;
		uint32_t kula_type;
	}ret_guess_t;

	CHECK_BODY_LEN(len, sizeof(ret_guess_t));
	
	ret_guess_t* cur = (ret_guess_t*)buf;
	DEBUG_LOG("state: %u, itemid; %u", cur->state, cur->itemid);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, cur->state, i);
	PKG_UINT32(msg, cur->itemid, i);
	PKG_UINT32(msg, cur->self_type, i);
	PKG_UINT32(msg, cur->kula_type, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int user_join_cosplay_race_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	int j = 0;
	uint32_t starid = 0;
	UNPKG_UINT32(body,starid, j);
	if(starid < 1 || starid > 6){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1 );
	}

	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C41E + starid - 1,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

    uint32_t score = 0;
	uint32_t i = 0;
	for (i = 0; i < p->item_cnt; i++){
	    uint32_t k = 0;
	    for(k = 0; k < 4; k++){
	        uint32_t tmpid = cos_star_items[starid - 1].item_score[k].itemid;
	        if (p->items[i] == tmpid){
	            score = score +  cos_star_items[starid - 1].item_score[k].score;	            
	        }
	    }
	}

	uint32_t cnt = 0;
	if (score == 100){
	    cnt = 20;
	}
	else if (score >= 50 && score <= 99){
	    cnt = 10;
	}
	else if (score >= 20 && score <= 49){
	    cnt = 8;
	}
    else if (score > 0 && score <= 19){
	    cnt = 5;
	}
	else{
	    cnt = 2;
	}

	*(uint32_t*)p->session = score;
    uint32_t dbbuf[2] = {starid, cnt};
	return  send_request_to_db(SVR_PROTO_USER_JOIN_COSPLAY_RACE, p, 8, dbbuf, p->id);
}

int user_join_cosplay_race_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    typedef struct{
		uint32_t count;
		char     nick[NICK_LEN];
	} cos_user_info_t;
	
    uint32_t score = 0;
    score = *(uint32_t*)p->session;
	uint32_t itemid = 0;
	uint32_t count = 0;
	uint32_t all_cnt = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 12);
	UNPKG_H_UINT32(buf,all_cnt,j);
	UNPKG_H_UINT32(buf,itemid,j);
	UNPKG_H_UINT32(buf,count,j);

    cos_user_info_t user_info;
    memcpy(user_info.nick, p->nick, NICK_LEN);
    user_info.count = all_cnt;
	send_request_to_db(SVR_PROTO_SYSARG_SET_COS_USER_INFO, NULL, sizeof(user_info), &user_info, p->id);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, score, i);
	PKG_UINT32(msg, itemid, i);
	PKG_UINT32(msg, count, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int sysarg_get_cos_rank_info_cmd(sprite_t *p, uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	uint32_t i = 0, date = 0;
	UNPKG_UINT32(buf, date, i);
	return send_request_to_db(SVR_PROTO_SYSARG_GET_COS_RANK_INFO, p, sizeof(date), &date, p->id);
}


int sysarg_get_cos_rank_info_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct cos_rank_head{
		uint32_t count;
	}cos_rank_head_t;

	typedef struct cos_rank{
		uint32_t userid;
		uint32_t cnt;
		char nick[NICK_LEN];
	}cos_rank_t;
	CHECK_BODY_LEN_GE(len, sizeof(cos_rank_head_t));

	cos_rank_head_t *head = (cos_rank_head_t*)buf;
	CHECK_BODY_LEN(len, sizeof(cos_rank_t)*head->count + sizeof(cos_rank_head_t));

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, head->count, j);
	cos_rank_t *cur_pos = (cos_rank_t*)(buf+ sizeof(cos_rank_head_t));
	uint32_t k = 0;
	for(; k < head->count; ++k){
		PKG_UINT32(msg, cur_pos->userid, j);
		PKG_UINT32(msg, cur_pos->cnt, j);
		PKG_STR(msg,cur_pos->nick, j, NICK_LEN);
		++cur_pos;
	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);

}

int get_user_cos_rank_flag_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t i = 0, flag = 0;
	UNPKG_UINT32(body, flag, i);
	 *(uint32_t*)p->session = flag;
	return  send_request_to_db(SVR_PROTO_SYSARG_GET_COS_RANK_FLAG, p, 0, NULL, p->id);
}

int get_user_cos_rank_flag_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    uint32_t is_flag = *(uint32_t*)p->session;
    
	uint32_t flag = 0;
	int j = 0;
	CHECK_BODY_LEN(len, 4);
	UNPKG_H_UINT32(buf,flag,j);

	uint32_t itemid = 0;

	if (!is_flag){
	    itemid = 0;
	    if (flag){
	        if(check_only_one_bit(p, 216)){
	            flag = 2;
	        }
	    }
	}
	else{
	    if (flag > 0){
	        if(set_only_one_bit(p, 216)){
	            itemid = 14283;
	            db_exchange_single_item_op(p, 202, itemid, 1, 0);
	        }
	        else{
	            flag = 2;
	            itemid = 0;
	        }
	    }
	}

    int i = sizeof(protocol_t);
	PKG_UINT32(msg, flag, i);
	PKG_UINT32(msg, itemid, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
	
}



