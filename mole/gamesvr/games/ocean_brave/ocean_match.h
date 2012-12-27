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

#ifndef OCEAN_MATCHER
#define OCEAN_MATCHER


#include "../mpog.hpp"


class Omatch:public mpog
{
public:
	int		handle_data( sprite_t* p, int cmd, const uint8_t body[], int len );
	int		handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code );
	int		handle_timeout( void* data );
	void	init( sprite_t* p );

	Omatch():m_wait_player(0){}
	~Omatch(){ }

private:
	sprite_t* m_wait_player;

};

#endif
