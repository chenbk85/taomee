/**
 *============================================================
 *  @file      GuardAI.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_GUARD_AI_HPP_
#define KFBTL_GUARD_AI_HPP_

extern "C" {
#include <libtaomee/log.h>
}

class MonsterAI;
class Player;

class GuardAI : public MonsterAI {
public:
	~GuardAI() {}
	
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
	
	//void UpdateAI(uint32 millisec );

	/**
	  * @brief this is a singleton
	  */
	static GuardAI* instance();
	
	/**
	  * @brief jugde if in guard range
	  */
	bool jugde_in_guard_range(const Player* player);
	
	/**
	  * @brief calculate the guard pos which the monster can move to
	  * @param player the monster
	  */
	bool calc_guard_pos(const Player* player);
	
	
	
private:
	GuardAI()
	{ 
		TRACE_LOG("GuardAI create"); 
	}
};

inline GuardAI*
GuardAI::instance()
{
	static GuardAI instance;

	return &instance;
}

#endif // KFBTL_GUARD_AI_HPP_