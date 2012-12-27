/*
 * =====================================================================================
 *
 *       Filename:  linker_game.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/10/2012 02:31:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee (), ericlee@taomee.com
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
#include "linker.h"

extern "C"
{

	void* create_game( game_group_t* grp )
	{
		return new (std::nothrow) Linker( grp );
	}

	int game_init()
	{
		return 0;
	}

	void game_destroy()
	{

	}
}
