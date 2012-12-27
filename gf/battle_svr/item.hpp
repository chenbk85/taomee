/**
 *============================================================
 *  @file      item.hpp
 *  @brief    item related functions are declared here.
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_ITEM_HPP_
#define KFBTL_ITEM_HPP_
extern "C" {
#include <libtaomee/project/stat_agent/msglog.h>
}

#include "fwd_decl.hpp"
#include <kf/item_impl.hpp>
#include "cli_proto.hpp"
#include "stage.hpp"


//--------------------------------------------------------------------------------
// struct
//--------------------------------------------------------------------------------
enum max_t {
	max_box_quality = 6,
	max_box_lv		= 5,
	max_box_items	= 10,
	max_treasure_box_cnt = 100,
	max_stage_box_cnt = 5,
	vip_max_lv = 10,
	score_max_cnt = 7,
	max_holiday_drop_type = 100,
};

enum box_type_t {
	normal_box = 1,
	vip_box = 2,
};

enum give_type_t {
	give_type_clothes = 1,
	give_type_normal_item = 2,
	give_type_player_attr = 3,
	give_type_skill = 4,
	give_type_max,
};

enum{
	give_type_xiaomee = 1,
	give_type_exp = 2,
	give_type_skillpoint = 3,
	give_type_fumo_point = 4,
};

enum duration_loss_per_t {
	weapon_loss = 1,
	armor_normal_loss = 10,
	armor_crit_loss = 2 * clothes_duration_ratio,
	armor_boss_loss = 2 * clothes_duration_ratio,
	jewelry_loss = 12,
};

enum duration_state_t {
	no_loss = 0,
	loss_50_per  = 50,
	loss_70_per  = 70,
	loss_80_per  = 80,
	loss_100_per = 100,
};

enum duration_type_t {
	weapon = 1,
	armor_normal,
	armor_crit,
	armor_boss,
	jewelry,
	all_clothes,
};

struct suit_addition_t {
    uint32_t    skill_id;
    uint32_t    cut_cd;
    uint32_t    cut_mp;
    uint32_t    attr_dmg;
    uint32_t    extra_dmg;
};

struct suit_step_t {
	uint32_t attire_num;
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
    //addition attr 
    suit_addition_t addition;
};

struct suit_t {
	uint32_t id;
	uint32_t cnt;
	suit_step_t suit_step[max_suit_step + 1];
};

struct weared_suit_t {
	uint32_t suit_id;
	uint32_t suit_cnt;
};

struct holiday_drop_item_t {
	uint32_t itemid;
	uint16_t mon_lv[2];
	uint32_t drop_odds;
};

struct holiday_drop_t {
	uint8_t  start_flg;
	uint32_t start_time;
	uint32_t end_time;
	uint32_t cnt;
	holiday_drop_item_t drop_item[10];
};

#pragma pack(1)

/*! treasure box simple info */
struct treasure_box_item_t {
	uint32_t give_type;
	uint32_t give_id;
	uint32_t give_cnt;
	uint32_t equip_part;
	uint32_t odds_lower;
	uint32_t odds_upper;
};

struct treasure_box_t {
	uint32_t	id;
	//uint32_t	lv;
	uint32_t	quality;
	uint32_t	items_cnt;
	treasure_box_item_t items[max_box_items];
};

struct box_id_info_t {
	uint32_t	box_cnt;
	uint32_t	box_id[max_stage_box_cnt];
};

struct stage_treasure_box_t {
	uint32_t	id;
	uint32_t	drop_lv;
	box_id_info_t	boxs[score_max_cnt];
};

struct vip_treasure_box_t {
	uint32_t	vip_lv;
	box_id_info_t	boxs;
};

struct db_treasure_box_elem_t {
	uint32_t	box_quality;
	uint32_t	get_flag;
	uint32_t	item_type;
	uint32_t	item_id;
	uint32_t	item_cnt;
	uint32_t	duration;
	uint32_t	unique_index;
};

struct db_treasure_box_rsp_t {
	uint32_t	box_type;
	uint32_t	box_id;
	uint32_t	box_cnt;
	db_treasure_box_elem_t	boxs[];
};

#pragma pack()

//--------------------------------------------------------------------------------
// global 
//--------------------------------------------------------------------------------

/*! manage all kinds of items */
extern GfItemManager* items;
/*! suit info*/
extern suit_t suit_arr[];

/*! treasure box info */
//extern treasure_box_t treasure_boxs[max_box_quality][max_box_lv];
extern treasure_box_t treasure_boxs[max_treasure_box_cnt];

extern stage_treasure_box_t stage_boxs[stage_max_num];
extern vip_treasure_box_t 	vip_boxs[vip_max_lv];
extern holiday_drop_t holiday_drop_arr[max_holiday_drop_type];
/***********************************************************************************************
 ** clothes duration function
 **********************************************************************************************/

/**
 * @brief player get clothes duration
 */
int player_clothes_duration_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief player open treasure box
 */
int player_open_treasure_box_cmd(Player* p, uint8_t* body, uint32_t bodylen);


/**
 * @brief callback for open a treasure box
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_player_treasure_box_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
/**
 * @brief player's clothes duration change in a battle
 */
void player_clothes_init(Player* p, const player_attr_t* attr);

/**
 * @brief player's clothes duration change in a battle
 */
void player_clothes_duration_chg(Player* p, uint32_t duration_type);

/**
 * @brief calc player's def & atk by clothes
 */
void calc_player_def_atk(Player* p);
/**
 * @brief calc player's Attribute by attire strengthen level
 */
void calc_player_strengthen_attire(Player* p);
/**
  * @brief return bitpos of unique items 
  * @return 0 none
  */
uint32_t get_unique_item_bitpos(uint32_t item_id);

/**
  * @brief add timer to start or stop holiday action 
  * @return 0 none
  */
void add_holiday_timer();

/**
 * @brief get player's clothes duration
 */
int duration_state_chg_noti(Player* p, player_clothes_info_t* cloth);

/**
  * @brief load unique items from an xml file
  * @return 0 on success, -1 on error
  */
int load_unique_items(xmlNodePtr cur);

/**
  * @brief load suit configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_suit(xmlNodePtr cur);

/**
  * @brief load treasure box configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_treasure_box(xmlNodePtr cur);

/**
  * @brief load stage box configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_stage_boxs(xmlNodePtr cur);

/**
  * @brief load vip box configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_vip_boxs(xmlNodePtr cur);

/**
  * @brief load holiday drop item from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_holiday_drop(xmlNodePtr cur);


/**
  * @brief check if the item's suit id exist
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int check_item_suit(const GfItem& itm, void* in);

/**
  * @brief date to unix time stamp
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int date2timestamp(const char* fromstr, const char* fmt, time_t &totime);

/**
 * @brief if get amber, do statistics
 */
inline void do_stat_log_got_amber(uint32_t item_id, uint32_t ro_type)
{
	const GfItem * itm = items->get_item(item_id);
	if (!itm) {
		DEBUG_LOG("item not exist:%u ", item_id);
		return;
	}
	if (items->is_skill_book(itm->category())) {
        uint32_t amber_id = item_id - 1200000 - ((ro_type - 1)* 200);
        do_stat_item_log(stat_log_got_amber, amber_id, ro_type, 1);
	}
}






#endif //KF_ITEM_HPP_
