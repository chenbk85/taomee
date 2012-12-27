#ifndef SNOWBALL_WAR_H
#define SNOWBALL_WAR_H

#include "item.h"

#define MAX_TEAM_PLAYERS	5

enum {
	ATTACK_ONE = 1,
	ATTACK_THREE,
	ATTACK_FIVE,
	ATTACK_BIG_BALL,
	ATTACK_USER,
	ATTACK_GIFT_BOX,
};

typedef struct snowball_team{
    uint32_t    life_value;
    uint32_t    count;
	uint32_t    players[MAX_TEAM_PLAYERS];
	uint32_t    pos[3];
}__attribute__(( packed )) snowball_team_t;

typedef struct snowball_timer {
	list_head_t timer_list;
}__attribute__(( packed )) snowball_timer_t;


int snowball_enter_quit_cmd(sprite_t* p, uint8_t* body, int bodylen);
int set_player_enter(int teamid, uint32_t userid);
int set_player_quit(int teamid, uint32_t userid);
int check_player_in(int teamid, uint32_t userid);
int check_player_pos_in(int teamid, uint32_t userid);
int notify_snowball_begin_end(uint32_t flag);
int snowball_attack_enemy_cmd(sprite_t* p, uint8_t* body, int bodylen);
int snowball_query_teams_info_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_time_left ();
int snowball_end_game(uint32_t flag);
int snowball_end_game_timeout();
int snowball_game_player_quit(sprite_t* p);
int set_player_leave_pos(int teamid, uint32_t userid);
int snowball_ocupy_pos_cmd(sprite_t* p, uint8_t* body, int bodylen);
int snowball_game_bonus_day_limit_callback(sprite_t *p) ;
int snowball_game_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen);
int send_snowball_game_bonus_to_db(sprite_t* p, uint32_t score);
int send_snowball_game_bonus_to_client(sprite_t* p);
int snowball_get_left_time_cmd(sprite_t* p, uint8_t* body, int bodylen);
int init_snowball_war_timer();
int get_teamid_player(uint32_t userid);
int print_team_players_info();

int snowball_lahm_attack_cmd(sprite_t* p, uint8_t* body, int bodylen);
int notify_attack_gift_box_over(sprite_t* p);

int snowball_game_new_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen);
int snowball_game_new_bonus_day_limit_callback(sprite_t *p);
int snowball_set_team_win(uint32_t teamid);
int snowball_set_all_gift_flag();

#endif

