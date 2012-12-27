/**
 *============================================================
 *  @file      summon_monster.hpp
 *  @brief    player related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_SUMMON_MON_HPP_
#define KF_SUMMON_MON_HPP_

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <kf/pet_attr.hpp>

#include <vector>
extern "C" {
#include <string.h>
//#include <glib.h>
	
#include <libtaomee/list.h>

#include <libtaomee/project/constants.h>
#include <libtaomee/project/types.h>
}

#include "fwd_decl.hpp"
#include "dbproxy.hpp"

#pragma pack(1)

enum {
	max_fight_value = 100,
	min_fight_value = 10,

    max_summon_learn_skills = 5,
	max_summon_skills = 10,
};

enum {
    skill_sum_login  = 1,
    skill_sum_evolve = 2,
    skill_sum_lv_up  = 3,
    skill_sum_hanch  = 4,
    skill_sum_renew  = 5,
    skill_sum_fresh  = 6,
};

struct scroll_enum_t {
    uint32_t skillid;
    uint32_t lv;
    uint32_t odds;
};

struct summon_skill_scroll_t {
    uint32_t id;
    uint32_t type;
    std::vector<scroll_enum_t> skill_vec;
};

struct summon_skill_t {
	uint32_t	skill_id;
	uint16_t	skill_lv;
	uint8_t		used_flag;
};

struct allocate_exp_t {
	uint32_t	mon_tm;
	uint32_t	allocated_exp;
};

enum summon_status_t {
	summon_in_bag       = 0,//休息
	summon_fight_take   = 1,//出战
	summon_fight_notake = 2,//待战
	summon_in_home      = 3,//小屋
	summon_in_offline   = 4,//挂机
	summon_in_guard     = 5 //守护阵
};

/**
  * @brief summon monster
  */
struct summon_mon_t {
	/*! type of a summon monster */
	uint32_t	mon_type;
	/*! the create time of the summon_mon's role */
	uint32_t	mon_tm;
	/*! summon_mon's nick name */
	char		nick[max_nick_size];
	/*! summon_mon's level */
	uint16_t	lv;
	/*! summon_mon's experience */
	uint32_t	exp;
	/*! summon_mon's fight value */
	uint16_t	fight_value;
	/*! summon renascence time*/
	uint16_t    renascence_time;
	/*! summon skill count*/
	uint16_t	skills_cnt;


	/*! summon uesd skill*/
	summon_skill_t	skills[max_summon_skills];
	/*! summon_mon's disable skills */
	uint8_t		disable_skills[max_summon_skills];
	/*! call or take back summon monster; 1:call; 0:take back */
	uint8_t		call_flag;

	uint32_t    attr_per; //宠物成长系数

	attr_data * pet_attr; //宠物的所有属性

};


#pragma pack()

struct summon_skill_info_t {
	uint32_t	skill_id;
	uint16_t	need_lv;
	uint16_t	top_skill_lv;
};


struct summon_info_t {
	uint32_t	summon_id;
	uint32_t	pre_summon_id;
	uint32_t    odds;
	char		name[max_nick_size];
	uint32_t	lv;
	uint32_t	next_lv;
	uint32_t	hatch_lv;
	uint32_t	need_vip_lv;
	uint32_t	evolve_need_item;
	uint32_t    renascence_time ;
	uint32_t    attr_type; //宠物属性类型
	int   base_per; //基础成长率
	summon_skill_info_t	skills[max_summon_skills];
};
//------------------------------------------------------------------
// structs
//------------------------------------------------------------------
#pragma pack(1)

struct sum_skill_t{
    uint32_t    skillid;
    uint32_t    skilllv;
};

struct summon_mon_info_t {
	uint32_t	mon_tm;
	uint32_t	mon_type;
	char 		nick[max_nick_size];
	uint32_t 	exp;
	uint32_t	lv;
	uint32_t	fight_value;
	uint32_t	fight_flag;
	int    attr_per;
    uint32_t    skill_cnt;
    //uint8_t     skills[max_summon_skills_len];
    sum_skill_t sum_skill[max_summon_learn_skills];
};

struct db_get_summon_list_rsp_t {
	uint32_t	mon_cnt;
	summon_mon_info_t	mons[];
};

struct db_hatch_summon_rsp_t {
	uint32_t 	pre_summon_tm;
	uint32_t 	summon_tm;
	uint32_t	summon_type;
	uint32_t 	summon_lv;
	uint32_t 	fight_value;
	char 		nick[max_nick_size];
	uint32_t 	status;
	uint32_t	used_itm_id;
};

struct db_set_summon_nick_rsp_t {
	uint32_t	summon_tm;
};

struct db_feed_summon_mon_rsp_t {
	uint32_t    mon_tm;
	uint32_t	total_value;
	uint32_t	used_itm_id;
};

struct db_set_fight_summon_rsp_t {
	uint32_t	mon_tm;
	uint32_t    flag;
};

struct db_evolve_summon_rsp_t {
	uint32_t	mon_type;
};

struct db_allocate_exp_t {
	uint32_t allocator_exp;
	uint32_t mon_tm;
	uint32_t mon_exp;
	uint32_t mon_lv;
};
struct db_re_evolve_t {
	uint32_t mon_tm;
	uint32_t mon_type; 
};


struct db_re_evolve_summon_rsp_t {
	uint32_t r_evolve_cnt;
	db_re_evolve_t re_evolve[];
};

struct db_use_skills_scroll_t {
    uint32_t    mon_tm;
    uint32_t    scrollid;
    uint32_t    flag;
    //uint8_t     skills_arr[max_summon_skills_len];
    sum_skill_t skill_info;
};

struct db_renew_summon_skills_t {
    uint32_t    mon_tm;
    uint32_t    src_id;
    //uint32_t    dst_id;
    //uint8_t     skills_arr[max_summon_skills_len];
    sum_skill_t skill_info;
};
struct db_fresh_attr_rsp_t {
	uint32_t item_id;
	uint32_t mon_type;
	uint32_t mon_tm;
	int attr_per;
    uint32_t vip_sum;
    uint32_t mon_lv;
    uint32_t mon_exp;
};

#pragma pack()


//--------------------------------------------------------------------------------
// global 
//--------------------------------------------------------------------------------
/*! the all skills*/
extern std::map<uint32_t, summon_info_t> summons_map_;

extern std::map<uint32_t, summon_skill_scroll_t> sum_scroll_map_;


summon_info_t * get_summon(uint32_t mon_type);


summon_skill_scroll_t * get_scroll(uint32_t scrollid);


uint32_t get_summon_base_type(player_t* p, uint32_t mon_tm);
uint32_t get_next_gradation_lv(uint32_t summon_id);
uint32_t get_summon_attr_type(uint32_t summon_id);
//------------------------------------------------------------------
// inlines
//------------------------------------------------------------------
//
inline void set_summon_attr(summon_mon_t * p_mon, attr_data * attr)
{
	if (p_mon->pet_attr) {
		delete p_mon->pet_attr;
	}
	p_mon->pet_attr = attr;
}

inline bool is_valid_montype(uint32_t mon_type)
{
	return (mon_type >= 1001 && mon_type <= 2000);
}

inline uint32_t calc_sum_skill_lv(uint32_t sum_lv, uint32_t need_lv, uint32_t top_skill_lv)
{
    uint32_t lv = sum_lv - need_lv + 1;
    lv = lv > top_skill_lv ? top_skill_lv : lv;

    return lv;
}

//------------------------------------------------------------------
// Cmds
//------------------------------------------------------------------
/**
 * @brief player hatch a new summon monster
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int hatch_summon_mon_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief set nick name for a summon monster
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int set_summon_nick_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief feed a summon monster
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int feed_summon_mon_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief set a summon monster can fight
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int set_fight_summon_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief get all summon monsters
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_summons_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int get_summons_attr_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int get_friend_summon_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief call or take back summon monsters
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int call_summon_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int fresh_summon_attr_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

/**
 * @brief evolve summon monster
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int evolve_summon_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief disable summon monster skills
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int disable_summon_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief use summon skills scroll
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int use_summon_skills_scroll_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief flag 1: replace 0: addition
 */
int summon_learn_skill(player_t* p, char* buf, uint32_t mon_tm, uint32_t src_skill, uint32_t new_skill, int flag);

/**
 * @brief renew summon monster skills
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int renew_summon_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief usr allocated exp in allocator to summon 
 *
 * @param p the request player
 * @param mon_tm the summon that exp was allocated to
 * @param allocated_exp the exp allocated
 *
 * @return  0 ok , -1 error and p will be deleted
 */
int allocate_exp_to_summon_cmd(player_t * p, uint8_t *body, uint32_t bodylen);

int get_allocator_exp_cmd(player_t *p, uint8_t *body, uint32_t bodylen);


//------------------------------------------------------------------
// request to dbproxy
//------------------------------------------------------------------
/**
  * @brief player set a new summon monster to db
  * @param p the requester
  * @param mon_type of the requestee
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_hatch_summon_mon(player_t* p, uint32_t item_id, uint32_t mon_type, uint32_t pre_summon_tm);

/**
  * @brief player set summon monster nick name
  * @param p the requester
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_set_summon_nick(player_t* p);

/**
  * @brief player set summon monster nick name
  * @param p the requester
  * @param itm_id the item id used
  * @param add_value the value add to fight_value
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_feed_summon_mon(player_t* p, uint32_t mon_tm, uint32_t itm_id, uint32_t add_value);

/**
  * @brief player set summon monster to fight
  * @param p the requester
  * @param mon_type of the requestee
  * @param flag 1: active, 2:deactive
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_set_fight_summon(player_t* p, uint32_t mon_type, uint32_t flag);

/**
  * @brief player set summon monster to fight
  * @param p the requester
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_get_summon_list(player_t* p, uint32_t uid, uint32_t role_tm);

/**
  * @brief player evolve summon
  * @param p the requester
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_evolve_summon(player_t* p);

int db_allocate_exp_to_summon(player_t *p, summon_mon_t * mon, uint32_t allocated_exp);



summon_mon_t* get_player_summon(player_t* p, uint32_t mon_tm);


bool  is_summon_role_type_exist(player_t* p, uint32_t role_type);

bool is_summon_final_status(uint32_t mon_type);
//------------------------------------------------------------------
// callback for handling package return from dbproxy
//------------------------------------------------------------------
/**
  * @brief callback for handling player get all summon monsters
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  * @see db_get_player
  */
int db_get_summon_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling player get a new summon monster
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  * @see db_get_player
  */
int db_hatch_summon_mon_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling player set summon monster nick
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  * @see db_get_player
  */
int db_set_summon_nick_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling player set summon monster nick
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  * @see db_get_player
  */
int db_feed_summon_mon_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling player set summon monster to fight
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  * @see db_get_player
  */
int db_set_fight_summon_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling player evolve summon
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  * @see db_get_player
  */
int db_evolve_summon_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for handling re_evolve summon
 *
 * @param p the requester
 * @param id id of the requestee
 * @param body body of pkg
 * @param bodylen pkg body length
 * @param ret 0 ok, -1 error
 *
 * @return 
 */
int db_re_evolve_summon_callback(player_t *p, userid_t id, void *body, uint32_t bodylen, uint32_t ret);

/**
 * @brief  callback  for handleing usr allocate allocator_exp to summon
 *
 * @param p the requester
 * @param id id of the requseter
 * @param body 
 * @param bodylen
 * @param ret
 *
 * @return 
 */
int db_allocate_exp_to_summon_callback(player_t *p, userid_t id, void *body, uint32_t bodylen, uint32_t ret);

/**
 * @brief  callback for allocate exp make summon lv up
 *
 * @param p the requester
 * @param id id of the requester
 * @param body
 * @param bodylen
 * @param ret
 *
 * @return 
 */
int db_allocate_exp_summon_lv_up_callback(player_t *p, userid_t id, void *body, uint32_t bodylen, uint32_t ret);


int db_fresh_summon_attr_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
/**
 *
 */
int db_use_sum_skills_scroll_callback(player_t * p, userid_t uid, void * body, uint32_t bodylen, uint32_t ret);

/**
 *
 */
int db_renew_summon_skills_callback(player_t * p, userid_t uid, void * body, uint32_t bodylen, uint32_t ret);
/**
 * @brief pack p's summon into buf
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_player_summon(const player_t* p, void* buf);

/**
 * @brief pack p's summon full info
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_player_summon_full_info(const player_t* p, void* buf);

/**
 * @brief pack p's summon info to battle
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_summon_to_btl(const player_t* p, void* buf, uint32_t battle_mode, bool take_flag = true);

void add_summon_new_skill(summon_mon_t* p_summon, uint32_t new_cnt, sum_skill_t *skill_data);

void set_summon_skill_lv(summon_mon_t* p_summon, uint32_t id, uint32_t lv);
void renew_summon_new_skill(summon_mon_t* p_summon, sum_skill_t *skill_data);
void set_summon_skills_ex(summon_mon_t* p_summon);

/**
 * @brief set summon skills
 * @param p_summon the point of summon
 * @return
 */
void set_summon_skills(summon_mon_t* p_summon, uint32_t old_type);

/**
 * @brief check does user have summon
 * @param 
 * @return bool
 */
 bool check_summon_type_exist(player_t* p, uint32_t mon_type);

 /**
 * @brief get  summon id
 * @param 
 * @return bool
 */
 uint32_t get_summon_tm_by_one_type(player_t* p, uint32_t mon_type);

 /**
  * @brief check the usr's summon is over valid
  *
  * @param p the usr
  * @param p_summon the summon 
  *
  * @return  true ok! false fatal
  */
 bool is_valid_summon(player_t *p, summon_mon_t * p_summon);


/**
* @brief del summon from player's summon list
* @param 
* @return bool
*/
uint32_t del_summon(player_t *p, uint32_t mon_tm);


summon_mon_t* get_player_offline_summon(player_t* p);

/**
  * @brief load summon skill scroll configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_summons_skills_scroll(xmlNodePtr cur);
/**
  * @brief unload summon monsters configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int unload_summons_skills_scroll();

/**
  * @brief load summon monsters configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_summons(xmlNodePtr cur);

/**
  * @brief unload summon monsters configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int unload_summons();
#endif // KF_SUMMON_MON_HPP_

