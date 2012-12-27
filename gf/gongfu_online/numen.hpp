/**
 *============================================================
 *  @file     numen.hpp
 *  @brief    player related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef __KF_NUMEN_HPP__
#define __KF_NUMEN_HPP__

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

enum numen_status_t {
    numen_waiting   = 0,
    numen_fighting  = 1,
    numen_guarding  = 2
};

enum {
	max_numen_skill_num = 3,
};
//------------------------------------------------------------------
// structs
//------------------------------------------------------------------
struct numen_skill_t {
    uint32_t    skill_id;
    uint32_t    skill_lv;
};

struct numen_obj_t {
	numen_obj_t() {
		numen_id = 0;
		numen_status = 0;
		memset(name, 0, sizeof(name));
	}
    uint32_t    numen_id;
    uint32_t    numen_status;
    char        name[max_nick_size];
    std::vector<numen_skill_t> skill_vec_;
};

struct numen_info_t {
    uint32_t    numen_id;
    char        name[max_nick_size];
    //uint32_t    odds;
    std::vector<numen_skill_t> skill_vec_;
};

struct numen_odds_t {
    uint32_t    numen_id;
    uint32_t    odds;
};

struct call_numen_odds_t {
    uint32_t    item_id;
    uint32_t    god_id;
    //std::vector<numen_odds_t> odds_vec_;
};

struct db_numen_obj_t {
    uint32_t    numen_id;
    char        name[max_nick_size];
    uint32_t    numen_status;
    uint32_t    skill_cnt;
    numen_skill_t skills[];
};

struct db_get_numen_list_rsp_t {
    uint32_t    numen_cnt;
    db_numen_obj_t numens[];
};

struct db_invite_numen_rsp_t {
    uint32_t    call_item_id;
    uint32_t    numen_type;
};

struct db_make_numen_sex_rsp_t {
    uint32_t    sex_item_id;
    uint32_t    numen_type;
    uint32_t    action; //1--add 2--renew
    uint32_t    old_id;
    uint32_t    old_lv;
    uint32_t    new_id;
    uint32_t    new_lv;
};

struct db_change_numen_nick_rsp_t {
    uint32_t    numen_type;
    char        name[max_nick_size];
};

#pragma pack()
//--------------------------------------------------------------------------------
// global 
//--------------------------------------------------------------------------------

//------------------------------------------------------------------
// inlines
//------------------------------------------------------------------
//
bool have_numen_in_fighting();

numen_obj_t* get_player_numen(player_t* p, uint32_t mon_tm);

bool is_numen_exist(player_t* p, uint32_t numen_type);

int pack_fight_numen(player_t* p, uint8_t* buf);
/**
 * @brief pack p's numen into buf
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_player_numen(const player_t* p, void* buf);

//------------------------------------------------------------------
// Cmds
//------------------------------------------------------------------
/**
 * @brief player invite a new numen
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int invite_numen_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief change numen status
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int change_numen_status_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief get all numen
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_numen_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


/**
 * @brief use skills scroll
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int god_use_skills_scroll_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief 洗技能
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int make_numen_sex_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief change numen nick
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int change_numen_nick_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

//------------------------------------------------------------------
// request to dbproxy
//------------------------------------------------------------------
/**
  * @brief player invite a new numen to db
  * @param p the requester
  * @param numen_type of the requestee
  * @return 0 on success, -1 on error
  */
int db_invite_numen(player_t* p, uint32_t call_item_id, uint32_t call_item_cnt, uint32_t numen_type);

/**
  * @brief player change numen status
  * @param p the requester
  * @param numen_type of the requestee
  * @param flag 1: active, 0:deactive
  * @return 0 on success, -1 on error
  */
int db_change_numen_status(player_t* p, uint32_t numen_type, uint32_t flag);

/**
  * @brief get all numen
  * @param p the requester
  * @return 0 on success, -1 on error
  */
int db_get_numen_list(player_t* p, uint32_t uid, uint32_t role_tm);

/**
  * @brief update numen skill
  * @param p the requester
  * @return 0 on success, -1 on error
  */
int db_make_numen_sex(player_t* p, uint32_t numen_type, uint32_t skill_id, uint32_t skill_lv, uint32_t id, uint32_t lv);

/**
  * @brief player change numen nick
  * @param p the requester
  * @param numen_type of the requestee
  * @param flag 1: active, 0:deactive
  * @return 0 on success, -1 on error
  */
int db_change_numen_nick(player_t* p, uint32_t numen_type, uint8_t* nick);

//------------------------------------------------------------------
// callback for handling package return from dbproxy
//------------------------------------------------------------------
/**
  * @brief callback for handling player get all numen
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_numen_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling player invite a new numen
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_invite_numen_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling player change numen status
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_change_numen_status_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling player update numen skill
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_make_numen_sex_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling player update numen skill
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_make_numen_sex_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling player change numen nick
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_change_numen_nick_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief load numens configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_numens(xmlNodePtr cur);

int unload_numens();

#endif // __KF_NUMEN_HPP__

