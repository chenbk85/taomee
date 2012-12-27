/**
 *============================================================
 *  @file      MiddleLvAI.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_MIDDLELV_AI_HPP_
#define KFBTL_MIDDLELV_AI_HPP_

extern "C" {
#include <libtaomee/log.h>
}

class MonsterAI;
class Player;

class MiddleLvAI : public MonsterAI {
public:
	~MiddleLvAI() {}
	
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
	
	//void UpdateAI(uint32 millisec );

	/**
	  * @brief extra wait action
	  */
	virtual bool extra_wait_action(Player* player) {return false;}
	
	/**
	  * @brief extra wait action
	  */
	virtual bool extra_attack_action(Player* player) {return false;}

	/**
	  * @brief extra wait action
	  */
	virtual bool extra_stuck_action(Player* player) {return false;}
	
	/**
	  * @brief this is a singleton
	  */
	static MiddleLvAI* instance();
	
protected:
	MiddleLvAI(){ TRACE_LOG("MiddleLvAI create"); }
};

inline MiddleLvAI*
MiddleLvAI::instance()
{
	static MiddleLvAI instance;

	return &instance;
}

#endif // KFBTL_MIDDLELV_AI_HPP_