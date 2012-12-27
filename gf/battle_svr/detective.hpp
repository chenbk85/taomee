/**
 *============================================================
 *  @file      detective.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_DETECTIVE_HPP_
#define KFBTL_DETECTIVE_HPP_

#include <vector>
extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
}

class Player;
struct player_move_t;
struct player_attack_t;


class MoveDetective {
public:
	MoveDetective() {}
	virtual ~MoveDetective() {}
	
	virtual int update(Player* p, player_move_t* p_mv) = 0;
};

class BoostMoveDetective : public MoveDetective {
public:
	BoostMoveDetective(){ TRACE_LOG("BoostMove create"); }
	~BoostMoveDetective() {}
	
	int update(Player* p, player_move_t* mv);
};


class SkipMoveDetective : public MoveDetective {
public:
	SkipMoveDetective(){ TRACE_LOG("SkipMoveDetective create"); }
	~SkipMoveDetective() {}
	
	int update(Player* p, player_move_t* mv);
};

class AtkMoveDetective : public MoveDetective {
public:
	AtkMoveDetective(){ TRACE_LOG("AtkMoveDetective create"); }
	~AtkMoveDetective() {}
	
	int update(Player* p, player_move_t* mv);
};



class AtkDetective {
public:
	AtkDetective() {}
	virtual ~AtkDetective() {}
	
	virtual int update(Player* p, player_attack_t* p_mv) = 0;
};

class BoostNormalAtkDetective : public AtkDetective {
public:
	BoostNormalAtkDetective(){ TRACE_LOG("BoostMove create"); }
	~BoostNormalAtkDetective() {}
	
	int update(Player* p, player_attack_t* mv);
};

class RobotNormalAtkDetective : public AtkDetective {
public:
	RobotNormalAtkDetective(){ TRACE_LOG("Robot create"); }
	~RobotNormalAtkDetective() {}
	
	int update(Player* p, player_attack_t* mv);
};



class Detective_mrg {
	public:
		Detective_mrg();
		~Detective_mrg();

		int move_detect(Player* p, player_move_t* mv);
		int atk_detect(Player* p, player_attack_t* atk);

	private:	
		std::vector<MoveDetective*> move_detect_vec;
		std::vector<AtkDetective*> atk_detect_vec;

	private:
		void begin_move(Player * p, player_move_t * mv);
		void end_move(Player * p, player_move_t * mv);
		void begin_atk(Player * p, player_attack_t* atk);
		void end_atk(Player * p, player_attack_t* atk);
};

extern Detective_mrg g_detect_mrg;


#endif // KFBTL_DETECTIVE_HPP_
