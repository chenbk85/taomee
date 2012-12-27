#ifndef MOLE_SPORTS
#define MOLE_SPORTS
//100 - 104 
#define TEAM_MEMBER_NUM                         100 
//105 - 109
#define TEAM_PRE_SCORE                          105
//110 - 114
#define NPC_POS_INFO                            110
#define db_get_sports_team(p_)	\
		send_request_to_db(SVR_PROTO_GET_SPORT_TEAM, (p_), 0, NULL, (p_)->id)

int sports_sign_up_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_sign_up_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_sports_team_callback(sprite_t* p, uint32_t id, char* buf, int len);
int sports_fin_pre_task_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_encourage_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_rand_sign_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_rand_sign_callback(sprite_t* p, uint32_t id, char* buf, int len);
int sports_add_medal_callback(sprite_t* p, uint32_t id, char* buf, int len);
int sports_get_task_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_start_task_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_fin_task_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_get_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
int sports_set_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
int sports_team_history_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_team_history_callback(sprite_t* p, uint32_t id, char* buf, int len);

int sports_get_npc_info(sprite_t *p);
int sports_get_npc_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int add_medal(sprite_t *p, int count, int type, int limited);
int do_sports_encourage(sprite_t *p);

int sports_water_maomao_tree(sprite_t *p, uint32_t uid, int ret);
int sports_watering_info_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_get_team_cloth_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int sports_reward_break_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_reward_reach_max_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_get_watering_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len);

int sports_chance_cylce_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int add_team_cloth(sprite_t* p, uint8_t* buf, int count);
int do_sports_chance_cycle(sprite_t *p);
int sports_self_info_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_chance_cycle_reward(sprite_t* p, uint8_t* buf, int count);

int sports_get_fire_stat_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sports_set_fire_start_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int do_sports_max_reward(sprite_t *p);
int get_reward_cmd(sprite_t *p, const uint8_t *body, int bodylen);

#endif
