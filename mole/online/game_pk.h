
#ifndef ONLINE_GAME_PK_H
#define ONLINE_GAME_PK_H

#include "benchapi.h"
#include "dbproxy.h"

int get_frd_game_grade_list_cmd(sprite_t* p, const uint8_t* body, int len);
int get_frd_game_grade_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_game_grade_list_cmd(sprite_t* p, const uint8_t* body, int len);
int get_game_grade_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_pk_info_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int set_pk_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_pk_history_list_cmd(sprite_t* p, const uint8_t* body, int len);
int get_pk_history_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_pk_score_cmd(sprite_t* p, const uint8_t* body, int len);
int get_pk_score_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_pk_trade_cmd(sprite_t* p, const uint8_t* body, int len);

static inline int db_update_game_grade(sprite_t* p, int gid, uint32_t grade, userid_t uid)
{
	uint32_t buff[3];
	buff[0] = uid;
	buff[1] = gid;
	buff[2] = grade;

	return send_request_to_db(SVR_PROTO_UPDATE_GAME_BEST_GRADE, p, 12, buff, 0);
}

static inline int db_update_pk_flag(sprite_t* p, int gid, uint32_t grade, userid_t pker, userid_t pkee)
{
	uint32_t buff[4];
	buff[0] = gid;
	buff[1] = pker;
	buff[2] = pkee;
	buff[3] = grade;

	return send_request_to_db(SVR_PROTO_UPDATE_PK_FLAG, p, 16, buff, 0);
}

static inline int db_add_history_pk_info(sprite_t* p, int gid, uint32_t pk_flag, int ret, userid_t pkee, uint32_t usr_score, uint32_t usr_pkee_score)
{
	uint32_t buff[10];
	buff[0] = p->id;
	memcpy(&buff[1], p->nick, 16);
	buff[5] = gid;
	buff[6] = get_now_tv()->tv_sec;
	buff[7] = usr_score;
	buff[8] = usr_pkee_score;
	buff[9] = pk_flag;

	return send_request_to_db(SVR_PROTO_ADD_PK_INFO, (ret ? p : 0), 40, buff, pkee);
}

#define db_get_grade_list_by_gid(p_, len, buf_) \
		send_request_to_db(SVR_PROTO_GET_GRADE_LIST_BY_GID, p_, len, buf_, 0)

#define db_get_grade_list_by_uid(p_, buf_) \
		send_request_to_db(SVR_PROTO_GET_GRADE_LIST_BY_UID, p_, 4, buf_, 0)

#define db_get_pk_list_by_uid(p_) \
		send_request_to_db(SVR_PROTO_GET_PK_LIST, p_, 0, NULL, (p_)->id)

#define db_get_pk_score(p_, buf) \
		send_request_to_db(SVR_PROTO_GET_PK_SCORE, p_, 4, buf, 0)

#define db_set_game_king(p_, buf, uid) \
		send_request_to_db(SVR_PROTO_SET_GAME_KING, p_, 4, buf, uid)


#endif
