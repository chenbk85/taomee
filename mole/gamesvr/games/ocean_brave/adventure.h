/*
 * =====================================================================================
 *
 *       Filename:  adventure.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/12/2012 04:49:49 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef OCEAN_ADVENURE_INCL
#define OCEAN_ADVENURE_INCL

#include "../mpog.hpp"

#define PLAYER_COUNT 2

enum ocean_adventure_proto{
	proto_adventure_game_start        = 30620,
	proto_adventure_game_store        = 30621,
	proto_adventure_game_over         = 30622,
	proto_adventure_game_notify       = 30623,
	proto_adventure_game_process      = 30624,

};

enum adventure_game_state{
	GAME_START        =      0,
	GAME_GOING        =      1,
	GAME_OVER         =      2,
};
class adventure{
public:
	int		handle_data( sprite_t* p, int cmd, const uint8_t body[], int len );
	int		handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code );
	int		handle_timeout( void* data );
	void	init( sprite_t* p );
	adventure(game_group_t *m_grp, sprite_t *player1, sprite_t* player2);
private:
	game_group_t* m_grp;
	sprite_t* m_players[PLAYER_COUNT];
	uint32_t m_process[PLAYER_COUNT];
	sprite_t* m_winner;
	uint32_t m_player_count;
	uint32_t m_game_state;
	uint32_t m_last_time;
	uint32_t m_pvp_time;

private:
	int notify_game_start();
	int notify_game_over(sprite_t *p = 0, bool flag = false);
	int notify_client_broadcast();
	int handle_pass_gate_process(sprite_t *p, uint32_t flag);

};
#endif
