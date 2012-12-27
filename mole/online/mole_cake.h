#ifndef MOLE_CAKE_INCL
#define MOLE_CAKE_INCL

#define MAKE_BIRTHDAY_CAKE_MAP 239

typedef enum  materail_flag{
	FLOUR   = 0, 
	CREAM   = 1,
	ALMOND  = 2,
	CANDLE  = 3,
	WATER   = 4,
}materail_flag_t;

//make cake state;
typedef enum cake_state{
	FIRST_STEP      =  0,
	SECOND_STEP     =  1,
	THIRD_STEP      =  2,
	FINISH_STEP     =  3,
}cake_state_t;

typedef struct cake_info{
	cake_state_t cur_state;
	uint32_t cal_cream_time;
	uint32_t flour;
	uint32_t water;
	uint32_t almond;
	uint32_t cream;
	uint32_t candle;
	
}cake_info_t;

typedef struct finish_cake_timer {
    list_head_t timer_list;
	uint32_t  stat_first;
}__attribute__(( packed )) finish_cake_timer_t;


//make birthday cake
int mole_make_birthday_cake_cmd(sprite_t* p, const uint8_t* body, int len);

//entry map,get cake information
int get_bithday_cake_curent_info_cmd(sprite_t* p, const uint8_t* body, int len);
//get cake ward
int get_make_birthday_cake_award_cmd(sprite_t* p, const uint8_t* body, int len);
int get_make_birthday_cake_award_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_make_birthday_cake_for_client(sprite_t *p, uint32_t id, char* buf, int len);

//notify client current cake information
int notify_all_client_players(sprite_t* p, int boolean);
int check_whether_legal_operate( int material);
int add_timer();
int handle_celebrate_finsh_cake_time_out();



//crystal celebrating dancing party
typedef enum{
	JOINING_DANCE         =  0,
	NOW_DANCING           =  1,
	DANCING_OVER          =  2,
}dancing_state_t;
typedef enum{
	JAZZ            =  0,
	SAMBA           =  1,
	RUMBA           =  2,
	CHACHA          =  3,
	WALTZ           =  4,
} dance_type_t;

#define GROUP_VS_NUMS 5
#define CRYSTAL_DANCING_MAP           244
typedef struct dacing_ball_timer {
	list_head_t timer_list;
	uint32_t  stat_first;
	uint32_t today;
}__attribute__(( packed )) dancing_ball_timer_t;

typedef struct group_dance{
	uint32_t position_cinderella;
	uint32_t position_prince;
	uint32_t rank;

}__attribute__(( packed )) group_dance_t;

typedef struct dancing_ball{
	dancing_state_t  dancing_state;
	uint32_t round;
	group_dance_t pairs[GROUP_VS_NUMS]; 
}__attribute__(( packed )) dancing_ball_t;

int random_arrage(uint32_t *rank, uint32_t count, int *top);
int check_player_on_seat(uint32_t seat, uint32_t type, uint32_t uid);
int notify_client_dacing_ball(sprite_t *p);
int kick_off_dacing_user(sprite_t *p);
int add_dacing_timer(uint32_t span);
int handle_pair_vs_pair_time_out();

int mole_join_in_dacing_cmd(sprite_t* p, const uint8_t* body, int len);
int get_whole_dancing_scene_cmd(sprite_t* p, const uint8_t* body, int len);
int get_dancing_random_item_cmd(sprite_t* p, const uint8_t* body, int len);
int give_award_crystal_dancing_ball(sprite_t *p, uint32_t id, char* buf, int len);

int check_dancing_three_times_cmd(sprite_t* p, const uint8_t* body, int len);
int get_crystal_dancing_times(sprite_t *p, uint32_t id, char* buf, int len);
int get_part_in_three_times_dancing(sprite_t *p, uint32_t id, char* buf, int len);

int check_four_celebrate_book_light_cmd(sprite_t* p, const uint8_t* body, int len);
int generate_book_light_flag(sprite_t *p, uint32_t id, char* buf, int len);

int mole_set_miss_note_cmd(sprite_t* p, const uint8_t* body, int len);
int get_mole_miss_note_day_times(sprite_t *p, uint32_t id, char* buf, int len);
int mole_set_miss_note_callback(sprite_t *p, uint32_t id, char* buf, int len);
int mole_get_enery_rock_stones_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_get_enery_rock_stones_callback(sprite_t *p, uint32_t id, char* buf, int len);
int mole_get_miss_note_cmd(sprite_t* p, const uint8_t* body, int len);
//int mole_get_miss_note_callback(sprite_t *p, uint32_t id, char* buf, int len);

int mole_get_make_ship_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_get_make_ship_callback(sprite_t *p, uint32_t id, char* buf, int len);

int mole_check_expedition_lock_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_check_expedition_lock_callback(sprite_t *p, uint32_t id, char* buf, int len);
int mole_get_chapter_state_callback(sprite_t *p, uint32_t id, char* buf, int len);

int mole_funfly_seahorse_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_funfly_seahorse_callback(sprite_t *p, uint32_t id, char* buf, int len);

/// merman expedition serials
int mole_merman_kingdom_expedition_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_merman_kingdom_expedition_callback(sprite_t *p, uint32_t id, char* buf, int len);

#define PRAY_ACTIVITY_MAP 12
int mole_get_pray_info_cmd(sprite_t* p, const uint8_t* body, int len);
int notify_client_pray_status(sprite_t *p);
int mole_contend_for_seat_cmd(sprite_t* p, const uint8_t* body, int len);
int kick_off_pray_user(sprite_t *p);

int mole_get_pray_activity_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_get_pray_activity_callback(sprite_t *p, uint32_t id, char* buf, int len);

int mole_set_pray_activity_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_set_pray_activity_callback(sprite_t *p, uint32_t id, char* buf, int len);

int mole_get_invade_monster_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_get_invade_monster_callback(sprite_t *p, uint32_t id, char* buf, int len);

int mole_attack_invade_monster_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_attack_invade_monster_callback(sprite_t *p, uint32_t id, char* buf, int len);


int get_srcoll_map_state_cmd(sprite_t* p, const uint8_t* body, int len);
int get_srcoll_map_state_callback(sprite_t *p, uint32_t id, char* buf, int len);

int mole_get_scroll_fragment_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_get_scroll_fragment_callback(sprite_t *p, uint32_t id, char* buf, int len);
int mole_do_work_for_others_cmd(sprite_t* p, const uint8_t* body, int len);

int mole_mee_mermaid_again_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_finish_merman_kingdom_expedition_cmd(sprite_t* p, const uint8_t* body, int len);
int mole_finish_merman_kingdom_expedition_callback(sprite_t *p, uint32_t id, char* buf, int len);


#endif
	