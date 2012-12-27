/**
 *============================================================
 *  @file      player.hpp
 *  @brief    player related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_PLAYER_HPP_
#define KFBTL_PLAYER_HPP_

#include <list>
#include <map>
#include <vector>

#include <boost/intrusive/list.hpp>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>

#include <kf/player_attr.hpp>

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/timer.h>

#include <libtaomee/project/constants.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/project/types.h>

#include <async_serv/dll.h>
}
#include <kf/item_impl.hpp>
//#include <kf/title_attr.hpp>
//#include "mempool/mempool.hpp"
//#include "state_machine/state_machine.hpp"
#include "moving_object.hpp"
#include "fwd_decl.hpp"
#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "monster.hpp"
#include "skill.hpp"

#include "ai/AI_interface.hpp"
#include "buff.hpp"
#include "aura.hpp"

class SkillEffects;
const uint32_t all_cooltime = 1000;

/**
  * @brief player's current direction
  */
enum player_dir_t {
	/*! left */
	dir_left	= 1,
	/*! right */
	dir_right	= 0,
	/*! around */
	dir_around	= 2,
};

/**
  * @brief battle status of a player
  */
enum player_btl_status_t {
	/*! player has got ready for the battle */
	player_ready_for_battle	= 1,
	/*! battle has been started and the player has been in the battle */
	player_in_battle		= 2,
	/*! team wait player has got ready for start the battle */
	player_team_btl_wait_ready = 3,
};

/**
  * @brief player's current team
  */
enum player_team_t {
	player_team_1 = 1,
	player_team_2,
	monster_team,
	barrier_team,  //该打碎物只能是人打碎，怪物和宠物不能打碎
	barrier_team_1, //该打碎物只能由怪物打碎，宠物和人不能打碎
	neutral_team_1, //中立友方单位，该单位为友方，但是不能被怪物攻击
};

enum monster_type {
    monster_boss = 1,
    monster_low,
    monster_middle,
    monster_major,
    monster_special,
};

enum {
	team_member_type_leader = 0,
	team_member_type_nor,
};

enum {
	reset_skills_item_id    = 1703001,
	repair_clothes_item_id  = 1703003,
	double_exp_buff_item_id = 1703002,
	relive_player_item_id 	= 1302000,
	relive_player_item_id_1	= 1302001,
	clear_random_one_debuff_item_id = 1303016,
	super_walnut_item_id = 1700065,
	clear_all_monster_item_id = 1700066,
};


struct buf_skill_t {
	uint32_t buf_type;
	uint32_t buf_duration;
	uint32_t per_trim;
	uint32_t start_tm;
	uint8_t	 db_buff_flg;
	uint32_t mutex_type;
};


/**
  * @brief type of map that holds all the players
  */
typedef std::map<userid_t, Player*> PlayerMap;

/**
  * @brief type of map that holds player all skills
  */
typedef std::map<uint32_t, player_skill_t> SkillMap;

/**
  * @brief type of map that holds the items player need to collect
  */
typedef std::multimap<uint32_t, player_task_item_t> TaskItemMap;

typedef std::list<player_task_item_t> TaskItemList;

/**
  * @brief type of map that holds the monsters player need to kill
  */
typedef std::map<uint32_t, player_task_monster_t> TaskMonMap;

/**
  * @brief type of map that holds the monster player need to kill
  */
//typedef std::vector<player_task_step_t> TaskMonVec;

/**
  * @brief type of map that holds the monster player need to kill
  */
typedef std::list<player_hpmp_chg_info_t> HpmpChgList;

/**
  * @brief type of map that holds the buf skill got from online 
  */
typedef std::map<uint32_t, buf_skill_t> BufSkillMap;



struct player_hpmp_chg_info_t {
//	uint32_t hp_chg;
//	uint32_t mp_chg;
	int			hp_chg;
	int			mp_chg;
//	uint16_t	rest_count;
//	uint16_t	interval;
};

struct player_hpmp_add_t {
	uint32_t hp_chg;
	uint32_t mp_chg;
};

struct player_pro_chg_info_t {
	uint32_t buff_type;

	int atk_chg;
	float def_chg;
};

struct judge_boost_plugin_t {
	uint8_t		judge_flag;
	uint32_t	judge_cnt;
	uint32_t	detect_cnt;
	int			pos_x;
	int			pos_y;
	
	struct timeval	last_judge_tm;
};

struct judge_skip_step_t {
	uint8_t	 judge_flag;
	uint32_t judge_cnt;
	uint32_t detect_cnt;
};

struct fumo_stage_info_t {
	uint16_t stage_id;
	uint8_t grade[max_stage_difficulty_type];
	fumo_stage_info_t(const uint16_t id, uint8_t const* grade_in) {
		stage_id = id;
		memcpy(grade, grade_in, max_stage_difficulty_type);
	}
};

struct suit_add_attr_t {
	//base attr
	uint32_t strength;
	uint32_t agility;
	uint32_t body_quality;
	uint32_t stamina;
	//attr level 2
	uint32_t atk;
	uint32_t def;
	uint32_t hit;
	uint32_t dodge;
	uint32_t crit;
	uint32_t hp;
	uint32_t mp;
	uint32_t add_hp;
	uint32_t add_mp;
	
	uint32_t skill_atk;
};


#pragma pack(1)

struct player_clothes_info_t {
	uint32_t 	clothes_id;
	uint32_t 	unique_id;
	uint16_t 	duration;
	uint16_t 	lv;
	uint32_t 	gettime;
	uint32_t 	timelag;

	uint16_t	init_duration;
	uint8_t		duration_state;
	uint8_t		duration_chg_flag;
};

struct player_clothes_t {
	uint8_t clothes_cnt;

	uint8_t weapon_cnt;
	uint8_t armor_cnt;
	uint8_t jewelry_cnt;

	player_clothes_info_t clothes[max_clothes_on];
};


/**
  * @brief info of an equipment of a player
  */
struct player_clothes_base_info_t {
	uint32_t clothes_id;
	uint32_t unique_id;
	uint16_t duration;
	uint16_t lv;
	uint32_t gettime;
	uint32_t timelag;
};

/**
  * @brief info of skills of a player
  */
struct player_skills_info_t {
	uint32_t skill_id;
	uint32_t skill_lv;
	uint8_t bind_flag;
};

struct player_task_item_t {
	uint32_t item_id;
	uint32_t monster_id;
	uint32_t drop_odds;
	uint32_t rest_cnt;
};

struct player_task_monster_t {
	uint32_t monster_id;
	uint32_t need_kill_count;
};

//struct task_step_t {
//	//uint32_t id;
//	//uint32_t task_type;
//	//uint32_t get_type;
//	uint32_t get_id;
//	uint32_t completeness;
//};

//struct player_task_step_t {
//	uint32_t    task_id;
//	uint16_t    step_cnt;
//	task_step_t step[max_task_step];
//	uint8_t     serv_buf[serv_buf_len];
//	uint8_t		change_flag;
//};


struct fumo_stage_t {
	uint32_t	stage_id;
	uint8_t		grade[max_stage_difficulty_type];
};

struct player_pick_item_rsp_t {
	uint32_t	item_id;
	uint32_t	unique_id;
};

struct player_pick_roll_item_rsp_t{
	uint32_t    err_code;
	uint32_t    roll_id;
	uint32_t    item_id;
	uint32_t    unique_id;
};

#ifdef DEV_SUMMON
struct player_summon_info_t {
	uint32_t    attr_type; //属性模板
	uint32_t    attr_per; //成长系数
	uint32_t	mon_type;
	uint32_t	mon_tm;
	char		nick[max_nick_size];
	uint16_t	mon_lv;
	uint32_t	mon_exp;
	uint16_t	mon_fight_value;
	uint16_t    renascence_time;
	uint16_t	skills_cnt;
	summon_skill_info_t	skills[max_summon_skills];
	//uint32_t    disable_skills[max_summon_skills];
};


struct buf_skill_rsp_t {
	uint32_t buf_type;
	uint32_t start_tm;
	uint32_t buf_duration;
	uint32_t per_trim;
	uint8_t  db_buff_flg;
	uint32_t mutex_type;
};

struct quality_t {
    quality_t() {
        hp      = 0;
        mp      = 0;
        addhp   = 0;
        addmp   = 0;
        atk     = 0;
        def     = 0;
        hit     = 0;
        dodge   = 0;
        crit    = 0;
    }
    /* ! btl quality */
    uint32_t    hp;
    uint32_t    mp;
    uint32_t    addhp;
    uint32_t    addmp;
    uint32_t    atk;
    uint32_t    def;
    uint32_t    hit;
    uint32_t    dodge;
    uint32_t    crit;
};

#endif

enum {
	max_numen_skill_num = 3,
};

struct numen_skill_t {
	uint32_t skill_id;
	uint32_t skill_lv;
};

struct numen_info_t {
	Player* 	owner;
	uint32_t numen_id;
	uint32_t numen_nick[max_nick_size];
	numen_skill_t skill_arr[max_numen_skill_num];
};

struct player_numen_info_t {
	uint32_t numen_id;
    char    name[max_nick_size];
	numen_skill_t skill_arr[max_numen_skill_num];
};

/**
  * @brief package from online to battle server
  */
struct player_attr_t {
	uint32_t	stage;
	uint32_t	difficulty;
	uint32_t	role_type;
	uint32_t	power_user;
	uint32_t	role_tm;
	userid_t	parentid;
	uint8_t     once_bit[max_once_bit_size];
	char		nick[max_nick_size];
	uint32_t	battle_mode;
    uint32_t    achieve_title;
	int16_t		lv;
	uint32_t	exp;
	uint32_t	allocator_exp;
	uint32_t	dexp_tm;
	uint8_t     half_exp;
	uint32_t	coins;
	uint32_t	skill_point;
	uint32_t	hp;
	uint32_t	mp;
	uint32_t    attire_gs;
	uint32_t    continue_win;
	uint32_t 	honor;
	uint32_t    vip;
	uint32_t    vip_level;
	uint32_t    max_bag_grid_count;
	uint32_t 	max_conti_win_times;
	uint8_t		btl_again;
	uint32_t	fumo_tower_top;
	uint32_t	fumo_tower_used_tm;
	//uint32_t	cur_tower_layer;
	uint32_t	show_state;
	uint32_t	taotai_do_times;
	uint32_t    taotai_win_times;
	uint32_t    team_lv;

	uint32_t    pass_cnt;

    quality_t   add_quality;
	uint8_t		unique_item_bit[20];
	uint32_t	clothes_num;
	player_clothes_base_info_t	clothes[max_clothes_on];
	
	uint32_t	task_items_num;
	player_task_item_t		task_items[max_task_items];

	/*! fumo info */
	uint32_t	fumo_points_today;

#ifdef DEV_SUMMON
	player_summon_info_t	summon_info;
#endif
	player_numen_info_t numen_info;
	uint32_t 	buf_skill_num;
    uint32_t    skills_num;
	uint32_t    home_btl_pet_num;
	uint8_t		buf[];
};

struct on_hit_info_t;

/**
  * @brief package from online to battle server
  */
struct btl_team_attr_t {
	uint32_t limit_num;
	uint32_t refuse_hot_join;
//	char	 nick[max_nick_size];
};

struct restart_player_attr_t {
	uint32_t	stage_id;
	uint32_t	difficulty;
	uint8_t		half_exp;
	uint32_t    task_items_num;
	player_task_item_t      task_items[max_task_items];
};

struct player_move_t {
	uint32_t    old_x;
	uint32_t    old_y;
	uint32_t    old_z;
	uint32_t	x;
	uint32_t	y;
	uint32_t	z;
	uint8_t		dir;
	uint8_t		mv_type;
	uint32_t	client_tm;
	uint32_t	client_msec;
};

struct player_stop_t {
	uint32_t    old_x;
	uint32_t    old_y;
	uint32_t	x;
	uint32_t	y;
	uint8_t		dir;
};

struct player_jump_t {
	uint32_t    old_x;
	uint32_t    old_y;
	uint32_t	x;
	uint32_t	y;
};

struct player_attack_t {
	uint32_t	skill_id;
	uint32_t	x;
	uint32_t	y;
	uint32_t	z;
	uint32_t    client_tm;
	uint32_t 	client_msec;
};

struct player_roll_item_t{
	uint32_t   battle_id;
	uint32_t   roll_id;
	int32_t    roll_flag;
};

struct addition_attr_t {
    uint16_t        cut_cd;
    uint16_t        cut_mp;
    uint16_t        attr_dmg;
    uint16_t        extra_dmg;
};

struct player_skill_t {
	player_skill_t(uint32_t id, uint32_t level, uint8_t key_flag = 0) {
		skill_id = id;
		lv = level;
        flag = key_flag;
		depend_damage_active_flag = 0;
		memset(&last_tv, 0x00, sizeof(timeval));
		skill_info = g_skills->get_skill(id);
        base_mp_consume = skill_info->mp_consume;
		p_skill_info = &(skill_info->lv_info[lv]);
        memset(&(skill_attr), 0x00, sizeof(addition_attr_t));
	}
	uint32_t		skill_id;
	uint32_t		lv;
    uint32_t        base_mp_consume;
	uint32_t        depend_damage_active_flag;
	uint8_t		    flag;

	skill_t    * skill_info;   
	
	timeval 		last_tv;
	
    //------------The effect of clothes----------------
    addition_attr_t skill_attr;
    //-------------------------------------------------

	skill_lv_info_t* p_skill_info;
};

/**
  * @brief stage score
  */
struct stage_score_t {
	stage_score_t()
	{
		init();
	}

	void init()
	{
		memset(this, 0, sizeof(stage_score_t));
	}

	uint32_t	start_tm;
	uint32_t	end_tm;
	uint16_t	on_hit_cnt;
	uint16_t	all_mon_cnt;
	uint16_t	kill_mon_cnt;
	uint16_t	deaded_cnt;
	uint16_t	all_drop_cnt;
	uint16_t	pick_drop_cnt;
	
	uint32_t	last_hit_tm;
	uint16_t	cur_hit_num;
	//uint16_t	hit_flag;

	uint16_t	sec_kill_cnt;
	uint16_t	break_skill_cnt;
	uint16_t	combo_hit_cnt;
	uint16_t	back_hit_cnt;


	uint32_t	get_exp;
	//uint32_t	kill_mon_exp;

	uint16_t	map_num;
	uint8_t		stage_grade;
};

struct join_team_attr_t {
	uint32_t room_id;
	uint32_t team_leader;
};

struct hot_join_team_attr_t {
    uint8_t  btl_type;
	uint32_t room_id;
};

struct tmp_team_pvp_attr_t {
    uint8_t  pvp_type;
	uint32_t room_id;
	uint32_t tmp_team_leader;
	uint32_t tmp_team_id;
	uint32_t tmp_team_member_cnt;
	uint32_t match_key;
};


struct detective_info_t {
	uint32_t begin_client_mv_tm;
	uint32_t begin_server_mv_tm;
	uint32_t last_client_mv_tm;
	uint32_t last_server_mv_tm;
	
	uint32_t mv_cnt_in_one_sec;

	uint32_t be_hit_tm;
	uint32_t hit_offset_x;
	uint32_t skip_mv_cnt_tmp;
	uint32_t skip_mv_tm_tmp;
	uint32_t skip_mv_total_cnt;

	uint32_t common_flag1;
	uint32_t common_flag2;
	uint32_t common_flag3;

	uint32_t last_client_atk_tm;
	uint32_t last_client_atk_msec;
	uint32_t begin_client_atk_tm;
	uint32_t begin_server_atk_tm;
	uint32_t atk_cnt_in_one_sec;

	//robot detect
	uint32_t last_skill_id;
	detective_info_t() {
		last_skill_id = 0;
		begin_client_mv_tm = 0;
	  	begin_server_mv_tm = 0;
		last_client_mv_tm = 0;
		last_server_mv_tm = 0;

		mv_cnt_in_one_sec = 0;

		be_hit_tm = 0;
		hit_offset_x = 0;
		skip_mv_cnt_tmp = 0;
		skip_mv_tm_tmp = 0;
		skip_mv_total_cnt = 0;

		common_flag1 = 0;
		common_flag2 = 0;
		last_client_atk_tm = 0;
		begin_client_atk_tm = 0;
		begin_server_atk_tm = 0;
		atk_cnt_in_one_sec = 0;
		common_flag3 = 0;
	}
	
};
#pragma pack()

//title_attr_data_mgr* get_title_attr_data_mgr();
/**
  * @brief Player
  */
//class Player : public MovingObject, public MemPool<Player> {
class Player : public MovingObject, public taomee::EventableObject {
public:
	/**
	  * @brief constructor
	  * @param uid user id
	  * @param fdsession fd session
	  */
	Player(userid_t uid = 0, fdsession_t* fdsession = 0);
	/**
	  * @brief destructor
	  */
	~Player();

	//----------------------------------
	// player's normal actions
	//----------------------------------
	/**
	  * @brief player move
	  */
	void move(const player_move_t* mv);
	/**
	  * @brief player jump
	  */
	void jump(const player_jump_t* mv);
	/**
	  * @brief player stop moving
	  */
	void stop(const player_stop_t* req);
    /**
     * @brief summon stand
     */
    void summon_stand(const Player * summon);
	/**
	  * @brief player enters a map
	  */
	void enter_map(uint32_t mid);
	/**
	  * @brief leave the current map
	  */
	void leave_map(bool leave_stage = false);
    /**
     * @brief monster in player map
     */
    bool is_monster_in_cur_map(uint32_t mon_id);
	/**
	  * @brief monster enters a map
	  */
	void monster_enter_map(map_t* m);
	/**
	  * @brief player attacks
	  */
	void attack(const player_attack_t* req);

	void talk(uint8_t * msg, uint32_t msg_len, uint32_t recv_id);

	/**
	  * @brief repair all attires
	   */	
	void repair_all_attire();

	/**
	  * @brief set all clothes 0 duration when it's dead
	  */
	void set_all_clothes_0_duration();

	/**
	  * @brief 
	  */
	void punish_exp_when_dead();

	/**
	  * @brief 
	  */
	void init_player_passive_skill(bool noti_flg = true);
	/**
	  * @brief set player revive
	   */
	void set_revive();
	/**
	  * @brief player uses a item
	  */
	void use_item(uint32_t item_id);
	
	/**
	  * @brief player pick a item
	  */
	void pick_item(uint32_t unique_id, uint32_t max_bag_grid_count);

	/**
	  * @brief enter map 'm'
	  */
	void do_enter_map(map_t* m, bool pre_in = true);


	/**
	  * @brief a player say something
	  */
	void speak(const void* buf, uint32_t buflen) const;

	//----------------------------------
	// about player's skills
	//----------------------------------
	/**
	  * @brief launch a skill
	  */
	void launch_skill(player_skill_t* m, uint32_t assign_x = 0, uint32_t assign_y = 0);


	/**
	 * @brief set all player skills cool down
	 */
	void set_all_skills_cd();

	void set_all_skills_ready();
	/**
	  * @brief appearance buf skill
	  */
	void do_item_buf(const GfItem* itm);
	
    /**
     * @brief bless buff
     */
    bool is_have_bless_buff(uint32_t buff_id);
	/**
	  * @brief appearance buf start
	  */
	void set_app_buf_start(uint32_t tm_now);

	/**
	  * @brief called each time when a player is hit
	  */
	bool on_hit(Player* atker, const skill_simple_info* p_skill_base, const skill_t* p_skill,  const skill_effects_t* p_skill_info, bool last_hit);

	bool can_be_hurted();

	/**
	 * @brief after on hit 
	 */
	int  after_on_hit(Player* target);
	/**
	 * @brief calc damage for attack
	 */
	int  calc_damage(Player* target, const skill_simple_info* p_skill_base, const skill_t* p_skill,  const skill_effects_t* p_skill_info, int& crit_atk);

	int calc_defthreshold_damage(Player* target, int damage);

	int calc_atkthreshold_damage(Player* target, int damage);
	
	uint32_t get_weapon_avg_value();
	/**
	 * @brief calc score for on hit
	 */
	int  calc_hit_score(Player* target, int32_t damages, int32_t crit_atk);
	/**
	  * @brief called each time when a player is hit bu buff skill
	  */
	bool on_buf_hit(Player* atker, const skill_simple_info* p_skill_base, const buff_effect_t* p_buf_info, uint32_t effect_target);

	/**
	  * @brief called each time when a player is hit bu buff skill
	  */
	bool on_buf_hit(Player* atker, uint32_t buff_id, uint32_t buff_lv = 0);
	
	/**
	  * @brief set buff skill on player
	  */
	void set_buf_on_player(uint32_t buf_type, uint32_t duration, uint32_t trim, uint8_t	up_flag);

	/**
	  * @brief set buff skill event
	  */
	void set_buf_event(taomee::TimedEvent** old_event, taomee::TimedEvent* new_event);

    /**
     * @brief set skill addition attr
     */
    void set_skill_addition_attr(uint32_t skill_id, addition_attr_t* pattr);
	void reset_skill_addition_attr();
    /**
     * @brief get skill addition  attr damage
     */
    uint32_t get_skill_attr_dmg(uint32_t skill_id);
    /**
     * @brief get skill addition  attr damage
     */
    uint32_t get_skill_extra_dmg(uint32_t skill_id);

	/**
	  * @brief set the cool time of the skill
	  */
	void set_skill_cool_time(uint32_t skill_id, int cool_time);

    /**
	  * @brief set the cool time of the skill
	  */
	void set_all_skill_cool_time(int cool_time);

	/**
	 * @brief get the cool time of the skill
	 */
	uint32_t get_skill_cool_time( uint32_t skill_id);

	/**
	 * @brief get the mp need of the skill
	 */
	double get_skill_mp_consumed(uint32_t skill_id);	
	/**
	  * @brief player destroy a skill
	  */
	void on_skill_end(Object* p_skill);

	/**
	  * @brief unpack all skill from online
	  */
	void unpack_all_skill(uint32_t skills_num, uint32_t buff_num, uint32_t home_btl_pets_num, const uint8_t* body);

	/**
	 * @brief calc depend skill damage for all skills
	 */
	bool is_skill_exist(uint32_t skill_id);
	void calc_depend_skill_damage();
	/**
	  * @brief notify skill effect
	  */
	void skill_effect_noti(uint32_t skill_id, uint32_t x, uint32_t y);
	//----------------------------------
	// about monster's AI
	//----------------------------------
	/**
	  * @brief monster select a skill to attack
	  */
	player_skill_t* select_skill(const Player* preyer, uint32_t skillid = 0);

	player_skill_t* select_specifed_skill(uint32_t skill_id);
	/**
	  * @brief monster select a skill to attack
	  */
	player_skill_t* select_assign_skill(const Player* player, uint32_t skillid);

	/**
	  * @brief monster select super_armor skill
	  */
	player_skill_t* select_super_armor_skill();

	/**
	  * @brief monster select buff skill
	  */
	player_skill_t* select_buff_skill(const Player* preyer, uint32_t buf_type = 1);


	/**
	  * @brief monster get the max attack region
	  */
	attack_region_t get_attack_region(uint32_t skill_id = 0);

	/**
	  * @brief monster call another monster
	  */
	void call_monster(uint32_t mon_id, uint32_t x, uint32_t y, int team = 3, int add_buff_id = 0, int add_aura_id = 0);

	/**
	  * @brief monster call any monsters
	  */
	void call_monsters(uint32_t mon_id, uint32_t mon_cnt);

	void call_map_summon(uint32_t summon_id, uint32_t x, uint32_t y, bool notify = true, bool owner_flg = true);

	void monster_speaks(uint32_t word_idx);
	/**
	  * @brief update a player's status
	  */
	void update(int milli_sec);

	//----------------------------------
	// about player's battles
	//----------------------------------
	/**
	  * @brief set battle status 'sts' on
	  */
	void set_battle_status(player_btl_status_t sts)
		{ btl_status = taomee::set_bit_on(btl_status, sts); }
	/**
	  * @brief set battle status 'sts' off
	  */
	void set_battle_status_off(player_btl_status_t sts)
		{ btl_status = taomee::set_bit_off(btl_status, sts); }

	/**
	  * @brief return true if the player is in battle, false otherwise
	  */
	bool in_battle() const;

	/**
	  * @brief pvp battle start after count down
	  */
	int pvp_battle_start();

	/**
	  * @brief set player to be the team leader
	  */
	void set_player_leader(Player* p);
	
    /**    
     * @brief 判断是否是 VIP;     
     */    
    bool is_vip_player()
    {
        return taomee::test_bit_on(vip, cur_vip);
    }

    /**
     * @brief 判断是否是 年费VIP;
     */
    bool is_vip_year_player()
    {
        return (is_vip_player() && taomee::test_bit_on(vip, year_vip));
    }

    bool has_player_opened_vip()
    {
        //return taomee::test_bit_on(vip, prev_vip);
        return vip != 0;
    }
	/**
	  * @brief flag player in team btl
	  */
	void set_player_in_team_btl()
		{ team_btl_flg = 1; }
	/**
	  * @brief if player in team btl
	  */
	bool is_player_in_team_btl()
		{ return team_btl_flg == 1; }	
	/**
	  * @brief 
	  */
	int proc_lv_matching(uint32_t& type);

	int end_pvp_wait(uint32_t& type);

	void add_pvp_end_ev()
		{
			timeval tv = *get_now_tv();
			tv.tv_sec += 60;
			uint32_t type = 1;
			pvp_end_ev = ev_mgr.add_event(*this, &Player::end_pvp_wait, type, tv, 60000, 0);

		}

	void remove_pvp_end_ev()
		{
			if (pvp_end_ev) {
				ev_mgr.remove_event(pvp_end_ev);
				pvp_end_ev = 0;
			}
		}

	//----------------------------------
	// about player's attr change
	//----------------------------------
	/**
	  * @brief initiate a player's attribute
	  */
	void init_player_attr(const player_attr_t* attr);

	void init_tmp_team_attr(const tmp_team_pvp_attr_t* p_attr);
	
	void add_player_team_attr(bool reset_cur_status);


	/**
	  * @brief init player info when battle restart
	  */
	void restart_init(const restart_player_attr_t* p_attr);
	
	/**
	 * @brief calc and init a player's battle attr
	 */
	void calc_player_attr(bool reset_cur_status = true);

	/**
	 * @brief calc and init a player's battle attr ex
	 */
	void calc_player_attr_ex(bool reset_cur_status);

	/**
	 * @brief calc and init a summon's battle attr
	 */
	void calc_summon_attr(bool reset_cur_status = true);

	/**
	 * @brief add fumo points to player
	 */	
	void add_fumo_points(uint32_t& get_points);
	/**
	  * @brief calc stage score and noti to the player
	  */
	void calc_stage_score(bool is_btl_over = true);
	
	/**
	  * @brief calc pvp score and noti to the player
	  */
	void calc_pvp_score(uint32_t result_type);

	/**
	  * @brief  reduce coins safe
	  */
	void reduce_coins(uint32_t in_coins)
		{ coins = in_coins > coins ? 0 : coins - in_coins;}

	/**
	  * @brief  reduce pvp coins safe
	  */
	void reduce_pvp_coins();

	/**
	  * @brief adjust position and direction of a player
	  */
	//该函数会通过坐标比较来改变dir方向，在AI中会调用到
	void adjust_position_ex(const Vector3D& newpos);
	//该函数只改变坐标，通过dir来设置方向，如果dir = -1表示不改变dir
	void adjust_position(const Vector3D& newpos, int dir = -1);
	/**
	  * @brief  get stage grade
	  */
	uint8_t get_stage_grade()
		{ return score.stage_grade;}
	
	/**
	  * @brief called when a player level up
	  */
	void on_lv_up();

	/**
	  * @brief pack chg basic info of a player to buf and returns the length of info that is packed
	  */
	int pack_player_attr_chg(uint8_t* buf);
	
	/**
	  * @brief pack basic info of a player to buf and returns the length of info that is packed
	  */
	int pack_basic_info(void* buf);

	/**
	  * @brief pack basic info of a player to buf and returns the length of info that is packed
	  */
	int pack_h_basic_info(void* buf);

	/**
	  * @brief for test
	  */
	int pack_player_attr(void* buf);

	/**
	  * @brief noti change attr to player
	  */
	void noti_chg_attr_to_player();
	/**
	  * @brief noti the hp mp infomation to map all
	  */
	void noti_hpmp_to_btl();

	/**
	  * @brief pack basic info of a player to buf and returns the length of info that is packed
	  */
	int pack_rt_birth_mon_basic_info(void* buf);


	/**
	  * @brief noti buf event infomation to map all
	  */
	void noti_buf_event_to_map(uint16_t buf_type, uint8_t flag, uint8_t up_flag);

	void noti_aura_event_to_map(uint16_t aura_type, uint8_t flag, uint8_t up_flag);
	/**
	  * @brief hp & mp change buf
	  */
	void hp_mp_chg_buf_event(int hp_chg, int mp_chg, uint32_t interval, uint32_t times);

	/**
	  * @brief per hp & mp change immediately
	  */
	void add_per_hp_mp_immediately(int per_hp_chg, int per_mp_chg);
	/**
	  * @brief summon anger noti
	  */
	void noti_summon_anger_to_player();
	//----------------------------------
	// about events' callbacks
	//----------------------------------
	/**
	  * @brief test a player's lag in each minutes
	  */
	int test_lag();
	/**
	  * @brief test if player use boos
	  */
	int judge_boost_plugin(uint8_t move_type);
	/**
	  * @brief test if player skip step
	  */
	int judge_skip_step_move(const player_move_t* mv);
	/**
	  * @brief set boost flag
	  */
	int judge_boost_flag();
	/**
	  * @brief set skip step flag
	  */
	int judge_skip_step_flag();
	/**
	  * @brief auto regenerate a player's hp and mp at each 10 secs
	  */
	int auto_regen_hpmp();
	/**
	  * @brief hp or mp of a player is changed
	  */
	int hpmp_chg(player_hpmp_chg_info_t& info);
	/**
	  * @brief atk or def of a player is changed
	  */
	int property_chg(uint32_t& buf_type);
	/**
	  * @brief set player invincible false
	  */
	int invincible_chg();
	/**
	  * @brief reduce a player's jewelry's duration at each 60 secs
	  */
	int reduce_jewelry_duration();
	
	/**
	  * @brief reduce summon monster's fight value at each 60 secs
	  */
	//int auto_reduce_fight_value();
	

	//----------------------------------
	// about player's summon monster
	//----------------------------------
	//

	uint32_t get_summon_resuscitate_time();
	/**
	  * @brief init summon info when battle restart
	  */
	void summon_init();
	
	/**
	  * @brief called when summon change attr
	  */
	void summon_attr_chg();
	
	/**
	  * @brief jugde if summon can fight
	  */
	bool if_can_fight();
	
	/**
	  * @brief jugde if summon can get exp
	  */
	bool if_can_get_exp();

	uint16_t get_limit_lv();
	
	bool is_boss()
		{
			if (!is_valid_uid(id) && mon_info->type == monster_boss) {
				return true;
			}
			return false;
		}

	void gain_coin(uint32_t coin);

	void reduce_exploit(uint32_t reduce_num);

	void add_exploit(uint32_t add_num);
public:
	//	USING_MEMPOOL_OPERATORS(Player);
	/**
	  * @brief set player objlist all dead
	  */
	void del_objlist();

public:
	void send_batlle_section();
	/**
	  * @brief add exp points to a player and level up if condition meets
	  */
	void add_exp(int gain_exp);

	/**
	  * @brief change a player's current hp and call 'set_dead' if the player's hp changes to zero
	  */
	void chg_hp(int hp_chg);
	/*
	 * @brief change a player's hp to max hp
	*/
	void full_hp();
	void recover_hp(uint32_t percent);	

	void fall_down(int seconds);

	void add_contest_exp(uint32_t stageid, uint32_t role_type, bool all_flag);
	/**
	  * @brief change a player's current mp
	  */
	void chg_mp(int mp_chg);
	/**
	  * @brief called each time when a player is killed by 'killer'
	  */
	void on_dead(Player* killer, bool can_get_exp = true);

	void suicide();


	void call_summon_after_dead();

	void call_summon_after_suicide();
	/**
	  * @brief called each time when a 'victim' is killed
	  */
	void on_player_killed(Player* victim, bool can_get_exp = true);

	bool is_in_no_exp_stage();
	/**
	  * @brief get exp from victim
	  */
 	void get_exp_from_victim(Player* victim);


	/**
	  * @brief called on all monsters each time when any 'p' leaves a map
	  */
	void on_player_leave_map(Player* p);

	/**
	  * @brief set player or monster pos after on hit
	  */
	void set_pos_after_onhit(const Player* atker, const skill_effects_t* p_skill_info, bool last_hit, uint32_t hit_flag);

	/**
	  * @brief test if this skill can use; if can, return the idx of the skills, or return -1;
	  */
	player_skill_t* can_use_skill(uint32_t skill_id, bool mp_use_flag);
	
	/**
	  * @brief test if this player can move,jump,stop
	  */
	bool can_action();

	/**
	  * @brief judge if 'target' can be hit
	  */
	bool is_hit(const Player* target, const skill_t* p_skill) const;
	
	/**
	  * @brief judge if a player can make a critical attack
	  */
	bool is_crit() const;


	bool is_invincibility();
	/**
	  * @brief pack the infomation of player on hit
	  */
	int pack_on_hit_info(uint8_t* buf, const Player*, const skill_simple_info*, uint32_t, uint32_t, const skill_effects_t*, bool, uint8_t);
	
	int pack_on_hit_info_ex(uint8_t* buf, const Player*, const skill_simple_info*, uint32_t, uint32_t, uint32_t, uint8_t, uint32_t,  bool,  uint8_t);

	int pack_on_hit_info(void * buf, on_hit_info_t * info);

	int calc_attack_impact(uint8_t* buf, Player* atker,  const skill_simple_info*, uint32_t, uint32_t, const skill_effects_t*, bool);
   /**
	 *  @brief notify damage for player 
	*/
	int notify_damage_by_buff(uint32_t atk_id, int32_t cur_hp, int32_t damage, uint32_t action = 0);

	player_skill_t*  get_skill(uint32_t skill_id);

	int notify_launch_skill_fail(uint32_t skill_id, uint32_t skill_lv);

	int notify_check_cheat();
	/**
	 * @brief notify player fall down
	 */
	int notify_fall_down(int seconds);

	uint32_t get_speed();

    int max_hp()const
	{
		return maxhp + hp_max_buf_trim + hp_max_pvp_trim;
	}
	
	void adjust_hp()
	{
		if( hp > max_hp()){
			hp = max_hp();
		}
	}

	void add_additional_hp(uint32_t add_hp)
	{
		hp += add_hp;
	}

	void add_additional_mp(uint32_t add_mp)
	{
		mp += add_mp;
	}
	
	bool is_hp_full()
	{
		return hp == max_hp();
	}

	int max_mp()
	{

		return maxmp + mp_max_buf_trim;
	}

	void change_show(uint32_t state = 0);

	void adjust_mp()
	{
		if(mp > max_mp()){
			mp = max_mp();
		}
	}
	//-- team btl info
	bool is_team_leader()
		{ return (team_job == team_member_type_leader); }

	/**
	  * @brief notify player ,it be kicked
	  */	
	int  notify_be_kicked();

	float get_rigidity_factor()
	{
		if(mon_info){
			return mon_info->rigidity_factor / 100.0; 
		}
		return 1.0;
	}


	/* just for test*/

	void add_all_monsters_buff(uint32_t buff_id);

	void proc_use_item_extra_logic(uint32_t item_id);

	void proc_move_extra_logic();

	void monster_stand_by_faint();
    void adjust_coordination();

	uint32_t add_skip_packet_count(uint32_t inc = 1)
	{
		cur_skip_packet_count = (cur_skip_packet_count + inc)%5;
		return cur_skip_packet_count ;
	}

	void rabbit_twinkling_move();

	void teleport(uint32_t x, uint32_t y);

	void send_player_move_noti(const Vector3D& new_pos, uint8_t move_type);
public:
	/*! player's id */
	userid_t	id;
	/*! player's parent id used in ambassador task*/
	userid_t	parentid;
	/*! the create time of the player's role */
	uint32_t	role_tm;
	/*! type of a player's role */
	uint32_t	role_type;
    /*! flag of power */
    uint32_t    power_user;
	/*! player's nick name */
	char		nick[max_nick_size];
	/*! vip flag */
	uint32_t	vip;
	/*! vip level */
	uint32_t    vip_level;
	/*! max bag grid count*/
	uint32_t    max_bag_grid_count;
	/*! player's money */
	uint32_t	coins;
	uint32_t	pre_coins;

	/*! max skip packet count*/
	uint32_t     cur_skip_packet_count;
	//----------------------------------
	// player's battle attribute
	//
	/*! player's level */
	uint16_t	lv;
	/*! player's experience */
	uint32_t	exp;
	uint32_t	allocator_exp;
    /*! the left time of double experience*/
	uint32_t	dexp_tm;
	/*! 0: regular experience;1: only half experience*/
	uint8_t half_exp;
    /*! */
    uint8_t     boss_killer_flg;
	/*! skill points */
	int 		pre_skill_point;
	int			skill_point;
	/*! strength */
	uint16_t	strength;
	/*! agility */
	uint16_t	agility_;
	uint16_t	agility() {
		return agility_ + agility_buf_trim;
	}
	/*! body quality */
	uint16_t	body_quality_;
	uint16_t	body_quality() {
		return body_quality_ + body_quality_trim;
	}
	/*! stamina */
	uint16_t	stamina;
	/*! player's current hp */
	int			hp;
	/*! player's current maxhp */
	int			maxhp;
	/*! player's clothes addhp */
	int			clothes_addhp;
	/*! player's current mp */
	int			mp;
	/*! player's max mp */
	int			maxmp;

	int			attire_gs;
    uint32_t    achieve_title;
	/*! player's show state */
	uint32_t	show_state;

    /*! player taotai game information */
    uint32_t    taotai_do_times;

    uint32_t    taotai_win_times;

	uint32_t    team_lv;

	uint32_t    pass_cnt;

	uint32_t    lucky_cnt;

	/*! player's  rigidity_factor*/
	uint32_t rigidity_factor;
	/*! call map summon when player after dead */
	uint32_t dead_call_summon;
	/*! speed */
	uint32_t speed;
	
	uint32_t suicide_call_summon;


	/*! fumo points got in today*/
	uint32_t	fumo_points_end;
	uint32_t	fumo_points_start;
	uint32_t	fumo_tower_top;
	uint32_t 	fumo_tower_used_tm;
	uint32_t	cur_tower_layer;

	uint32_t	damage;

	/*! record the time of loading stage*/
	uint32_t 	load_tm;
        
        //player magic find and gold greed attr
        uint32_t        mf_rate; 
        uint32_t        greed_rate;
        
        uint32_t        buf_mf_rate;
        uint32_t        buf_greed_rate;
       

	//pvp attr
	/*! player continue win count*/
	uint32_t 	continue_win;
	uint8_t		btl_again;
	uint32_t 	honor;
	uint32_t 	exploit;
	uint32_t 	max_conti_win_times;
	uint8_t		time_out_lv;

	/*! player's clothes addmp */
	int			clothes_addmp;
	/*! player's pure attack power */
	uint32_t	atk;
	/*! weapon's attack power */
	uint16_t	weapon_atk[2];	
	/*! critical attack rate */
	uint16_t	crit_rate;
	/*! defence rate */
	float		def_rate;
	/*! defence value */
	//now there are 2 attributes of defence in player  one is defence value, the other is defence rate
	uint32_t    def_value;
	/*! def threshold value only used for monster*/
	uint32_t    def_threshold_value;
	uint32_t    atk_threshold_value;
	/*! add attribute for pet*/
	uint32_t    hit_value;      //命中值
	uint32_t    dodge_value;    //躲闪值
	uint32_t    accurate_value; //精准值
	uint32_t    crit_value;     //暴击值
	/*! hit rate */
	float		hit_rate;
	/*! dodge rate */
	float		dodge_rate;

    /*! god guard */
    quality_t   add_quality;
	/*! current map of a player */
	map_t*		cur_map;
	/*! player's move direction,left: 0 , left up 4, up 2, right up 6, right 0, right down 7, down 3, left down 5*/
	uint8_t		dir;
	/*! player's body direction, right: 0, left: 1 */
	uint8_t		body_dir;
	/*! team that the player belongs to */
	uint8_t		team;

	/*! suit attr of player*/
	suit_add_attr_t suit_add_attr;
	
	/*! clothes' info */
	player_clothes_t clothes_info;

	/*! summon monster */
	Player*			my_summon;
	summon_mon_t 	summon_info;

    uint32_t    common_flag_;

	Player*			my_numen;
	numen_info_t	numen_info;
	
	uint16_t	get_max_fight_value()
	{
		return 100 + mp_max_buf_trim;
	}
	struct timeval  mon_update_tv;

	void  add_pet_fight_value(uint32_t add_value);
	int  revive_pet();
	/*! the monster info , if the player is monsters*/
	const monster_t*		mon_info;

	/*! if the boss of monster can super armor*/
	bool  super_armor;
	
	/*! an instance of the state machine class */
	AIInterface* i_ai;

	/*! judge if player use plugin */
	judge_boost_plugin_t	boost_info;
	judge_skip_step_t		skip_step_info;
;	
	/*! lag in milliseconds */
	uint16_t	lag;
	/*! lag in average */
	uint16_t	avg_lag;
	/*! time when a test was started */
	timeval		test_start_tv;
	/*! seqno of a test */
	uint32_t	test_seq;

	/*! battle status */
	uint32_t	btl_status;

	/*! battle */
	Battle*		btl;

	/** 
	 * @brief watcing btl
	 */
	Battle*    watch_btl;

	/*! the unique id of the item */
	uint32_t	cur_item_picking;

	uint32_t    cur_picking_summon;

	/*! gobal cool time */
	timeval		g_cool_time;
	/*! the time on hit */
	timeval		stuck_end_tm;
	/*! the time fall fly end */
	timeval		flying_end_tm;

	/*! invincible statue */
	bool		invincible;
	uint16_t	invincible_time;
	bool		undead_flag;

    /*! final contest over parament */
    uint16_t    revive_count;
    uint32_t    dead_start_tm;

	/*! ai */
	bool		hit_fly_flag;
	uint32_t	damage_ration;

	/*! 这个变量只是记录单位时间内攻击次数的，用来检测是否有外挂*/
	uint32_t    	  attack_times;
	struct timeval    attack_timestamp;
	/*! 这个变量只是记录单位时间内MOVE次数，用来检测是否有外挂*/
	uint32_t    move_times;
	struct timeval    move_timestamp;
	/*! cheat的阀值*/
	uint32_t    cheat_value;

	uint32_t 	tmp_team_id;
	uint32_t	tmp_team_leader;

	void init_cheat_check_data()
	{
		attack_times = 0;
		memset(&attack_timestamp, 0, sizeof(attack_timestamp));
		move_times = 0;
		memset(&move_timestamp, 0, sizeof(move_timestamp));
		cheat_value = 0;
	}


	/*! point to current skill launched */
	Skill*		p_cur_skill;
	/*! the skill player can use , including normal attack ,which is a special skill*/
	SkillMap	skills_map;
	BufSkillMap	buf_skill_map;

	/*!每次被击飞后改变量被赋值为1，用来处理时序BUG的stop, 或者move后该变量赋值为0*/
	uint32_t    fly_flag;
	void        set_fly_flag(uint32_t flag)
	{
		fly_flag = flag;
	}	
	uint32_t   get_fly_flag()
	{
		return fly_flag;
	}

	/*!home btl pets */
	std::vector<uint32_t>  home_btl_pets;
	void add_home_btl_pets(uint32_t pet_id)
	{
		home_btl_pets.push_back(pet_id);
	}
	void clear_home_btl_pets()
	{
		home_btl_pets.clear();
	}


	//buf trim
	int 		critical_max_damage_buf_trim;
	int 		body_quality_trim;
	int 		agility_buf_trim;
	int 		atk_buf_trim;
	float		def_buf_trim;
	int	 		def_value_buf_trim;
	float		hit_buf_trim;
	float		crit_buf_trim;
	float		dodge_buf_trim;
	int 		hp_max_buf_trim;
	int 		hp_max_pvp_trim;
	int 		mp_max_buf_trim;
	float       exp_factor_trim;
	float		summon_mon_exp_factor_trim;
	float		player_only_exp_factor_trim;
	float       skill_cd_time_buf_trim;
	float       skill_mana_buf_trim;
	float       atk_damage_change_buf_trim;
	float       sustain_damage_change_buf_trim;
	int			sustain_damage_value_change_buf_trim;
	float       speed_change_buf_trim;
	//tmp buff
	uint32_t	tmp_buff_flag;


	//------------------------------/*! the item need to collect for a task */
	TaskItemMap task_item_map;

	TaskItemList stage_task_list;
	/*! the monsters need to kill for a task */
	TaskMonMap task_mon_map;
	/*! the tasks buf */
	//TaskMonVec	task_mon_vector;
		
	/*! sign some one-off event  if hapended */
	uint8_t		once_bit[max_once_bit_size];
	/*! the items that player have got */
	uint8_t		unique_item_bit[20];

	/*! the info of hpmp auto add */
	player_hpmp_add_t	auto_add_hpmp;

	/*! the score of finish a stage */
	stage_score_t	score;

	detective_info_t detective_info;
	/*! be hit count*/
	uint32_t be_hit_count;
	void reset_be_hit_count()
	{
		be_hit_count = 0;
	}
	
	void inc_be_hit_count()
	{
		be_hit_count++;
		be_attacked_cnt ++;
	}
	uint32_t get_be_hit_count()
	{
		return be_hit_count;
	}


	uint32_t    be_attacked_cnt;
	uint32_t    use_skill_cnt;
	/*! the record of box opened */
	bool	can_open_box;
	uint8_t	normal_box_opened;
	uint8_t	vip_box_opened;

	/*! save cmd id that is currently under processing */
	uint16_t	waitcmd;
	GQueue*		pkg_queue;

	/*! hook that is used to link all players whose waitcmd is not 0 together */
	ObjectHook	awaiting_hook;
	/*! a hook to link all monsters together */
	ObjectHook	monhook;

	taomee::TimedEvent* time_ev;
	taomee::TimedEvent* pvp_end_ev;

	/*! save socket fd from parent process */
	int				fd;
	/*! fd session */
	fdsession_t*	fdsess;
	
	/*! true if we need to save a player's attr */
	bool	need_sav_;

	/*! player's bufflist*/
	std::list<buff*> m_bufflist;
	/*! player's auralist*/
	std::list<aura*> m_auralist;
	
	int pack_status_info(uint8_t* buf);

	bool in_specal_double_time;
	bool in_specal_double_time_2;
	bool in_specal_double_time_witch_can_still_use_double_tm_item;
private:
	/*! player's job in the btl team */
	uint8_t team_job;
	/*! flag show that if the player in team btl */
	uint8_t	team_btl_flg;
	
	/*! link all the projectiles launched by a player */
	ObjList	objlist_;
};

/**
  * @brief type of member option for type of 'MonList'
  */
typedef boost::intrusive::member_hook<Player, ObjectHook, &Player::monhook> MonHookOption;
/**
  * @brief type of an intrusive object list
  */
typedef boost::intrusive::list<Player, MonHookOption, boost::intrusive::constant_time_size<false> > MonList;

/*! an intrusive list to link all monsters together */
extern MonList g_monlist;

/**
 * @brief type of player list who have summon monster
 */
typedef std::list<Player*> PlayerList;

class MonlistMrg {
public:
	enum {
		max_monlist_arr_cnt		= 3,
	};
public:
	
	void update_monlist(int time_elapsed) {

		const timeval* tv = get_now_tv();

		int cur_time_elapsed = timediff2(*tv, mon_update_tv_arr[cur_update_idx]);
		
		TRACE_LOG("UPDATE MONLIST idx %u tm elapsed %u %lu %lu", cur_update_idx, cur_time_elapsed, tv->tv_sec, tv->tv_usec);
		mon_update_tv_arr[cur_update_idx] = *get_now_tv();
		MonList::iterator nx = p_cur_monlist->begin();
		for (MonList::iterator it = nx; it != p_cur_monlist->end(); it = nx) {
			++nx;
			if (!it->is_dead()) {
				it->update(cur_time_elapsed);

			} else {
				delete &(*it);
			}
		}
		cur_update_idx++;
		cur_update_idx = cur_update_idx % max_monlist_arr_cnt;
		p_cur_monlist = g_monlist_arr + cur_update_idx;
	}
	void insert_monlist(Player& mon) {

		TRACE_LOG("INSERT MONLIST idx %u ", cur_insert_idx);
		g_monlist_arr[cur_insert_idx].push_back(mon);
		cur_insert_idx++;
		cur_insert_idx = cur_insert_idx % max_monlist_arr_cnt;
	}

	MonlistMrg() {
		cur_insert_idx = 0;
		cur_update_idx = 0;
		p_cur_monlist = g_monlist_arr + cur_update_idx;

		for (uint32_t i = 0; i < max_monlist_arr_cnt; i++) {
			mon_update_tv_arr[i] = *get_now_tv();
		}
	}
private:
	MonList g_monlist_arr[max_monlist_arr_cnt];
	static struct timeval mon_update_tv_arr[max_monlist_arr_cnt];
	MonList* p_cur_monlist;

	uint32_t cur_update_idx;
	uint32_t cur_insert_idx;
};

extern MonlistMrg g_monlist_mrg;
/*! an intrusive list to link all player who have summon monster together */
//extern PlayerList g_summon_list;
//---------------------------------------------------------------------------
inline bool is_boss_type(uint32_t type) 
{
	return (type == monster_boss);
}

inline bool is_summon_mon(userid_t role_type) 
{
	return (role_type < 2000 && role_type > 1000);
}


inline void
Player::launch_skill(player_skill_t* player_skill, uint32_t assign_x, uint32_t assign_y)
{
	Skill* p_skill = new Skill(this, player_skill->skill_id, player_skill->lv, player_skill->depend_damage_active_flag, assign_x, assign_y);


	if( is_valid_uid(id) && p_skill->get_skill_type() == normal_skill)//如果是玩家的普通攻击技能则不修改p_cur_skill
	{
		//do_nothing
	}
	else
	{
		p_cur_skill = p_skill;
	}

	skill_t *p_skill_config = g_skills->get_skill(player_skill->skill_id);
	if (p_skill_config->skill_begin_handle) {
		p_skill_config->skill_begin_handle(this, p_skill_config->skill_begin_handle_agrv);
	}
	
	objlist_.push_back(p_skill);
	g_skill_list.push_back(p_skill);

}

inline void
Player::adjust_position_ex(const Vector3D& v)
{
	Vector3D newpos = v;
	int tmp = newpos.x() - pos().x();
	if(tmp > 0){
		dir = dir_right;
	}else if(tmp <0){
		dir = dir_left;
	}
	set_pos(newpos);
}


inline void
Player::adjust_position(const Vector3D& v, int new_dir)
{
	Vector3D newpos = v;
	/*
	int tmp = newpos.x() - pos().x();
	if (tmp > 0) {
		dir = dir_right;
	} else if (tmp < 0) {
		dir = dir_left;
	}
	*/
	if(new_dir != -1){
		dir = new_dir;
	}
	if (new_dir == 6 || new_dir == 0 || new_dir == 7) {
		body_dir = 0;
	}
	if (new_dir == 4 || new_dir == 1 || new_dir == 5) {
		body_dir = 1;
	}
	set_pos(newpos);
}

inline bool
Player::in_battle() const
{
	return (taomee::test_bit_on(btl_status, player_in_battle) && !is_dead());
}

inline bool
Player::is_hit(const Player* target, const skill_t* p_skill) const
{

	int hit = (hit_rate + hit_buf_trim) * (target->dodge_rate - target->dodge_buf_trim) * (lv + 15) / (target->lv + 15) * 100;
	TRACE_LOG("[%u %u]:[%f %f %u %u]",id, target->id, hit_rate, target->dodge_rate, lv, target->lv);

	if (is_valid_uid(target->id) || is_summon_mon(target->role_type)) {
		if (hit < 30) {
			hit = 30;
		}
	}

//	if(p_skill && p_skill->type != normal_skill)
//	{
//		return true;
//	}
//
	return (rand() % 100) < hit;
}

inline bool
Player::is_crit() const
{
	int  crit = crit_rate + crit_buf_trim;
	if (is_valid_uid(id) || is_summon_mon(role_type)) {
		if (crit > 70) {
			crit = 70;
		}
	}
	return (rand() % 100) < crit;
}



inline void
Player::full_hp()
{	
	if (hp < max_hp()) {
		hp = max_hp();
		noti_hpmp_to_btl();
	}
}

inline void 
Player::recover_hp(uint32_t percent)
{
	uint32_t add_hp = max_hp() *  ( (float)percent / 100.0);
	hp += add_hp;

	if( hp > max_hp()){
		hp = max_hp();
	}
	noti_hpmp_to_btl();
}

inline void
Player::chg_hp(int hp_chg)
{
	hp += hp_chg;
	if (hp > max_hp()) {
		hp = max_hp();
	} else if (hp <= 0) {
		if (undead_flag) {
			hp = 1;
		} else {
			hp = 0;
			set_dead();
			p_cur_skill = NULL;
			del_objlist();
			if (!is_valid_uid(id)) {
				ev_mgr.remove_events(*this);
			}
		}
	}
}

inline void
Player::chg_mp(int mp_chg)
{
	mp += mp_chg;
	if (mp > max_mp()) {
		mp = max_mp();
	} else if (mp < 0) {
		mp = 0;
	}
}

inline bool
Player::if_can_fight()
{
	//return (summon_info.fight_value > min_fight_value);
	return true;
}


//--------------------------------------------------------------------


/**
  * @brief allocate and add a player to this server
  * @return pointer to the newly added player
  */
Player* add_player(userid_t uid, fdsession_t* fdsess);
/**
  * @brief delete player
  * @param p player to be deleted
  */
void del_player(Player* p);

/**
  * @brief set need_sav_ false 
  * @param p player 
  */
void set_unneed_save(Player* p);

/**
  * @brief clear players' info whose online server is down
  * @param fd online fd
  */
void clear_players(int fd = -1);
/**
  * @brief get player by user id
  * @param uid user id
  * @return pointer to the player if found, 0 otherwise
  */

//--------------------------------------------------------

/**
 * @brief add here for platform of statstics
 */
inline void do_stat_log_achieve_child_num(Player* p)
{
    uint32_t buf[1];
	buf[0] = p->id;
	msglog(statistic_logfile, stat_log_achieve_child_num, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [buf=%u]",
        stat_log_achieve_child_num, buf[0]);
}
/**
 * @brief add here for platform of statstics
 */
inline void do_stat_log_boost_num(uint32_t type)
{
    uint32_t buf[1];
	buf[0] = 1;
	msglog(statistic_logfile, stat_log_boost_num + type, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [buf=%u]",
        stat_log_boost_num, buf[0]);
}
/**
 * @brief add here for platform of statstics
 */
inline void do_stat_log_role_exp(uint32_t uid, uint32_t role_type, uint32_t exp)
{
    uint32_t buf[2] = {0};
    buf[0] = uid;
    buf[1] = exp;
    uint32_t cmd = stat_log_role_exp;
    msglog(statistic_logfile, cmd + role_type, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [type=%u] buf[%u, %u]", cmd, role_type, buf[0], buf[1]);
}


inline bool is_in_not_use_double_tm_time(Player *p)
{
	if (p->in_specal_double_time || p->in_specal_double_time_2) {
		return true;
	}
	return false;
}

inline bool is_player_dir_right(Player* p)
{
	return p->body_dir == 0;
}
//--------------------------------------------------------
/**
  * @brief set a player's unique item bit
  */
int db_set_unique_item_bit(Player* p);

/**
  * @brief set a player's lv, exp, after lvup
  */
int db_set_player_base_info(Player* p);


/**
  * @brief set a player's lv, exp, sp, hp and mp
  */
int db_set_player_basic_attr(Player* p);

/**
  * @brief set a player's lv, exp, sp, hp and mp
  */
int db_set_summon_basic_attr(Player* summon, Player* owner);

/**
  * @brief set a player's pvp info
  */
int db_set_player_pvp_info(Player* p, uint32_t win_flg, uint32_t failed_flg);

/**
  * @brief set a player's task buf
  */
int db_set_player_task_buf(Player* p);

/**
  * @brief set a player kill boss
  */
int db_set_player_kill_boss(Player* p, uint32_t monsterid);

/**
  * @brief add ambassador achieve num
  */
int db_add_amb_achieve_num(Player* p, uint32_t parentid);

/**
  * @brief set user flag
  */
int db_set_user_flag(Player* p);

/**
  * @brief callback for using a item
  * @param p the player who initiated the request to dbproxy
  * @param id the requester id
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_player_pick_item_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief get fumo points
  */
uint16_t get_fumo_points(uint32_t difficulty, uint32_t stage_grade, uint16_t lv);

/**
  * @brief get fumo points in fumo tower
  */
uint16_t get_tower_fumo_point(uint32_t layer, uint32_t player_lv, uint32_t stage_grade);

/**
 * @brief get player by uid
 */
Player* get_player(userid_t uid);

/**
 * @brief player routing
 */
void player_map_routing(struct timeval cur_time);


/**
 *@brief player timer
 */
void player_timer(Player* p, struct timeval cur_time);


void notify_delete_player_obj(Player* p);


void init_watch_list();

bool is_player_in_watch_list(uint32_t uid);

int is_enemy(const Player * atker, const Player * target);
#endif // KFBTL_PLAYER_HPP_

