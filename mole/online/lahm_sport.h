
#ifndef ONLINE_LAHM_SPORT_H
#define ONLINE_LAHM_SPORT_H

#include "benchapi.h"
#include "dbproxy.h"

//For lahm sport
#define PROTO_SIGN_UP 					1131
#define PROTO_BECOME_SHOVEL				1132
#define PROTO_BECOME_SHOVEL_EXPIRE		1133
#define PROTO_FINISH_SHOVEL				1134
#define PROTO_LAHM_SBM_RACE_SCORE		1135
#define PROTO_LAHM_GET_SCORE_LIST		1136
#define PROTO_LAHM_LIST_TEAM_SCORE		1137
#define PROTO_LAHM_GET_BEST_SCORE		1138
#define PROTO_LAHM_GET_TEAM_MADELS		1139
#define PROTO_LAHM_GET_MADLE_AND_CUP    1140

#define PROTO_SET_LAHM_TEAM				1151
#define PROTO_GET_LAHM_TEAM				1152
#define PROTO_GET_LAHM_TEAM_CLOTH		1153

#define SVR_PROTO_GET_LAHM_SIGN_UP_TAG	0xD04C
#define SVR_PROTO_GET_LAHM_SIGN_UP_LIST	0XD04D
#define SVR_PROTO_LAHM_SIGN_UP			0XD14E
#define SVR_PROTO_LAHM_SBM_RACE_SCORE	0XD148
#define SVR_PROTO_LAHM_GET_SCORE_LIST	0XD04F
#define SVR_PROTO_LAHM_LIST_TEAM_SCORE	0XD04B
#define SVR_PROTO_LAHM_GET_BEST_SCORE	0XD049
#define SVR_PROTO_LAHM_GET_TEAM_MADELS	0XD04A


#define MAX_GAME_SCORE 100000
#define GAME_START_ID 1
#define GAME_END_ID 5

enum {
	MAP_ID_4 = 4,
	MAP_ID_8 = 8,
	MAP_ID_41 = 41
};



#define get_lahm_sign_up_tag(p_) \
		send_request_to_db(SVR_PROTO_GET_LAHM_SIGN_UP_TAG, p_, 0, NULL, (p_)->id)

#define CHECK_TEAM_VALID(team)\
	do{\
		if ((team) == 0) {\
			return send_to_self_error(p, p->waitcmd, -ERR_have_not_choose_team, 1);\
		}\
	} while(0)

int get_lahm_sign_up_tag_callback(sprite_t* p, uint32_t id, char* buf, int len);

int sign_up_sport_callback(sprite_t* p, uint32_t id, char* buf, int len);

int sign_up_sport_cmd(sprite_t* p, const uint8_t* body, int bodylen);

int become_shovel_cmd(sprite_t * p,const uint8_t * body,int len);

int finish_shovel_cmd(sprite_t *p, const uint8_t *body, int len);

int submit_race_score_cmd(sprite_t * p,const uint8_t * body,int bodylen);

int submit_race_score_callback(sprite_t * p,uint32_t id, char * buf,int len);

int get_pet_score_list_cmd(sprite_t * p,const uint8_t * body, int bodylen);

int get_pet_score_list_callback(sprite_t * p,uint32_t id, char * buf,int len);

int list_team_score_cmd(sprite_t * p,const uint8_t * body, int bodylen);

int list_team_score_callback(sprite_t * p,uint32_t id, char * buf,int len);

int get_lahm_best_score_cmd(sprite_t * p,const uint8_t * body,int bodylen);

int get_lahm_best_score_callback(sprite_t * p,uint32_t id,char * buf,int len);

int get_lahm_team_madels_cmd(sprite_t * p,const uint8_t * body,int bodylen);

int get_lahm_team_madels_callback(sprite_t * p,uint32_t id,char * buf,int len);

int get_lahm_madel_and_cup_cmd(sprite_t * p,const uint8_t * body,int bodylen);

int set_lahm_team_cmd(sprite_t * p,const uint8_t * body,int bodylen);

int set_lahm_team_callback(sprite_t * p,uint32_t id,char * buf,int len);

int get_lahm_team_cmd(sprite_t * p,const uint8_t * body,int bodylen);

int get_lahm_team_callback(sprite_t * p,uint32_t id,char * buf,int len);

int get_lahm_team_cloth_cmd(sprite_t * p,const uint8_t * body,int bodylen);

int get_lahm_team_cloth_callback(sprite_t * p,uint32_t team);

int give_npc_shop_dish_cmd(sprite_t * p,const uint8_t * body,int bodylen);

int give_npc_shop_dish_check(sprite_t* p, uint32_t id, char* buf, int len);

int give_npc_shop_dish_callback(sprite_t * p,uint32_t id,char * buf,int len);

int super_lamu_party_get_cmd(sprite_t * p,const uint8_t * body,int bodylen);

int super_lamu_party_get_callback(sprite_t * p);

int ding_pigu_add_score_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int  ding_pigu_get_team_callbak(sprite_t *p, char *buf, int len);

int ding_pigu_add_score_callback(sprite_t *p, uint32_t id, char *buf, int len);

int sport_team_get_max_score(uint32_t uid, void* buf, int len);

int get_team_score_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_team_score_callback(sprite_t *p, uint32_t id, char *buf, int len);

int get_team_medal_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_team_medal_callback(sprite_t *p, uint32_t id, char *buf, int len);

int give_sth_accord_max_score_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int give_sth_get_team_callback(sprite_t *p, char *buf, int len);

int add_sh_for_wim_tem_member_callback(sprite_t *p, uint32_t id, char *buf, int len);

int add_medal_by_play_game_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_team_id_before_add_medal(sprite_t *p, const char *buf, int lne);

int add_medal_by_play_game_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int cal_medal_num(sprite_t *p, uint32_t game_id, uint32_t game_xiaomee,
	uint32_t game_score, uint32_t game_level, uint32_t *medal_num);

int get_top_10_pet_score_cmd(sprite_t *p, const uint8_t *buf, int len);

int get_top_10_pet_score_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int sport_stage_start_cmd(sprite_t *p, uint32_t id, const uint8_t *buf, int len);

int expire_sport_stage_end(void *owner, void *data);

int get_diff_time_start_to_now_cmd(sprite_t *p, const uint8_t *buf, int len);

int get_myself_medal_num_cmd(sprite_t *p, const uint8_t *buf, int len);

int get_myself_medal_num_callback(sprite_t *p, uint32_t id, const char *buf, int len);


int get_lahm_team_rank_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int get_lahm_team_prize_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int get_lahm_team_prize_1_callback(sprite_t * p,uint32_t id,char * buf,int len);
int get_lahm_team_prize_2_callback(sprite_t * p,uint32_t id,char * buf,int len);



#endif

