/**
 *============================================================
 *  @file      LowLvAI.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_STUPID_AI_HPP_
#define KFBTL_STUPID_AI_HPP_

extern "C" {
#include <libtaomee/log.h>
}

class MonsterAI;
class Player;

class LowLvAI : public MonsterAI {
public:
	~LowLvAI() {}
	
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
	static LowLvAI* instance();
	
private:
	LowLvAI(){ TRACE_LOG("LowLvAI create"); }
};

inline LowLvAI*
LowLvAI::instance()
{
	static LowLvAI instance;

	return &instance;
}

#endif // KFBTL_STUPID_AI_HPP_