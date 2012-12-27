/*
 * =====================================================================================
 *
 *       Filename:  matcher.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/19/2011 10:07:48 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef MATCHER_H
#define MATCHER_H
#define PLAYER_MAX_LEVEL	40

#include "../mpog.hpp"

class Player;

class Matcher:public mpog
{
public:
	int		handle_data( sprite_t* p, int cmd, const uint8_t body[], int len );
	int		handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code );
	int		handle_timeout( void* data );
	void	init( sprite_t* p );

	Matcher();
	~Matcher();
private:
	bool	AddPlayer( sprite_t* p, char* data, int32_t len );
	bool	RemovePalyer( Player* p );
	Player*	GetPlayer( sprite_t* p );
private:
	typedef std::map<sprite_t*, Player*>	MapPlayer_t;
	typedef MapPlayer_t::iterator			MapPlayerIt_t;
	typedef MapPlayer_t::value_type			MapPlayerElem_t;
	MapPlayer_t	m_mapPlayer;						//for get player by sprite_t ptr
	Player*		m_pWaitPlayers[PLAYER_MAX_LEVEL];	//for match

};

#endif
