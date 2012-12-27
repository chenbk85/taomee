/**
 *============================================================
 *  @file      AI_interface.cpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */
 
#include "MonsterAI.hpp"
//#include "lua_engine.hpp"
#include "../player.hpp"
#include "../player_status.hpp"


AIInterface::AIInterface(Player* mon, uint32_t type)
	:mon_(mon)
{
	select_ai(type);
	
	target         = 0;
	item_target    = 0;
	target_path.reserve(10);
	get_one_pos_flag = false;
	
	state_last_tm_ = 0;
	state_keep_tm_ = 0;
	update_tm_ = 0;
	trace_target_flag_ = false;
	find_path_flag_ = true;
	move_dist_ = 0;
	hatred_ = 0;
	ai_type_ = type;
	
	clientele = 0;
	guard_range = 0;
	call_mon_flag = false;
	
	ready_skill_id_ = 0;
	
	speed_radio_ = 1.0;
	run_flag_ = false;
	
	common_flag_ = 0;
	common_flag1_ = 0;
	common_flag2_ = 0;
	common_flag3_ = 0;
	item_flag = 0;

	for (uint32_t i = 0; i < AI_EVENT_NUM; i++) {
		ai_event_tm_[i] = 0;
	}
}

void AIInterface::reset()
{
	target         = 0;
	item_target    = 0;
	target_path.reserve(10);
	get_one_pos_flag = false;

	state_last_tm_ = 0;
	state_keep_tm_ = 0;
	update_tm_ = 0;
	trace_target_flag_ = false;
	find_path_flag_ = true;
	move_dist_ = 0;
	hatred_ = 0;
	ai_type_ = 0;

	clientele = 0;
	guard_range = 0;
	call_mon_flag = false;

	ready_skill_id_ = 0;

	speed_radio_ = 1.0;
	run_flag_ = false;

	common_flag_ = 0;
	common_flag1_ = 0;
	common_flag2_ = 0;
	common_flag3_ = 0;

	for (uint32_t i = 0; i < AI_EVENT_NUM; i++) {
		ai_event_tm_[i] = 0;
	}
}

AIInterface::~AIInterface()
{
	mon_ = 0;
	mon_ai_ = 0;
}

void 
AIInterface::set_state_keep_tm(uint32_t keep_tm)
{
	mon_->i_ai->state_last_tm_ = get_now_tv()->tv_sec;
	mon_->i_ai->state_keep_tm_ = keep_tm;
}

bool 
AIInterface::judge_state_in_tm()
{
	if (mon_->i_ai->state_keep_tm_ == 0) {
	   return false;
	}
	
	uint32_t now = get_now_tv()->tv_sec;
	if ( now - mon_->i_ai->state_last_tm_ < mon_->i_ai->state_keep_tm_ ) {
		return true;
	}
	set_state_keep_tm(0);
	return false;
}

bool 
AIInterface::judge_update_tm(uint32_t cmp_tm)
{
	if (update_tm_ > cmp_tm) {
		update_tm_ = 0;
	  return true;
	}
	
	return false;
}

void
AIInterface::change_state(uint32_t new_state)
{
	//new state
	pre_ai_state_ = cur_ai_state_;
	cur_ai_state_ = new_state;
}

void
AIInterface::UpdateAI(int millisec)
{
	update_event_tm(millisec);

	update_tm_ += millisec;

	if(mon_ != NULL){
		if( is_effect_exist_in_player(mon_, faint_effect_type)){
			cur_ai_state_ = WAIT_STATE;
			return ;
		}
	}
	if (is_summon_mon(mon_->role_type)) {
		TRACE_LOG("%u %u", mon_->role_type, cur_ai_state_);	
	}

	if (cur_ai_state_ == WAIT_STATE) {
		mon_ai_->wait(mon_, millisec);
	} else if (cur_ai_state_ == LINGER_STATE) {
		mon_ai_->linger(mon_, millisec);
	} else if (cur_ai_state_ == MOVE_STATE) {
		mon_ai_->move(mon_, millisec);
	} else if (cur_ai_state_ == EVADE_STATE) {
		mon_ai_->evade(mon_, millisec);
	} else if (cur_ai_state_ == ATTACK_STATE) {
		mon_ai_->attack(mon_, millisec);
	} else if (cur_ai_state_ == STUCK_STATE) {
		mon_ai_->stuck(mon_, millisec);
	} else if (cur_ai_state_ == DEFENSE_STATE) {
		mon_ai_->defense(mon_, millisec);
	}
}

void
AIInterface::select_ai(uint32_t ai_type, uint32_t mon_id)
{
//	m_func_names = g_lua_engine.get_ai_func_names(mon_->role_type);
//	//printf("AI create:%u %u %u\n",  mon_->role_type, mon_id, m_func_names);
//	if (m_func_names) {
//		mon_ai_ = ai_factory->select_lua_ai(ai_type);
//	} else {
	mon_ai_ = ai_factory->select_ai(ai_type, mon_id);
//	}

	//mon_ai_ = ai_factory->select_ai(ai_type, mon_id);
	pre_ai_type_ = ai_type_;
	ai_type_ = ai_type;
	
	cur_ai_state_ = WAIT_STATE;
	g_ai_state_ = 0;
	
	init_pos.set_x(mon_->pos().x());
	init_pos.set_y(mon_->pos().y());
}


void AIInterface::change_ai( MonsterAI* ai, uint32_t ai_type)
{
	mon_ai_ = ai;

	pre_ai_type_ = ai_type_;
	ai_type_ = ai_type;

	cur_ai_state_ = WAIT_STATE;
	g_ai_state_ = 0;
	reset();
}

void
AIInterface::return_pre_ai()
{
	if (pre_ai_type_) {
		select_ai(pre_ai_type_);
	}
}

void
AIInterface::set_target_pos(const Vector3D& new_pos)
{
	target_pos = new_pos;

	Vector3D v(target_pos.x() - mon_->pos().x(), -(mon_->pos().y() - target_pos.y()));
	v.normalize();
	//v *= (mon_->mon_info->spd * speed_radio_);
	v *= (mon_->get_speed()* speed_radio_);

	mon_->set_velocity(v);
	//printf("monster move: uid=%u v=%s pos=%s %s\n", mon_->id,
	//			mon_->velocity().to_string().c_str(),	target_pos.to_string().c_str(), mon_->pos().to_string().c_str());
}

void 
AIInterface::reset_velocity()
{
	if ( !(target_pos.is_zero()) ) {
		set_target_pos(target_pos);
		get_one_pos_flag = true;
	}
}
void
AIInterface::set_run(bool run_flag)
{
	run_flag_ = run_flag;
	speed_radio_ = run_flag_ ? 1.6 : 1.0;
}

int
AIInterface::set_event_tm(uint32_t event_tm, uint32_t event_idx) 
{
	if (event_idx > AI_EVENT_NUM || event_idx <= 0 || ai_event_tm_[event_idx - 1] != 0) {
		return 0;
	}

	ai_event_tm_[event_idx - 1] = event_tm;

	return 1;
}

int
AIInterface::check_event_tm(uint32_t event_idx)
{
	if (event_idx > AI_EVENT_NUM || event_idx <= 0 || ai_event_tm_[event_idx - 1] != 1) {
		return 0;
	}

	ai_event_tm_[event_idx - 1] = 0;
	return 1;


}

void
AIInterface::update_event_tm(uint32_t tm)
{
	for (uint32_t i = 0; i < AI_EVENT_NUM; i++) {
		if (ai_event_tm_[i] > 1) {
			ai_event_tm_[i] = ai_event_tm_[i] <= tm ? 1 : ai_event_tm_[i] - tm;
		}
	}
}
