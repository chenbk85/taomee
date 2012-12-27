/**
 *  *============================================================
 *     @file      LowLvAI.hpp
 *     @brief    all states of a player are defined here
 *      
 *     compiler   gcc4.1.2
 *     platform   Linux
 *        
 *     copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *          
 *  *============================================================
**/


#ifndef _MOVE_AI_HPP_
#define _MOVE_AI_HPP_


extern "C" 
{
#include <libtaomee/log.h>
}


class MonsterAI;
class Player;


class MoveAI: public MonsterAI
{
public:
	~MoveAI(){}
	static MoveAI* instance();
	void wait(Player* player, int millisec);
	void linger(Player* player, int millisec);
	void move(Player* player, int millisec);
	void evade(Player* player, int millisec);
	void attack(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	void defense(Player* player, int millisec);
	virtual void get_next_point(Player* player, uint32_t& x,  uint32_t& y);
	virtual bool check_battle_finish(Player* player);
	virtual bool check_change_ai(Player* player);
protected:
	MoveAI(){ TRACE_LOG("MoveAI create");}
};

inline MoveAI* MoveAI::instance()
{
	static MoveAI instance;
	return &instance;
}

/*-------------------------------------------------------------------*/

class YaoYaoMoveAI: public MoveAI
{
public:
	~YaoYaoMoveAI(){}
	static YaoYaoMoveAI* instance();
	virtual void get_next_point(Player* player, uint32_t& x,  uint32_t& y);
	virtual bool check_change_ai(Player* player);
protected:
	YaoYaoMoveAI(){ TRACE_LOG("YaoYaoMoveAI create");}
};

inline YaoYaoMoveAI* YaoYaoMoveAI::instance()
{
	static YaoYaoMoveAI instance;
	return &instance;
}

/*-------------------------------------------------------------------*/

class BoxWatcherAI1: public MoveAI
{
public:
	~BoxWatcherAI1(){ }
	static BoxWatcherAI1* instance();
	virtual void get_next_point(Player* player, uint32_t& x,  uint32_t& y);
protected:
	BoxWatcherAI1(){  TRACE_LOG("BoxWatcherAI1 create"); }
};

inline BoxWatcherAI1* BoxWatcherAI1::instance()
{
	static BoxWatcherAI1 instance;
	return &instance;
}

/*-------------------------------------------------------------------*/

class BoxWatcherAI2: public MoveAI
{
public:
	~BoxWatcherAI2(){}
	static BoxWatcherAI2* instance();
	virtual void get_next_point(Player* player, uint32_t& x,  uint32_t& y);
protected:
	BoxWatcherAI2(){TRACE_LOG("BoxWatcherAI2 create"); }
};

inline BoxWatcherAI2* BoxWatcherAI2::instance()
{
	static BoxWatcherAI2 instance;
	return &instance;
}

/*----------------------------------------------------------------------*/

class CorpseAI: public MoveAI
{
public:
	~CorpseAI(){}
	static CorpseAI* instance();
	void get_next_point(Player* player, uint32_t& x,  uint32_t& y);
	void move(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	bool check_change_ai(Player* player);
protected:
	CorpseAI(){ TRACE_LOG("CorpseAI create"); }
};

inline CorpseAI* CorpseAI::instance()
{
	static CorpseAI instance;
	return &instance;
}

/*-------------------------------------------------------------------------*/
class CorpseBossAI: public MoveAI
{
public:
	~CorpseBossAI(){ }
	static CorpseBossAI* instance();
	void get_next_point(Player* player, uint32_t& x,  uint32_t& y);
	void move(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	bool check_change_ai(Player* player);
protected:
	CorpseBossAI(){ TRACE_LOG("CorpseBossAI create"); }
};
inline CorpseBossAI* CorpseBossAI::instance()
{
	static CorpseBossAI instance;
	return &instance;
}


/*-------------------------------------------------------------------------*/
class MoveCloseAttackAI: public MoveAI
{
public:
	~MoveCloseAttackAI(){}
	static MoveCloseAttackAI* instance();
	void get_next_point(Player* player, uint32_t& x,  uint32_t& y);
	void move(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	bool check_change_ai(Player* player);
	bool check_wall1_exist(Player* player);
	bool check_wall2_exist(Player* player);
	bool check_wall3_exist(Player* player);
protected:
	MoveCloseAttackAI(){ TRACE_LOG("MoveCloseAttackAI create");}
};

inline MoveCloseAttackAI* MoveCloseAttackAI::instance()
{
	static MoveCloseAttackAI instance;
	return &instance;
}

/*---------------------------------------------------------------------------*/
class MoveBombAI: public MoveCloseAttackAI
{
public:
	~MoveBombAI(){}
	static MoveBombAI* instance();
	void get_next_point(Player* player, uint32_t& x,  uint32_t& y);
	void move(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	bool check_change_ai(Player* player);
protected:
	MoveBombAI(){ TRACE_LOG("MoveBombAI");}
};

inline MoveBombAI* MoveBombAI::instance()
{
	static MoveBombAI instance;
	return &instance;
}

/*-------------------------------------------------------------------------*/
class MoveBossAI: public MoveCloseAttackAI
{
public:
	~MoveBossAI(){}
	static MoveBossAI* instance();
	void get_next_point(Player* player, uint32_t& x,  uint32_t& y);
	void move(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	bool check_change_ai(Player* player);
protected:
	MoveBossAI(){ TRACE_LOG("MoveBossAI");}
};

inline MoveBossAI* MoveBossAI::instance()
{
	static MoveBossAI instance;
	return &instance;
}

/*---------------------------------------------------------------------------*/

class MoveFarAttackAI: public MoveCloseAttackAI
{
public:
	~MoveFarAttackAI(){}
	static MoveFarAttackAI* instance();
	void get_next_point(Player* player, uint32_t& x,  uint32_t& y);
	void move(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	bool check_change_ai(Player* player);
protected:
	MoveFarAttackAI(){ TRACE_LOG("MoveFarAttackAI");}
};

inline MoveFarAttackAI* MoveFarAttackAI::instance()
{
	static MoveFarAttackAI instance;
	return &instance;
}

/*---------------------------------------------------------------------------*/
class RunStone_AI: public MoveCloseAttackAI
{
public:
	~RunStone_AI(){}
	static RunStone_AI* instance();
	void get_next_point(Player* player, uint32_t& x,  uint32_t& y);

	void wait(Player * player, int millisec);
	void move(Player* player, int millisec);
	void stuck(Player* player, int millisec);
	bool check_change_ai(Player* player);
protected:
	RunStone_AI(){ TRACE_LOG("MoveBombAI");}
};

inline RunStone_AI* RunStone_AI::instance()
{
	static RunStone_AI instance;
	return &instance;
}

















#endif
