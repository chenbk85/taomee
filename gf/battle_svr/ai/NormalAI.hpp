/**
 *============================================================
 *  @file      NormalAI.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_NORMAL_AI_HPP_
#define KFBTL_NORMAL_AI_HPP_

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
}

class MonsterAI;
class Player;

class NormalAI : public MonsterAI {
public:
	~NormalAI() {}
	
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	
	//void UpdateAI(uint32 millisec );

	/**
	  * @brief this is a singleton
	  */
	static NormalAI* instance();
	
private:
	NormalAI(){ TRACE_LOG("NormalAI create"); }
};

inline NormalAI*
NormalAI::instance()
{
	static NormalAI instance;

	return &instance;
}

#endif // KFBTL_NORMAL_AI_HPP_