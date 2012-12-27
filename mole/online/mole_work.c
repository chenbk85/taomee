
#include <statistic_agent/msglog.h>

#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "tasks.h"
#include "central_online.h"
#include "mole_work.h"



#define db_set_engineer_level(p_, buf_)	\
		send_request_to_db(SVR_PROTO_SET_ENGINEER_LEVEL, (p_), 4, buf_, (p_)->id)

/*
 *@ get work number
 */
int set_work_num_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t work_exid[2] = {1, 8};

	int j = 0;
	uint32_t work_type;
	UNPKG_UINT32(body, work_type, j);
	*(uint32_t*)p->session = work_type;
	if (work_type > 2 || work_type == 0) {
		ERROR_RETURN(("bad work type\t[id=%u type=%u]", p->id, work_type), -1);
	}

	DEBUG_LOG("SET WORK NUM\t[%u %u]", p->id, work_type);
	return db_set_sth_done(p, work_exid[work_type - 1], 1, p->id);
}

/*
 *@ get work number, process db's reply
 */
int set_work_num_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t total_count = *(uint32_t*)buf;

	DEBUG_LOG("SET WORK NUM CALLBACK\t[%u %u]", p->id, total_count);
	if (total_count == 6) {
		uint32_t type = 2; // level 2
		p->engineer = 2;
		return db_set_engineer_level(p, &type);
	}
	if (total_count == 30) {
		uint32_t type = 3; // level 3
		p->engineer = 3;
		return db_set_engineer_level(p, &type);
	}
	if (total_count == 60) {

		uint32_t type = 4; // level 3
		p->engineer = 4;
		return db_set_engineer_level(p, &type);
	}

	uint32_t work_type = *(uint32_t*)p->session;
	if (work_type == 1)
	{
	    uint32_t info_m[2] = {1, p->id};
	    msglog(statistic_logfile, 0x04040001, now.tv_sec, info_m, sizeof(info_m));
	}
	else if (work_type == 2)
	{
	    uint32_t info_m[2] = {1, p->id};
	    msglog(statistic_logfile, 0x04040000, now.tv_sec, info_m, sizeof(info_m));
	}

	response_proto_uint32_uint32(p, p->waitcmd, p->yxb, 0, 0);
	return 0;
}

/*
 *@ get work number
 */
int get_work_num_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	DEBUG_LOG("GET WORK NUM\t[%u]", p->id);
	return db_get_work_num(p);
}

/*
 *@ get work number, process db's reply
 */
int get_work_num_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 20);

	uint32_t total_count = *(uint32_t*)buf;
	uint32_t last_week_tom_count = *(uint32_t*)(buf + 4);
	uint32_t last_week_nik_count = *(uint32_t*)(buf + 8);
	uint32_t this_week_tom_count = *(uint32_t*)(buf + 12);
	uint32_t this_week_nik_count = *(uint32_t*)(buf + 16);

	DEBUG_LOG("GET WORK NUM CALLBACK\t[%u %u %u %u %u %u]", p->id, total_count, last_week_tom_count,
			last_week_nik_count, this_week_tom_count, this_week_nik_count);
	switch (p->waitcmd) {
	case  PROTO_GET_WORK_NUM:
	{
		int j = sizeof(protocol_t);
		PKG_UINT32(msg, total_count, j);
		PKG_UINT32(msg, last_week_tom_count, j);
		PKG_UINT32(msg, last_week_nik_count, j);
		PKG_UINT32(msg, this_week_tom_count, j);
		PKG_UINT32(msg, this_week_nik_count, j);
		init_proto_head(msg, p->waitcmd, j);
		return send_to_self(p, msg, j, 1);
	}
	case  PROTO_GET_WORK_SALARY:
	{
		uint32_t xiaomee_count = 0;
		if (p->engineer == 4)
			xiaomee_count = 100 * (last_week_tom_count + last_week_nik_count);
		else
			xiaomee_count = 50 * (last_week_tom_count + last_week_nik_count);

		DEBUG_LOG("GET WORK SALARY\t[%u %u]", p->id, xiaomee_count);
		return db_change_xiaome(p, xiaomee_count, ATTR_CHG_roll_back, 0, p->id);
	}
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}
}

/*
 *@ get work number
 */
int get_work_salary_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	DEBUG_LOG("GET WORK SALARY\t[%u]", p->id);

	return db_set_sth_done(p, 1000000013, 1, p->id);
}


/*
 *@ get work number, process db's reply
 */
int set_engineer_level_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	switch (p->waitcmd) {
	case PROTO_SET_WORK_NUM:
	{
		if (p->engineer == 4) {
			DEBUG_LOG("UPLOAD CONSTRUCT CARD\t[%u]", p->id);
			unwear_item(p, 12861);
			db_single_item_op(0, p->id, 12861, 1, 4);
			db_single_item_op(0, p->id, 12861, 1, 0);
			db_single_item_op(0, p->id, 12905, 1, 1);
			#ifdef TW_VER
				send_postcard("湯米", 0, p->id, 1000079, "   恭喜你成為莊園的建設總工程師！你的建設卡已經升級為建設金卡嘍，快去百寶箱里看看吧！", 0);
			#else
				send_postcard("汤米", 0, p->id, 1000079, "   恭喜你成为庄园的建设总工程师！你的建设卡已经升级为建设金卡喽，快去百宝箱里看看吧！", 0);
			#endif
			response_proto_uint32_uint32(p, p->waitcmd, p->yxb, 1, 0);
			send_attr_update_noti(0, p, p->id, 3);
			return 0;
		}
		response_proto_uint32_uint32(p, p->waitcmd, p->yxb, 0, 0);
		return 0;
	}
	case PROTO_SET_WORKER:
		db_single_item_op(0, p->id, 12861, 1, 1);
		response_proto_head(p, p->waitcmd, 0);
		return 0;
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}
}

/*
 *@ set worker
 */
int set_worker_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{

	DEBUG_LOG("SET WORKER\t[%u]", p->id);
	uint32_t type_level = 1;
	p->engineer = 1;
	return db_set_engineer_level(p, &type_level);
}

/*
 *@ mole_game
 */
int get_mole_self_games_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	return send_request_to_db(SVR_PROTO_GET_GAME_EXP_RANK, p, 0, NULL, p->id);
}
int get_mole_self_game_rank_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	*(uint32_t*)p->session = *(uint32_t*)buf;
	DEBUG_LOG("rank: %u ---", *(uint32_t*)p->session);
	return send_request_to_db(SVR_PROTO_MOLE_GET_SELF_GAMES, p, 0, NULL, p->id);

}
int get_mole_self_games_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t gameid;
		uint32_t score;
	}game_info_t;
	CHECK_BODY_LEN_GE(len, 8);
	uint32_t game_exp = *(uint32_t*)(buf);
	uint32_t count = *(uint32_t*)(buf+4);
	CHECK_BODY_LEN(len, sizeof(game_info_t)*count + 8);

	uint32_t rank = *(uint32_t*)p->session;
	DEBUG_LOG("game_exp: %u, rank: %u, count: %u",game_exp, rank, count);
	
	game_info_t * head = (game_info_t*)(buf+8);
	int i = sizeof (protocol_t);
	PKG_UINT32(msg, game_exp, i);
	PKG_UINT32(msg, rank, i);
	PKG_UINT32(msg, count, i);
	int k = 0;
	for(; k < count; ++k){
		DEBUG_LOG("gameid: %u, score: %u",(head+k)->gameid, (head+k)->score);
		PKG_UINT32(msg, (head+k)->gameid, i);
		PKG_UINT32(msg, (head+k)->score, i);
	}
	
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int mole_enter_game_hall_cmd(sprite_t *p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	
	return send_request_to_db(SVR_PROTO_MOLE_ENTER_GAME_HALL, p, 0, NULL, p->id);

}
int mole_enter_game_hall_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t userid;
		char nick[16];
		uint32_t gameid;
		uint32_t score;
		uint32_t datetime;
	}game_hall_t;
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, sizeof(game_hall_t) * count + sizeof(count));

	game_hall_t* cur = (game_hall_t*)(buf+4);
	int i = sizeof (protocol_t);
	PKG_UINT32(msg, count, i);
	int k = 0;
	for(; k < count; ++k){
		PKG_UINT32(msg, (cur+k)->userid, i);
		PKG_STR (msg, (cur+k)->nick, i, 16);
		PKG_UINT32(msg, (cur+k)->gameid, i);
		PKG_UINT32(msg, (cur+k)->score, i);
		PKG_UINT32(msg, (cur+k)->datetime, i);
	}
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);

}
