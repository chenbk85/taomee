/*
 * =====================================================================================
 *
 *       Filename:  candle_game.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/26/2012 01:46:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


extern "C"
{
#include <libtaomee/log.h>
#include "../../config.h"
#include "../../game.h"
}

#include <new>
#include <vector>
#include "candle.h"

extern "C"
{

	void* create_game( game_group_t* grp )
	{
		return new  candle( grp );
	}

	int game_init()
	{
		return 0;
	}

	void game_destroy()
	{

	}
}
