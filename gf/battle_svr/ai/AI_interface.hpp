/**
 *============================================================
 *  @file      AI_interface.hpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_AI_INTERFACE_HPP_
#define KFBTL_AI_INTERFACE_HPP_

#include <boost/pool/object_pool.hpp>
#include <libtaomee++/memory/mempool.hpp>
#include "../moving_object.hpp"

#define AI_EVENT_NUM	5

class MonsterAI;
class Player;
struct AIFuncNames;

//class AIInterface : public MemPool<AIInterface> {
class AIInterface : public taomee::MemPool {
public:
	/**
	  * @brief AIInterface Constructor
	  */
	AIInterface(Player* mon, uint32_t ai_type = 0);
	
  /**
	  * @brief AIInterface Destructor 
	  */
	~AIInterface();
	
	
	void reset();
	/**
	  * @brief get cur_ai_state_
	  */
	uint32_t cur_state()
		{ return cur_ai_state_; }
	
	/**
	  * @brief change state
	  */
	void change_state(uint32_t new_state);
	
	/**
	  * @brief judge if in some state
	  */
	bool is_in_state(uint32_t state)
		{ return state == cur_ai_state_; }
	
    /**
	  * @brief update ai
	  */	
	void UpdateAI(int millisec);

	/**
	  * @brief set ai type
	  */
	void select_ai(uint32_t ai_type, uint32_t mon_id = 0);

	void change_ai( MonsterAI* ai, uint32_t ai_type);	
	/**
	  * @brief return to pre ai
	  */
	void return_pre_ai();
	
    /**
	  * @brief get ai type
	  */
	uint32_t ai_type()
		{ return ai_type_; }
		
	/**
	  * @brief set state_keep_tm
	  */
	void set_state_keep_tm(uint32_t keep_tm);
	
	/**
	  * @brief set state_keep_tm
	  */
	bool judge_state_in_tm();
	
	/**
	  * @brief set update_tm
	  */
	bool judge_update_tm(uint32_t cmp_tm);
	
	/**
	  * @brief judge the monster if trace target
	  */
	bool judge_is_trace()
		{ return trace_target_flag_; }
	
	/**
	  * @brief set monster trace_target_flag_
	  */
	void set_trace_flag(bool flag = true)
		{ trace_target_flag_ = flag; }
	
	/**
	  * @brief set a position to walk to and also set the velocity accordingly
	  */
	void set_target_pos(const Vector3D& new_pos);

	/**
	  * @brief reset velocity
	  */
	void reset_velocity();
	/**
	  * @brief get degree of hatred
	  */
	uint32_t hatred() const
		{ return hatred_; }
		
	/**
	  * @brief set degree of hatred
	  */
	void set_hatred(uint32_t hatred)
		{ hatred_ = hatred; }
		
	/**
	  * @brief set monster run
	  */
	void set_run(bool run_flag=true);
	
	/**
	  * @brief set monster run
	  */
	bool if_monster_run()
		{ return run_flag_;}

	/**
	  * @brief set event time
	  */
	int set_event_tm(uint32_t event_tm, uint32_t event_idx = 1);

	/**
	  * @brief check event time
	  */
	int check_event_tm(uint32_t event_idx = 1);
private:
	/**
	  * @brief update event time
	  */
	void update_event_tm(uint32_t tm);

public:
	/*! attacking target of a player */
	const Player*			target;
	const ItemDrop *        item_target;
	/*! a position around the targeting player */
	Vector3D				target_pos;
	/*! a position around the targeting player */
	//Vector3D				target_pos2;
	/*! path to the target */
	std::vector<KfAstar::Point>	target_path;
	bool get_one_pos_flag;
	
	/*! monster born pos */
	Vector3D init_pos;
	
	/*! count move dist */
	int move_dist_;
	
	bool find_path_flag_;
	
	/*! about guard ai */
	Player* 	clientele;
	int			guard_range;
	bool		call_mon_flag;
	
	/*! skill id be launch */
	uint32_t	ready_skill_id_;
	
	/*! can used any way */
	uint32_t	common_flag_;
	uint32_t	common_flag1_;
	uint32_t	common_flag2_;
	uint32_t	common_flag3_;
	/*summon pick item target*/
	uint32_t    item_flag;

	/*! lua name */
	const AIFuncNames* m_func_names;

private:
    /*! the monster has the ai */
	Player* mon_;
	/*! the monster's ai */
	MonsterAI* mon_ai_;

	/*! global ai state */
	uint32_t g_ai_state_;
    /*! record current state */
	uint32_t cur_ai_state_;
	/*! record previous state */
	uint32_t pre_ai_state_;
	
	/*! some state need keep time*/
	uint32_t state_keep_tm_;
	/*! some state last time */
	uint32_t state_last_tm_;
	
	/*! low frequency update*/
	uint32_t update_tm_;
	
	/*! the type of ai */
	uint32_t ai_type_;
	uint32_t pre_ai_type_;
	
	/*! record the monster tracing target */
	bool trace_target_flag_;
	
	uint32_t hatred_;
	
	/*! monster speed radio */
	float speed_radio_;
	bool	run_flag_;
	
	/*! AI eveny time */
	uint32_t ai_event_tm_[AI_EVENT_NUM];
};

#endif // KFBTL_AI_INTERFACE_HPP_
