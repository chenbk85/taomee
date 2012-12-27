/**
 *============================================================
 *  @file      LuaAI.cpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */
#ifndef KFBTL_LUA_AI_HPP_
#define KFBTL_LUA_AI_HPP_

extern "C" {
#include <libtaomee/log.h>
}

class HighLvAI;
class Player;


/****************************************************************
 * Lua Boss AI
 ****************************************************************/
class BossLuaAI : public HighLvAI {
public:
	~BossLuaAI() {}
	
	/**
	  * @brief this is a singleton
	  */
	static BossLuaAI* instance();
	
	/**
	  * @brief extra wait action
	  */
	bool extra_wait_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_attack_action(Player* player);

	/**
	  * @brief extra wait action
	  */
	bool extra_stuck_action(Player* player);
	
private:
	BossLuaAI(){ TRACE_LOG("BossLuaAI create"); }
};

inline BossLuaAI*
BossLuaAI::instance()
{
	static BossLuaAI instance;

	return &instance;
}

#endif

