//hide_and_seek.h
#ifndef HIDE_AND_SEEK_H
#define HIDE_AND_SEEK_H

enum FORCE_DEF
{
	FD_INVALID	= -1,
	FD_ANGEL	= 0,
	FD_CLOWN	= 1,
	FD_MAX
};

typedef struct has_wait_node_s
{
	int32_t 		prev_idx;
	int32_t 		next_idx;
	uint32_t		user_id;
} __attribute__((packed)) has_wait_node_t;

typedef struct has_node_info_s
{
	uint8_t			is_used;
	has_wait_node_t	data;
} __attribute__((packed)) has_node_info_t;

typedef struct has_user_pair_s
{
	uint32_t	angel_id;
	uint32_t	clown_id;
	uint32_t	begin_time;
	uint8_t		is_used;
} __attribute__((packed)) has_user_pair_t;

typedef struct has_my_has_info_s
{
	uint32_t	i_am_angel;
	uint32_t	winner_id;
} __attribute__((packed)) has_my_has_info_t;

enum HAS_STATE
{
	HS_IDLE,			//空闲
	HS_WAIT,			//排队中
	HS_INGAME,			//游戏中
};

typedef struct has_game_over_back_s
{
	uint32_t	i_am_angel;
	uint32_t	is_winner;
	uint32_t 	item_count;
	uint32_t	item_id;
} __attribute__((packed)) has_game_over_back_t;

typedef struct has_timer {
	list_head_t timer_list;
}__attribute__(( packed )) has_timer_t;




int has_join_game_cmd(sprite_t* p, const uint8_t* body, int len);

int has_quit_game_cmd(sprite_t* p, const uint8_t* body, int len);

int has_game_over_cmd(sprite_t* p, const uint8_t* body, int len);

int has_game_over_callback(sprite_t* p, uint32_t id, char* buf, int len);

int has_get_queue_info_cmd(sprite_t * p,const uint8_t * body,int len);

int has_remove_player_from_queue(uint32_t id);

void has_initilize_data();

int has_clear_my_info(sprite_t* p);

int has_request_game_over( sprite_t* p, int is_angel, int is_win );

int has_time_out( void* onwer, void* data );



#endif
