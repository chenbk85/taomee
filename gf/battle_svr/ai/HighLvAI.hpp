/**
 *============================================================
 *  @file      HighLvAI.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_HIGH_AI_HPP_
#define KFBTL_HIGH_AI_HPP_

extern "C" {
#include <libtaomee/log.h>
}

class MonsterAI;
class Player;

class HighLvAI : public MonsterAI {
public:
	~HighLvAI() {}
	
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
	static HighLvAI* instance();

	/**
	  * @brief boss call other monster to guard
	  */
	void call_monster(Player* player);
	
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
	  * @brief extra defense action
	  */
	virtual bool extra_defense_action(Player* player) {return false;}

	virtual bool extra_move_action(Player* player) {return false;}
	
protected:
	HighLvAI(){ TRACE_LOG("HighLvAI create"); }
};

inline HighLvAI*
HighLvAI::instance()
{
	static HighLvAI instance;

	return &instance;
}

#endif // KFBTL_HIGH_AI_HPP_
