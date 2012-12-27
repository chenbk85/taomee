#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "hide_and_seek.h"

#include "exclu_things.h"

#define HAS_MAX_MAP_NUM		1
#define HAS_MAP_0			7		//东部游乐场
#define HAS_MAP_1			10		//开心农场

#define HAS_LIST_RESERVED_SIZE	500
#define HAS_MAX_USER_PAIR_SIZE	300
#define HAS_TIMER				60*5
#define HAS_GAME_TIMER			90

static has_node_info_t node_info[HAS_LIST_RESERVED_SIZE];

//for list
static int32_t			list_head[HAS_MAX_MAP_NUM];
static int32_t			list_end[HAS_MAX_MAP_NUM];
static int				list_usage[HAS_MAX_MAP_NUM];

//for user pairs
static has_user_pair_t	user_pair[HAS_MAX_USER_PAIR_SIZE];

static has_timer_t		game_timer;

void has_initilize_data()
{
	memset( node_info, 0, sizeof(node_info) );
	int i;
	for( i = 0; i < HAS_MAX_MAP_NUM; ++i )
	{
		list_head[i] = -1;
		list_end[i] = -1;
		list_usage[i] = FD_INVALID;
	}
    INIT_LIST_HEAD(&game_timer.timer_list);
	ADD_TIMER_EVENT(&game_timer, has_time_out, NULL, get_now_tv()->tv_sec + HAS_TIMER );
}

static int malloc_node_buf()
{
	int i;
	for( i = 0; i < HAS_LIST_RESERVED_SIZE; ++i )
	{
		if( 0 == node_info[i].is_used )
		{
			node_info[i].is_used = 1;
			memset( &node_info[i].data, 0, sizeof(has_wait_node_t) );
			return i;
		}
	}
	return -1;
}

static has_wait_node_t* get_node_ptr( int idx )
{
	if( idx >=0 && idx < HAS_LIST_RESERVED_SIZE )
	{
		if( 1 == node_info[idx].is_used )
		{
			return &node_info[idx].data;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

static void free_node_buf( int idx )
{
	if( idx >=0 && idx < HAS_LIST_RESERVED_SIZE )
	{
		node_info[idx].is_used = 0;
	}
}
static int is_buf_used( int idx )
{
	if( idx >=0 && idx < HAS_LIST_RESERVED_SIZE )
	{
		if( node_info[idx].is_used == 0 )
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

inline static int list_get_next_idx( int idx )
{
	if( idx >=0 && idx < HAS_LIST_RESERVED_SIZE )
	{
		return get_node_ptr(idx)->next_idx;
	}
	else
	{
		return -1;
	}
}

inline static int list_get_prev_idx( int idx )
{
	if( idx >=0 && idx < HAS_LIST_RESERVED_SIZE )
	{
		return get_node_ptr(idx)->prev_idx;
	}
	else
	{
		return -1;
	}
}

inline static uint32_t list_get_data( int idx )
{
	if( idx >=0 && idx < HAS_LIST_RESERVED_SIZE )
	{
		return get_node_ptr(idx)->user_id;
	}
	else
	{
		return 0;
	}
}

inline static void list_remove( int* head, int* end, int idx )
{
	if( idx >=0 && idx < HAS_LIST_RESERVED_SIZE )
	{
		if( is_buf_used(idx) == 0)
		{
			return;
		}
		has_wait_node_t* ptr_prev = get_node_ptr( list_get_prev_idx(idx) );
		has_wait_node_t* ptr_next = get_node_ptr( list_get_next_idx(idx) );
		if( ptr_prev != NULL )
		{
			ptr_prev->next_idx = list_get_next_idx(idx);
		}
		if( ptr_next != NULL )
		{
			ptr_next->prev_idx = list_get_prev_idx(idx);
		}
		if( *head == idx )
		{
			*head = list_get_next_idx(idx);
		}
		if( *end == idx )
		{
			*end = list_get_prev_idx(idx);
		}
		free_node_buf(idx);
	}
}

inline static void list_push_back(int* head, int* end, int idx)
{
	//if not empty
	if( *head != -1 )
	{
		if( *end != -1 )
		{
			get_node_ptr(*end)->next_idx = idx;
			get_node_ptr(idx)->prev_idx = *end;
			get_node_ptr(idx)->next_idx= -1;
			*end = idx;
		}
		else
		{
			ERROR_LOG ("list_push_back invalid list end idx");
		}
	}
	//empty list
	else
	{
		*head = idx;
		*end = idx;
		get_node_ptr(idx)->prev_idx = -1;
		get_node_ptr(idx)->next_idx = -1;
	}
}

inline static void list_pop(int *head, int *end)
{
	if( *head == -1 )
	{
		return;
	}
	int next = get_node_ptr(*head)->next_idx;
	if(	next != -1 )
	{
		get_node_ptr(next)->prev_idx = -1;
		free_node_buf(*head);
		*head = next;
	}
	else
	{
		free_node_buf(*head);
		*head = -1;
		*end = -1;
	}
}

inline static uint32_t list_get_size( int *head )
{
	uint32_t size = 0;
	int idx = *head;
	while( idx != -1 )
	{
		size++;
		idx = list_get_next_idx(idx);
	}
	return size;
}

inline static int get_has_map_id( map_id_t id )
{
	int has_map_id = -1;
	switch(id)
	{
	case HAS_MAP_0:
		{
			has_map_id = 0;
		}
		break;
	case HAS_MAP_1:
		{
			has_map_id = 1;
		}
		break;
	default:
		has_map_id = -1;
		break;
	}
	return has_map_id;
}

inline static void remove_invalid_data(sprite_t* p, int* head, int*end)
{
	sprite_t* sp_other = get_sprite( list_get_data(*head) );		//initialize the condition
	while( sp_other == NULL ||										//the player is invalid
		sp_other->tiles == NULL ||
		sp_other->tiles->id != p->tiles->id ||						//the player in incorrect maps
		sp_other == p )												//the opposite side is myself
	{
		if( sp_other != NULL )
		{
			sp_other->has_state = HS_IDLE;
		}
		list_pop( head,  end );
		if( -1 == *head )
		{
			break;
		}
		sp_other = get_sprite( list_get_data(*head) );
	}
}


int has_join_game_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID( p->id );
	DEBUG_LOG("[%u] request to join queue++++++++++++++++++++++++++++++++++++++++++++++++++++", p->id);
	if( p->has_state != HS_IDLE )
	{
		DEBUG_LOG("[%u]:ERR_has_you_are_busy_now+++++++++++++++++++++++++++++++++++++", p->id);
		return send_to_self_error( p, p->waitcmd, -ERR_has_you_are_busy_now, 1 );
	}
	uint32_t force_id;
	CHECK_BODY_LEN( len, sizeof(uint32_t) );
	int j = 0;
	UNPKG_UINT32( body, force_id, j );

	if( force_id >= FD_MAX )
	{
		DEBUG_LOG("[%u]:ERR_has_invalid_force+++++++++++++++++++++++++++++++++++++", p->id);
		return send_to_self_error( p, p->waitcmd, -ERR_has_invalid_force, 1 );
	}

	int map_id = get_has_map_id( p->tiles->id );

	if( map_id < 0 || map_id >= HAS_MAX_MAP_NUM )
	{

		DEBUG_LOG("[%u]:ERR_has_invalid_map+++++++++++++++++++++++++++++++++++++", p->id);
		return send_to_self_error( p, p->waitcmd, -ERR_has_invalid_map, 1 );
	}

	int32_t b_get_pair = 0;
	int32_t angel_id;
	int32_t clown_id;
	// the queue is not empty
	if( -1 != list_head[map_id] && FD_INVALID != list_usage[map_id] )
	{
		//get opposite force
		if( force_id != list_usage[map_id] )
		{
			remove_invalid_data( p, &list_head[map_id], &list_end[map_id] );

			if( -1 != list_head[map_id] )
			{
				uint32_t other_id = list_get_data(list_head[map_id]);
				angel_id = force_id == FD_ANGEL? p->id : other_id;
				clown_id = force_id == FD_ANGEL? other_id : p->id;

				int i;
				int is_success = 0;
				for( i = 0; i < HAS_MAX_USER_PAIR_SIZE; ++i )
				{
					if(  0 == user_pair[i].is_used )
					{
						user_pair[i].angel_id = angel_id;
						user_pair[i].clown_id = clown_id;
						user_pair[i].is_used = 1;
						user_pair[i].begin_time = get_now_tv()->tv_sec;
						//pop node
						list_pop(&list_head[map_id], &list_end[map_id]);
						b_get_pair = 1;
						is_success = 1;
						DEBUG_LOG("game begin, angel:[%u]\tclown:[%u]+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++", angel_id, clown_id);
						break;
					}
				}
				if( !is_success )
				{
					ERROR_LOG ("has_join_game_cmd pair list out of range");
				}
			}

		}
	}

	//push back
	if( 0 == b_get_pair )
	{
		int idx = malloc_node_buf();
		if( -1 == idx )
		{
			DEBUG_LOG("[%u]:ERR_has_the_queue_is_full+++++++++++++++++++++++++++++++++++++", p->id);
			return send_to_self_error( p, p->waitcmd, -ERR_has_the_queue_is_full, 1 );
		}
		get_node_ptr(idx)->user_id = p->id;
		list_push_back(&list_head[map_id], &list_end[map_id], idx);
		list_usage[map_id] = force_id;
		p->has_state = HS_WAIT;
		DEBUG_LOG("[%u] join game queue curr force is[%d]+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++", p->id, list_usage[map_id]);
	}

	//answer the client request
	int k = sizeof(protocol_t);
	PKG_UINT32( msg, force_id, k );
	PKG_UINT32( msg, b_get_pair, k );
	init_proto_head( msg, p->waitcmd, k );
	int ret = send_to_self( p, msg, k, 1 );

	if( ret != 0 )
	{
		return ret;
	}
	// notify game begin msg to players
	if( 1 == b_get_pair )
	{
		k = sizeof(protocol_t);
		PKG_UINT32( msg, angel_id, k );
		PKG_UINT32( msg, clown_id, k );
		init_proto_head( msg, PROTO_HAS_GAME_START, k );
		sprite_t* p_angel = get_sprite(angel_id);
		sprite_t* p_clown = get_sprite(clown_id);
		if( NULL != p_angel )
		{
			//统计玩的次数
			uint32_t msgbuff[2]= {p_angel->id, 1};
			msglog(statistic_logfile, 0x0408B3B2, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
			ret = send_to_self( p_angel, msg, k, 0 );
			if( ret != 0 )
			{
				return ret;
			}
			p_angel->has_state = HS_INGAME;
		}
		if( NULL != p_clown )
		{
			{
				//统计玩的次数
				uint32_t msgbuff[2]= {p_clown->id, 1};
				msglog(statistic_logfile, 0x0408B3B2, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
			}
			ret = send_to_self( p_clown, msg, k, 0 );
			p_clown->has_state = HS_INGAME;
		}
	}
	return ret;

}

int has_remove_player_from_queue(uint32_t id)
{
	CHECK_VALID_ID( id );
	int map_id;
	for( map_id = 0; map_id < HAS_MAX_MAP_NUM; ++map_id )
	{
		int idx = list_head[map_id];
		while( idx != -1)
		{
			if( id == list_get_data(idx) )
			{
				list_remove( &list_head[map_id], &list_end[map_id], idx );
				break;
			}
			else
			{
				idx = list_get_next_idx(idx);
			}
		}
	}
	return 0;
}


int has_quit_game_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	DEBUG_LOG("[%u] request to quit game++++++++++++++++++++++++++++++++++++++++++++++++++++", p->id);
	uint32_t winner_id = 0;
	uint32_t i_am_angel;
	if( p->has_state == HS_INGAME )
	{
		int i;
		for(i = 0; i < HAS_MAX_USER_PAIR_SIZE; ++i)
		{
			if( 1 == user_pair[i].is_used && ( user_pair[i].angel_id == p->id || user_pair[i].clown_id == p->id ) )
			{
				//clear
				user_pair[i].is_used = 0;

				if( user_pair[i].angel_id == p->id )
				{
					winner_id = user_pair[i].clown_id;
					i_am_angel = 1;
				}
				else
				{
					winner_id = user_pair[i].angel_id;
					i_am_angel = 0;
				}
				break;
			}
		}
		if( winner_id == 0 )
		{
			ERROR_LOG ("has_quit_game_cmd not find the user id");
		}
		else
		{
			int ret = 0;
			ret = has_request_game_over( p, i_am_angel, 0 );
			if( ret != 0 )
			{
				return ret;
			}
			p->has_state = HS_IDLE;

			DEBUG_LOG("[%u] quit the game and the game over winner:[%u]+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++", p->id, winner_id);

			sprite_t* p_win = get_sprite(winner_id);

			if( NULL != p_win )
			{
				ret = has_request_game_over( p_win, i_am_angel^(uint32_t)0x1, 1 );
				p_win->has_state = HS_IDLE;
			}
			if( ret != 0 )
			{
				return ret;
			}
		}
	}
	else if( p->has_state == HS_WAIT )
	{
		DEBUG_LOG("[%u] leave the queue+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++", p->id);
		has_remove_player_from_queue(p->id);
		p->has_state = HS_IDLE;
	}

	int k = sizeof(protocol_t);
	init_proto_head( msg, p->waitcmd, k );
	return send_to_self( p, msg, k, 1 );

}

int has_game_over_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	DEBUG_LOG("[%u] request game over++++++++++++++++++++++++++++++++++++++++++++++++++++", p->id);
	if( p->has_state != HS_INGAME )
	{
		DEBUG_LOG("[%u]:ERR_has_have_not_begin+++++++++++++++++++++++++++++++++++++", p->id);
		return send_to_self_error(p, p->waitcmd, -ERR_has_have_not_begin, 1);
	}
	int ret = -1;
	int i;
	for( i = 0; i < HAS_MAX_USER_PAIR_SIZE; ++i )
	{
		if( 0 == user_pair[i].is_used )
		{
			continue;
		}
		if( p->id == user_pair[i].angel_id || p->id == user_pair[i].clown_id )
		{
			//remove from pair list
			user_pair[i].is_used = 0;
			uint32_t i_am_angel = p->id == user_pair[i].angel_id ? 1 : 0;
			uint32_t other_id = 1 == i_am_angel ? user_pair[i].clown_id : user_pair[i].angel_id;
			DEBUG_LOG("Game Over Winner:[%u] angel:[%u] clown:[%u]+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++", i_am_angel, user_pair[i].angel_id, user_pair[i].clown_id );
			sprite_t* loser_ptr = get_sprite(other_id);
			if( NULL != loser_ptr )
			{
				uint32_t tmp_val = 0x1;
				ret = has_request_game_over( loser_ptr, i_am_angel^tmp_val, 0 );
				if( ret != 0 )
				{
					return ret;
				}
				loser_ptr->has_state = HS_IDLE;
			}
			p->has_state = HS_IDLE;
			return has_request_game_over( p, i_am_angel, 1 );
		}
	}
	//something wrong
	ERROR_LOG ("has_game_over_cmd not find the user pair");
	return ret;
}

int has_game_over_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN( len, sizeof(has_game_over_back_t) );
	has_game_over_back_t* back_info = (has_game_over_back_t*)buf;
	int j = sizeof(protocol_t);
	PKG_UINT32( msg, back_info->i_am_angel, j );
	PKG_UINT32( msg, back_info->is_winner, j );
	PKG_UINT32( msg, back_info->item_count, j );
	PKG_UINT32( msg, back_info->item_id, j );
	uint32_t is_complete = 0;
	if( PROTO_HAS_GAME_OVER == p->waitcmd )
	{
		is_complete = 1;
	}
	init_proto_head( msg, PROTO_HAS_GAME_OVER, j );
	return send_to_self( p, msg, j, is_complete );
}

int has_clear_my_info(sprite_t * p)
{
	CHECK_VALID_ID(p->id);
	if( p->has_state == HS_WAIT )
	{
		has_remove_player_from_queue(p->id);
	}
	else if( p->has_state == HS_INGAME )
	{
		uint32_t winner_id;
		uint32_t is_angel;
		int i;
		for(i = 0; i < HAS_MAX_USER_PAIR_SIZE; ++i)
		{
			if( 1 == user_pair[i].is_used && ( user_pair[i].angel_id == p->id || user_pair[i].clown_id == p->id ) )
			{
				user_pair[i].is_used = 0;
				if( user_pair[i].angel_id == p->id )
				{
					winner_id = user_pair[i].clown_id;
					is_angel = 1;
				}
				else
				{
					winner_id = user_pair[i].angel_id;
					is_angel = 0;
				}
				break;
			}
		}
		if( winner_id == 0 )
		{
			ERROR_LOG ("has_quit_game_cmd not find the user id");
		}
		else
		{
			DEBUG_LOG("[%u] quit game+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++", p->id);
			DEBUG_LOG("Notify winner:[%u]+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++", winner_id);

			int ret = has_request_game_over( p, is_angel, 0 );
			if( 0 != ret )
			{
				return ret;
			}
			sprite_t* p_win = get_sprite(winner_id);
			if( NULL != p_win )
			{
				ret = has_request_game_over( p_win, is_angel^(uint32_t)0x1, 1 );
				p_win->has_state = HS_IDLE;
			}
			return ret;
		}
	}
	return 0;
}

int has_request_game_over( sprite_t* p, int is_angel, int is_win )
{
	CHECK_VALID_ID(p->id);

	int out_cnt = 0;

	if( 1 == is_win )
	{
		if( ISVIP(p->flag) )
		{
			out_cnt = 20;
		}
		else
		{
			out_cnt = 10;
		}
	}
	else
	{
		out_cnt = 2;
	}

	uint32_t item_id;
	if( 1 == is_win )
	{
		if( 1 == is_angel )
		{
			item_id = 1351058;
		}
		else
		{
			item_id = 1351057;
		}
	}
	else
	{
		if( 1 == is_angel )
		{
			item_id = 1351058;
		}
		else
		{
			item_id = 1351057;
		}
	}

	uint32_t db_buf[]={is_angel, is_win, out_cnt, item_id};

	return send_request_to_db( SVR_PROTO_HAS_GAME_OVER, p, sizeof(db_buf), db_buf, p->id );
}

int has_time_out( void* onwer, void* data )
{
	int i;
	uint32_t cur_time = get_now_tv()->tv_sec;
	for( i = 0; i < HAS_MAX_USER_PAIR_SIZE; ++i )
	{
		if( 1 == user_pair[i].is_used && user_pair[i].begin_time + HAS_GAME_TIMER < cur_time )
		{
			user_pair[i].is_used = 0;
		}
	}

	ADD_TIMER_EVENT(&game_timer, has_time_out, NULL, cur_time + HAS_TIMER );

	return 0;
}

int has_get_queue_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id);
	int j = sizeof(protocol_t);
	int map_id = get_has_map_id(p->tiles->id);
	uint32_t queue_size = 0;
	uint32_t queue_usage = 0;
	if( map_id >=0 && map_id < HAS_MAX_MAP_NUM )
	{
		queue_size = list_get_size(&list_head[map_id]);
		queue_usage = list_usage[map_id];
	}
	PKG_UINT32( msg, queue_size, j );
	PKG_UINT32( msg, queue_usage, j );
	init_proto_head( msg, p->waitcmd, j );
	return send_to_self( p, msg, j, 1 );
}


