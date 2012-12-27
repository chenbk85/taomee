/**
 *============================================================
 *  @file      SummonMonAI.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_SUMMON_MON_AI_HPP_
#define KFBTL_SUMMON_MON_AI_HPP_

extern "C" {
#include <libtaomee/log.h>
}
#include "HighLvAI.hpp"


//typedef bool (*extra_func)(Player * p);

class MonsterAI;
class Player;

/****************************************************************
 * summon monster
 ****************************************************************/
class SummonAI : public MonsterAI {
public:
	~SummonAI() {}
	
	virtual void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	virtual void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	virtual void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);

	virtual bool pick_item(Player * player);

	virtual ItemDrop * get_item_drop_target(Player * player);

	void launch_unique_skill(Player* player, uint32_t skill_id);
    bool summon_move(Player* player, int millisec);
	
	virtual	bool extra_wait_action(Player * player);

	virtual bool extra_status_action(Player * player);

	/**
	  * @brief this is a singleton
	  */
	static SummonAI* instance();
	
protected:
	SummonAI(){ TRACE_LOG("SummonAI create"); }
};



inline SummonAI*
SummonAI::instance()
{
	static SummonAI instance;

	return &instance;
}

class SumFury_AI : public SummonAI {
public:
	~SumFury_AI() {}
	bool extra_wait_action(Player * player);
	bool extra_status_action(Player * player);
	static SumFury_AI* instance();
private:
	SumFury_AI(){ TRACE_LOG("Summon Fury create"); }
};

inline SumFury_AI * 
SumFury_AI::instance()
{
	static SumFury_AI instance;
	return &instance;
}

class SumWood_AI : public SummonAI {
public:
	~SumWood_AI() {}
	bool extra_wait_action(Player * player);
	bool extra_status_action(Player * player);
	static SumWood_AI* instance();
private:
	SumWood_AI(){ TRACE_LOG("Summon Fury create"); }
};

inline SumWood_AI * 
SumWood_AI::instance()
{
	static SumWood_AI instance;
	return &instance;
}
class SpeedSum_AI: public HighLvAI 
{
 public:
	 ~SpeedSum_AI() {}
	 virtual void wait(Player* player, int millisec);
	 virtual bool extra_wait_action(Player * player);
	 virtual void stuck(Player * player, int millisec);
	 virtual void move(Player * player, int millisec);
	 static SpeedSum_AI * instance();
protected:
	 SpeedSum_AI()
	 {
		 TRACE_LOG("SpeedSum AI create!");
	 }
};

inline SpeedSum_AI * 
SpeedSum_AI::instance()
{
	static SpeedSum_AI instance;
	return &instance;
}

class SOneSum_AI: public SpeedSum_AI 
{
 public:
	 ~SOneSum_AI()
	 {

	 }
	 virtual bool extra_wait_action(Player* player);
	 static SOneSum_AI *  instance();
 protected:
	 SOneSum_AI()
	 {
		 TRACE_LOG("SOneSum_AI create!");
	 }
};

inline SOneSum_AI * 
SOneSum_AI::instance()
{
	static SOneSum_AI instance;
	return &instance;
}

class STwoSum_AI : public SpeedSum_AI
{
 public:
	 ~STwoSum_AI()
	 {
	 }
	 virtual bool extra_wait_action(Player* player);
	 static STwoSum_AI * instance();

 protected:
	 STwoSum_AI()
	 {
		 TRACE_LOG("STwoSum_AI create!");
	 }
};

inline STwoSum_AI *
STwoSum_AI::instance()
{
	static STwoSum_AI instance;
	return &instance;
}

#endif // KFBTL_SUMMON_MON_AI_HPP_
