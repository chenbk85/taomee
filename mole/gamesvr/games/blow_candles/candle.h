/*
 * =====================================================================================
 *
 *       Filename:  candle.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/26/2012 02:03:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#ifndef CANDLE_INCL
#define CANDLE_INCL

#include "../mpog.hpp"

enum blow_candle_game_proto{
	proto_bcg_game_start         = 30610,//game start
	proto_bcg_game_blow          = 30611,//player blow candle
	proto_bcg_game_over          = 30612,//game over
	proto_bcg_game_load_res      = 30613,//player load resource ok
	proto_bcg_game_round_over    = 30614,//one round game end
	proto_bcg_game_notify_info   = 30615,//notify client game info ,build connect
};

enum game_state_t{
	GM_RS_LOAD    = 0,//resource of game load now
	GM_UNDER_WAY  = 1,//in play game now
	GM_OVER       = 2,//game over
};
enum timer_event_t
{
	TE_PLAYER_BLOW	   = 0,		//waiting for player blow candles
	TE_RESOURCE_LOAD   = 1,		//waiting for client load resources
};

enum candle_color_t{
	NOTHING            = 0,
	WHILTE_COLORE      = 1,
	YELLOW_COLORE      = 2,
	RED_COLORE         = 3,
	BRIGHT_COLORE      = 4,
};
enum game_round_t{
	FIRST_GAME          = 1,
	SECOND_GAME         = 2,
	THIRD_GAME          = 3,
	FOURTH_GAME         = 4,
	FIIFTH_GAME         = 5,
	GAME_END            = 6,
	
};
struct coord_candle_t{
	int x; //x_coordinate
	int y;//y_coordinate
	candle_color_t state;// 0 no candle, 1 white candle, 2 yellow candle, 3 red candle, 4 bright white candle
	inline bool operator == ( const coord_candle_t& other )
	{
		return (this->x == other.x && this->y == other.y);
	}
};

struct coordinate_t{
	int x;
	int y;
};

struct player_t{
	sprite_t *m_player;
	uint32_t badge;
	uint32_t xiaomee;
	uint32_t mvp_team;
	uint32_t m_sport_team;
	uint32_t m_win_count;
	uint32_t db_step;
};

class candle:public mpog
{
public:
	int		handle_data( sprite_t* p, int cmd, const uint8_t body[], int len );
	int		handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code );
	int		handle_timeout( void* data );
	void	init( sprite_t* p );
	candle( game_group_t* grp );
	~candle();

private:
	int notify_game_start();
	int notify_game_over(player_t *winner, bool escape = false);
	int notify_round_over();
	void notify_client_game_info();
	int check_blow_candle_over(uint8_t len);
	int check_range_candle_legal(coordinate_t *points, uint8_t count);
	void start_timer( timer_event_t evt, uint32_t time );
	bool end_timer( bool force = false );
	player_t* get_player(sprite_t *p){
		for(uint8_t k = 0; k < 2; ++k){
			if(m_players[k].m_player->id == p->id){
				return  &m_players[k];
			}
		}
		return 0;
	}
	void random_n_number(uint8_t count, uint8_t *array);
	int blow_candles(player_t *p, const uint8_t* body, uint32_t count);
	game_round_t next_game_round(game_round_t var_round);


private:
	list_head_t	 m_timer;
	game_state_t m_game_state;//the state of game
	game_round_t m_round_cnt;//the round of game
	player_t m_players[2];// two players
	coord_candle_t* m_map;//the arrangement and state  of candles
	game_group_t*	m_grp;
	timer_event_t	m_timer_event;
	uint32_t	m_timer_id;
	uint32_t	m_timer_counter;
	uint8_t	m_client_cnt;
	int8_t m_on_offensive;
	uint8_t current_operator;
	uint32_t m_db_ret;

};

#endif
