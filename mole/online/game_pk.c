
#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "tasks.h"

#include "game_pk.h"

/*
 *@ get game grade list of friends
 */
int get_frd_game_grade_list_cmd(sprite_t* p, const uint8_t* body, int len)
{
	int count;
	uint32_t gid;
	uint32_t buff[SPRITE_FRIENDS_MAX + 2];
	uint32_t* uid = buff + 2;

	DEBUG_LOG ("GET FRIEND GAME GRADE LIST\t[%u]", p->id);
	if (IS_GUEST_ID (p->id) || IS_NPC_ID (p->id)) {
		ERROR_RETURN(("not a register user\t[%u]", p->id), -1);
	}

	CHECK_BODY_LEN_GE(len, 4);

	int i = 0;
	UNPKG_UINT32(body, gid, i);
	UNPKG_UINT32(body, count, i);
	buff[0] = gid;
	buff[1] = count;
	if (count > SPRITE_FRIENDS_MAX) {
		ERROR_RETURN(("too many users, count=%d", count), -1);
	}
	CHECK_BODY_LEN(len, count * 4 + 8);

	int j = 0;
	for (; j != count; ++j) {
		UNPKG_UINT32(body, uid[j], i);
		CHECK_VALID_ID(uid[j]);
		//DEBUG_LOG ("USER ID\t[%u %u %u]", p->id, gid, uid[j]);
	}

	return db_get_grade_list_by_gid(p, i, buff);
}

/*
 *@ get the friend grade list
 */
int get_frd_game_grade_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct score_list
{
	uint32_t uid;
	uint32_t score;
}__attribute__((packed))score_list_t;

	CHECK_BODY_LEN_GE(len, 4);
	int count = *(uint32_t*)buf;
	int l = count * sizeof(score_list_t) + 4;
	CHECK_BODY_LEN(len, l);

	int i = sizeof(protocol_t);
	int loop;
	score_list_t* slp = (score_list_t*)(buf + 4);
	PKG_UINT32(msg, count, i);
	DEBUG_LOG ("GET FRIEND GAME GRADE CALLBACK\t[%u %u]", p->id, count);
	for(loop = 0; loop < count; loop++, slp++){
		PKG_UINT32(msg, slp->uid, i);
		PKG_UINT32(msg, slp->score, i);
		//DEBUG_LOG ("USER SCORE\t[%u %u %u]", p->id, slp->uid, slp->score);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/*
 *@ get game grade list
 */
int get_game_grade_list_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	int j = 0;
	userid_t uid;
	UNPKG_UINT32(body, uid, j);
	if (IS_GUEST_ID (uid) || IS_NPC_ID (uid)) {
		ERROR_RETURN(("not a register user\t[%u %u]", p->id, uid), -1);
	}

	DEBUG_LOG ("GET GAME GRADE LIST\t[%u]", p->id);
	return db_get_grade_list_by_uid(p, &uid);
}

/*
 *@ get the friend grade list
 */
int get_game_grade_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct pk_score_list
{
	uint32_t gid;
	uint32_t score;
	uint32_t pk_flag;
}__attribute__((packed))pk_score_list_t;

	CHECK_BODY_LEN_GE(len, 4);
	int count = *(uint32_t*)buf;
	int l = count * sizeof(pk_score_list_t) + 4;
	CHECK_BODY_LEN(len, l);

	int i = sizeof(protocol_t);
	int loop;
	pk_score_list_t* psl = (pk_score_list_t*)(buf + 4);
	PKG_UINT32(msg, count, i);
	for(loop = 0; loop < count; loop++, psl++){
		PKG_UINT32(msg, psl->gid, i);
		PKG_UINT32(msg, psl->score, i);
		PKG_UINT32(msg, psl->pk_flag, i);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/*
 *@ vote flower or mud
 */
int set_pk_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t pkee;
	uint32_t gid;
	uint32_t score;

	CHECK_BODY_LEN(bodylen, 12);

	int j = 0;
	UNPKG_UINT32(body, pkee, j);
	UNPKG_UINT32(body, gid, j);
	UNPKG_UINT32(body, score, j);

	if(p->id == pkee){
		ERROR_RETURN(("%u,error can't PK yourself", p->id), -1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session, pkee, p->sess_len);
	PKG_H_UINT32(p->session, gid, p->sess_len);
	PKG_H_UINT32(p->session, score, p->sess_len);

	DEBUG_LOG("SET PK INFO\t[%u %u %u %u]", p->id, pkee, gid, score);
	return db_set_sth_done(p, 139, 30, p->id);
}

/*
 *@ vote someone's home, handle db 's reply
 */
int set_pk_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);

	uint32_t usr_score = *(uint32_t*)(buf);
	uint32_t usr_pkee_score = *(uint32_t*)(buf + 4);
	int 	 pk_flag = *(uint32_t*)(buf + 8);
	userid_t pkee = *(uint32_t*)p->session;
	uint32_t gid = *(uint32_t*)(p->session + 4);
	//uint32_t score = *(uint32_t*)(p->session + 8);

	db_add_history_pk_info(p, gid, pk_flag, 0, pkee, usr_score, usr_pkee_score);
	DEBUG_LOG("ADD PK TO HISTORY\t[%u]", p->id);
	int 	tmp_value;
	uint32_t HLB = 0;
	if (pk_flag == 1) {  // pker succ
		tmp_value = 20;
		HLB = 190389;
		db_single_item_op(0, p->id, HLB, 1, 1);
	} else {
		tmp_value = 5;
	}
	uint32_t itmid = 0;
	if (p->game_king < 400 && p->game_king + tmp_value >= 400) {
		itmid = 160407;
#ifdef TW_VER
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成為火心勇士，一枚火心勇士勛章已經放入你的小屋倉庫啦！", 0);
#else
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成为火心勇士，一枚火心勇士勋章已经放入你的小屋仓库啦！", 0);
#endif
	}

	if (p->game_king < 2400 && p->game_king + tmp_value >= 2400) {
		itmid = 160408;
#ifdef TW_VER
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成為火光勇士，一枚火光勇士勛章已經放入你的小屋倉庫啦！", 0);
#else
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成为火光勇士，一枚火光勇士勋章已经放入你的小屋仓库啦！", 0);
#endif
	}

	if (p->game_king < 4400 && p->game_king + tmp_value >= 4400) {
		itmid = 160409;
#ifdef TW_VER
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成為火焰勇士，一枚火焰勇士勛章已經放入你的小屋倉庫啦！", 0);
#else
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成为火焰勇士，一枚火焰勇士勋章已经放入你的小屋仓库啦！", 0);
#endif
	}
	if (p->game_king < 6400 && p->game_king + tmp_value >= 6400) {
		itmid = 160421;
#ifdef TW_VER
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成為火岩勇士，一枚火岩勇士勛章已經放入你的小屋倉庫啦！", 0);
#else
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成为火岩勇士，一枚火岩勇士勋章已经放入你的小屋仓库啦！", 0);
#endif
	}

	if (p->game_king < 8400 && p->game_king + tmp_value >= 8400) {
		itmid = 160422;
#ifdef TW_VER
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成為火翼勇士，一枚火翼勇士勛章已經放入你的小屋倉庫啦！", 0);
#else
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成为火翼勇士，一枚火翼勇士勋章已经放入你的小屋仓库啦！", 0);
#endif
	}

	if (p->game_king < 10400 && p->game_king + tmp_value >= 10400) {
		itmid = 160423;
#ifdef TW_VER
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成為火神勇士，一枚火神勇士勛章已經放入你的小屋倉庫啦！", 0);
#else
		send_postcard("洛克行政官", 0, p->id, 1000074, "哈哈，你真是太棒啦！恭喜你成为火神勇士，一枚火神勇士勋章已经放入你的小屋仓库啦！", 0);
#endif
	}
	if (itmid) {
		char buf[20];
		int i = 0;
		PKG_H_UINT32(buf, 1, i);
		PKG_H_UINT32(buf, itmid, i);
		PKG_H_UINT32(buf, 1, i);
		PKG_H_UINT32(buf, 1, i);
		PKG_H_UINT32(buf, 0, i);
		DEBUG_LOG("YXW GIFT[%u %u]", p->id, itmid);
		send_request_to_db(SVR_PROTO_BUY_ITEM, 0, 20, buf, p->id);
	}

	p->game_king += tmp_value;
	db_set_game_king(0, &tmp_value, p->id);
	DEBUG_LOG("YXW VALUE[%u %u]", p->id, p->game_king);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, gid, j);
	PKG_UINT32(msg, p->id, j);
	PKG_UINT32(msg, pkee, j);
	PKG_UINT32(msg, usr_score, j);
	PKG_UINT32(msg, usr_pkee_score, j);
	PKG_UINT32(msg, HLB, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/*
 *@ get pk history list
 */
int get_pk_history_list_cmd(sprite_t* p, const uint8_t* body, int len)
{
	if (IS_GUEST_ID (p->id) || IS_NPC_ID (p->id)) {
		ERROR_RETURN(("not a register user\t[%u]", p->id), -1);
	}

	DEBUG_LOG ("GET PK HISTORY LIST\t[%u]", p->id);
	return db_get_pk_list_by_uid(p);
}

/*
 *@ get the friend grade list
 */
int get_pk_history_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct pk_info
{
	uint32_t 	uid;
	char 		nick[16];
	uint32_t 	gid;
	uint32_t		pk_time;
	uint32_t 	usr_score;
	uint32_t 	usr_pkee_score;
	uint32_t	pk_flag;
}__attribute__((packed))pk_info_t;

	CHECK_BODY_LEN_GE(len, 4);
	int count = *(uint32_t*)buf;
	int l = count * sizeof(pk_info_t) + 4;
	CHECK_BODY_LEN(len, l);

	int i = sizeof(protocol_t);
	int loop;
	pk_info_t* pi = (pk_info_t*)(buf + 4);
	PKG_UINT32(msg, count, i);
	DEBUG_LOG ("PK HISTORY CNT\t[%u %u]", p->id, count);
	for(loop = 0; loop < count; loop++, pi++){
		PKG_UINT32(msg, pi->uid, i);
		PKG_STR(msg, pi->nick, i, 16);
		PKG_UINT32(msg, pi->gid, i);
		PKG_UINT32(msg, pi->pk_time, i);
		PKG_UINT32(msg, pi->usr_score, i);
		PKG_UINT32(msg, pi->usr_pkee_score, i);
		PKG_UINT32(msg, pi->pk_flag, i);
		DEBUG_LOG ("PK HISTORY LIST\t[%u %u %u %u %u %u]", p->id, pi->uid, pi->gid, pi->usr_score, pi->usr_pkee_score, pi->pk_flag);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/*
 *@ get user PK score
 */
int get_pk_score_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	int j = 0;
	userid_t uid;
	UNPKG_UINT32(body, uid, j);
	if (IS_GUEST_ID (uid) || IS_NPC_ID (uid)) {
		ERROR_RETURN(("not a register user\t[%u %u]", p->id, uid), -1);
	}

	DEBUG_LOG ("GET PK SCORE\t[%u %u]", p->id, uid);
	return db_get_pk_score(p, &uid);
}

/*
 *@ get the friend grade list
 */
int get_pk_score_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);
	int win_num = *(uint32_t*)buf;
	int lose_num = *(uint32_t*)(buf + 4);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, win_num, i);
	PKG_UINT32(msg, lose_num, i);
	PKG_UINT32(msg, p->game_king, i);

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int get_pk_trade_cmd(sprite_t* p, const uint8_t* body, int len)
{
    CHECK_BODY_LEN(len, 4);
    int i = 0;
    uint32_t type;
    UNPKG_UINT32(body, type, i);
    if(type > 3) {
        ERROR_RETURN(("type error %d", type), -1);
    }
    static int pk_score_level[] = {4400, 6400, 8400, 10400};
    static int pk_exclu_id[] = {1000, 1001, 1002, 1003};
    if(p->game_king < pk_score_level[type]) {
        return send_to_self_error(p, p->waitcmd, -ERR_pk_level_unreached, 1);
    }
    return do_exchange_item(p, pk_exclu_id[type], 0);
}


