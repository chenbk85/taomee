#include <string>

#include <libtaomee++/pathfinder/astar/astar.hpp>

#include "battle.hpp"
#include "player.hpp"

#include "skill.hpp"
#include "player_status.hpp"
#include "skill_imme_effect.hpp"
#include "skill_additional_damage.hpp"
//#include "app_log.hpp"
using namespace taomee;

/*! hold all buff effects */
//buff_effect_t buff_effects[max_buf_effects];
/*! hold all skills */
GobalSkill* g_skills;
/*! a list to link all launched skills together */
ObjList g_skill_list;

std::vector<skill_begin_handle_elem> skill_begin_handle_vec;

//skill begin handle
void skill_begin_handle_suck_hp(Player* launcher, const uint32_t* in)
{
	uint32_t percent = in[0];
	Battle* p_btl = launcher->btl;
	if (p_btl) {
		Player* enemy = p_btl->get_an_live_enemy(launcher);
		if (enemy) {
			int suck_hp = percent * enemy->max_hp() / 100;
			enemy->chg_hp(-suck_hp);
			enemy->noti_hpmp_to_btl();
			launcher->chg_hp(suck_hp);
			launcher->noti_hpmp_to_btl();
		}
	}
	TRACE_LOG("i'm here,%u", launcher->id);
}

void skill_begin_handle_suck_mp(Player* launcher, const uint32_t* in)
{
	uint32_t percent = in[0];
	Battle* p_btl = launcher->btl;
	if (p_btl) {
		Player* enemy = p_btl->get_an_live_enemy(launcher);
		if (enemy) {
			int suck_mp = percent * enemy->max_mp() / 100;
			enemy->chg_mp(-suck_mp);
			enemy->noti_hpmp_to_btl();
			launcher->chg_mp(suck_mp);
			launcher->noti_hpmp_to_btl();
		}
	}
	TRACE_LOG("i'm here,%u", launcher->id);
}

void skill_begin_handle_suck_hpmp(Player* launcher, const uint32_t* in)
{
	skill_begin_handle_suck_hp(launcher, in);
	skill_begin_handle_suck_mp(launcher, in + 1);
}

void skill_begin_handle_summon_monster(Player* launcher, const uint32_t* in)
{
	uint32_t monster_id = in[0];
	uint32_t each_cnt = in[1];
	uint32_t total_cnt = in[2];

	if (launcher->cur_map && launcher->btl) {
		uint32_t exist_cnt = launcher->cur_map->get_monster_cnt(monster_id);
		if (exist_cnt < total_cnt) {
			uint32_t will_be = exist_cnt + each_cnt;
			uint32_t del_cnt = will_be > total_cnt ? will_be - total_cnt : 0;

			for (uint32_t i = 0; i < del_cnt; i++) {
				Player* mon = launcher->cur_map->get_one_monster(monster_id);
				if (mon) {
					mon->suicide();
				}
			}
			for (uint32_t i = 0; i < each_cnt; i++) {
				launcher->call_monster(monster_id, 
					launcher->pos().x(), launcher->pos().y(), 
					launcher->team, 0, 0);
			}
		}
	}
}
//--------------------------------------------------------------------------
// class BufEffects Methods
//--------------------------------------------------------------------------
/**
  * @brief constructor
  * @param shooter the player who launched this attack
  * @param skillid id of the skill that is launched
  */

BufEffect::BufEffect(Player* shooter, uint32_t skillid, uint32_t type, uint32_t lv)
{
	launcher_ = shooter;
	
	skill_base_info_.id = skillid;
	skill_base_info_.type = type;
	skill_base_info_.lv = lv;
	
	map_ = launcher_->cur_map;
	
	cur_effects_idx_ = 0;
	p_buf_effect_ = 0;
}

/**
  * @brief default destructor
  */

BufEffect::~BufEffect()
{
	launcher_ = 0;
	map_ = 0;
	p_buf_effect_ = 0;
}

/**
  * @brief init the SkillEffects before use
  */


void
BufEffect::init(uint32_t buff_id, uint32_t buff_target, uint32_t imme_buff_id)
{
	buff_id_ = buff_id;
	buff_target_ = buff_target;
	imme_buff_id_ = imme_buff_id;

	cur_effects_idx_++;
}

/**
  * @brief update attribute of this object
  */

void 
BufEffect::update(int time_elapsed)
{
	affect();
}

int
BufEffect::affect()
{
	//buf only affect oneself
	if (buff_target_ == oneself) {
		if (imme_buff_id_ != 0) { //瞬间效果
			process_skill_effect_logic(launcher_, imme_buff_id_, launcher_);
		} else {
			add_player_buff(launcher_, buff_id_, 0, 0, true, true);
		}
		return 0;
	}

	uint32_t effect_target = 0;
	for (PlayerSet::const_iterator it = map_->monsters.begin(); it != map_->monsters.end(); ++it) {
		Player* p = *it;
		effect_target = 0;
		if (!p->is_dead()) {
			if (launcher_->team != p->team && buff_target_ == enemys) 
			{
				effect_target = enemys;
			} 
			else if (launcher_->team == p->team && buff_target_ == friends) 
			{
				effect_target = friends;
			}
			if (effect_target) {
				TRACE_LOG("player[%u] on hit buff", p->id);
				if (imme_buff_id_ != 0) {
					process_skill_effect_logic(launcher_, imme_buff_id_, p);
				} else {
					add_player_buff(p, buff_id_, 0);
				}
			}
		}
	}

	for (PlayerSet::const_iterator it = map_->players.begin(); it != map_->players.end(); ++it) {
		Player* p = *it;
		effect_target = 0;
		if (!p->is_dead()) {
			if (launcher_->team != p->team && buff_target_ == enemys) {
				effect_target = enemys;
			} else if (launcher_->team == p->team && buff_target_ == friends) {
				effect_target = friends;
			}

			if (effect_target) {
				TRACE_LOG("player[%u] on hit buff", p->id);
				if (imme_buff_id_ != 0) {
					process_skill_effect_logic(launcher_, imme_buff_id_, p);
				} else {
					add_player_buff(p, buff_id_, 0);
				}
			}
		}

        //summmon
        if (p->my_summon && !p->my_summon->is_dead()) {
            if (launcher_->team != p->my_summon->team && buff_target_ == enemys) {
                effect_target = enemys;
            } else if (launcher_->team == p->team && buff_target_ == friends) {
                effect_target = friends;
            }

            if (effect_target) {
                if (imme_buff_id_ != 0) {
                    process_skill_effect_logic(launcher_, imme_buff_id_, p->my_summon);
                } else {
				   TRACE_LOG(" Monster FUCK FUCK player[%u] on hit buff", p->my_summon->role_type);
                   add_player_buff(p->my_summon, buff_id_, 0);
                }
            }
        }
	}

	return 0;

}
	
//--------------------------------------------------------------------------
// class SkillEffects Methods
//--------------------------------------------------------------------------
/**
  * @brief constructor
  * @param shooter the player who launched this attack
  * @param skillid id of the skill that is launched
  */
SkillEffects::SkillEffects(Player* shooter, uint32_t skillid, uint32_t type, uint32_t lv, uint32_t depend_damage_flag)
{
	launcher_ = shooter;
	skill_base_info_.id = skillid;
	skill_base_info_.type = type;
	skill_base_info_.lv = lv;
	skill_base_info_.depend_damage_active_flag = depend_damage_flag;
	map_ = launcher_->cur_map;
	
	cur_effects_idx_ = 0;
	p_skill_effects_ = 0;
	duration_ = 0;
	finish_flag_ = false;
	skill_info_ = NULL;
	last_excess_tm_ = 0;
    end_pos_.init(0, 0, 0);
}

/**
  * @brief default destructor
  */
SkillEffects::~SkillEffects()
{
	launcher_ = 0;
	map_ = 0;
	p_skill_effects_ = 0;
	skill_info_ = NULL;
}

/**
  * @brief init the SkillEffects before use
  */
void 
SkillEffects::init(struct skill_effects_t* p_skill_effects, skill_t* p_skill, uint32_t assign_x, uint32_t assign_y)
{
	TRACE_LOG("launcher [%u %u %u]",launcher_->id, launcher_->pos().x(), launcher_->pos().y());
	p_skill_effects_ = p_skill_effects;
	skill_info_ = p_skill;

	//compute the pos of the base_skill
	int dir = p_skill_effects_->dir;
	Vector3D offset_pos(p_skill_effects_->region_x/2 + p_skill_effects_->region_offset - p_skill_effects_->region_x_reverse/2,p_skill_effects_->offset_y_dir==dir_down ? p_skill_effects_->offset_y : -(p_skill_effects_->offset_y),0);
	Vector3D new_pos;
	if ( dir == launcher_front || dir == launcher_sector_front ) {//front of player
		new_pos = launcher_->pos() + (is_player_dir_right(launcher_) ? offset_pos : offset_pos.get_reverse());
		absolute_dir_ = is_player_dir_right(launcher_) ? dir_right : dir_left;
	} else if ( dir == launcher_back || dir == launcher_sector_back ){//back of player
		new_pos = launcher_->pos() - (is_player_dir_right(launcher_) ? offset_pos : offset_pos.get_reverse());
		absolute_dir_ = is_player_dir_right(launcher_)==dir_right ? dir_left : dir_right;
	} else if (dir == launcher_around){
		new_pos = launcher_->pos();
		absolute_dir_ = dir_around;
	} else {
		new_pos.set_x(assign_x); 
		new_pos.set_y(assign_y); 
		absolute_dir_ = dir_around;
	}
	set_pos(new_pos);
	set_region(p_skill_effects_->region_x, p_skill_effects_->region_y);

//	if(p_skill->id == 100506)
//	{
//		TRACE_LOG("SkillEffects::init skill_id:%u,  effect_pos:[%u,%u,%u],  launcher_pos[%u,%u,%u]",
//				p_skill->id,
//				dir,
//				new_pos.x(),
//				new_pos.y(),
//				launcher_->dir,
//				launcher_->pos().x(),
//				launcher_->pos().y()
//				);
//	}

	//init others
	uint32_t atk_duration = 0;
	if (!is_valid_uid(launcher_->id) && skill_base_info_.type == 4) {
		atk_duration = launcher_->mon_info->atk_duration;
	}
	duration_ = p_skill_effects_->duration + atk_duration;
	duration_ = ( duration_ < last_excess_tm_ ) ? 0 : duration_ - last_excess_tm_;
	last_excess_tm_ = 0;

	finish_flag_ = false;
	effection_ = true;
	last_hit_ = false;
	damage_delay_ = p_skill_effects_->damage_delay;
	next_damage_ = p_skill_effects_->persistence;
	cur_effects_idx_++;
	
	if ( (dir==target_around || dir==assign_around) && assign_x !=0 && launcher_) {
		launcher_->skill_effect_noti(p_skill_effects_->effect_id, assign_x, assign_y);
	}
	
	//如果是技能召唤物
	if( skill_base_info_.type == map_summon_skill )
	{
		map_summon_object* m = create_map_summon_object( p_skill_effects_->imme_map_summon);
	    if( m == NULL)return;
		m->set_pos( launcher_->pos(), launcher_->cur_map);
	    m->set_owner_id(launcher_->id, launcher_->team, launcher_->btl);
	    launcher_->btl->add_map_summon(m, true);
		return;
	}
}



bool SkillEffects::CanHurtPlayer(Player * p) 
{
	if( p_skill_effects_->dir == launcher_sector_front)
	{
		uint32_t dir = launcher_->dir == dir_left ? 1 : 2;
		return check_sector(p,  p_skill_effects_->radius,  p_skill_effects_->angle, dir)
		   	&& p->can_be_hurted();
	}	
	else if( p_skill_effects_->dir == launcher_sector_back)
	{
		uint32_t dir = launcher_->dir == dir_left ? 2 : 1;
		return check_sector(p,  p_skill_effects_->radius,  p_skill_effects_->angle, dir) 
			&& p->can_be_hurted();
	}
	return collision(p) && p->can_be_hurted(); 
}


bool SkillEffects::check_hit( const Player* p)
{
	if( p_skill_effects_->dir == launcher_sector_front)
	{
		uint32_t dir = launcher_->dir == dir_left ? 1 : 2;
		return check_sector(p,  p_skill_effects_->radius,  p_skill_effects_->angle, dir);
	}	
	else if( p_skill_effects_->dir == launcher_sector_back)
	{
		uint32_t dir = launcher_->dir == dir_left ? 2 : 1;
		return check_sector(p,  p_skill_effects_->radius,  p_skill_effects_->angle, dir);
	}
	return collision(p); 
}


/**
  * @brief the SkillEffects how to affect the object in the map
  */
int 
SkillEffects::affect()
{
	
	bool hit_success = false;

	TRACE_LOG("SKILL AFFECT %u %u", this->pos().x(), this->pos().y());

	for (PlayerSet::const_iterator it = map_->monsters.begin(); it != map_->monsters.end(); ) {
		Player* p = *it;
		++it;
		//友方的怪物或者友方中立怪物不能被攻击
		if (((launcher_->team != p->team) && !p->is_dead()) && (p->team != neutral_team_1)) {
			if (CanHurtPlayer(p)){
				TRACE_LOG("player[%u] on hit", p->id);
				hit_success = p->on_hit(launcher_, &skill_base_info_, skill_info_, p_skill_effects_, last_hit_);
			}
		}
		if( p->team == barrier_team && is_valid_uid(launcher_->id) && !is_summon_mon(launcher_->role_type) )//该类型打碎物只能由玩家打碎，怪物和宠物无法打碎
		{
			if (CanHurtPlayer(p)){
				//ERROR_LOG("player[%u] on hit map ooooooooooooooooooooooooid %u", 
				//		p->id);
				hit_success = p->on_hit(launcher_, &skill_base_info_, skill_info_, p_skill_effects_, last_hit_);
			}
		}
		if (p->team == barrier_team_1 && !is_valid_uid(launcher_->id) &&  !is_summon_mon( launcher_->role_type) )
			//该类型打碎物只能由怪物打碎玩家和宠物无法打碎
		{
			if (CanHurtPlayer(p)){
				TRACE_LOG("player[%u] on hit", p->id);
				hit_success = p->on_hit(launcher_, &skill_base_info_, skill_info_, p_skill_effects_, last_hit_);
			}
		}

	}

	for (PlayerSet::const_iterator it = map_->barriers.begin(); it != map_->barriers.end(); ) 
	{
		Player* p = *it;
		++it;
		if( p->is_dead()) continue;
		if( p->team == barrier_team && is_valid_uid(launcher_->id) && !is_summon_mon(launcher_->role_type) )//该类型打碎物只能由玩家打碎，怪物和宠物无法打碎
		{
			if(CanHurtPlayer(p)){
				//ERROR_LOG("player[%u] on hit map ooooooooooooooooooooooooid %u", 
				//		p->id);
				hit_success = p->on_hit(launcher_, &skill_base_info_, skill_info_, p_skill_effects_, last_hit_);
			}
		}
		if (p->team == barrier_team_1 && !is_valid_uid(launcher_->id) &&  !is_summon_mon( launcher_->role_type) )
			//该类型打碎物只能由怪物打碎玩家和宠物无法打碎
		{
			if(CanHurtPlayer(p)){
				TRACE_LOG("player[%u] on hit", p->id);
				hit_success = p->on_hit(launcher_, &skill_base_info_, skill_info_, p_skill_effects_, last_hit_);
			}
		}
	}

	for (PlayerSet::const_iterator it = map_->players.begin(); it != map_->players.end(); ) {
		Player* p = *it;
		++it;
		if ((launcher_->team != p->team) && !p->is_dead() && launcher_->team != neutral_team_1) {
			if(CanHurtPlayer(p)){
				if (p->is_invincibility()) {
//					ERROR_LOG("Player %u  is INVINCIBILITY", p->id);
				}
				TRACE_LOG("player[%u] on hit", p->id);
				hit_success = p->on_hit(launcher_, &skill_base_info_, skill_info_, p_skill_effects_, last_hit_);
			}
		} 

        //summmon
        if (p->my_summon && !p->my_summon->is_dead() && launcher_->team != p->my_summon->team && launcher_->team != neutral_team_1) {
            if(CanHurtPlayer(p->my_summon)){
                TRACE_LOG(" summon player[%u] on hit buff", p->my_summon->role_type);
                hit_success = p->my_summon->on_hit(launcher_, &skill_base_info_, skill_info_, p_skill_effects_, last_hit_);
            }
        }

	}

	if (p_skill_effects_->disrupte == 1 && hit_success) {
		finish_flag_ = true;
	}
	
	return 0;
}


/**
  * @brief get the damage of the effect to the preyer
  */
uint32_t 
SkillEffects::get_damage(const Player* preyer)
{
    int can_hit_flag = 0;
	//if the effect can move, compute the hole damage region
	if ( p_skill_effects_->speed != 0 ) {
		int dir = p_skill_effects_->dir;
		uint32_t count = duration_ / next_damage_;
		uint32_t offset_x = p_skill_effects_->speed *  count;
		//uint32_t offset_x = p_skill_effects_->speed * ( count + 1 );
        if ( p_skill_effects_->offset_y != 0 ) {
            for (uint32_t i = 0; i < count; i ++) {
                int new_x_pos = calc_x_pos(p_skill_effects_->offset_angle,p_skill_effects_->speed);
                int new_y_pos = calc_y_pos(p_skill_effects_->offset_angle,p_skill_effects_->speed);
                Vector3D offset_pos(new_x_pos, p_skill_effects_->offset_y_dir == dir_down ? new_y_pos : - new_y_pos, 0);
                Vector3D new_pos = launcher_->pos() + (launcher_->dir==dir_right ? offset_pos : offset_pos.get_reverse());
                set_pos(new_pos);
                set_region(p_skill_effects_->region_x, p_skill_effects_->region_y);
                if ( check_hit(preyer) ) {
                    can_hit_flag = 1;
                    break;
                }
            }
        } else {
            Vector3D offset_pos(offset_x/2, 0, 0);
            Vector3D new_pos;
            if ( dir == launcher_front || dir == launcher_sector_front ) {//front of player
                new_pos = launcher_->pos() + (launcher_->dir==dir_right ? offset_pos : offset_pos.get_reverse());
            } else if ( dir == launcher_back || launcher_sector_back ){//back of player
                new_pos = launcher_->pos() - (launcher_->dir==dir_right ? offset_pos : offset_pos.get_reverse());
            } else if (dir == launcher_around ){
                new_pos = launcher_->pos();
            } else {
                return 0;
            }
            set_pos(new_pos);
            set_region(offset_x, p_skill_effects_->region_y);
        }
	}

	if ( !(check_hit(preyer) || can_hit_flag)) {
		return 0;
	}

	//compute damage
	uint32_t dmgs = 0;
	uint32_t base_dmgs = p_skill_effects_->base_damage[0] * launcher_->atk / 100
		        + p_skill_effects_->base_damage[1] * launcher_->weapon_atk[0] / 100;
	for (uint32_t i = 0; i < 5; i++) {
		base_dmgs += p_skill_effects_->elem_damage[i];
	}
	dmgs = base_dmgs * preyer->def_rate;
	
	return dmgs;
}
	
/**
  * @brief get the region of effect
  */
attack_region_t
SkillEffects::get_atk_region()
{
	attack_region_t rgn;

	if ( p_skill_effects_->dir == target_around || p_skill_effects_->dir == assign_around ) {
		return rgn;
	}
	if ( p_skill_effects_->speed != 0 ) {
		//int dir = p_skill_effects_->dir;
		uint32_t count = duration_ / next_damage_;
		rgn.x = p_skill_effects_->region_x * (count + 1);
	} else {
		rgn.x = p_skill_effects_->region_x;
	}
	rgn.y = p_skill_effects_->region_y;

	return rgn;
}

void SkillEffects::move_effect_to_next_pos()
{
	int new_x_pos = calc_x_pos(p_skill_effects_->offset_angle,p_skill_effects_->speed);
	int new_y_pos = calc_y_pos(p_skill_effects_->offset_angle,p_skill_effects_->speed);
	Vector3D offset_pos(new_x_pos, p_skill_effects_->offset_y_dir == dir_down ? new_y_pos : - new_y_pos, 0);
	Vector3D new_pos = pos() + (absolute_dir_==dir_right ? offset_pos : offset_pos.get_reverse());
	//if (skill_info_->lv_info[lv].body_move_x)
	if (end_pos_.x() != 0) {
		if (absolute_dir_== dir_right && new_pos.x() >= end_pos_.x()) {
			new_pos.set_x(end_pos_.x() - ( p_skill_effects_->region_x) / 2);
			finish_flag_ = true;
		} else if (absolute_dir_== dir_left && new_pos.x() <= end_pos_.x()) {
			new_pos.set_x(end_pos_.x() + ( p_skill_effects_->region_x) / 2);
			finish_flag_ = true;
		}
	}
	set_pos(new_pos);
}

/**
  * @brief update attribute of this object
  */
void 
SkillEffects::update(int time_elapsed)
{
	if ( !finish_flag_ ) {
		TRACE_LOG("player[%u] launch a base skill[%u %u],time_elapsed[%u],left_time[%u]",
				launcher_->id, skill_base_info_.id, p_skill_effects_->base_skill_id, time_elapsed, duration_);
		//if have damage delay
		if (damage_delay_ != 0) {
			if (damage_delay_ < negligible_time) {
				damage_delay_ = 0;
			} else {
				damage_delay_ = (damage_delay_ <= (uint32_t)time_elapsed) ? 0 : damage_delay_ - (uint32_t)time_elapsed;
			}
			effection_ = damage_delay_ == 0 ? true : false;
		}

		//if the skill have persistence, set effection_ every intervals
		if ( p_skill_effects_->persistence > 0 && !effection_ && !damage_delay_) {
			next_damage_ = (next_damage_ < (uint32_t)time_elapsed) ? 0 : next_damage_ - (uint32_t)time_elapsed;
			if ( next_damage_ < negligible_time ) {
				effection_ = true;
				next_damage_ = p_skill_effects_->persistence;
				if (duration_ < next_damage_) {
					last_hit_ = true;
				}
				//if the skill can move
				if ( p_skill_effects_->speed > 0 ) {
					//set new pos
					//Vector3D offset_pos(p_skill_effects_->speed, 0, 0);
					move_effect_to_next_pos();
                }
			}
		} else if ( p_skill_effects_->persistence == 0 ) {
			last_hit_ = true;
		}

		//if effection_ is true , calc the damage
		if (effection_) {
			affect();
			effection_ = false;
		}

		//calc the left time of the skill effect
		uint32_t tm_elapsed = (uint32_t)time_elapsed;

		//calc the surplus duraion_
		if ( duration_ < tm_elapsed ) {
			last_excess_tm_ = tm_elapsed - duration_;
			duration_ = 0;
			TRACE_LOG("last_excess_tm [%u]",last_excess_tm_);
		} else {
			duration_ = duration_ - tm_elapsed;
		}

		if ( duration_ <= negligible_time ) {
			finish_flag_ = true;
		}

		if (p_skill_effects_->buff_id && p_skill_effects_->buff_target == oneself && launcher_) {
			add_player_buff(launcher_, p_skill_effects_->buff_id, 0);
		}
	}
}


//--------------------------------------------------------------------------
// class Skill Methods
//--------------------------------------------------------------------------
/**
  * @brief constructor
  * @param shooter the player who launched this attack
  * @param skillid id of the skill that is launched
  */
Skill::Skill(Player* shooter, uint32_t skill_id, uint32_t lv, uint32_t depend_damage_active_flag, uint32_t assign_x, uint32_t assign_y) : MovingObject(shooter->pos())
{
	skill_effect_index_ = 0;
	launcher_ = shooter;
	map_ = shooter->cur_map;
	skill_id_ = skill_id;
	lv_ = lv;

//	p_buf_effect_ = NULL;
//	p_skill_effects_ = NULL;
//
	assign_pos_.set_x(assign_x);
	assign_pos_.set_y(assign_y);

	skill_info_ = g_skills->get_skill(skill_id_);
	TRACE_LOG("skill info[%u %u %u]", launcher_->id, skill_info_->id, skill_info_->type);

	create_skill_effects(depend_damage_active_flag);

	start_duration_ = skill_info_->lv_info[lv_].start_duration;
	action_duration_ = skill_info_->lv_info[lv_].action_duration;
	end_duration_ = skill_info_->lv_info[lv_].end_duration;
	action_stage_ = start_stage;
	effect_stage_ = start_stage;

	activation_ = false;
	break_info_.cur_step = 0;
	break_info_.step_type = 0;
	break_info_.step_duration = 0;

	if (skill_info_->unbreak_info[0].step_type) {
		break_info_.cur_step = break_step_1;
		break_info_.step_type = skill_info_->unbreak_info[0].step_type;
		break_info_.step_duration = skill_info_->unbreak_info[0].step_duration;
	}

	if (skill_info_->lv_info[lv_].body_move_x > 0) {
		calc_player_moveable_pos(shooter);
	}
}

/**
  * @brief default destructor
  */
Skill::~Skill()
{
	launcher_ = 0;
	map_ = 0;
	skill_info_ = 0;

	if (p_skill_effects_) {
		delete p_skill_effects_;
		p_skill_effects_ = 0;
	}
	
	if (p_buf_effect_) {
		delete p_buf_effect_;
		p_buf_effect_ = 0;
	}
	
	for( uint32_t i = 0; i< skill_effect_arrays_.size(); i++)
	{
		SkillEffects* pEffect = skill_effect_arrays_[i];
		if( pEffect != NULL){
			delete pEffect;
			pEffect = NULL;
		}
	}

	TRACE_LOG("~Skill %u", skill_id_);
}

void Skill::create_skill_effects(uint32_t depend_damage_active_flag)
{
	if (skill_info_->is_atk_skill() || skill_info_->is_map_summon_skill()) 
	{
		p_skill_effects_ = new SkillEffects(launcher_, skill_id_, skill_info_->type, lv_, depend_damage_active_flag);
		p_buf_effect_ = 0;
	} 
	else if(  skill_info_->is_continue_atk_skill() || skill_info_->is_mult_continue_atk_skill() )
	{
		for(uint32_t i = 0; i < skill_info_->lv_info[lv_].effects_num; i++)
		{
			SkillEffects *pSK = new SkillEffects(launcher_, skill_id_, skill_info_->type, lv_, depend_damage_active_flag);
			skill_effect_arrays_.push_back(pSK);
		}
		p_buf_effect_ = NULL;
		p_skill_effects_ = NULL;
	}
	else
	{
		p_buf_effect_ = new BufEffect(launcher_, skill_id_, skill_info_->type, lv_);
		p_skill_effects_ = NULL;
	}

}

void Skill::calc_player_moveable_pos(Player * shooter)
{
	KfAstar::Point pt_start(launcher_->pos().x(), launcher_->pos().y());
	Vector3D offset_pos(skill_info_->lv_info[lv_].body_move_x, 0, 0);
	if ( skill_info_->lv_info[lv_].body_move_dir == 1 ) {//front of player
		launcher_move_pos = launcher_->pos() + (launcher_->dir==dir_right ? offset_pos : offset_pos.get_reverse());
		TRACE_LOG("launcher_move_pos x = %u, y= %u, line = %u", launcher_move_pos.x(), launcher_move_pos.y(), __LINE__);
	} else if ( skill_info_->lv_info[lv_].body_move_dir == 2 ) {//back of player
		launcher_move_pos = launcher_->pos() - (launcher_->dir==dir_right ? offset_pos : offset_pos.get_reverse());
		TRACE_LOG("launcher_move_pos x = %u, y= %u, line = %u", launcher_move_pos.x(), launcher_move_pos.y(), __LINE__);
	}

	uint16_t x = launcher_move_pos.x() < 0 ? 0 : launcher_move_pos.x();
	KfAstar::Point pt_end(x, launcher_move_pos.y());
	TRACE_LOG("move pos[%u %u]", launcher_move_pos.x(), launcher_move_pos.y());
	//const KfAstar::Points* pt = launcher_->cur_map->path->find_linear_path(pt_start, pt_end);
	KfAstar::Points* pt = NULL;
	KfAstar* p_star = shooter->btl->get_cur_map_path( shooter->cur_map );
	if(p_star)
	{
		pt = (KfAstar::Points*)p_star->find_linear_path(pt_start, pt_end);
		launcher_move_pos.set_x((*pt)[0].x);
		p_skill_effects_->set_end_pos((*pt)[0].x, 0, 0);
	}
}

//hit_type ： 眩晕 3
//         ： 击飞，倒地为 2
//         ： 减速为1
bool Skill::judge_can_be_break(uint32_t hit_type)
{
	if (hit_type <= break_info_.step_type && break_info_.step_duration) {
		return false;
	}
	return true;
}

void Skill::update_skill_break_info(int time_elapsed)
{
	if (!break_info_.step_duration) {
		return;
	}

	if ((int)break_info_.step_duration > time_elapsed) {
		break_info_.step_duration -= time_elapsed;
	} else {
		break_info_.step_duration = 0;
	}

	if (break_info_.cur_step == break_step_1 && break_info_.step_duration < 50) {
		if (skill_info_->unbreak_info[1].step_type) {
			break_info_.step_type = skill_info_->unbreak_info[1].step_type;
			break_info_.step_duration = skill_info_->unbreak_info[1].step_duration;
			break_info_.cur_step = break_step_2;
		}
	}

	if (break_info_.step_duration < 50) {
		break_info_.step_duration = 0;
		break_info_.step_type = 0;
	}
}

void Skill::skill_start_action(int time_elapsed)
{
	TRACE_LOG("skillupdate in start_stage %u %u pos[%u,%u]", get_skill_id(), action_duration_, launcher_->pos().x(), launcher_->pos().y());
	//set boss invincible at once when skill start
	if (launcher_ && !is_valid_uid(launcher_->id)
			&& (launcher_->mon_info->type == 1)
			&& skill_info_->type != normal_skill) {
		launcher_->invincible = true;
	}

	start_duration_ = ( start_duration_ < (uint32_t)time_elapsed ) ? 0 : start_duration_ - (uint32_t)time_elapsed;
	if ( start_duration_ < negligible_time ) {
		uint32_t real_skill_cd_time = launcher_->get_skill_cool_time(skill_id_);
		//launcher_->set_skill_cool_time(skill_id_, skill_info_->lv_info[lv_].cool_down);
		launcher_->set_skill_cool_time(skill_id_, real_skill_cd_time);
		if ( skill_info_->lv_info[lv_].effects_num == 0 /*&& !p_buf_effect_*/) {
			action_stage_ = end_stage;
			effect_stage_ = finish_stage;
		} else {
			action_stage_ = attack_stage;
			effect_stage_ = attack_stage;
			//the player invincible
//			if ( skill_info_->type != normal_skill) {
//				launcher_->invincible = true;
//			}
			init_skill_attack_effects();
		}
	}
}

void Skill::init_skill_attack_effects()
{
	if (p_skill_effects_) {
		p_skill_effects_->init(&(skill_info_->lv_info[lv_].effects[0]), skill_info_, assign_pos_.x(), assign_pos_.y());
	}else if( get_skill_type() == continue_atk_skill){
		if (skill_effect_index_ < skill_effect_arrays_.size()){
			SkillEffects* pSE = skill_effect_arrays_[  skill_effect_index_ ];
			pSE->init(&(skill_info_->lv_info[lv_].effects[ skill_effect_index_]), skill_info_,  assign_pos_.x(), assign_pos_.y() );
			skill_effect_index_ ++;
		}
	} else if ( get_skill_type() == mult_continue_atk_skill) {
		for (uint32_t i = 0; i < skill_effect_arrays_.size(); i++) {
			SkillEffects* pSE = skill_effect_arrays_[i];
			pSE->init( &(skill_info_->lv_info[lv_].effects[i]), skill_info_, assign_pos_.x(),assign_pos_.y() );
		}
	}else if (p_buf_effect_){
		//init buff effect
		if (skill_info_->lv_info[lv_].effects[0].buff_id != 0 || 
				skill_info_->lv_info[lv_].effects[0].imme_buff != 0) {
			uint32_t buff_id = skill_info_->lv_info[lv_].effects[0].buff_id;
			uint32_t buff_target = skill_info_->lv_info[lv_].effects[0].buff_target;
			uint32_t imme_buff = skill_info_->lv_info[lv_].effects[0].imme_buff;
			p_buf_effect_->init(buff_id, buff_target, imme_buff);
		}
	}
}

void Skill::skill_attack_action(int time_elapsed)
{
	TRACE_LOG("skillupdate in attack_stage %u %u pos[%u %u]", get_skill_id(), action_duration_, launcher_->pos().x(), launcher_->pos().y());
	action_duration_ = ( action_duration_ < (uint32_t)time_elapsed ) ? 0 : action_duration_ - (uint32_t)time_elapsed;
	if ( action_duration_ < negligible_time ) {
		action_stage_ = end_stage;
		launcher_->invincible = false;
		if (skill_info_->lv_info[lv_].body_move_x > 0) {
			TRACE_LOG("launcher_move_pos x = %u, y= %u, ad=%u line = %u", launcher_move_pos.x(), launcher_move_pos.y(), action_duration_, __LINE__);
			launcher_->set_pos(launcher_move_pos);
		}

		if ( end_duration_ == 0 ) {
			action_stage_ = finish_stage;
			if( !(is_valid_uid(launcher_->id) && get_skill_type() == normal_skill ))
			{
				launcher_->p_cur_skill = 0;
				TRACE_LOG("skillupdate in start_stage1 %u %u", launcher_->id, get_skill_id());
			}
			TRACE_LOG("action finish");
		}
	}

//	skill_effect_update(time_elapsed);
}

void Skill::skill_end_action(int time_elapsed)
{
	TRACE_LOG("skillupdate in end_stage %u pos[%u %u]", get_skill_id(), launcher_->pos().x(), launcher_->pos().y());
	end_duration_ = ( end_duration_ < (uint32_t)time_elapsed ) ? 0 : end_duration_ - (uint32_t)time_elapsed;
	if ( end_duration_ < negligible_time ) {
		action_stage_ = finish_stage;
		if ( !(is_valid_uid(launcher_->id) && get_skill_type() == normal_skill ))
		{
			launcher_->p_cur_skill = 0;

		}
		TRACE_LOG("action finish");
	}
//	if (action_stage_ == finish_stage && effect_stage_ == finish_stage) {
//		skill_effect_over_action(time_elapsed);
//	}
}

void Skill::skill_buff_effect_update()
{
	p_buf_effect_->affect();
	uint32_t effects_idx = p_buf_effect_->cur_effects_idx();
	if ( effects_idx >= skill_info_->lv_info[lv_].effects_num ) {
		effect_stage_ = finish_stage;
	} 
	else 
	{
		uint32_t buff_id = skill_info_->lv_info[lv_].effects[effects_idx].buff_id;
		uint32_t buff_target = skill_info_->lv_info[lv_].effects[effects_idx].buff_target;
		uint32_t imme_buff = skill_info_->lv_info[lv_].effects[effects_idx].imme_buff;
		p_buf_effect_->init(buff_id, buff_target, imme_buff);
	}

}

void Skill::continue_skill_effect_update(int time_elapsed)
{
	for( uint32_t i =0; i< skill_effect_index_; i++)
	{
		SkillEffects* pSE = skill_effect_arrays_[ i ];
		if (pSE == NULL)continue;
		pSE->update(time_elapsed);
	}
	if( skill_effect_index_ == skill_info_->lv_info[lv_].effects_num)
	{
		int end_flag = 0;
		for( uint32_t i = 0;  i < skill_effect_arrays_.size(); i++)
		{
			SkillEffects* pSE = skill_effect_arrays_[ i ];
			if(pSE == NULL) continue;
			if( !pSE->is_finished() ){
				end_flag += 1;
			}
		}
		if(!end_flag){
			effect_stage_ = finish_stage;
		}
	}
	if (skill_effect_index_ < skill_effect_arrays_.size() )
	{
		SkillEffects* pSE = skill_effect_arrays_[ skill_effect_index_ ];
		pSE->init(&(skill_info_->lv_info[lv_].effects[ skill_effect_index_]), skill_info_,  assign_pos_.x(), assign_pos_.y() );
		skill_effect_index_ ++;
	}

}

void Skill::mult_continue_skill_effect_update(int time_elapsed)
{
	for (uint32_t i = 0; i < skill_info_->lv_info[lv_].effects_num; i++) {
		SkillEffects* pSE = skill_effect_arrays_[i];
		if (pSE == NULL) continue;
		pSE->update(time_elapsed);
	}
	int end_flag = 0;
	for (uint32_t i = 0; i < skill_info_->lv_info[lv_].effects_num; i++) {
		SkillEffects* pSE = skill_effect_arrays_[i];
		if (pSE == NULL) continue;
		if ( !pSE->is_finished() ) {
			end_flag += 1;
		}
	}
	if (!end_flag) {
		effect_stage_ = finish_stage;
	}

}

void Skill::normal_skill_effect_update(int time_elapsed)
{
	if (p_skill_effects_) {
		p_skill_effects_->update(time_elapsed);
		if ( p_skill_effects_->is_finished() ) {
			uint32_t effects_idx = p_skill_effects_->cur_effects_idx();
			if ( effects_idx == skill_info_->lv_info[lv_].effects_num ) 
			{
				effect_stage_ = finish_stage;
			} 
			else 
			{
				p_skill_effects_->init(&(skill_info_->lv_info[lv_].effects[effects_idx]), skill_info_, assign_pos_.x(), assign_pos_.y());
			}

		}
	}
}

void Skill::skill_effect_update(int time_elapsed)
{
	if (p_buf_effect_) 
	{
		skill_buff_effect_update();	
	}

	//////////////////////////////////////////////////////////////////////////
	//attack skill
	if (get_skill_type() == continue_atk_skill)
	{
		continue_skill_effect_update(time_elapsed);
	} else if (get_skill_type() == mult_continue_atk_skill) {
		mult_continue_skill_effect_update(time_elapsed);
	}
	else
	{
		normal_skill_effect_update(time_elapsed);
	}
}

void Skill::skill_effect_over_action(int time_elapsed)
{
	TRACE_LOG("skillupdate in finish_stage %u pos[%u %u]", get_skill_id(), launcher_->pos().x(), launcher_->pos().y());
	launcher_->invincible = false;
	launcher_->on_skill_end(this);
	set_dead();

	int tmp_dis = 45;
	if (get_skill_id() == 200207) {
		launcher_->call_map_summon(1, launcher_->pos().x(), launcher_->pos().y() + tmp_dis);
		launcher_->call_map_summon(1, launcher_->pos().x() > tmp_dis ? launcher_->pos().x() - tmp_dis : 0, 
				launcher_->pos().y() > tmp_dis ? launcher_->pos().y() - tmp_dis : 0);
		launcher_->call_map_summon(1, launcher_->pos().x() + tmp_dis, 
				launcher_->pos().y() > tmp_dis ? launcher_->pos().y() - tmp_dis : 0);
	}
}

/**
  * @brief update attribute of this object
  */
void 
Skill::update(int time_elapsed)
{
	update_skill_break_info(time_elapsed);	
	if (action_stage_ != finish_stage) {
		if (action_stage_ == start_stage) {
			skill_start_action(time_elapsed);
		} else if (action_stage_ == attack_stage) {
			skill_attack_action(time_elapsed);	
		} else if (action_stage_ == end_stage) {
			skill_end_action(time_elapsed);	
		}
	}

	if (effect_stage_ == attack_stage) {
		skill_effect_update(time_elapsed);
		return;
	}

	if (action_stage_ == finish_stage && effect_stage_ == finish_stage) {
		skill_effect_over_action(time_elapsed);
	}

}

//--------------------------------------------------------------------------
// class GobalSkill Methods
//--------------------------------------------------------------------------
GobalSkill::GobalSkill(const char* filename):filename_(filename)
{
	
}

GobalSkill::~GobalSkill()
{
	std::map<uint32_t,struct base_skill_t*>::iterator it1;
	for ( it1 = base_skills_map_.begin(); it1 !=  base_skills_map_.end(); ++it1) {
		free(it1->second);
	}
	base_skills_map_.clear();
	
	std::map<uint32_t,struct skill_t*>::iterator it2;
	for ( it2 = skills_map_.begin(); it2 !=  skills_map_.end(); ++it2 ) {
		free(it2->second);
	}
	skills_map_.clear();
}

base_skill_t*
GobalSkill::get_base_skill(uint32_t skill_id)
{
	std::map<uint32_t,struct base_skill_t*>::iterator it = base_skills_map_.find(skill_id);
	if ( it != base_skills_map_.end() ) {
		return it->second;
	}
	
	return 0;
}

skill_t*
GobalSkill::get_skill(uint32_t skill_id)
{
	std::map<uint32_t,struct skill_t*>::iterator it = skills_map_.find(skill_id);
	if ( it != skills_map_.end() ) {
		return it->second;
	}
	
	return 0;
}

bool is_skill_can_be_silent(uint32_t skill_id)
{
    skill_t* p_skill = g_skills->get_skill(skill_id);
    if (p_skill && p_skill->nature == 1) {
        return false;
    }
    return true;
}

buff_effect_t*
GobalSkill::get_buff_effect(uint32_t buff_id, uint32_t buff_lv)
{
	buff_lv = buff_lv == 0 ? 1 : buff_lv;
	if (buff_id > max_buff_effects || buff_id == 0 || 
		buff_lv > max_skill_lv || buff_lv == 0 || 
		buff_effects[buff_id - 1][buff_lv - 1].id == 0) {
		WARN_LOG("buff id error[%u %u]", buff_id, buff_lv);
		return 0;
	}

	return &(buff_effects[buff_id - 1][buff_lv - 1]);
}

void init_skill_begin_handle()
{
	skill_begin_handle_vec.clear();
	regedit_skill_begin_handle(skill_begin_handle_suck_hp);
	regedit_skill_begin_handle(skill_begin_handle_suck_mp);
	regedit_skill_begin_handle(skill_begin_handle_suck_hpmp);
	regedit_skill_begin_handle(skill_begin_handle_summon_monster);
}

int
GobalSkill::load_skills()
{
	init_skill_begin_handle();

	xmlDocPtr doc = xmlParseFile(filename_.c_str());
	if (!doc) {
		throw XmlParseError(std::string("failed to parse item file '") + filename_ + "'");
		ERROR_LOG("failed to parse item file!");
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (!cur) {
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("xmlDocGetRootElement error when loading item file '") + filename_ + "'");
		ERROR_LOG("xmlDocGetRootElement error when loading item file!");
	}
	
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("BaseSkill"))) {
			struct base_skill_t* p_base_skill = (struct base_skill_t*)malloc(sizeof(struct base_skill_t));
			memset(p_base_skill, 0x00, sizeof(struct base_skill_t));
			get_xml_prop(p_base_skill->id, cur, "ID");
			get_xml_prop_def(p_base_skill->duration, cur, "Duration", 0);
			
			
			base_skills_map_.insert(std::map<uint32_t,struct base_skill_t*>::value_type(p_base_skill->id, p_base_skill));
			
		} else if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Skill"))) {
			struct skill_t* p_skill = reinterpret_cast<struct skill_t*>(malloc(sizeof(struct skill_t)));
			memset(p_skill, 0x00, sizeof(struct skill_t));
			get_xml_prop(p_skill->id, cur, "ID");

			get_xml_prop(p_skill->type, cur, "Type");
			get_xml_prop_def(p_skill->ai_control_flag, cur, "AIControl", 0);
			get_xml_prop_def(p_skill->nature, cur, "Nature", 0);

			get_xml_prop_def(p_skill->auto_config, cur, "Auto_config", 0);
			get_xml_prop_def(p_skill->role_type, cur, "Role_type", 0);
			//get_xml_prop_def(p_skill->base_calc_percent, cur, "Base_percent", 0);
			get_xml_prop_def(p_skill->step_calc_percent, cur, "Step_percent", 0);

			get_xml_prop_def(p_skill->add_type, cur, "Add_type", 0);
			get_xml_prop_def(p_skill->add_start, cur, "Add_start", 0);
			get_xml_prop_def(p_skill->add_step, cur, "Add_step", 0);
			get_xml_prop_def(p_skill->base_mp_consume, cur, "BaseMpPer", 0);
			get_xml_prop_def(p_skill->mp_consume, cur, "MpConsumed", 0);

			char skill_begin_handle_name[skill_begin_handle_name_lenth] = "";
			get_xml_prop_raw_str_def(skill_begin_handle_name, cur, "BeginHandle", "");
			if (skill_begin_handle_name[0] != 0) {
				regedit_skill_begin_handle_to_skill(skill_begin_handle_name);
			}
			get_xml_prop_arr_def(p_skill->skill_begin_handle_agrv, cur, "BeginHandleArgv", 0);
			//for uninterrupt step
			get_xml_prop_def(p_skill->unbreak_info[0].step_type, cur, "UnInterruptStep1", 0);
			get_xml_prop_def(p_skill->unbreak_info[0].step_duration, cur, "Step1Duration",  0);
			if (p_skill->unbreak_info[0].step_type && !p_skill->unbreak_info[0].step_duration) {
				ERROR_LOG("LOAD SKILL %u ERROR On UnIterrupt STEP One", p_skill->id);
				return -1;
			}

			get_xml_prop_def(p_skill->unbreak_info[1].step_type, cur, "UnInterruptStep2", 0);
			get_xml_prop_def(p_skill->unbreak_info[1].step_duration, cur, "Step2Duration", 0);

			if (p_skill->unbreak_info[1].step_type && 
					(!p_skill->unbreak_info[1].step_duration 
					 	|| !p_skill->unbreak_info[0].step_type)) {

				ERROR_LOG("LOAD SKILL %u ERROR On UnIterrupt Step Two ", p_skill->id);
				return -1;
			}



			// load configs for each Skill
			if (load_an_skill(cur->xmlChildrenNode, p_skill) == -1) {
				throw XmlParseError(std::string("failed to load an skill"));
				return -1;
			}
			
			skills_map_.insert(std::map<uint32_t, struct skill_t*>::value_type(p_skill->id, p_skill));
		}
		
		cur = cur->next;
	}
	
	TRACE_LOG("============= Start Dumping base_skills =============");
	std::map<uint32_t,struct base_skill_t*>::iterator it1;
	for ( it1 = base_skills_map_.begin(); it1 !=  base_skills_map_.end(); ++it1) {
		base_skill_t* p_base_skill = it1->second;
		TRACE_LOG("base_skill id,duration: %u,%u", p_base_skill->id, p_base_skill->duration);
	}
	TRACE_LOG("============= End Dumping base_skills =============\n");
	TRACE_LOG("============= Start Dumping skills =============");
	std::map<uint32_t,struct skill_t*>::iterator it2;
	for ( it2 = skills_map_.begin(); it2 !=  skills_map_.end(); ++it2) {
		skill_t* p_skill = it2->second;
		TRACE_LOG("skill id: %u", p_skill->id);
		for ( uint32_t i = 0; i <= max_skill_lv; i++ ) {
			TRACE_LOG("--skill lv[%u] : [%u %u %u %u %u %u %u  %u]", i, p_skill->lv_info[i].lv, p_skill->lv_info[i].use_lv,
					p_skill->lv_info[i].stars, p_skill->lv_info[i].hp_consumed, 
			   		p_skill->lv_info[i].cool_down, p_skill->lv_info[i].start_duration, 
					p_skill->lv_info[i].action_duration, p_skill->lv_info[i].end_duration);
			
			for ( uint32_t j = 0; j < p_skill->lv_info[i].effects_num; j++ ) {
				skill_effects_t* p_step = &(p_skill->lv_info[i].effects[j]);
				TRACE_LOG("--skill effects[%u] [%u %u %u]", j, p_step->base_skill_id, p_step->pre_action, p_step->duration);
				TRACE_LOG("--skill effects[%u] [%u %u %u %u]", j, p_step->dir, p_step->region_x, p_step->region_y, p_step->height);
				TRACE_LOG("--skill effects[%u] [%u %u %u %u]", j, p_step->speed,p_step->offset_angle,p_step->disrupte, p_step->persistence);
				TRACE_LOG("--skill effects[%u] [%u %u %u %u %u %u %u %u]", j, p_step->base_damage[0], p_step->base_damage[1],
					   	p_step->elem_damage[0], p_step->elem_damage[1], p_step->elem_damage[2], p_step->elem_damage[2],
						p_step->elem_damage[3], p_step->elem_damage[4]);
			}
		}
	}
	TRACE_LOG("============= End Dumping base_skills =============\n");
	
	xmlFreeDoc(doc);
	return 0;
}

int
GobalSkill::load_an_skill(xmlNodePtr cur, struct skill_t* p_skill)
{
	uint32_t lv;

	if(p_skill->auto_config)//自动生成
	{
		while (cur)
		{
			if( !xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Influence")))
			{
				get_xml_prop_def(p_skill->depend_info.skill_id, cur, "Skill_id", 0);
				get_xml_prop_def(p_skill->depend_info.numberic_damage, cur, "Percent_damage", 0);
				get_xml_prop_def(p_skill->depend_info.percent_damage, cur, "Numberic_damage", 0);
			}
			
			if( !xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Level")))
			{
				get_xml_prop(lv, cur, "Lv");
				if ( lv != 1)
				{
					ERROR_LOG("lv[%u] you need config lv = 1 skill or you will create auto config skill data fail",lv);
					return -1;
				}
				TRACE_LOG("ttttt %u, %u", p_skill->id, lv);
				get_xml_prop_def(p_skill->lv_info[lv].use_lv, cur, "UseLv", 0);
				get_xml_prop_def(p_skill->lv_info[lv].stars, cur, "Stars", 0);
				get_xml_prop_def(p_skill->lv_info[lv].hp_consumed, cur, "HpConsumed", 0);
				//get_xml_prop_def(p_skill->lv_info[lv].mp_consumed, cur, "MpConsumed", 0);
				get_xml_prop_def(p_skill->lv_info[lv].cool_down, cur, "CoolDown", 0);
				get_xml_prop_def(p_skill->lv_info[lv].start_duration, cur, "StartDuration", 0);
				get_xml_prop_def(p_skill->lv_info[lv].end_duration, cur, "EndDuration", 0);
				get_xml_prop_def(p_skill->lv_info[lv].body_move_dir, cur, "BodyMoveDir", 1);
				get_xml_prop_def(p_skill->lv_info[lv].body_move_x, cur, "BodyMoveX", 0);
			
				if (load_an_effects(cur->xmlChildrenNode, &(p_skill->lv_info[lv])) == -1)return -1;


				break;
			}
			cur = cur->next;
		}	
		
		for( uint32_t i = 2; i <  max_skill_lv; i++)
		{
			p_skill->lv_info[i].lv          =    i;
			p_skill->lv_info[i].use_lv 		= 	 p_skill->lv_info[ i -1 ].use_lv;
			p_skill->lv_info[i].stars       =    p_skill->lv_info[ i -1 ].stars;
			p_skill->lv_info[i].hp_consumed =    p_skill->lv_info[ i -1 ].hp_consumed;
			//p_skill->lv_info[i].mp_consumed =    p_skill->lv_info[ i -1 ].mp_consumed;
			p_skill->lv_info[i].cool_down   =    p_skill->lv_info[ i -1 ].cool_down;
			p_skill->lv_info[i].start_duration = p_skill->lv_info[ i -1 ].start_duration;
			p_skill->lv_info[i].end_duration   = p_skill->lv_info[ i -1 ].end_duration;
			p_skill->lv_info[i].body_move_dir  = p_skill->lv_info[ i -1 ].body_move_dir;
			p_skill->lv_info[i].body_move_x	   = p_skill->lv_info[ i -1 ].body_move_x;
			p_skill->lv_info[i].action_duration = p_skill->lv_info[ i -1 ].action_duration;
			TRACE_LOG("ttttt %u, %u %u", p_skill->id, p_skill->lv_info[i].action_duration ,p_skill->lv_info[ 1 ].action_duration);
			if (load_an_effects(cur->xmlChildrenNode, &(p_skill->lv_info[i]), p_skill) == -1)return -1;
		}


	}
	else
	{	
		while (cur) 
		{
			if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Level"))) 
			{
				get_xml_prop(lv, cur, "Lv");
				if ( lv > max_skill_lv) 
				{
					ERROR_LOG("lv[%u] more than MAX_SKILL_LV",lv);
					return -1;	
				}		
				p_skill->lv_info[lv].lv = lv;
				get_xml_prop_def(p_skill->lv_info[lv].use_lv, cur, "UseLv", 0);
				get_xml_prop_def(p_skill->lv_info[lv].stars, cur, "Stars", 0);
				get_xml_prop_def(p_skill->lv_info[lv].hp_consumed, cur, "HpConsumed", 0);
				//get_xml_prop_def(p_skill->lv_info[lv].mp_consumed, cur, "MpConsumed", 0);
				get_xml_prop_def(p_skill->lv_info[lv].cool_down, cur, "CoolDown", 0);
				get_xml_prop_def(p_skill->lv_info[lv].start_duration, cur, "StartDuration", 0);
				get_xml_prop_def(p_skill->lv_info[lv].end_duration, cur, "EndDuration", 0);
				get_xml_prop_def(p_skill->lv_info[lv].body_move_dir, cur, "BodyMoveDir", 0);
				get_xml_prop_def(p_skill->lv_info[lv].body_move_x, cur, "BodyMoveX", 0);
				TRACE_LOG("%u", p_skill->id);
				if (load_an_effects(cur->xmlChildrenNode, &(p_skill->lv_info[lv])) == -1) 
				{
					return -1;
				}
			}
			cur = cur->next;
		}	
	}
	return 0;
}

int
GobalSkill::load_buf_effect(xmlNodePtr cur, struct skill_lv_info_t* lv_info)
{
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Effect"))) {		
			lv_info->action_duration = 300;
		}
		cur = cur->next;
	}

	return 0;
}


int GobalSkill::load_an_effects(xmlNodePtr cur, skill_lv_info_t* lv_info, skill_t* p_skill)
{
	lv_info->effects_num = p_skill->lv_info[1].effects_num;
	memcpy( lv_info->effects, p_skill->lv_info[1].effects, sizeof( lv_info->effects));	

	uint32_t temp_damage = get_addition_damage_by_index(p_skill->role_type, p_skill->add_type, p_skill->add_start, p_skill->add_step,  lv_info->lv );
	for( uint32_t i =0; i < lv_info->effects_num; i++)
	{
		skill_effects_t* p_effect = &lv_info->effects[i];
		for( uint32_t m =0; m < base_num;  m++)
		{
			float add_value = (float)p_effect->base_damage[m] * 
								(/*p_skill->base_calc_percent +*/ p_skill->step_calc_percent * (lv_info->lv - 1)) / 100;

			p_effect->base_damage[m] += add_value;
			
		}	
		for( uint32_t n =0; n < elem_num; n++)
		{
			float add_value = (float)p_effect->elem_damage[n] * 
								(/*p_skill->base_calc_percent +*/ p_skill->step_calc_percent * (lv_info->lv - 1)) / 100;
			
			p_effect->elem_damage[n] += add_value;
		}
		p_effect->addition_damage = temp_damage / lv_info->effects_num;
	}
	//lv_info->mp_consumed = temp_damage /6 ;
	return 0;	
}


int
GobalSkill::load_an_effects(xmlNodePtr cur, struct skill_lv_info_t* lv_info)
{
	uint32_t i = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Effect"))) {
			if ( i >= max_step ) {
				ERROR_LOG("step num over than max_step[%u %u]",i, max_step);
				return -1;
			}

			get_xml_prop_def(lv_info->effects[i].base_skill_id, cur, "BaseSkillID", 0);
			get_xml_prop_def(lv_info->effects[i].pre_action, cur, "PreAction", 0);
			
			uint32_t add_duration = 0;
			uint32_t base_duration = 0;
			get_xml_prop_def(add_duration, cur, "AddDuration", 0);
			if ( lv_info->effects[i].base_skill_id != 0 ) {
				const base_skill_t* base_skill = get_base_skill(lv_info->effects[i].base_skill_id);
				if ( !base_skill ) {
					ERROR_LOG("cant find base skill[%u]", lv_info->effects[i].base_skill_id);
					return -1;
				}
				base_duration = base_skill->duration;
			}
			lv_info->effects[i].duration = add_duration + base_duration;
			//special,take care: the start duration come from the first action
			if ( i == 0 ) {
				if ( lv_info->effects[i].duration < lv_info->start_duration ) {
					ERROR_LOG("start_duration error[%u %u %u %u]", i, add_duration, lv_info->effects[i].base_skill_id, lv_info->start_duration);
					return -1;
				}
				lv_info->effects[i].duration -= lv_info->start_duration;
			}
			if ( lv_info->effects[i].base_skill_id != 0 || lv_info->effects[i].pre_action != 0 ) {
				lv_info->action_duration += lv_info->effects[i].duration;
			}
			
			get_xml_prop_def(lv_info->effects[i].dir, cur, "Dir", 0);
			get_xml_prop_def(lv_info->effects[i].region_x, cur, "RegionX", 0);
			get_xml_prop_def(lv_info->effects[i].region_y, cur, "RegionY", 0);
			get_xml_prop_def(lv_info->effects[i].region_offset, cur, "RegionOffSet", 0);
			get_xml_prop_def(lv_info->effects[i].region_x_reverse, cur, "RegionXReverse", 0);
			get_xml_prop_def(lv_info->effects[i].height, cur, "Height", 0);
			get_xml_prop_def(lv_info->effects[i].radius, cur, "Radius", 0);
			get_xml_prop_def(lv_info->effects[i].angle, cur, "Angle", 0);


			get_xml_prop_def(lv_info->effects[i].speed, cur, "Speed", 0);
			get_xml_prop_def(lv_info->effects[i].offset_angle, cur, "OffsetAngle", 0);
			get_xml_prop_def(lv_info->effects[i].offset_y_dir, cur, "OffsetYDir", 0);
			get_xml_prop_def(lv_info->effects[i].offset_y, cur, "OffsetY", 0);
			get_xml_prop_def(lv_info->effects[i].disrupte, cur, "Disrupte", 0);
			get_xml_prop_def(lv_info->effects[i].persistence, cur, "Persistence", 0);
			get_xml_prop_def(lv_info->effects[i].move_with_body, cur, "MoveWithBody", 0);
			
			get_xml_prop_def(lv_info->effects[i].damage_delay, cur, "DamageDelay", 0);
			get_xml_prop_def(lv_info->effects[i].effect_id, cur, "EffectID", 0);

			get_xml_prop_def(lv_info->effects[i].base_damage[0], cur, "ProDamage", 0);
			get_xml_prop_def(lv_info->effects[i].base_damage[1], cur, "WeaponDamage", 0);
			get_xml_prop_def(lv_info->effects[i].elem_damage[0], cur, "WaterDamage", 0);
			get_xml_prop_def(lv_info->effects[i].elem_damage[1], cur, "TreeDamage", 0);
			get_xml_prop_def(lv_info->effects[i].elem_damage[2], cur, "GoldDamage", 0);
			get_xml_prop_def(lv_info->effects[i].elem_damage[3], cur, "SoilDamage", 0);
			get_xml_prop_def(lv_info->effects[i].elem_damage[4], cur, "FireDamage", 0);
			get_xml_prop_def(lv_info->effects[i].per_damage, cur, "PerDamage", 0);
			lv_info->effects[i].addition_damage = 0;

			get_xml_prop_def(lv_info->effects[i].prey_duration, cur, "PreyDuration", 0);
			get_xml_prop_def(lv_info->effects[i].offset_x, cur, "OffsetX", 0);
			get_xml_prop_def(lv_info->effects[i].fall_down, cur, "FallDown", 0);
			get_xml_prop_def(lv_info->effects[i].fly_duration, cur, "FlyDuration", 0);
			get_xml_prop_def(lv_info->effects[i].rand_fall, cur, "RandFall", 0);
			
			get_xml_prop_def(lv_info->effects[i].buff_id, cur, "BuffID", 0);
			get_xml_prop_def(lv_info->effects[i].buff_lv, cur, "BuffLv", 1);
			get_xml_prop_def(lv_info->effects[i].aura_id, cur, "AuraID", 0);
			get_xml_prop_def(lv_info->effects[i].buff_target, cur, "Target", 0);
			get_xml_prop_def(lv_info->effects[i].imme_buff, cur, "ImmeBuff", 0);
			get_xml_prop_def(lv_info->effects[i].imme_map_summon, cur, "MapSummon", 0);
			
			get_xml_prop_def(lv_info->effects[i].always_hit, cur, "AlwaysHit", 0);

			i++;
		}
		cur = cur->next;
	}

	lv_info->effects_num = i;
	
	return 0;
}

int
GobalSkill::load_buff_effects()
{
	xmlDocPtr doc = xmlParseFile("./conf/buff_effects.xml");
	if (!doc) {
		ERROR_LOG("failed to parse buffeffects file!");
		return -1;
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (!cur) {
		xmlFreeDoc(doc);
		ERROR_LOG("xmlDocGetRootElement error when loading item file!");
		return -1;
	}
	
	// load items from an xml file
	memset(&buff_effects, 0x00, sizeof(buff_effects));
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("BuffEffect"))) {
			uint32_t buff_id = 0;
			uint32_t buff_type = 0;
			get_xml_prop(buff_id, cur, "ID");
			if (buff_id == 0 || buff_id > max_buff_effects) {
				ERROR_LOG("buff effects id error[%u]", buff_id);
				return -1;
			}
			get_xml_prop(buff_type, cur, "BuffType");
			//load lv info
			xmlNodePtr cur_chd = cur->xmlChildrenNode;
			while (cur_chd) {
				if (!xmlStrcmp(cur_chd->name, reinterpret_cast<const xmlChar*>("Buff"))) {
					uint32_t buff_lv = 0;
					get_xml_prop(buff_lv, cur_chd, "Lv");
					if (buff_lv > max_skill_lv || buff_lv == 0 || buff_effects[buff_id - 1][buff_lv - 1].id != 0) {
						ERROR_LOG("buff effects id reduplicate[%u %u]", buff_id, buff_lv);
						return -1;
					}

					buff_effect_t* p_buff = &(buff_effects[buff_id - 1][buff_lv - 1]);
					p_buff->id = buff_id;
					p_buff->buff_type = buff_type;
					get_xml_prop(p_buff->duration, cur_chd, "Duration");
					get_xml_prop(p_buff->region, cur_chd, "Region");
					get_xml_prop(p_buff->target, cur_chd, "Target");
					get_xml_prop(p_buff->value, cur_chd, "Value");
					get_xml_prop(p_buff->time_lag, cur_chd, "TimeLag");
				}
				cur_chd = cur_chd->next;
			}
		}
		cur = cur->next;
	}
	
	return 0;
}

int
GobalSkill::reload_skills()
{
	
	for (std::map<uint32_t, base_skill_t*>::iterator it = base_skills_map_.begin(); 
		it != base_skills_map_.end(); ++it) {
		base_skill_t* p = it->second;
		if (p) {
			delete p;
		}
	}

	for (std::map<uint32_t, skill_t*>::iterator it = skills_map_.begin(); 
		it != skills_map_.end(); ++it) {
		skill_t* p = it->second;
		if (p) {
			delete p;
		}
	}

	base_skills_map_.clear();
	skills_map_.clear();

	try {
		load_skills();
	} catch (...) {
		INFO_LOG("reload skills catched exception");
		return -1;
	}
	INFO_LOG("reload skills succ");
	return 0;
}


