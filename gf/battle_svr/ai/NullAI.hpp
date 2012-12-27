/**
 *============================================================
 *  @file      NullAI.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_NULL_AI_HPP_
#define KFBTL_NULL_AI_HPP_

class MonsterAI;
class Player;

class NullAI : public MonsterAI {
public:
	~NullAI() {}
	
	void wait(Player* player, int millisec) {}
	void linger(Player* player, int millisec) {}
	void move(Player* player, int millisec) {}
	void evade(Player* player, int millisec) {}
	void attack(Player* player, int millisec) {}
	void stuck(Player* player, int millisec) {}
	void defense(Player* player, int millisec) {}
	
	//void UpdateAI(uint32 millisec ) {}

	/**
	  * @brief this is a singleton
	  */
	static NullAI* instance();
	
private:
	NullAI(){ TRACE_LOG("NullAI create"); }
};

inline NullAI*
NullAI::instance()
{
	static NullAI instance;

	return &instance;
}

#endif // KFBTL_NULL_AI_HPP_