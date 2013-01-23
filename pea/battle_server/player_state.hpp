/** 
 * ========================================================================
 * @file player_state.hpp
 * @brief 
 * @version 1.0
 * @date 2012-03-16
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



#ifndef H_PLAYER_STATE_H_2012_03_16
#define H_PLAYER_STATE_H_2012_03_16

#include "pea_common.hpp"
#include "buff.hpp"

typedef class Player c_player;


/////////////////////////////////////////////////////
// buff
//

class c_player_buff_mrg {
public:
	c_player_buff_mrg(c_player *p);
	~c_player_buff_mrg();

	int init_player_buff();
	int final_player_buff();
public:

	int add_player_buff(uint32_t buff_id, uint32_t creator_id, uint32_t duration_time = 0, bool notify = true);

	int add_player_buff(c_buff * p_buff, bool notify = true);
	
	int del_player_buff(c_buff * p_buff, bool notify);
	
	int del_player_buff(uint32_t buff_id, bool notify = true);
	
	int del_player_all_buff(uint32_t buff_type, bool notify = true);
	
	int del_player_all_buff();
	
	
	
	int del_random_player_buff(uint32_t buff_type, uint32_t max_count = 1);
	
	int reset_player_buff(uint32_t buff_id);
	
	
	bool is_player_buff_exist(uint32_t buff_id);
	bool is_effect_exist(uint32_t effect_type);
	
	c_buff * get_player_buff(uint32_t buff_id);

	
	void special_buff_routing(struct timeval cur_time, uint32_t type);

	void noti_buf_event_to_map(uint16_t buf_id, uint8_t buf_type, uint8_t flag);
	
	void notify_add_buff_to_player(c_buff * p_buff);

	void notify_del_buff_to_player(c_buff * p_buff);


	void player_passive_buff_trigger(struct timeval cur_time, uint32_t trigger_type);
	// player additional attribute
	int calc_player_buff_additional_attribute();

public:
	uint32_t get_player_base_additional_attr_by_effect(c_base_effect* p_base_effect, uint32_t type);
	uint32_t get_base_attr_by_type(c_buff* p_buff, uint32_t type);
	uint32_t get_addition_damage_by_buff(c_buff* p_buff);
	float get_addition_damage_by_effect(c_base_effect* p_base_effect);
public:
	uint32_t damage_trim() {
		return damage_trim_;
	}
public:
	obj_attr* attr_buff_trim;
	obj_attr* attr_debuff_trim;
private:
	// 玩家的buff
    std::vector<c_buff *> m_buff;
	c_player* owner;

	//other attr
	float damage_trim_;
};



class c_player_state_mrg {

public:
	c_player_state_mrg(c_player* p);
	~c_player_state_mrg();
	int init_player_status();
	int final_player_status();

public:
	uint32_t calc_merge_attr();
	uint32_t get_attr(uint32_t attr_id) {
		return merge_attr->id2value(attr_id);
	}
	void set_attr(uint32_t attr_id, uint32_t value) {
		merge_attr->set_attr(attr_id, value);
	}
	
	int	damage_trim() {
		return player_buff_mrg->damage_trim();
	}

public:
	//player attr
	void inc_player_hp(uint32_t value);

	void dec_player_hp(uint32_t value);
	
public:
	c_player_buff_mrg* player_buff_mrg;
	
	obj_attr* merge_attr;
public:
	c_player* owner;

};




#endif
