#ifndef ONLINE_RACE_H
#define ONLINE_RACE_H

typedef struct team_race_list{
	uint32_t	team;
	uint32_t	race1;
	uint32_t	race2;
	uint32_t	race3;
	uint32_t	race4;
}__attribute__((packed)) team_race_list_t;

typedef struct team_score_list_t{
	uint32_t	team;
	uint32_t	score;
	uint32_t dayscore;
}__attribute__((packed)) team_score_list_t;

static inline int db_add_team_score(sprite_t* p, uint32_t team, uint32_t score)
{
	uint32_t buff[2];
	buff[0] = team;
	buff[1] = score;
	return send_request_to_db(SVR_PROTO_RACE_UPDATE_SCORE, p, 8,  buff, 0 );
}

int give_flower_to_motherland_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_motherland_flower_cmd(sprite_t* p, const uint8_t* body, int bodylen);

int stop_renqi(void* owner, void* data);
int set_start_renqi(void* owner, void* data);

int race_sign_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int race_sign_callback(sprite_t* p, uint32_t id, char* buf, int len);
int confirm_add_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int confirm_add_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_self_score_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_self_score_callback(sprite_t* p, uint32_t id, char* buf, int len);
int update_team_race_value_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int  update_team_race_value_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_team_race_value_list_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_team_race_value_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_team_score_list_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_team_score_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
int someone_come_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_user_race_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_user_taday_score_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_user_taday_score_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_user_race_cnt_cmd(sprite_t* p, const uint8_t* body, int bodylen);

int desc_race_callback(sprite_t* p, uint32_t id, char* buf, int len);

int db_get_race_team(sprite_t* p);
int get_team_callback(sprite_t* p, uint32_t id, char* buf, int len);

int confirm_add_callback_final(sprite_t* p);
int get_renqi_flag_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int do_send_motherland_flower(sprite_t* p, uint32_t count);
int get_special_award_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int exchg_xuanfeng_jijile_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int do_exchg_xuanfeng_jijile(sprite_t* p, uint32_t count);
int get_xuanfeng_car_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_user_num_in_ship_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_next_flight_time_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int race_award_prize_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int race_award_commemorate(sprite_t* p, const uint8_t* body, int bodylen);

int do_check_give_or_no(sprite_t *p, uint32_t count);
int do_check_give_or_no_ex(sprite_t *p);
int race_winner_or_no_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int do_get_xuanfeng_car(sprite_t* p, uint32_t count, uint8_t* buf);

#endif

