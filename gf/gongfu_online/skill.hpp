/**
 *============================================================
 *  @file      skill.hpp
 *  @brief    item related functions are declared here. (talk_cmd, ...)
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_SKILL_HPP_
#define KF_SKILL_HPP_
#include <map>

#include "player.hpp"


struct skill_info_t {
	/*! skill lv */
	uint32_t lv;
	/*! skill user lv */
	uint32_t use_lv;
	/*! skill stars */
	uint32_t stars;
	/*! sp that skill upgrade to this lv needed */
	uint32_t sp;
	/*! coins that skill upgrade to this lv needed */
	uint32_t coins;
};

struct pre_skills_t {
	uint32_t skill_id;
	uint32_t skill_lv;
};

struct skills_t {
	/*! skill id */
	uint32_t id;
	/*! skill max lv*/
	uint32_t max_lv;

	uint32_t role_type;
	
	uint32_t pre_skills_cnt;
	pre_skills_t pre_skills[max_pre_skill_cnt];

	/*! skill info*/
	skill_info_t skill_info[max_skill_level + 1];
};

struct role_pre_skill_t {
	uint32_t skill_id;
	uint8_t  skill_lv;
};

struct role_skill_t {
	uint32_t skill_id;
	uint32_t need_role_lv;
	uint8_t  pre_skill_cnt;
	role_pre_skill_t  pre_skill[max_pre_skill_cnt];
};
struct buff_skill_t {
	buff_skill_t(uint32_t buff_id_in, uint32_t duration_in) {
		buff_id = buff_id_in;
		duration = duration_in;
	}
	uint32_t buff_id;
	uint32_t duration;
};

//--------------------------------------------------------------------------------
// function
//--------------------------------------------------------------------------------
//

const buff_skill_t* get_buff(uint32_t buff_id);
/**
  * @brief pack the skill of the player have binded
  * @param p the player
  * @param buf
  * @return the idx
  */
int pack_player_skill_bind(player_t* p, void* buf);

int send_get_buff_skill_rsp(player_t* p, add_buf_rsp_t* data);

int reset_all_skills(player_t* p, uint32_t extera_point) ;

int db_get_skills(player_t* p) ;

int db_learn_new_skill(player_t* p, uint32_t skill_id);

int pack_will_toke_skills(player_t* p, uint8_t* buf);

//--------------------------------------------------------------------------------
// commands
//--------------------------------------------------------------------------------

/**
* @brief player learn new skill
*/
int learn_new_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief get the skill of the player have owned
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_skills_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief bind the skill of the player can use in the battle
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int skill_bind_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief update player's skill
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int skill_upgrade_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


/**
* @brief player get buff skill
*/
int get_buff_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
* @brief player get buff skill list
*/
int get_buff_skill_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

bool is_buff_alive(player_t* p, uint32_t buff_id);

//--------------------------------------------------------------------------------
// Requests to Dbproxy
//--------------------------------------------------------------------------------

int db_learn_new_skill_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
/**
  * @brief callback for reset skill
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_reset_skill_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);


/**
  * @brief callback for getting skills
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_get_skills_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for binding skills
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_skill_bind_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for upgrading skills
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_skill_upgrade_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

//---------------------------------------------------------------------

/**
  * @brief load skill configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_skills(xmlNodePtr cur);

/**
  * @brief load buff skill configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_buff_skills(xmlNodePtr cur);

//--------------------------------------------------------------------------------
// struct
//--------------------------------------------------------------------------------
#pragma pack(1)
struct learn_new_skill_rsp_t {
	uint32_t skill_id;
	uint32_t skill_lv;
	uint32_t left_points;
	uint32_t item_id;
};


struct db_get_skill_elem_t {
	uint32_t skill_id;
	uint32_t skill_lv;
	uint32_t skill_points;
};

struct reset_skill_rsp_t {
	uint32_t left_skill_points;
};
/**
  * @brief response from dbproxy to conform the request of skill bind
  */
struct get_skills_rsp_t {
	uint32_t left_sp;
	uint32_t skills_cnt;
	db_get_skill_elem_t skill[];
};

struct db_skill_bind_elem_t {
	uint32_t skill_id;
	uint32_t skill_lv;
	uint32_t bind_key;
};

/**
  * @brief response from dbproxy to conform the request of skill bind
  */
struct skill_bind_rsp_t {
	uint32_t	bind_cnt;
	db_skill_bind_elem_t	skill_bind[];
};

/**
  * @brief response from dbproxy to conform the request of skill upgrade
  */
struct skill_upgrade_rsp_t {
	uint32_t skill_id;
	uint32_t skill_lv;

	uint32_t left_coins;
	uint32_t use_point;
	uint32_t left_point; 
};


#pragma pack()

//--------------------------------------------------------------------------------
// global 
//--------------------------------------------------------------------------------
/*! the all skills*/
extern std::map<uint32_t, skills_t*> skills_map_;

#endif //KF_SKILL_HPP_
