
#ifndef ONLINE_MOLE_WORK_H
#define ONLINE_MOLE_WORK_H




#define db_get_work_num(p_)	\
		send_request_to_db(SVR_PROTO_GET_WORK_NUM, (p_), 0, NULL, (p_)->id)

static inline int db_add_work_num(sprite_t* p, uint32_t type, userid_t uid)
{
	return send_request_to_db(SVR_PROTO_ADD_WORK_NUM, p, 4, &type, uid);
}

int set_work_num_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_work_num_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_work_num_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_work_num_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_engineer_level_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_work_salary_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_worker_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_mole_self_games_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_mole_self_games_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_mole_self_game_rank_callback(sprite_t* p, uint32_t id, char* buf, int len);

int mole_enter_game_hall_cmd(sprite_t *p, uint8_t* body, int bodylen);
int mole_enter_game_hall_callback(sprite_t* p, uint32_t id, char* buf, int len);

#endif

