#include <list>
#include <map>
#include <vector>

#include <boost/intrusive/list.hpp>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>

extern "C" 
{
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/project/types.h>
#include <async_serv/dll.h>
}

#include "player.hpp"
#include "battle_manager.hpp"
#include "player_skill.hpp"
#include "bullet.hpp"
#include "player_attribute.hpp"

using namespace taomee;
using namespace std;

std::map<uint32_t, Player*> all_players;

Player::Player(uint32_t user_id, fdsession_t* fdsession)
{
	fdsess = fdsession;
	if(fdsess) {
		id = user_id;
		fd = fdsess->fd;
		pkg_queue = g_queue_new();
	} else {
		id = Object::id();
		fd = 0;
		pkg_queue = NULL;
	}
	
	waitcmd = 0;
	server_id = 0;
    eye_model = 0;
    resource_id = 0;
	memset(nick, 0, sizeof(nick));
    exp2level_calc = NULL;

	btl = NULL;
	btl_player_state = btl_player_no_state;
	btl_round_state  = battle_round_no;
	init_btl_statistics_data();
	init_attr();
    init_player_skills(this);
	pet = NULL;
	player_state_mrg = new c_player_state_mrg(this);
}

Player::~Player()
{
	if(is_valid_uid(id) && pkg_queue != NULL) {
		cached_pkg_t* pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(pkg_queue));
		while(pkg) {
			g_slice_free1(pkg->len, pkg);
			pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(pkg_queue));
		}
		g_queue_free(pkg_queue);
		pkg_queue = NULL;
	}
	btl = NULL;
	btl_round_state = battle_round_no;
	if(exp2level_calc) {
		exp2level_factory::getInstance()->destroy_calculator(exp2level_calc);
		exp2level_calc = NULL;
	}
	btl_player_state = btl_player_no_state;
	final_btl_statistics_data();
	final_attr();
	final_player_skills(this);
	delete player_state_mrg;
}

bool Player::init_attr()
{
	attr = new obj_attr();
	return true;
}

bool Player::final_attr()
{
	if(attr)
	{
		delete attr;
		attr = NULL;
	}
	return true;
}

uint32_t Player::get_attr_value(uint16_t attr_type)
{
	return attr->id2value(attr_type);
}

void Player::set_attr_value(uint32_t attr_type, uint32_t value)
{
	uint32_t* p_value = attr->id2p(attr_type);
	if(p_value) {
		*p_value = value;	
	}
}

uint32_t Player::get_merge_attr_value(uint16_t attr_type)
{
	return player_state_mrg->get_attr(attr_type);
}

void Player::set_merge_attr_value(uint32_t attr_type, uint32_t value)
{
	player_state_mrg->set_attr(attr_type, value);
}

void Player::init_player_attr(Battle* btl, online_proto_syn_player_info_in* p_in)
{
	exp2level_calc = exp2level_factory::getInstance()->create_calculator(PLAYER_CALC_TYPE);
	server_id = p_in->server_id;
	
    set_attr_value(OBJ_ATTR_HP, p_in->hp);
    set_attr_value(OBJ_ATTR_EXP, p_in->exp);
    set_attr_value(OBJ_ATTR_LEVEL, p_in->level);
    set_attr_value(OBJ_ATTR_MAGIC, p_in->magic);
    set_attr_value(OBJ_ATTR_AGILITY, p_in->agility);
   	set_attr_value(OBJ_ATTR_LUCK, p_in->luck);
    set_attr_value(OBJ_ATTR_ANGER, p_in->anger);
    set_attr_value(OBJ_ATTR_ATK, p_in->atk);
    set_attr_value(OBJ_ATTR_DEF, p_in->defence);
	set_attr_value(OBJ_ATTR_STRENGTH, p_in->strength);
    set_attr_value(OBJ_ATTR_HAPPY, p_in->happy);
    set_attr_value(OBJ_ATTR_SOUL, p_in->soul);
    set_attr_value(OBJ_ATTR_INTENSITY, p_in->intensity);
    set_attr_value(OBJ_ATTR_CRIT, p_in->crit);
    set_attr_value(OBJ_ATTR_DOUBLE, p_in->double_hit);
    set_attr_value(OBJ_ATTR_SPEED, p_in->speed);
    set_attr_value(OBJ_ATTR_CRIT_DAMAGE, p_in->crit_damage);
    set_attr_value(OBJ_ATTR_TENACITY, p_in->tenacity);
	set_attr_value(OBJ_ATTR_STRENGTH, p_in->strength);	
	
    eye_model = p_in->model.eye_model;
	resource_id = p_in->model.resource_id;
	team = p_in->team;
	strcpy((char*)nick, (char*)p_in->nick);
	
	YJ_DEBUG_LOG("team %u init player attr %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u",
			team, 
			get_attr_value(0), get_attr_value(1), 
			get_attr_value(2), get_attr_value(3), 
			get_attr_value(4), get_attr_value(5), 
			get_attr_value(6), get_attr_value(7), 
			get_attr_value(8), get_attr_value(9), 
			get_attr_value(10), get_attr_value(11), 
			get_attr_value(12), get_attr_value(13), 
			get_attr_value(14), get_attr_value(15));

	if(team == TEAM_ONE) {
		pos* pt = btl->get_stage_data()->get_rand_red_pos();
		set_x(pt->x);
		set_y(pt->y);
		set_dir(pt->dir);
		TRACE_TLOG("init player pos, u=%u, team=%u, pos=(%u,%u)",
				id, team, pt->x, pt->y);
	} else if(team == TEAM_TWO) {
		pos* pt = btl->get_stage_data()->get_rand_blue_pos();
		set_x(pt->x);
		set_y(pt->y);
		set_dir(pt->dir);
		TRACE_TLOG("init player pos, u=%u, team=%u, pos=(%u,%u)",
				id, team, pt->x, pt->y);
	} else {
		ERROR_TLOG("invalid u=%u, team=%u", id, team);	
	}
	set_rectangle(p_in->fight_pet.width, p_in->fight_pet.height);
	
	player_state_mrg->init_player_status();
}


int Player::init_player_pet(online_proto_syn_player_info_in * p_in)
{
    if (0 == p_in->fight_pet.pet_id) {
        return 0;
    }

    pet = new c_pet(this);
    if (NULL == pet) {
        return -1;
    }

    if (0 != pet->init(&p_in->fight_pet)) {
        delete pet;
        pet = NULL;
        return -1;
    }

    return 0;
}

bool Player::init_player_skill(online_proto_syn_player_info_in * p_in)
{
	for(uint32_t i =0; i< p_in->skills.size(); i++) {
		btl_skill_info_t* skill_info = &p_in->skills[i];	
		player_skill* p_skill = new player_skill(skill_info->skill_id, skill_info->skill_lv);
		add_player_skill(this, p_skill);

		if(i == 0) {
			default_skill_lv_id = skill_info->skill_id * SKILL_ODDS + skill_info->skill_lv;	
		}
	}
	return true;
}


bool Player::check_in_battle()
{
    return btl != NULL;	
}

void Player::attach_battle(Battle* btl_ptr)
{
    btl = btl_ptr;
}

void Player::dettach_battle()
{
    btl = NULL;
}

bool Player::init_btl_statistics_data()
{
    player_win_flag = btl_player_draw; 		
    total_atk_count = 0;
    total_hit_count = 0;
    total_damage = 0;
    total_exp = 0;
    total_extern_exp = 0;

    target_exp_data_maps = new std::map<uint32_t, target_exp_data>();
    return true;
}

bool Player::final_btl_statistics_data()
{
    player_win_flag = btl_player_draw;
    total_atk_count = 0;
    total_hit_count = 0;
    total_damage = 0;
    total_exp = 0;
    total_extern_exp = 0;

    delete target_exp_data_maps;
    target_exp_data_maps = NULL;

    return true;
}

float  Player::get_team_exp_factor(uint32_t battle_mode)
{
    switch(battle_mode) {
	case single_pvp_mode:
	case single_pve_mode:
		return 1.0;
		break;

	case team_pvp_mode:
	case team_pve_mode:
		return 1.1;
		break;

	default:
		return 1.0;
    }
    return 1.0;
}

float  Player::get_win_exp_factor(uint32_t win_flag)
{
    switch(win_flag) {
	case btl_player_win:
		return 1.0;
		break;

	case btl_player_lose:
		return 0.2;
		break;

	default:
		return 0.2;	
    }
    return 0.2;
}

float  Player::get_level_exp_factor(uint32_t pet_lv, uint32_t target_lv)
{
    int than_lv = pet_lv - target_lv;

    if(than_lv >= 2) {
        return 1.0;	
    } else if(than_lv == 3) {
        return 0.8;	
    } else if(than_lv == 4) {
        return 0.6;	
    } else if(than_lv == 5) {
        return 0.4;	
    }
    return 0.0;
}

void Player::process_timer(struct timeval cur_time, uint32_t type)
{
	if (type == player_action_step_before_atk) {
		this->player_state_mrg->player_buff_mrg->special_buff_routing(cur_time, EFFECT_TRIGGER_BEFORE_TURN);
	}
	if (type == player_action_step_after_atk) {
		this->player_state_mrg->player_buff_mrg->special_buff_routing(cur_time, EFFECT_TRIGGER_AFTER_TURN);
	}
	if (type == player_action_step_free) {
		this->player_state_mrg->player_buff_mrg->special_buff_routing(cur_time, EFFECT_TRIGGER_NORMAL);
	}
}

Player* get_player(uint32_t userid)
{
    std::map<uint32_t, Player*>::iterator pItr = all_players.find(userid);
    if(pItr != all_players.end()) {
        return pItr->second;
    }
    return NULL;
}

bool is_player_exist(uint32_t userid)
{
    std::map<uint32_t, Player*>::iterator pItr = all_players.find(userid);	
    return pItr != all_players.end();
}

Player* add_player(uint32_t userid, fdsession_t* fdsess)
{
    if(is_player_exist(userid))return NULL;
    Player* p = new Player(userid, fdsess);
    all_players[userid] = p;
    return p;
}

void del_player(Player* p)
{
	if( is_player_exist(p->id)) {
        all_players.erase(p->id);	
    }
    delete p;
}

void clear_players(int fd)
{
    std::map<uint32_t, Player*>::iterator pItr = all_players.begin();
    while (pItr != all_players.end()) {
        Player* p = pItr->second;
        if (p->fd == fd) {
            Battle* btl = p->btl;
            if(btl) {
            	btl->del_player(p->id);
				if (btl->get_cur_players_count() == 0) {
	                battle_mgr::get_instance()->del_battle(btl->get_battle_id());
	                battle_factory::get_instance()->destroy_battle(btl);
	                btl = NULL;	
				}
            } else {
                ::del_player(p);	
            }
            pItr = all_players.begin();
            continue;
        }
        ++pItr;
    }
}


void all_players_routing(struct timeval cur_time)
{
    std::map<uint32_t, Player*>::iterator pItr = all_players.begin();
    for(; pItr != all_players.end(); ++pItr) {
        Player* p = pItr->second;
        player_timer(p, cur_time);
    }
}

void player_timer(Player* p, struct timeval cur_time)
{

}

void Player::on_hit_target(bullet* p_bullet, Player* target)
{
}

void Player::on_hit(bullet* p_bullet, Player* atker)
{
	uint32_t crit_flg = 0;
	uint32_t damage = p_bullet->calc_damage(this, crit_flg);
	dec_hp(damage);
	this->btl->notify_all_obj_attr_chg_delay(this, select_type(OBJ_ATTR_HP), 
		p_bullet->syn_number_, p_bullet->bullet_times_, crit_flg);
	atker->inc_total_damage(damage);
	ERROR_TLOG("player %u reduce hp %u", this->id, damage);

	if(this->is_dead())
	{
		this->btl->notify_all_player_dead_delay(this, p_bullet->syn_number_, p_bullet->bullet_times_);
		ERROR_TLOG("player %u dead", this->id);
	}
}

void Player::inc_hp(uint32_t value)
{
	player_state_mrg->inc_player_hp(value);
}

void Player::dec_hp(uint32_t value)
{
	player_state_mrg->dec_player_hp(value);
}

void Player::set_player_win_flag(uint32_t flag)
{
    player_win_flag = flag;
}

uint32_t Player::get_player_win_flag()
{
    return player_win_flag;
}

