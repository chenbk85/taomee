#ifndef WEEK_ACTIVITY_H_
#define WEEK_ACTIVITY_H_

#include "benchapi.h"

extern uint32_t explor_time_begin;
typedef struct mine_explor_timer {
	list_head_t timer_list;
}__attribute__(( packed )) mine_explor_timer_t;

typedef struct guess_item_price
{
    uint32_t 	date;
    uint32_t 	itmid;
    uint32_t 	price;
}__attribute__((packed))guess_item_price_t;

typedef struct user_info
{
    uint32_t 	userid;
    char		nick[USER_NICK_LEN];
}__attribute__((packed))user_info_t;

#define GUESS_RIGHT_MAX  50

typedef struct guess_right_users
{
    uint32_t 	    count;
    user_info_t 	users[GUESS_RIGHT_MAX];
}__attribute__((packed))guess_right_users_t;

typedef struct guess_item_timer {
	list_head_t timer_list;
}__attribute__(( packed )) guess_item_timer_t;

typedef struct user_item_info
{
    uint32_t 	itemid;
    char		nick[USER_NICK_LEN];
    uint32_t 	price;
}__attribute__((packed))user_item_info_t;

typedef struct user_item_db_info
{
    uint32_t 	userid;
    char		nick[USER_NICK_LEN];
    uint32_t 	itemid;
    uint32_t 	price;
    uint32_t 	date;
}__attribute__((packed))user_item_db_info_t;


typedef struct speech_user_info
{
    uint32_t    time;
    uint32_t 	userid;
    uint32_t 	last_time;
    uint32_t 	last_userid;
}__attribute__((packed))speech_user_info_t;

typedef struct speech_info_timer {
	list_head_t timer_list;
}__attribute__(( packed )) speech_info_timer_t;

typedef struct items_pro{
	uint32_t itemid;
	uint32_t count;
}__attribute__(( packed )) items_pro_t;
typedef struct  greate_produce{
	uint32_t type;//order type;
	items_pro_t order[3];// order items
	items_pro_t award[3];// finish order get items
}__attribute__(( packed )) greate_produce_t;

struct  mvp_team_info {
	uint32_t logo;
	uint32_t teamid;
	uint8_t name[16];
}__attribute__((packed));

typedef struct cheer_team_info
{
    uint32_t    teacher_act;
    uint32_t 	team_score[2];
    uint32_t 	team_rate[2];
    uint32_t 	user_act[6];
    uint32_t 	userid[6];
}__attribute__((packed))cheer_team_info_t;

typedef struct cos_item_score
{
    uint32_t    itemid;
    uint32_t 	score;
}__attribute__((packed))cos_item_score_t;

typedef struct cos_item_score_info
{
    cos_item_score_t    item_score[4];
}__attribute__((packed))cos_item_score_info_t;


int cheer_team_init();
int cheer_team_clear_user_info(sprite_t *p);
int check_user_in_pos(uint32_t userid);
int cheer_team_user_act_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int notify_cheer_team_turn_end(sprite_t* p, uint32_t win_team, uint32_t teacher_act, 
    uint32_t itemid, uint32_t cnt);
int notify_cheer_team_user_leave(sprite_t* p, uint32_t userid, uint32_t pos, uint32_t act);
int query_cheer_team_info_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_candy_friend_cnt_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_candy_friend_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_candy_share_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_candy_share_bonus_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_candy_from_friend_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_candy_from_friend_callback(sprite_t* p, uint32_t id, char* buf, int len);
int add_as_type_int_data_cmd(sprite_t* p, uint8_t* body, int bodylen);
int add_as_type_int_data_callback(sprite_t* p, uint32_t id, char* buf, int len);
int query_as_type_int_data_cmd(sprite_t* p, uint8_t* body, int bodylen);
int query_as_type_int_data_callback(sprite_t* p, uint32_t id, char* buf, int len);

int add_mine_map_piece_cmd(sprite_t* p, uint8_t* body, int bodylen);
int add_mine_map_piece_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_mine_map_pieces_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_mine_map_pieces_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_mine_key_chance_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_all_chance_count_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_mine_key_chance_callback(sprite_t* p, uint32_t id, char* buf, int len);
int query_mine_key_chance_cmd(sprite_t* p, uint8_t* body, int bodylen);
int query_mine_key_chance_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_mine_map_key_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_mine_map_key_callback(sprite_t* p, uint32_t id, char* buf, int len);
int query_mine_map_explor_time_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_gold_compass_users_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_gold_compass_users_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int guess_date_item_price_cmd(sprite_t* p, uint8_t* body, int bodylen);
int guess_item_price_day_limit_callback(sprite_t *p);
int get_guess_right_user_info_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_guess_right_user_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_sprint_festival_login_info_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_sprint_festival_login_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_sprint_festival_login_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_sprint_festival_login_bonus_callback(sprite_t* p, uint32_t id, char* buf, int len);


int get_vip_continue_week_login_info_cmd(sprite_t *p, uint8_t* body, int bodylen);
int get_vip_continue_week_login_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_vip_week_login_award_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_vip_week_login_award_callback(sprite_t* p, uint32_t id, char* buf, int len);

int add_user_item_score_cmd(sprite_t * p,const uint8_t * body,int len);
int add_user_item_score_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_user_items_score_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_user_items_score_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_user_cutepig_gold_key_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_user_cutepig_gold_key_callback(sprite_t* p, uint32_t id, char* buf, int len);
int speech_user_enter_or_leave_cmd(sprite_t* p, uint8_t* body, int bodylen);
int speech_user_get_user_info_cmd(sprite_t* p, uint8_t* body, int bodylen);
int speech_user_clean( );
int speech_user_leave(sprite_t* p);

//mole great produce
int get_majesty_or_urgent_order_cmd(sprite_t*p, uint8_t* body, int len);
int get_majesty_or_urgen_order_callback(sprite_t* p, uint32_t id, char* buf, int len);

int put_majesty_or_urgent_order_cmd(sprite_t* p, uint8_t* body, int len);

int finish_majesty_or_urgent_order_cmd(sprite_t* p, uint8_t* body, int len);
int finish_majesty_or_urgent_order_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_greate_produce_fisrt_thousand_callback(sprite_t* p, uint32_t id, char* buf, int len);

//KFC party
int put_user_KFC_decoration_cmd(sprite_t *p, uint8_t *body, int len);
int get_user_KFC_decoration_cmd(sprite_t *p, uint8_t *body, int len);
int get_user_KFC_decoration_callback(sprite_t *p, uint32_t id, char* buf, int len);

int vote_KFC_friend_decoration_cmd(sprite_t *p, uint8_t *body, int len);
int vote_KFC_friend_decorate_callback(sprite_t *p, uint32_t id, char* buf, int len);
int get_poll_friend_decoration_cmd(sprite_t *p, uint8_t *body, int len);
int get_poll_friend_decoration_callback(sprite_t *p, uint32_t id, char* buf, int len);

int user_catch_butterfly_angel_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_catch_butterfly_angel_callback(sprite_t* p, uint32_t id, char* buf, int len);

int create_lahm_sport_mvp_team_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int create_lahm_sport_mvp_team_callback(sprite_t *p, uint32_t id, const char *buf, int len);
int enter_lahm_sport_mvp_team_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int enter_lahm_sport_mvp_team_callback(sprite_t *p, uint32_t id, const char *buf, int len);
int leave_lahm_sport_mvp_team_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int leave_lahm_sport_mvp_team_callback(sprite_t *p, uint32_t id, const char *buf, int len);
int user_get_sport_mvp_team_callback(sprite_t *p, uint32_t id, const char *buf, int len);
int get_sysarg_mvp_teamid_callback(sprite_t *p, uint32_t id, const char *buf, int len);
int dissolve_lahm_sport_mvp_team_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int dissolve_lahm_sport_mvp_team_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_lahm_sport_mvp_team_info_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int get_lahm_sport_mvp_team_info_callback(sprite_t *p, uint32_t id, char* buf, int len);

int user_change_lahm_sport_team_cmd(sprite_t *p, uint8_t *buf, int len);
int user_change_lahm_sport_team_callback(sprite_t *p, uint32_t id, char *buf, int len);

int user_get_mvp_team_ranking_cmd(sprite_t *p, uint8_t *buf, int len);
int user_get_mvp_team_ranking_callback(sprite_t *p, uint32_t id, char* buf, int len);
int get_two_team_medal_cnt_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_two_team_medal_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_lahm_sport_sysarg_mvp_team_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int get_lahm_sport_sysarg_mvp_team_callback(sprite_t *p, uint32_t id, char* buf, int len);

int choice_cute_piglet_rush_activity_cmd(sprite_t *p, const uint8_t *buf, int len);
int check_cute_piglet_champion_activity_cmd(sprite_t *p, const uint8_t *buf, int len);
int check_cute_piglet_champion_activity_callback(sprite_t *p, uint32_t id, char* buf, int len);

int get_piglet_champion_activity_award_cmd(sprite_t *p, const uint8_t* buf, int len);
int get_piglet_champion_activity_award_callback(sprite_t *p, uint32_t id, char* buf, int len);

int user_water_jack_and_modou_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_water_jack_and_modou_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_water_time_jack_and_modou_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_water_time_jack_and_modou_callback(sprite_t* p, uint32_t id, char* buf, int len);
int user_shake_dice_for_vip_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_shake_dice_for_vip_callback(sprite_t* p, uint32_t id, char* buf, int len);
int user_get_signal_flare_cmd(sprite_t* p, const uint8_t* body, int len);
int user_get_shake_dice_left_time_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_get_shake_dice_left_time_callback(sprite_t* p, uint32_t id, char* buf, int len);


///kula birthday celebrite activity

int user_guess_stone_scissors_cloth_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_guess_stone_scissors_cloth_callback(sprite_t* p, uint32_t id, char* buf, int len);
int user_join_cosplay_race_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_join_cosplay_race_callback(sprite_t* p, uint32_t id, char* buf, int len);
int sysarg_get_cos_rank_info_cmd(sprite_t *p, uint8_t *buf, int len);
int sysarg_get_cos_rank_info_callback(sprite_t *p, uint32_t id, char* buf, int len);
int get_user_cos_rank_flag_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_user_cos_rank_flag_callback(sprite_t* p, uint32_t id, char* buf, int len);


#endif 

