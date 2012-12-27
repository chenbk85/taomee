/**
 *============================================================
 *  @file      MonsterAI.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_MONSTER_AI_HPP_
#define KFBTL_MONSTER_AI_HPP_

#include "../player.hpp"

/**
  * @brief ai state
  */
enum ai_state_t {
	WAIT_STATE = 1,
	LINGER_STATE,
	MOVE_STATE,
	EVADE_STATE,
	ATTACK_STATE,
	STUCK_STATE,	
	DEFENSE_STATE,	
};

/**
  * @brief ai type
  */
enum ai_type_t {
	NULL_AI = 0,
	NORMAL_AI = 1,

	BOSS_AI,
	LOW_LV_AI,
	MIDDLE_LV_AI,
	HIGH_LV_AI,
	
	LONG_RANGE_ATK_AI,
	GUARD_AI,
	
	SUMMON_MON_AI,
	
	SPECIAL_MON_AI,
};


class MonsterAI {
public:
	/**
	  * @brief MonsterAI Destructor 
	  */
	virtual ~MonsterAI(){}
	
	/**
	  * @brief virtual member function  
	  */
	virtual void wait(Player *, int tm_diff) = 0;
	virtual void linger(Player *, int tm_diff) = 0;
	virtual void move(Player *, int tm_diff) = 0;
	virtual void evade(Player* , int tm_diff) = 0;
	virtual void attack(Player *, int tm_diff) = 0;
	virtual void stuck(Player *, int tm_diff) = 0;
	
	virtual void defense(Player *, int tm_diff) = 0;
	
	//virtual void UpdateAI(const uint32_t diff ) = 0;
	
//protected:
public:
	/**
	  * @brief get the target
	  * @param player the monster
	  */
	virtual Player* get_target(const Player* player);
	
	/**
	  * @brief summon find a mon to attack
	  * @param summon the summon monster
	  */
	Player* get_summon_target(const Player* summon);

	virtual ItemDrop * get_item_drop_target(Player * summon);
	
	/**
	  * @brief get the monster count near the target
	  * @param player the monster
	  */
	uint32_t get_near_target_cnt(const Player* player);
	
	/**
	  * @brief get the monster count in the map
	  * @param player the monster
	  */
	uint32_t get_monster_cnt(const Player* player, uint32_t monster_id);
	
	/**
	  * @brief control the monster to move to target pos
	  * @param player the monster
	  * @param new_pos the pos the monster attach this time
	  * @param topos the target pos
	  */
	void move_to_target_pos(Player* player, Vector3D& new_pos, const Vector3D& topos, uint32_t millisec);
	
	/**
	  * @brief get one target pos from target_path
	  * @param player the monster
	  */
	void get_one_target_pos(const Player* player);
	
	/**
	  * @brief calculate the target pos which the monster can move to attack player
	  * @param player the monster
	  */
	bool calc_target_pos(Player* player, uint32_t assign_skill_id = 0);
	
	/**
	  * @brief calculate the target pos which the LRA monster can move to attack player
	  * @param player the monster
	  */
	bool calc_long_atk_pos(Player* player);
	
	/**
	  * @brief calculate the linger pos which the monster can linger
	  * @param player the monster
	  */
	bool calc_linger_pos(const Player* player);
	
	/**
	  * @brief calculate the target pos which the monster can move
	  * @param player the monster
	  */
	bool calc_interval_move_pos(const Player* player);
	
	/**
	  * @brief calculate the dodge pos which the monster can dodge after be attacked
	  * @param player the monster
	  */
	bool calc_dodge_pos(const Player* player);
	
	/**
	  * @brief calculate the evade pos which the monster can stay away from player
	  * @param player the monster
	  */
	bool calc_evade_pos(const Player* player);
	
	/**
	  * @brief calculate the evade pos which the monster can stay away from player
	  * @param player the monster
	  */
	bool calc_new_evade_pos(const Player* player);
	
	/**
	  * @brief calculate the return pos which the monster return to born pos
	  * @param player the monster
	  */
	bool calc_return_pos(const Player* player);
	
	/**
	  * @brief set the return pos which the summon monster can atk
	  * @param player the monster
	  */
	void set_summon_atk_pos(Player* player);

	/**
	  * @brief monster move to target
	  * @param player the monster
	  * @return reach target:return true
	  */
	bool monster_move(Player* player, int millisec);


	bool monster_move_ex(Player* player, int millisec);	
	/**
	  * @brief monster stand
	  * @param player the monster
	  */
	void monster_stand(const Player* player);
	
	/**
	  * @brief monster speak
	  * @param player the monster
	  */
	void monster_speak(const Player* player, uint32_t word_idx);
	
	/**
	  * @brief monster move to target
	  * @param player the monster
	  * @return reach target:return true
	  */
	void send_monster_move_noti(const Player* player, const Vector3D& new_pos, uint8_t move_type = 2);
	
	/**
	  * @brief monster attack player
	  * @param player the monster
	  * @param skill the skill which monster used to attack player 
	  */
	void monster_attack(Player* player, player_skill_t* skill, uint32_t assign_x = 0, uint32_t assign_y = 0);
	
	/**
	  * @brief monster attack player, but without notify to player
	  * @param player the monster
	  * @param skill the skill which monster used to attack player 
	  */
	void monster_attack_without_noti(Player* player, player_skill_t* skill, uint32_t assign_x = 0, uint32_t assign_y = 0);
	
	/**
	  * @brief summon monster attack monster
	  * @param player the monster
	  * @param skill the skill which monster used to attack player 
	  */
	void summon_attack(Player* player, player_skill_t* skill, uint32_t assign_x = 0, uint32_t assign_y = 0);

	void summon_pick_item(Player * player, uint32_t unique_id);

	/**
	  * @brief boss use buff skill
	  * @param player the monster
	  */
	int use_buf_skill(Player* player, uint32_t buf_type = 0);

	/**
	  * @brief the monster teleport to another place 
	  */
	void teleport(Player* player, uint32_t x, uint32_t y);
	
	/**
	  * @brief judge monster or player be at dead's door
	  */
	bool is_near_dead(const Player* p)
		{ return ( p->hp < p->maxhp / 10 ); }
	
    /**
	  * @brief judge monster or player is serious injury
	  */
	bool is_serious_injury(const Player* p)
		{ return ( p->hp > p->maxhp / 10 && p->hp < p->maxhp / 2 ); }

	bool is_injury(const Player *p) 
		{ return (p->hp < p->maxhp / 3);}
	
    /**
	  * @brief judge monster or player is serious injury
	  */
	bool is_slight_injury(const Player* p)
		{ return ( p->hp < p->maxhp && p->hp > p->maxhp / 2 ); }

	bool is_hp_range(const Player* p, int hp_min, int hp_max)
		{ return (p->hp < p->maxhp * hp_max / 100 && p->hp >= p->maxhp * hp_min / 100); }
};


//----------------------------------------------------------
// AIFactory class
//----------------------------------------------------------
class AIFactory {
public:
	/**
	  * @brief set ai type
	  */
	MonsterAI* select_ai(uint32_t ai_type, uint32_t mon_id);
	
	/**
	  * @brief set ai type
	  */
	MonsterAI* select_lua_ai(uint32_t ai_type);
	
	/**
	  * @brief this is a singleton
	  */
	static AIFactory* instance();
private:
	/**
	  * @brief AIFactory Constructor
	  */
	AIFactory(){}
};

inline AIFactory*
AIFactory::instance()
{
	static AIFactory instance;

	return &instance;
}

#define ai_factory AIFactory::instance()



#endif // KFBTL_MONSTER_AI_HPP_
