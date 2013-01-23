#include "cli_proto.hpp"
#include "battle.hpp"
#include "battle_impl.hpp"
#include "battle_manager.hpp"
#include "attack_obj.hpp"

static int send_btl_user_list(Player* p, Battle* btl);
static int send_player_skill_list(Player* p);


/////////////////////////////static function begin///////////////////////////////////
int send_player_skill_list(Player* p)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	uint32_t count = p->skill_maps->size();

	pack(pkgbuf, count, idx);

	std::map<uint32_t, player_skill*>::iterator pItr = p->skill_maps->begin();
	for(; pItr != p->skill_maps->end(); ++pItr)
	{
		player_skill* p_skill = pItr->second;

		pack(pkgbuf, p_skill->p_data->skill_id, idx);
		pack(pkgbuf, p_skill->p_data->skill_lv, idx);
	}

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_get_btl_player_skill_list_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int send_btl_user_list(Player* p, Battle* btl)
{
	if( p->btl != btl)return 0;

	std::list<Player*>::iterator pItr = btl->playerlist.begin();
	
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, btl->get_battle_id(), idx);
	pack(pkgbuf, btl->get_battle_stage_id(), idx);
	pack(pkgbuf, (uint32_t)btl->playerlist.size(), idx);


	for(; pItr != btl->playerlist.end(); ++pItr)
	{
		Player* obj = *pItr;

//		obj->set_attr_value(OBJ_ATTR_HP, 100);
//		obj->set_attr_value(OBJ_ATTR_MAX_HP_LIMIT, 100);

	 	pack(pkgbuf, obj->id, idx);
		pack(pkgbuf, obj->nick, sizeof(obj->nick), idx);
		pack(pkgbuf, obj->team, idx);
		pack(pkgbuf, obj->get_merge_attr_value(OBJ_ATTR_EXP), idx);
		pack(pkgbuf, obj->get_merge_attr_value(OBJ_ATTR_LEVEL), idx);
		pack(pkgbuf, obj->get_merge_attr_value(OBJ_ATTR_HP), idx);
		pack(pkgbuf, obj->get_merge_attr_value(OBJ_ATTR_MAX_HP_LIMIT), idx);
		
		pack(pkgbuf, obj->get_merge_attr_value(OBJ_ATTR_STRENGTH), idx);
        if (NULL != obj->pet)
        {
            pack(pkgbuf, obj->pet->m_id, idx);
            pack(pkgbuf, obj->pet->m_level, idx);
        }
        else
        {
            pack32(pkgbuf, 0, idx);
            pack32(pkgbuf, 0, idx);
        }

		pack(pkgbuf, (uint32_t)obj->get_x(), idx);
		pack(pkgbuf, (uint32_t)obj->get_y(), idx);
		pack(pkgbuf, obj->get_dir(), idx);

		YJ_DEBUG_LOG("send userlist exp=%u lv=%u hp=%u strength=%u", obj->get_merge_attr_value(OBJ_ATTR_EXP), 
			obj->get_merge_attr_value(OBJ_ATTR_LEVEL), obj->get_merge_attr_value(OBJ_ATTR_HP), obj->get_merge_attr_value(OBJ_ATTR_STRENGTH));

	}
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_get_btl_user_list_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


/////////////////////////////static funtion end/////////////////////////////////////////



int online_proto_create_btl(DEFAULT_ARG)
{
	online_proto_create_btl_in *p_in = P_IN;
	
	if(p->check_in_battle())return 0;	

	Battle* btl = battle_factory::get_instance()->create_battle(p, single_pvp_mode, p_in->stage, p_in->player_count);

	if(btl == NULL)return 0;

	btl->add_player(p);

	battle_mgr::get_instance()->add_battle(btl);

	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, btl->btl_id, idx);
	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int online_proto_syn_player_info(DEFAULT_ARG)
{
	online_proto_syn_player_info_in* p_in = P_IN;
	
	Battle* btl = battle_mgr::get_instance()->get_battle_by_id(p_in->battle_id);
	
	if(btl == NULL) return 0;

	if(btl->get_btl_map_data() == NULL)
	{
		btl->random_init_btl_stage_map();	
	}

	p->init_player_attr(btl, p_in);

    p->init_player_pet(p_in);

	p->init_player_skill(p_in);	

	if( !btl->check_creater(p))
	{
		btl->add_player(p);
	}

	p->change_btl_player_state(btl_player_begin_load_state);

	if(btl->check_syn_players())
	{
		btl->change_btl_state(battle_wait_load_state);
		btl->notify_all_load_resouce();	
	}
	p->clear_waitcmd();
	return 0;
}


int online_proto_load_resource(DEFAULT_ARG)
{
	ERROR_TLOG("----------load_resource uid = %u--------------", p->id);
	
	online_proto_load_resource_in* p_in = P_IN;

	Battle* btl = battle_mgr::get_instance()->get_battle_by_id(p_in->battle_id);
	
	if(btl == NULL)return 0;

	if(!btl->is_player_exist(p->id))return 0;

	if(!p->check_btl_player_state(btl_player_begin_load_state) )return 0;

	p->change_btl_player_state(btl_player_load_ok_state);

	p->clear_waitcmd();

	if(btl->check_load_resource_players())
	{
		struct timeval cur_time = *get_now_tv();
		btl->change_all_btl_player_state(btl_player_running_state);
		btl->change_btl_state(battle_running_state);
		btl->init_btl_begin_time();
		btl->init_round_delay_timer(cur_time, BATTLE_ROUND_DELAY_TIME);
		btl->notify_all_battle_start();	
	}

	if(btl->check_battle_end())
	{
		btl->change_btl_state(battle_destroy_state);
		btl->change_all_btl_player_state(btl_player_end_state);
		btl->calc_battle_statistics_data();
		btl->notify_all_battle_end();
	}
	return 0;
}


int online_proto_get_btl_user_list(DEFAULT_ARG)
{
	online_proto_get_btl_user_list_in* p_in = P_IN;
	
	Battle* btl = battle_mgr::get_instance()->get_battle_by_id(p_in->battle_id);

	if(btl == NULL)return 0;

	if(!btl->is_player_exist(p->id))return 0;
	
	if(!p->check_btl_player_state(btl_player_begin_load_state) )return 0;

	return 	send_btl_user_list(p, p->btl);	
}


int online_proto_player_move(DEFAULT_ARG)
{
	p->clear_waitcmd();

	online_proto_player_move_in* p_in = P_IN;
	
	struct timeval  cur_time = *get_now_tv();

	if(!p->btl->check_player_turn(p))return 0;

	if(!p->check_btl_player_state(btl_player_running_state) )return 0;

	if(!p->btl->check_btl_state(battle_running_state))return 0;

	if(!p->btl->check_move_able(p, p_in->pos_x, p_in->pos_y)){
		ERROR_TLOG("player can NOT move to map_id = %u, uid = %u, x = %f, y = %f, new_x = %u, new_y = %u", p->btl->get_battle_stage_id(), p->id, p->get_x(), p->get_y(),  p_in->pos_x, p_in->pos_y);
		return send_header_to_player(p, p->waitcmd, BTL_ERR_MOVE_VALID_POS, 1);
	}

	p->set_x(p_in->pos_x);
	p->set_y(p_in->pos_y);
	p->set_dir(p_in->dir);
	p->set_angle(p_in->angle);
	p->set_gun_angle(p_in->gun_angle);
	YJ_DEBUG_LOG("move pos: %.0lf, %.0lf\n", p->get_x(), p->get_y());
	ERROR_TLOG("player move map_id = %u uid = %u , x= %f, y= %f", p->btl->get_battle_stage_id(), p->id, p->get_x(), p->get_y());

	p->btl->notify_all_player_move(p);

	bool ret = p->btl->player_fall_down(p);
	if(ret)
	{ 
		p->btl->notify_all_fall_down(p);
		if(p->is_dead())
		{
			p->btl->notify_all_player_dead(p);
			if(p->btl->check_battle_end())
			{
				p->btl->change_btl_state(battle_end_state);
				p->btl->change_all_btl_player_state(btl_player_end_state);
			}
			p->btl->goto_next_round(cur_time, BATTLE_ROUND_DELAY_TIME + 8000);	
		}
	}
	return 0;
}

int online_proto_player_ready_attack(DEFAULT_ARG)
{
	struct timeval  cur_time = *get_now_tv();

	if(!p->btl->check_player_turn(p))return 0;

	if(!p->check_btl_player_state(btl_player_running_state) )return 0;

	if(!p->btl->check_btl_state(battle_running_state))return 0;

	if( p->ready_attack_timer.check_timer_flag() && ! p->ready_attack_timer.is_timer_finish(cur_time) )
	{
		p->ready_attack_timer.set_timer_flag(0);
		p->attack_timer.init_base_duration_timer(cur_time, BATTLE_ATTACK_TIME);
		p->btl->notify_all_player_ready_attack(p);		
	}

	p->clear_waitcmd();
	return 0;
}

int online_proto_player_attack(DEFAULT_ARG)
{
	struct timeval  cur_time = *get_now_tv();
	online_proto_player_attack_in* p_in = P_IN;

	if (p_in->gun_angle > 360 || p_in->muzzle_velocity > 100000000) {
		return send_header_to_player(p, p->waitcmd, BTL_ERR_INVALID_PARA, 1);
	}
	YJ_DEBUG_LOG("%u attack %u %u skill=%u", p->id, p_in->gun_angle, p_in->muzzle_velocity, p->cur_skill_lv_id);
	if(!p->btl->check_player_turn(p)){
		return send_header_to_player(p, p->waitcmd, BTL_ERR_NOT_YOUR_TURN, 1);
	}	
	if(!p->check_btl_player_state(btl_player_running_state) ){
		return send_header_to_player(p, p->waitcmd, BTL_ERR_PLAYER_RUNNING_STATE, 1);	
	}
	if(!p->btl->check_btl_state(battle_running_state)){
		return send_header_to_player(p, p->waitcmd, BTL_ERR_BTL_RUNNING_STATE, 1);  	
	}
	if( !(p->attack_timer.check_timer_flag() && ! p->attack_timer.is_timer_finish(cur_time) )){
		return send_header_to_player(p, p->waitcmd, BTL_ERR_ATTACK_TIME_OUT, 1);	
	}

	uint32_t skill_id = p->cur_skill_lv_id / SKILL_ODDS;
	uint32_t skill_lv = p->cur_skill_lv_id % SKILL_ODDS;
	if(p->cur_skill_lv_id  == 0)
	{
		skill_id = p->default_skill_lv_id / SKILL_ODDS;
		skill_lv = p->default_skill_lv_id % SKILL_ODDS;
	}

	player_skill* p_skill = get_player_skill(p, skill_id);
	if(p_skill == NULL){
		return send_header_to_player(p, p->waitcmd, BTL_ERR_INVALID_PARA, 1);  	
	}
//	if( p_skill->check_cd(p->btl->cur_round_num) ){
//		return send_header_to_player(p, p->waitcmd, BTL_ERR_SKILL_CD, 1);   	
//	}
	if(p_skill->p_data->skill_type == 0){
		return send_header_to_player(p, p->waitcmd, BTL_ERR_INVALID_PARA, 1);   	
	}

//	p_skill->set_cd( p->btl->cur_round_num + p_skill->p_data->skill_cd_time);
	p->btl->notify_all_player_attack(p, skill_id, skill_lv, p_in->gun_angle, p_in->muzzle_velocity);
	p->attack_timer.set_timer_flag(0);
	
	attack_obj* p_obj = create_attack_obj(skill_id,  skill_lv, p_in->gun_angle, p_in->muzzle_velocity, cur_time, p);
	uint32_t process_time = 0;
	p_obj->process_attack_obj(cur_time, process_time);
	ERROR_TLOG("------------attack_process_time = %u----------", process_time);
	destroy_attack_obj(p_obj);
	p_obj = NULL;

	p->inc_total_atk_count();
	p->btl->notify_all_player_attack_end(p);
	p->cur_skill_lv_id = 0;
	p->clear_waitcmd();

	if(p->btl->check_battle_end())
	{
		p->btl->change_btl_state(battle_end_state);
		p->btl->change_all_btl_player_state(btl_player_end_state);
	}
	p->btl->goto_next_round(cur_time, process_time + BATTLE_ROUND_DELAY_TIME + 8000);

	return 0;	
}

int online_proto_player_cancel_battle(DEFAULT_ARG)
{
	Battle* btl = p->btl;
	ERROR_TLOG("btl = %u, del player = %u", btl->btl_id, p->id);

	uint32_t uid = p->id;

	btl->del_player(uid);

	btl->notify_all_leave_battle(uid);

	if(btl->check_battle_end())
	{
		btl->change_btl_state(battle_destroy_state);
		btl->change_all_btl_player_state(btl_player_end_state);
		btl->calc_battle_statistics_data();
		btl->notify_all_battle_end();
	}

	if(btl->get_cur_players_count() == 0)
	{
		ERROR_TLOG("del btl = %u", btl->btl_id);
		battle_mgr::get_instance()->del_battle(btl->get_battle_id());	
		battle_factory::get_instance()->destroy_battle(btl);
		btl = NULL;
	}
	return 0;	
}

int online_proto_progress_bar(DEFAULT_ARG)
{
	online_proto_progress_bar_in* p_in = P_IN;
	
	if( !p->check_in_battle())return 0;

	if(!p->check_btl_player_state(btl_player_begin_load_state) )return 0;

	p->btl->notify_all_progress_bar(p_in->uid, p_in->progress);
	
	p->clear_waitcmd();
	return 0;
}

int online_proto_player_cancel_turn(DEFAULT_ARG)
{
	ERROR_TLOG("player cancel_turn p = %u--------", p->id);
	p->clear_waitcmd();

	if( !p->check_in_battle())return 0;

	/*
	if( p->btl->check_btl_state(battle_end_state))
	{
		p->btl->change_btl_state(battle_destroy_state);
		p->btl->calc_battle_statistics_data();
		p->btl->notify_all_battle_end();
		return 0;
	}
	*/

	if(!p->check_btl_player_state(btl_player_running_state) )return 0;

	if(!p->btl->check_btl_state(battle_running_state))return 0;

	struct timeval cur_time = *get_now_tv();
	p->btl->goto_next_round(cur_time, BATTLE_ROUND_DELAY_TIME);
	return 0;
}

int online_proto_player_finish_turn(DEFAULT_ARG)
{
	ERROR_TLOG("player finish_turn p = %u--------", p->id);
	p->clear_waitcmd();

	if( !p->check_in_battle())return 0;

	p->set_btl_round_state(battle_round_end);

	if( !p->btl->check_all_player_round_state(battle_round_end) )
	{
		return 0;
	}

	if( p->btl->check_btl_state(battle_end_state))
	{
		p->btl->change_btl_state(battle_destroy_state);
		p->btl->calc_battle_statistics_data();
		p->btl->notify_all_battle_end();
		return 0;
	}

	struct timeval cur_time = *get_now_tv();
	p->btl->goto_next_round(cur_time, BATTLE_ROUND_DELAY_TIME);
	return 0;
}

int online_proto_select_skill(DEFAULT_ARG)
{
	online_proto_select_skill_in* p_in = P_IN;

	if(!p->btl->check_player_turn(p)){
		return send_header_to_player(p, p->waitcmd, BTL_ERR_NOT_YOUR_TURN, 1);
	}
	if(!p->check_btl_player_state(btl_player_running_state) ){
		return send_header_to_player(p, p->waitcmd, BTL_ERR_PLAYER_RUNNING_STATE, 1);
	}
	if(!p->btl->check_btl_state(battle_running_state)){
		return send_header_to_player(p, p->waitcmd, BTL_ERR_BTL_RUNNING_STATE, 1);
	}
	player_skill* p_skill =  get_player_skill(p, p_in->skill_id);

	if(p_skill == NULL){
		return send_header_to_player(p, p->waitcmd, BTL_ERR_INVALID_PARA, 1);
	}
	if( p_skill->check_cd(p->btl->cur_round_num) ){
		return send_header_to_player(p, p->waitcmd, BTL_ERR_SKILL_CD, 1);	
	}

	p_skill->set_cd( p->btl->cur_round_num + p_skill->p_data->skill_cd_time);
	p->btl->notify_all_select_skill(p, p_in->skill_id, p_in->skill_lv);
	if(p_skill->p_data->skill_type == 0)
	{
	
	}
	else
	{
		p->cur_skill_lv_id = p_in->skill_id * SKILL_ODDS + p_in->skill_lv;	
	}
	YJ_DEBUG_LOG("select skill %u %u %u", p_in->skill_id, p_in->skill_lv, p_skill->p_data->skill_type);
	p->clear_waitcmd();
	return 0;
}


int online_proto_get_btl_user_skill_list(DEFAULT_ARG)
{
	p->clear_waitcmd();
	if(!p->check_btl_player_state(btl_player_begin_load_state) )return 0;
	return send_player_skill_list(p);
}
