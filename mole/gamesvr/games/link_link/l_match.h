/*
 * =====================================================================================
 *
 *       Filename:  cmatch.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/10/2012 01:43:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */



#ifndef COMMON_MATCH_HPP_INCL
#define COMMON_MATCH_HPP_INCL

#include<vector>
#include <queue>
#include <map>
extern "C" {
#include  <libtaomee/log.h>
#include  <gameserv/game.h>
}
#include "../mpog.hpp"


/*
 * @brief 普通配对，如果有玩家等待，第二个玩家进入后立即配对
 */
class Cmatch : public mpog
{

public:
	Cmatch():m_wait_player(0){}
	void init(sprite_t* p);
	int  handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code);
	int  handle_timeout(void* data);
	int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
	~Cmatch() 
	{
		ERROR_LOG("Card Game's manage: Match DESTRUCTOR");
	}

private:
	sprite_t* m_wait_player;
};
#endif
