/*
 * =====================================================================================
 *
 *       Filename:  c_link.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/08/2012 10:02:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#ifndef CONMUNICATION_LINK_INCL
#define CONMUNICATION_LINK_INCL

#include "../mpog.hpp"

#define PLAYER_COUNT 2

enum l_link_proto{
	
	proto_linking_game_start        = 30616,
	proto_linking_game_operate      = 30617,
	proto_linking_game_over         = 30618,
	proto_linking_game_notify       = 30619,
	//proto_linking_game_broadcast    = 30620,

};

enum timer_event_t
{
	TE_PLAYER_DOING_LINK	   = 0,		//waiting for player blow candles
};

enum l_link_game_state{
	GAME_PALYING               =  1,
	GAME_OVER                  =  2,
};
class l_link;

class c_link:public mpog{

	public:
		int		handle_data( sprite_t* p, int cmd, const uint8_t body[], int len );
		int		handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code );
		int		handle_timeout( void* data );
		void	init( sprite_t* p );
		c_link(game_group_t *m_grp, sprite_t *player1, sprite_t* player2);
		~c_link();

	private:
		 game_group_t*	m_grp;
		 l_link* m_player_link[PLAYER_COUNT];
		 sprite_t* m_players[PLAYER_COUNT];
		 uint32_t score[PLAYER_COUNT];
		 sprite_t* m_winner;

		 l_link_game_state m_linking_state;
		 list_head_t m_timer;
		 uint32_t	m_timer_id;
		 uint32_t	m_timer_counter;
		 timer_event_t	m_timer_event;

	private:
		int notify_game_start();
		int notify_game_over(sprite_t *p = 0, bool flag = false);
		int notify_client_broadcast();
		void start_timer(timer_event_t evt, uint32_t time);
		bool end_timer( bool force = false );
		int do_linking_game(sprite_t* p, const uint8_t body[], int len );
}; 

#endif
