#ifndef KFBTL_SKILL_HPP_
#define KFBTL_SKILL_HPP_

/**
 *============================================================
 *  @file      skill.hpp
 *  @brief    skills
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

extern "C" {
#include <libtaomee/log.h>
}

#include <libtaomee++/conf_parser/xmlparser.hpp>

#include <vector>
#include <map>
#include "moving_object.hpp"

const double pi = 3.1415926;
/**
  * @brief constant
  */
enum constant {
	max_buff_effects = 100,
	max_skill_lv = 100,
	max_step = 11,
	base_num = 2,
	elem_num = 5,
	negligible_time = 20,
	skill_begin_handle_name_lenth = 128,
};

typedef  void (*skill_begin_handle_type)(Player*, const uint32_t*);

struct skill_begin_handle_elem {
	char skill_begin_handle_name[skill_begin_handle_name_lenth];
	skill_begin_handle_type proc;

	skill_begin_handle_elem(const char* handle_name, skill_begin_handle_type proc_in) {
		strcpy(skill_begin_handle_name, handle_name);
		proc = proc_in;
	}
};


#define regedit_skill_begin_handle(func) \
		do {\
        	skill_begin_handle_elem tmp_skill_begin_handle_elem(#func, func); \
	        skill_begin_handle_vec.push_back(tmp_skill_begin_handle_elem); \
	    } while (0)

#define regedit_skill_begin_handle_to_skill(func_name) \
		do {\
        	std::vector<skill_begin_handle_elem>::iterator it;\
			for ( it = skill_begin_handle_vec.begin(); it !=  skill_begin_handle_vec.end(); ++it) {\
				if (strcmp(it->skill_begin_handle_name, func_name) == 0) {\
					p_skill->skill_begin_handle = it->proc; TRACE_LOG("find skill begin regedit %u %s", p_skill->id, func_name);\
				}\
			}\
			if (!(p_skill->skill_begin_handle)) {\
				TRACE_LOG("not find skill begin regedit %u %s", p_skill->id, func_name);\
			}\
	    } while (0);


/**
  * @brief skill type
  */
enum skill_type {
	passive_skill = 1,
	buff_skill = 2,
	attack_skill = 3,
	normal_skill = 4,
	unique_skill = 5,
	map_summon_skill = 6,
	continue_atk_skill = 7,
	mult_continue_atk_skill = 8,
};

enum skill_dir_t {
	launcher_front = 1,
	launcher_back = 2,
	launcher_around = 3,
	target_around = 4,
	assign_around = 5,
	launcher_sector_front = 6,
	launcher_sector_back = 7,
};

enum skill_offset_dir_t {
    dir_up    = 1,
    dir_down  = 0,
};
/**
  * @brief buff type
  */
enum buff_type {
	atk_buf 		= 1,
	def_buf			= 2,
	hit_buf			= 3,
	crit_buf		= 4,
	dodge_buf		= 5,
	hp_max_buf		= 6,
	mp_max_buf		= 7,
	full_hp_buf		= 8,
	full_mp_buf		= 9,
	slow_speed_buf	= 10,
	up_speed_buf	= 11,
	
	mon_around_buf,
};

/**
  * @brief effect_target
  */
enum effect_target_t {
	oneself = 1,
	friends = 2,
	enemys	= 3,
};

/**
  * @brief information about base skills
  */
struct base_skill_t {
	uint32_t	id;
	uint32_t	duration;
};

/**
  * @brief information about skills
  */
struct skill_effects_t {
	uint32_t	base_skill_id;
	uint32_t	pre_action;
	uint32_t	duration;

	uint32_t 	dir;
	uint32_t	region_x;
	uint32_t	region_y;
	uint32_t	region_x_reverse;
	uint32_t    region_offset;
	uint32_t	height;
	uint32_t    radius;//半径
	uint32_t    angle; //角度
	
	uint32_t	damage_delay;
	uint32_t	effect_id;

	uint32_t	speed;
    uint32_t    offset_angle;
    uint32_t    offset_y_dir;
    uint32_t    offset_y;
	uint32_t	disrupte;

	uint32_t	persistence;
	uint32_t	move_with_body;

	uint32_t	base_damage[base_num];
	uint32_t	elem_damage[elem_num];
	uint32_t	per_damage;
	uint32_t    addition_damage;

	uint32_t	prey_duration;
	uint32_t	offset_x;
	uint32_t	fall_down;
	uint32_t	fly_duration;//击飞或者击倒的飞行时间
	uint32_t	rand_fall;

	uint32_t	buff_id;
	uint32_t	buff_lv;
	uint32_t 	aura_id;
	uint32_t	buff_target;

	uint32_t	imme_buff;
	uint32_t    imme_map_summon;

	uint32_t	always_hit;
};


struct buff_effect_t {
	uint32_t	id;
	uint32_t	buff_type;
	uint32_t	duration;
	uint32_t 	region;
	uint32_t	target;
	uint32_t	value;
	uint32_t	time_lag;
};

typedef struct skill_depend_damage
{
	skill_depend_damage()
	{
		skill_id = 0;
		numberic_damage = 0;
		percent_damage = 0;
	}
	uint32_t skill_id;
	uint32_t numberic_damage;
	uint32_t percent_damage;
}skill_depend_damage;

struct skill_lv_info_t {
	uint32_t	lv;
	uint32_t	use_lv;
	uint32_t	stars;
	uint32_t	hp_consumed;
	//uint32_t	mp_consumed;
	uint32_t	cool_down;
	uint32_t	start_duration;
	uint32_t	action_duration;
	uint32_t	end_duration;
	uint32_t	body_move_dir;
	uint32_t	body_move_x;
	uint32_t	effects_num;
	skill_effects_t	effects[max_step];
};

struct skill_interrupt_t {
   uint32_t step_type;
   uint32_t step_duration;
};

struct skill_t {
	uint32_t	id;
	uint32_t	type;
	uint32_t	ai_control_flag;
    uint32_t    nature; //1--不能被沉默
	
	//自动生成其他级别的数据 
	uint32_t    role_type;
	uint32_t    auto_config;
	//uint32_t    base_calc_percent;
	uint32_t    step_calc_percent;
	//附加伤害
	uint32_t    add_type;
	uint32_t    add_start;
	uint32_t    add_step;
	uint32_t    base_mp_consume; //消耗基础百分比
	uint32_t    mp_consume; //固定耗蓝值

	skill_begin_handle_type skill_begin_handle;
	uint32_t	skill_begin_handle_agrv[10];
	
	//技能依赖附加伤害
	skill_depend_damage  depend_info;	

	skill_interrupt_t  unbreak_info[2];

	skill_lv_info_t	lv_info[max_skill_lv + 1];

    bool is_mult_continue_atk_skill()
    {
        return type == mult_continue_atk_skill;
    }

	bool is_continue_atk_skill()
	{
		return type == continue_atk_skill;
	}

	bool is_atk_skill()
	{
		return (type == attack_skill || type == normal_skill); 
	}
	
	bool is_buff_skill() 
	{
		return type == buff_skill;
	}

	bool is_unique_skill()
	{
		return type == unique_skill;
	}

	bool is_map_summon_skill()
	{
		return type == map_summon_skill;
	}

	bool is_normal_skill()
	{
		return type == normal_skill;
	}
};

struct skill_simple_info {
	skill_simple_info()
	{
		id = 0;
		type = 0;
		lv = 0;
		depend_damage_active_flag = 0;
	}
	uint32_t id;
	uint32_t type;
	uint32_t lv;
	uint32_t depend_damage_active_flag;
};

struct attack_region_t {
	attack_region_t()
		{ x = 0; y = 0; }

	friend bool operator<(const attack_region_t& r1, const attack_region_t& r2)
		{ return (r1.x * r1.y) < (r2.x * r2.y); }

	//uint32_t atk_dir;
	uint32_t x;
	uint32_t y;
};


class BufEffect : public MovingObject {
public:
	
	  
	BufEffect(Player* shooter, uint32_t skillid, uint32_t type, uint32_t lv);
	~BufEffect();

	void init(uint32_t buff_id, uint32_t buff_target, uint32_t imme_buff);

	int affect();	

	uint32_t cur_effects_idx() const
		{ return cur_effects_idx_; }

	void update(int time_elapsed);
	
private:
	Player*		launcher_;
	map_t*		map_;

	uint32_t buff_id_;
	uint32_t buff_target_;
	uint32_t imme_buff_id_;

	uint32_t cur_effects_idx_;
	skill_simple_info skill_base_info_;
	const buff_effect_t* p_buf_effect_;
};


//class SkillEffects : public MovingObject, public MemPool<SkillEffects> {
class SkillEffects : public MovingObject {
public:
	/**
	  * @brief constructor
	  * @param shooter the player who launched this attack
	  * @param skillid id of the skill that is launched
	  */
	SkillEffects(Player* shooter, uint32_t skillid, uint32_t type, uint32_t lv, uint32_t depend_damage_flag);
	/**
	  * @brief default destructor
	  */
	~SkillEffects();

	/**
	  * @brief init the SkillEffect before use
	  */
	void init(struct skill_effects_t* p_skill_step, skill_t* p_skill,  uint32_t assign_x = 0, uint32_t assign_y = 0);

	/**
	  * @brief get the finish_flag_
	  */
	bool is_finished() const
		{ return finish_flag_; }
		
	/**
	  * @brief get the cur_step_idx_
	  */
	uint32_t cur_effects_idx() const
		{ return cur_effects_idx_; }

	/**
	  * @brief the SkillEffect how to affect the object in the map
	  */
	int affect();

	bool check_hit(const Player* p);	

	bool CanHurtPlayer(Player * p);
	/**
	  * @brief get the damage of the effect to the preyer
	  */
	uint32_t get_damage(const Player* preyer);
	
	/**
	  * @brief get the region of effect
	  */
	attack_region_t get_atk_region();

    /**
     * @move skillEffect calculate x_pos
     */
    double calc_x_pos(double angle, double speed, double timelag = 1.00) {
        return speed * timelag * cos((angle * pi) / 180.00);
    }

	/**
     * @move skillEffect calculate x_pos
     */
    double calc_y_pos(double angle, double speed, double timelag = 1.00) {
        return speed * timelag * sin((angle * pi) / 180.00);
    }

    /**
     * @brief the interface of set end_pos
     */
    void set_end_pos(int x, int y, int z) {
        end_pos_.set_x(x);
        end_pos_.set_y(y);
        end_pos_.set_z(z);
    }
	/**
	  * @brief update attribute of this object
	  */
	void update(int time_elapsed);

public:
//	USING_MEMPOOL_OPERATORS(SkillEffects);
private:
	void move_effect_to_next_pos();
private:
	/*! the player who launched this attack */
	Player*		launcher_;
	/*! at which map this baseskill was launched */
	map_t*		map_;

	/*! skill_t */
	skill_t*    skill_info_;

	/*! simple info of this skill */
	skill_simple_info skill_base_info_;

	
	/*! the idx of the skill step */
	uint32_t cur_effects_idx_;
	/*! the info of the skill step */
	struct skill_effects_t* p_skill_effects_;

	/*! the duration_ of the skill step */
	uint32_t	duration_;
	/*! the finish_flag_ of the skill step */
	bool		finish_flag_;
	/*! the skill have any attack effect */
	bool		effection_;
	/*! the delay of damage */
	uint32_t	damage_delay_;
	/*! the skill damage interval */
	uint32_t	next_damage_;
	
	/*! the excess time of last effect */
	uint32_t	last_excess_tm_;
	
	/*! the last hit of this effect, help for fall down after hit*/
	bool		last_hit_;
	
	/*! the absolute dir of the effect */
	uint32_t	absolute_dir_;
	
	/*! the end pos of this skilleffect if skill can move with body */
	Vector3D	end_pos_;
};

//class Skill : public MovingObject, public MemPool<Skill> {
class Skill : public MovingObject {
public:
	/**
	  * @brief constructor
	  * @param shooter the player who launched this attack
	  * @param skillid id of the skill that is launched
	  */
	Skill(Player* shooter, uint32_t skillid, uint32_t lv, uint32_t depend_damage_active_flag, uint32_t assign_x = 0, uint32_t assign_y = 0);
	
	/**
	  * @brief default destructor
	  */
	~Skill();

	/**
	  * @brief update attribute of this object
	  */
	void update(int time_elapsed);
	
	uint32_t get_skill_type()
	{
		return skill_info_->type;
	}
	uint32_t get_skill_id()
	{
		return skill_id_;
	}
	
	bool judge_can_be_break(uint32_t hit_type);

public:
//	USING_MEMPOOL_OPERATORS(Skill);

private:
	/**
	  * @brief update buff skill
	  */
	//void buf_update(int time_elapsed);
	void create_skill_effects(uint32_t depend_damage_active_flag);

	void calc_player_moveable_pos(Player * shooter);

	void skill_start_action(int time_elapsed);
	void skill_attack_action(int time_elapsed);
	void skill_end_action(int time_elapsed);
	void skill_effect_update(int time_elapsed);
	void skill_effect_over_action(int time_elapsed);

	void init_skill_attack_effects();

	void normal_skill_effect_update(int time_elapsed);
	void skill_buff_effect_update();
	void continue_skill_effect_update(int time_elapsed);
	void mult_continue_skill_effect_update(int time_elapsed);

private:
	/*! the player who launched this attack */
	Player*		launcher_;
	/*! at which map this Skill was launched */
	map_t*		map_;
	/*! ID of this skill */
	uint32_t	skill_id_;
	/*! lv of this skill */
	uint32_t	lv_;
	/*! information of this skill */
	skill_t*	skill_info_;
	
	/*! the start duration of this skill */
	uint32_t	start_duration_;
	/*! the start duration of this skill */
	uint32_t	action_duration_;
	/*! the end duration of this skill */
	uint32_t	end_duration_;

	/*! total step of the skill playing*/
	std::vector<SkillEffects*> skill_effect_arrays_;
	uint32_t                   skill_effect_index_;
	/*! current step of the skill playing */
	SkillEffects* 	p_skill_effects_;
	/*! current step of the skill playing */
	BufEffect* 		p_buf_effect_;
	/*! current step of the skill playing */
	uint32_t 	action_stage_;
	/*! current step of the skill playing */
	uint32_t 	effect_stage_;

	/*! activate the skill */
	bool		activation_;

	/*! record the launcher next pos if can move when launching skill */
	Vector3D	launcher_move_pos;

	/*! the assign pos of skill */
	Vector3D	assign_pos_;

	enum {
	  break_step_1 = 1,
	  break_step_2 = 2,
	};

	struct player_skill_interrupt_t {
		uint32_t cur_step;
		uint32_t step_type;
		uint32_t step_duration;
	};
    player_skill_interrupt_t break_info_;
		
private:
	void update_skill_break_info(int time_elapsed);

	enum stage {
		start_stage = 1,
		attack_stage = 2,
		end_stage = 3,
		finish_stage = 4
	};
};



class GobalSkill {
public:
	GobalSkill(const char* filename);
	~GobalSkill();

	base_skill_t* 	get_base_skill(uint32_t skill_id);
	skill_t* 		get_skill(uint32_t skill_id);
	buff_effect_t*	get_buff_effect(uint32_t buff_id, uint32_t	buff_lv = 0);

	int load_skills();
	int reload_skills();
private:
	int load_buff_effects();
	int load_an_effects(xmlNodePtr cur, skill_lv_info_t* lv_info);
	int load_an_effects(xmlNodePtr cur, skill_lv_info_t* lv_info, skill_t* p_skill);
	int load_buf_effect(xmlNodePtr cur, skill_lv_info_t* lv_info);
	int load_an_skill(xmlNodePtr cur, skill_t* p_skill);

private:
	std::string filename_;
	
	std::map<uint32_t, base_skill_t*> base_skills_map_;
	std::map<uint32_t, skill_t*> skills_map_;

	buff_effect_t	buff_effects[max_buff_effects][max_skill_lv];
};


/*! hold all buff effects */
//extern buff_effect_t buff_effects[max_buf_effects];
/*! hold all skills */
extern GobalSkill* g_skills;
/*! a list to link all launched skills together */
extern ObjList g_skill_list;

bool is_skill_can_be_silent(uint32_t skill_id);

void skill_begin_handle_suck_hp(void*);
#endif // KFBTL_SKILL_HPP_

