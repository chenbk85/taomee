#ifndef KFBTL_MONSTER_HPP_
#define KFBTL_MONSTER_HPP_

/**
 *============================================================
 *  @file      monster.hpp
 *  @brief    monsters
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

extern "C" {
#include <stdint.h>
}

#include <map>
#include <libtaomee++/conf_parser/xmlparser.hpp>

#include "object.hpp"
#include "fwd_decl.hpp"
#include "mechanism_drop.hpp"
//------------------------------------------------------------------
// monster
//------------------------------------------------------------------
typedef std::map<uint32_t, ItemDrop> ItemDropMap;

struct task_drop_t {
	uint32_t	task_id;
	uint32_t	item_id;
	uint32_t	drop_odds;
	uint32_t    max_odds;
};

struct other_drop_t {
	uint32_t	item_id;
	uint32_t	drop_odds_upper;
	uint32_t	drop_odds_lower;
	uint32_t    max_odds;
};

struct clothes_drop_t {
	uint32_t	drop_lv;
	uint32_t	drop_id;
	uint32_t	drop_odds_upper;
	uint32_t	drop_odds_lower;
};

struct items_drop_t {
	uint32_t	drop_lv;
	uint32_t	drop_odds_upper;
	uint32_t	drop_odds_lower;
	uint32_t    max_odds;
};

struct unique_item_drop_t {
	uint32_t	item_id;
	uint32_t	role_type;
	uint32_t	bit_pos;
	uint32_t	drop_odds_upper;
	uint32_t	drop_odds_lower;
	uint32_t    max_odds;
};

struct mob_drop_t
{
	uint32_t mob_id;
	uint32_t drop_odds_upper;
	uint32_t drop_odds_lower;
	uint32_t max_odds;
};

struct buff_drop_t {
	uint32_t	buff_id;
	//uint32_t	buff_lv;
	uint32_t	drop_odds_upper;
	uint32_t	drop_odds_lower;
	uint32_t    max_odds;
};

struct unique_item_pos_t {
	uint32_t	item_id;
	uint8_t		bit_pos;
	uint32_t    max_odds;
	unique_item_pos_t(uint32_t id, uint32_t pos): item_id(id), bit_pos(pos) {
	}
};

struct card_drop_t {
	uint32_t item_id;
	uint32_t drop_odds_upper;
	uint32_t drop_odds_lower;
	uint32_t max_odds;
};

struct item_drop_t {
	std::map<uint32_t, task_drop_t>		task_drop;

	uint32_t		lucky_drop_num;
	other_drop_t	lucky_drop[max_drop_num]; 

	uint32_t		material_drop_num;
	other_drop_t	material_drop[max_drop_num]; 
	
	uint32_t		clothes_drop_num;
	clothes_drop_t	clothes_drop[max_drop_num];

	uint32_t		items_drop_num;
	items_drop_t	items_drop[max_drop_num];

	uint32_t		unique_items_drop_num;
	unique_item_drop_t unique_items_drop[max_drop_num];
	
	uint32_t		buffs_drop_num;
	buff_drop_t		buff_drop[max_drop_num];

	uint32_t        mechanism_drop_id;
	mechanism_drop_data  *p_data;	

	uint32_t        card_drop_num;
	card_drop_t    card_drop[max_drop_num];

};

enum item_state_t {
	can_pick = 0,
	cannot_pick
};

enum {
	monster_max_num	= 40000,
	max_quality		= 6,
};

enum {
	monster_attr_type_std 	   = 1,
	monster_attr_type_high_atk = 2,
	monster_attr_type_high_hp  = 3,
	monster_attr_type_boss	   = 4,
};
class ItemDrop : public Object {
public:
	/**
	  * @brief constructor
	  * @param item_id id of drop item
	  */
	ItemDrop(uint32_t item_id, userid_t uid = 0)
	{ 
		item_id_ = item_id;
		item_state_ = can_pick;
		owner_userid = uid;
   	}
	
	/**
	  * @brief default destructor
	  */
	~ItemDrop() {}

	/**
	  * @brief get the item_id_
	  */
	uint32_t item_id() const
		{ return item_id_; }

	/**
	  * @brief get the item_state_
	  */
	uint32_t item_state() const
		{ return item_state_; }

	/**
	  * @brief set the item_state_
	  */
	void set_state(uint32_t state)
		{ item_state_ = state; }
	
	/**
	  * @brief pack the infomation of the item
	  */
	int pack_item_info(uint8_t* buf);

	/**
	  * @brief judge if player can see this drop item
	  */	
	bool can_be_saw_by_player(userid_t uid)
		{ return (!owner_userid || owner_userid == uid); }

	/**
	  * @brief judge if all players can see this drop item
	  */	
	bool can_be_saw_by_all()
		{ return (!owner_userid); }	
	/**
	  * @brief get owner id
	  */	
	userid_t get_owner_id()
		{ return owner_userid; }	

	/**
	  * @brief set user who the drop item belong to
	  */	
	void set_owner(userid_t uid)
		{ owner_userid = uid; }
private:
	/*! ID of this drop item */
	uint32_t	item_id_;
	/*! state of this drop item */
	uint32_t	item_state_;
	/*! user who own this drop item*/
	userid_t	owner_userid;
};

struct mon_skill_t {
	uint32_t	skill_id;
	uint32_t	lv;
};

struct monster_t {
	uint32_t	id;
	char		name[max_nick_size];
	uint16_t	flag;
	uint16_t	type;
	uint8_t		attr_type;
	uint16_t	lv;
	uint8_t		long_range_atk;
	uint32_t    cur_hp;
	uint32_t	hp;
	uint16_t	mp;
	uint32_t	atk;
	float		def_rate;
	uint32_t    def_threshold_value;
	uint32_t    atk_threshold_value;
	uint32_t    team;
	float		dodge_rate;
	float		hit_rate;
	uint16_t	exp;
	uint16_t	sp;
	uint16_t	spd;
	int			visual_field;
	int			visual_field_sqrt;
	int16_t		len;
	int16_t		width;
	uint16_t	height;

	uint32_t	atk_duration;

	uint16_t	stage_diff;
	uint8_t		activity_stage;
	uint16_t	undead;
	uint32_t    aura_id;
	uint8_t		no_world_drop;
	int			skill_num;
	uint32_t    rigidity_factor;// 被攻击的硬直系数，默认为100
	uint32_t    dead_call_summon;// 死亡后召唤一些特殊效果,默认为0
	uint32_t    suicide_call_summon;//自杀后召唤一些特殊效果,默认为0
	uint32_t    makemoney; //can drop gold, default 1;

	mon_skill_t	skills[player_max_skill_num];

	item_drop_t	drops;
};

typedef std::map<uint32_t, monster_t> MonstersMap;

int create_monsters(map_t* m, Battle* btl);

int call_monster_to_map(map_t* m, Battle* btl, uint32_t mon_id, int x, int y, int team = 3, int add_buff_id = 0, int add_aura_id = 0);

const monster_t* get_monster(uint32_t id, uint32_t diffculty = 1);

void monster_killed_task(Player* p, Player* killer);

void monster_drop_item_to_player(Player* p, Player* killer);

void monster_drop_amber_to_player(Player* killer, int x, int y);

void monster_drop_item(Player* p, Player* killer);

bool mechanism_drop_to_all_players(Player* p_monster);

void treasure_box_drop_items(Player* p, uint32_t item_id, uint32_t item_cnt);

int pack_mon_attr(uint32_t monid, void* buf);

int pack_item_drop(Player* p, uint32_t item_id, uint8_t* buf, const KfAstar::Points* points, userid_t owner_uid= 0);

int pack_item_drop(map_t* map, uint32_t item_id, uint8_t* buf, const KfAstar::Points* points, userid_t owner_uid= 0);

void calc_mon_attr(monster_t* p_out_mon, Player* p, uint16_t mon_lv, uint32_t stage_diff, uint32_t attr_type);

//------------------------------------------------------------------
// monster
//------------------------------------------------------------------a
#ifdef DEV_SUMMON
#pragma pack(1)

struct summon_skill_info_t {
	uint32_t    skill_id;
	uint16_t    skill_lv;
	uint8_t     used_flag;
};

#pragma pack()


struct summon_mon_t {
	Player* 	owner;

	bool		active_flag;
	
	uint32_t	mon_type;
	uint32_t	mon_tm;
	uint16_t	mon_lv;
	uint32_t	mon_exp;
	char		nick[max_nick_size];
	uint16_t	max_fight_value;
	uint16_t	fight_value;
	uint16_t	anger_value;
	uint16_t    renascence_time;
	uint32_t    attr_type;
	uint32_t    attr_per;	
	uint16_t	skills_cnt;
	summon_skill_info_t	skills[max_summon_skills];
	//uint32_t    disable_skills[max_summon_skills];
	
	uint32_t trigger_skill_id;

	bool add_anger(uint32_t add_value) 
	{
		if (!is_final_state() || anger_value >= 100) {
			return false;
		}
		anger_value = (anger_value + add_value > 100) ? 100 : anger_value + add_value;
		return true;
	}

	bool is_final_state()
	{
		return ((mon_type % 10) >= 5);
	}

};

Player*	create_summon(Player* owner);

Player*	create_numen(Player* owner);

#endif

class SpecialDropInfo {
	public:
		uint32_t stage_id;
		uint32_t mon_id;
		uint32_t drop_time_id;
		uint32_t drop_odds;
		uint32_t item_id;
		uint32_t buff_id;
		uint32_t mon_type;

	public:
		SpecialDropInfo()
		{
			stage_id = 0;
		 	mon_id = 0;
			drop_time_id = 0;
			drop_odds = 0;
            buff_id = 0;
			mon_type = 0;
		}

		bool CanDropByMonster(Player * p);
};


class SpecialDrop{
	public:
		SpecialDrop()
		{
			
		}
		
		std::vector<SpecialDropInfo> drop_info_arr;

		void init(const char* xml_file);
		int load_special_drop(xmlNodePtr cur);
};

class SpecialDropsMrg : public SpecialTimeMrg{
	private:
	//	SpecialTimeMrg time_limit_mrg;
		std::map<uint32_t, SpecialDrop> special_drop_map;
		
	public:
		int pack_special_drop(Player * p, Player * killer, const  KfAstar::Points * points, uint8_t * buf, uint32_t& drop_num);
		
		void init();
		void initSpecialDrops(uint32_t stage_mode);
	public:
		SpecialDropsMrg()
		{
			
		}
};



/** 
 * @brief  区域随机命中判断
 * 
 * @param low_case 区间下限
 * @param uper_case 区间上限
 * @param luck_val 幸运影响值
 * @param rand_seed 总区间长度
 * 
 * @return 
 */
inline bool judge_luck(uint32_t low_case,
	  				  uint32_t uper_case,
					  uint32_t rand_seed,
					  uint32_t luck_val = 0)
{

	uint32_t rand_value = rand() % rand_seed;

	uint32_t dis = ((uper_case - low_case) / 100 ) * (luck_val + 100) ;

	if ( rand_value < dis ) {
		return true;
	}	

	return false;
}

inline bool is_player(uint32_t role_type)
{
	if (role_type > 4) {
		return false;
	}
	return true;
}


void update_challenge_attr(Player * p, Battle * btl);

void update_challenge_ai(Battle * btl);

extern SpecialDropsMrg special_drop_mrg;


//------------------------------------------------------------------
// function
//------------------------------------------------------------------

/**
  * @brief load monster configs from an xml file
  * @return 0 on success, -1 on error
  */
int load_nor_monsters(xmlNodePtr cur);

int load_easy_monsters(xmlNodePtr cur);

int load_hard_monsters(xmlNodePtr cur);

int load_hell_monsters(xmlNodePtr cur);

int load_tower_monsters(xmlNodePtr cur);

int load_epic_monsters(xmlNodePtr cur);

int load_legend_monsters(xmlNodePtr cur);


int load_easy_quality(xmlNodePtr cur);

int load_nor_quality(xmlNodePtr cur);

int load_hard_quality(xmlNodePtr cur);

int load_hell_quality(xmlNodePtr cur);

int load_tower_quality(xmlNodePtr cur);

int load_epic_quality(xmlNodePtr cur);

int load_legend_quality(xmlNodePtr cur);

/**
  * @brief load quality configs from an xml file
  * @return 0 on success, -1 on error
  */
int load_quality(xmlNodePtr cur, uint32_t stage_mode);

/**
  * @brief load quality configs from an xml file
  * @return 0 on success, -1 on error
  */
int reload_quality();

/**
  * @brief load item drop configs from an xml file
  * @return 0 on success, -1 on error
  */
int load_item_drop(xmlNodePtr cur, item_drop_t* p_drops);


/**
  * @brief reload monsters configs from an xml file
  * @return 0 on success, -1 on error
  */
int reload_monsters();

void set_monster_attr(Player* p, const monster_t* mon, Battle* btl, uint16_t mon_lv);


int calc_monster_gold_drop(Player *p, Player * killer);

bool player_teleport(Player* player, int x, int y);



#endif // KFBTL_MONSTER_HPP_
