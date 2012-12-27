#include <cstdio>
#include <libtaomee++/inet/pdumanip.hpp>
#include <kf/player_attr.hpp>

using namespace taomee;

extern "C" {
#include <glib.h>

#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
}

#include "utils.hpp"
#include "dbproxy.hpp"
#include "cli_proto.hpp"
#include "item.hpp"
#include "battle.hpp"
#include "login.hpp"
#include "numen.hpp"
#include "apothecary.hpp"
#include "stat_log.hpp"
#include "summon_monster.hpp"
#include "global_data.hpp"
#include "common_op.hpp"

//------------------------------------------------------------------
// statics
//------------------------------------------------------------------

//------------------------------------------------------------------
// inlines
//------------------------------------------------------------------
//
bool have_numen_in_fighting(player_t* p)
{
    std::vector<numen_obj_t>::iterator it = p->m_numens->begin();
    for (; it != p->m_numens->end(); ++it) {
        if (it->numen_status == numen_fighting) {
            return true;
        }
    }
    return false;
}

void change_numen_status(player_t* p, uint32_t numen_id, uint32_t status)
{
    std::vector<numen_obj_t>::iterator it = p->m_numens->begin();
    for (; it != p->m_numens->end(); ++it) {
        it->numen_status = numen_waiting;
        if (it->numen_id == numen_id) {
            it->numen_status = status;
        }
    }
}

call_numen_odds_t* get_call_numen_rule(uint32_t rule_item_id)
{
    std::map<uint32_t, call_numen_odds_t>::iterator it =
        call_numen_rule_map_.find(rule_item_id);
    if (it != call_numen_rule_map_.end()) {
        return &(it->second);
    }
    return NULL;
}

uint32_t get_numen_type(uint32_t threshold, call_numen_odds_t* rule)
{
    return rule->god_id;
    /*
    uint32_t odds = 0;
    std::vector<numen_odds_t>::iterator it = rule->odds_vec_.begin();
    for (; it != rule->odds_vec_.end(); ++it) {
        odds += it->odds;
        if (threshold <= odds) {
            return it->numen_id;
        }
    }*/
    return 0;
}

numen_info_t* get_numen_config_info(uint32_t numen_type)
{
    std::map<uint32_t, numen_info_t>::iterator it = numens_map_.find(numen_type);
    if (it != numens_map_.end()) {
        return &(it->second);
    }
    return NULL;
}

numen_obj_t* get_player_numen(player_t* p, uint32_t numen_id)
{
    std::vector<numen_obj_t>::iterator it = p->m_numens->begin();
    for (; it != p->m_numens->end(); ++it) {
        if (it->numen_id == numen_id) {
            return &(*it);
        }
    }

    return NULL;
}
numen_obj_t* get_fighting_numen(player_t* p)
{
    std::vector<numen_obj_t>::iterator it = p->m_numens->begin();
    for (; it != p->m_numens->end(); ++it) {
        if (it->numen_status == numen_fighting) {
            return &(*it);
        }
    }
    return 0;
}


int pack_fight_numen(player_t* p, uint8_t* buf)
{
	int idx = 0;
	numen_obj_t obj;
	numen_obj_t* p_fight = get_fighting_numen(p);
	if (!p_fight) {
		p_fight = &obj;	
	}
	pack_h(buf, p_fight->numen_id, idx);
	pack(buf, p_fight->name, max_nick_size, idx);
	std::vector<numen_skill_t>::iterator it = p_fight->skill_vec_.begin();
	std::vector<numen_skill_t>::iterator end = p_fight->skill_vec_.end();
	int i = 0;
	for (; it != end; ++it) {
		if (i < max_numen_skill_num) {
			numen_skill_t* p_skill = &(*it);
			pack_h(buf, p_skill->skill_id, idx);
			pack_h(buf, p_skill->skill_lv, idx);
			i++;
		}
	}
	for (uint32_t j = 0; j < (uint32_t)(max_numen_skill_num - i); j++) {
		pack_h(buf, 0, idx);
		pack_h(buf, 0, idx);
	}
	return idx;
}

bool is_numen_exist(player_t* p, uint32_t numen_type)
{
    std::vector<numen_obj_t>::iterator it = p->m_numens->begin();
    for (; it != p->m_numens->end(); ++it) {
        if (it->numen_id == numen_type) {
            return true;
        }
    }

    return false;
}

void add_player_numen(player_t* p, numen_obj_t* p_obj)
{
    if (is_numen_exist(p, p_obj->numen_id)) return;

    p->m_numens->push_back(*p_obj);
}

void update_numen_skill(player_t* p, uint32_t numen_type, numen_skill_t* old_skill, numen_skill_t* new_skill)
{
    numen_obj_t* p_numen = get_player_numen(p, numen_type);
    if (p_numen == NULL) return;

    if (new_skill == NULL) return;

        //TRACE_LOG("numen skill ---> uid[%u] [%u] [0 | %u]", p->id, );
    if (old_skill == NULL) {
        p_numen->skill_vec_.push_back(*new_skill);
    } else {
        std::vector<numen_skill_t>::iterator it = p_numen->skill_vec_.begin();
        for (; it != p_numen->skill_vec_.end(); ++it) {
            if (it->skill_id == old_skill->skill_id && it->skill_lv == old_skill->skill_lv) {
                it->skill_id = new_skill->skill_id;
                it->skill_lv = new_skill->skill_lv;
                break;
            }
        }
    }
}

/**
 * @param type 0--->summon 1--->god
 */
scroll_enum_t* get_new_skill(uint32_t scroll_id, uint32_t type)
{
    summon_skill_scroll_t * scrolls = get_scroll(scroll_id);
    if (scrolls == NULL) return NULL;

    if (scrolls->type != 1) return NULL;

    uint32_t threshold = rand() % 1000;
    uint32_t odds = 0;
    for (uint32_t i = 0; i < scrolls->skill_vec.size(); i++) {
        odds += scrolls->skill_vec[i].odds;
        TRACE_LOG(" odds --> [%u]", odds);
        if (threshold < odds) {
            return &(scrolls->skill_vec[i]);
        }
    }

    return NULL;
}

bool is_god_nature_skill(uint32_t numen_id, uint32_t skill_id)
{
    numen_info_t* numen_info = get_numen_config_info(numen_id);
    std::vector<numen_skill_t>::iterator it = numen_info->skill_vec_.begin();
    for (; it != numen_info->skill_vec_.end(); ++it) {
        if (it->skill_id == skill_id) {
            return true;
        }
    }
    return false;
}

/**
 * @return -1: error 1:the same skill 2:skill exist but level is up 3:skill not exist
 */
int check_player_god_skill_info(player_t* p, uint32_t numen_id, numen_skill_t *skill)
{
    numen_obj_t* god = get_player_numen(p, numen_id);
    if (god == NULL) return -1;

    std::vector<numen_skill_t>::iterator it = god->skill_vec_.begin();
    for (; it != god->skill_vec_.end(); ++it) {
        if (it->skill_id == skill->skill_id) {
            if (it->skill_lv < skill->skill_lv) {
                return 2;
            }
            return 1;
        }
    }

    return 3;
}

numen_skill_t* get_player_god_skill(player_t* p, uint32_t numen_id, uint32_t skill_id)
{
    numen_obj_t* god = get_player_numen(p, numen_id);
    if (god == NULL) return NULL;

    std::vector<numen_skill_t>::iterator it = god->skill_vec_.begin();
    for (; it != god->skill_vec_.end(); ++it) {
        if (it->skill_id == skill_id) {
            return &(*it);
        }
    }

    return NULL;
}

int get_player_god_skill_cnt(player_t* p, uint32_t numen_id)
{
    numen_obj_t* god = get_player_numen(p, numen_id);
    if (god == NULL) return -1;

    return (int)(god->skill_vec_.size());
}

bool is_player_god_can_add_skill(player_t* p, uint32_t numen_id)
{
    int god_skill_cnt = get_player_god_skill_cnt(p, numen_id);
    TRACE_LOG("numen skill cnt --->[%u][%u %d]", p->id, numen_id, god_skill_cnt);
    if (god_skill_cnt > 0 && god_skill_cnt < max_numen_skill_num) {
        return true;
    }
    return false;
}

/**
 * @brief use scroll replace or add skill
 */
int send_make_numen_sex_pkg2db(player_t* p, uint32_t scrollid, uint32_t god_id, uint32_t action, numen_skill_t *pold, numen_skill_t *pnew)
{
    if (pnew == NULL) return 0;

    int idx = 0;
    pack_h(dbpkgbuf, scrollid, idx); 
    pack_h(dbpkgbuf, god_id, idx); 
    //1--nothing; 2--level up; 3--add; 4--choose; 5--replace
    pack_h(dbpkgbuf, action, idx);  
    if (pold) {
        pack_h(dbpkgbuf, pold->skill_id, idx); 
        pack_h(dbpkgbuf, pold->skill_lv, idx); 
    } else {
        pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx); 
        pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx); 
    }
    pack_h(dbpkgbuf, pnew->skill_id, idx); 
    pack_h(dbpkgbuf, pnew->skill_lv, idx); 
    return  send_request_to_db(p, p->id, p->role_tm, dbproto_make_numen_sex, dbpkgbuf, idx);
}


/**
 * @brief pack p's numen into buf
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_player_numen(const player_t* p, void* buf)
{
    int idx = 0;
    pack(buf, static_cast<uint32_t>(p->m_numens->size()), idx);

    TRACE_LOG("numen ----> [ %zu ]", p->m_numens->size());
    std::vector<numen_obj_t>::iterator it = p->m_numens->begin();

    for (; it != p->m_numens->end(); ++it) {
        TRACE_LOG("numen ----> [%u %u %zu]", it->numen_id, it->numen_status, it->skill_vec_.size());
        pack(buf, it->numen_id, idx);
        pack(buf, it->numen_status, idx);
        pack(buf, it->name, max_nick_size, idx);
        pack(buf, static_cast<uint32_t>(it->skill_vec_.size()), idx);
        for (uint32_t i = 0; i < it->skill_vec_.size(); i++) {
            TRACE_LOG("numen skill ----> [%u %u]", it->skill_vec_[i].skill_id, it->skill_vec_[i].skill_lv);
            pack(buf, it->skill_vec_[i].skill_id, idx);
            pack(buf, it->skill_vec_[i].skill_lv, idx);
        }
        /*
        for (std::vector<numen_skill_t>::iterator skill_it = it->skill_vec_.begin();
                skill_it != it->skill_vec_.end(); ++skill_it) {
            pack(buf, skill_it->skill_id, idx);
            pack(buf, skill_it->skill_lv, idx);
        }*/
    }
    return idx;
}

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
int invite_numen_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t item_id = 0;
    unpack(body, item_id, idx);

    if (!p->my_packs->check_bag_item_count(item_id, 1)) {
        return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
    }

    uint32_t odds = rand() % 100;
    call_numen_odds_t* rule = get_call_numen_rule(item_id);
    if (rule == NULL) {
        return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
    }

    uint32_t numen_type = get_numen_type(odds, rule);
    if (numen_type == 0 || is_numen_exist(p, numen_type)) {
        //db_del_item_elem_t item_elem = {item_id, 1};
        //player_del_items(p, &item_elem, 1, false);

        idx = sizeof(cli_proto_t);
        pack(pkgbuf, item_id, idx);
        pack(pkgbuf, static_cast<uint32_t>(0), idx);
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
    }

	KDEBUG_LOG(p->id, "INVITE GOD\t[item_id=%u god_id=%d]", item_id, numen_type);
    return db_invite_numen(p, item_id, 1, numen_type);
}

/**
 * @brief change numen status
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int change_numen_status_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t numen_type = 0, status = 0;
    unpack(body, numen_type, idx);
    unpack(body, status, idx);

    if (p->battle_grp) {
        return send_header_to_player(p, p->waitcmd, cli_err_unable_in_battle, 1);
    }

    db_change_numen_status(p, numen_type, status);
    TRACE_LOG("numen status ----> [%u][%u %u]", p->id, numen_type, status);

    change_numen_status(p, numen_type, status);

    idx = sizeof(cli_proto_t);
    pack(pkgbuf, numen_type, idx);
    pack(pkgbuf, status, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);	
}

/**
 * @brief change numen nick
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int change_numen_nick_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t numen_type = 0;
    uint8_t nick[max_nick_size] = {0};
    unpack(body, numen_type, idx);
    unpack(body, nick, max_nick_size, idx);

    if (p->battle_grp) {
        return send_header_to_player(p, p->waitcmd, cli_err_unable_in_battle, 1);
    }

    nick[max_nick_size - 1] = '\0';
    CHECK_DIRTYWORD(p, nick);
    CHECK_INVALID_WORD(p, nick);

    TRACE_LOG("numen nick ----> [%u][%u %s]", p->id, numen_type, nick);
	numen_obj_t* p_numen = get_player_numen(p, numen_type);
	if (p_numen) {
		memcpy(p_numen->name, nick, sizeof(p_numen->name));
	}
    return db_change_numen_nick(p, numen_type, nick);
}

/**
 * @brief use skills scroll
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int god_use_skills_scroll_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t god_id = 0, scrollid = 0;
    unpack(body, god_id, idx);
    unpack(body, scrollid, idx);

    if ( !(p->my_packs->check_bag_item_count(scrollid, 1)) ) {
        return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
    }

    scroll_enum_t *skill_enum = get_new_skill(scrollid, 1);
    if (skill_enum == NULL) {
        return -1;
    }

    numen_skill_t new_skill = {0};
    new_skill.skill_id = skill_enum->skillid;
    new_skill.skill_lv = skill_enum->lv;

    TRACE_LOG("GOD NEW SKILL uid[%u] god[%u] [%u %u]", p->id, god_id, new_skill.skill_id, new_skill.skill_lv);

    int skill_status = check_player_god_skill_info(p, god_id, &new_skill);
	KDEBUG_LOG(p->id, "GOD USE SCROLL\t[id=%u skill_status=%d]", scrollid, skill_status);
    // 1:the same skill 
    // 2:skill exist but level is up 
    // 3:skill not exist (reaching the cap or not)
    if (skill_status == 1) {
        db_del_item_elem_t item_elem = {scrollid, 1};
        player_del_items(p, &item_elem, 1, false);

        idx = sizeof(cli_proto_t);
        pack(pkgbuf, scrollid, idx);
        pack(pkgbuf, god_id, idx);
        pack(pkgbuf, static_cast<uint32_t>(1), idx);
        //pack(pkgbuf, new_skill.skill_id, idx);
        //pack(pkgbuf, new_skill.skill_lv, idx);
        pack(pkgbuf, new_skill.skill_id, idx);
        pack(pkgbuf, new_skill.skill_lv, idx);
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
    } else if (skill_status == 2) { 
        //level up
        return send_make_numen_sex_pkg2db(p, scrollid, god_id, 2, NULL, &new_skill);
    } else if (skill_status == 3) {
        if (is_player_god_can_add_skill(p, god_id)) { 
            // add new skill
            return send_make_numen_sex_pkg2db(p, scrollid, god_id, 3, NULL, &new_skill);
        } else {
            //send client for player choose
            db_del_item_elem_t item_elem = {scrollid, 1};
            player_del_items(p, &item_elem, 1, false);

            idx = sizeof(cli_proto_t);
            pack(pkgbuf, scrollid, idx);
            pack(pkgbuf, god_id, idx);
            pack(pkgbuf, static_cast<uint32_t>(4), idx);
            //pack(pkgbuf, static_cast<uint32_t>(0), idx);
            //pack(pkgbuf, static_cast<uint32_t>(0), idx);
            pack(pkgbuf, new_skill.skill_id, idx);
            pack(pkgbuf, new_skill.skill_lv, idx);
            init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
            return send_to_player(p, pkgbuf, idx, 1);
        }
    }
    return skill_status;
}


/**
 * @brief 洗技能
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int make_numen_sex_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t numen_type = 0;
    numen_skill_t old_skill = {0};
    numen_skill_t new_skill = {0};
    unpack(body, numen_type, idx);
    unpack(body, old_skill.skill_id, idx);
    //unpack(body, old_skill.skill_lv, idx);
    unpack(body, new_skill.skill_id, idx);
    unpack(body, new_skill.skill_lv, idx);

    if (p->battle_grp) {
        return send_header_to_player(p, p->waitcmd, cli_err_unable_in_battle, 1);
    }

    TRACE_LOG("numen ----> [%u][%u][%u][%u %u]", p->id, numen_type, old_skill.skill_id,
        new_skill.skill_id, new_skill.skill_lv);

    if (is_god_nature_skill(numen_type, old_skill.skill_id)) {
        return send_header_to_player(p, p->waitcmd, cli_err_summon_no_learn_this_skill, 1);
    }

    numen_skill_t *pold = get_player_god_skill(p, numen_type, old_skill.skill_id);
    if (pold == NULL) {
        ERROR_LOG("god old skill not exit !!! [%u][%u][%u]", p->id, numen_type, old_skill.skill_id);
        return -1;
    }
    
    return send_make_numen_sex_pkg2db(p, 0, numen_type, 5, pold, &new_skill);
}

/**
 * @brief get all numen
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_numen_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    /*
    if (p->m_numens) {
        int idx = sizeof(cli_proto_t);
        idx += pack_player_numen(p, pkgbuf + idx);
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);	
    } else {
    */
        return db_get_numen_list(p, p->id, p->role_tm);
    //}
}


//------------------------------------------------------------------
// request to dbproxy
//------------------------------------------------------------------
/**
  * @brief player invite a new numen to db
  * @param p the requester
  * @param numen_type of the requestee
  * @return 0 on success, -1 on error
  */
int db_invite_numen(player_t* p, uint32_t call_item_id, uint32_t call_item_cnt, uint32_t numen_type)
{
    numen_info_t* numen_info = get_numen_config_info(numen_type);
    if (numen_info == NULL) return -1;

    int idx = 0;
    pack_h(dbpkgbuf, call_item_id, idx);
    pack_h(dbpkgbuf, numen_type, idx);
    pack(dbpkgbuf, numen_info->name, max_nick_size, idx);
    pack_h(dbpkgbuf, static_cast<uint32_t>(numen_info->skill_vec_.size()), idx);
    for (uint32_t i = 0; i < numen_info->skill_vec_.size(); i++) {
        pack_h(dbpkgbuf, numen_info->skill_vec_[i].skill_id, idx);
        pack_h(dbpkgbuf, numen_info->skill_vec_[i].skill_lv, idx);
    }
    return send_request_to_db(p, p->id, p->role_tm, dbproto_invite_numen, dbpkgbuf, idx);
}

/**
  * @brief player change numen status
  * @param p the requester
  * @param numen_type of the requestee
  * @param flag 1: active, 0:deactive
  * @return 0 on success, -1 on error
  */
int db_change_numen_status(player_t* p, uint32_t numen_type, uint32_t status)
{
    int idx = 0;
    pack_h(dbpkgbuf, numen_type, idx);
    pack_h(dbpkgbuf, status, idx);
    return send_request_to_db(0, p->id, p->role_tm, dbproto_change_numen_status, dbpkgbuf, idx);
}

/**
  * @brief player change numen nick
  * @param p the requester
  * @param numen_type of the requestee
  * @param flag 1: active, 0:deactive
  * @return 0 on success, -1 on error
  */
int db_change_numen_nick(player_t* p, uint32_t numen_type, uint8_t* nick)
{
    int idx = 0;
    pack_h(dbpkgbuf, numen_type, idx);
    pack(dbpkgbuf, nick, max_nick_size, idx);
    return send_request_to_db(p, p->id, p->role_tm, dbproto_change_numen_nick, dbpkgbuf, idx);
}

/**
  * @brief get all numen
  * @param p the requester
  * @return 0 on success, -1 on error
  */
int db_get_numen_list(player_t* p, uint32_t uid, uint32_t role_tm)
{
    return send_request_to_db(p, p->id, p->role_tm, dbproto_get_numen_list, 0, 0);
}


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
int db_get_numen_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    db_get_numen_list_rsp_t* rsp = reinterpret_cast<db_get_numen_list_rsp_t*>(body);
    CHECK_VAL_GE(bodylen, sizeof(db_get_numen_list_rsp_t) + rsp->numen_cnt * sizeof(db_numen_obj_t));

    //db_numen_obj_t *p_db_numen = rsp->numens;
    uint8_t *p_offset = (uint8_t*)(rsp->numens);
    for (uint32_t i = 0; i < rsp->numen_cnt; i++) {
        db_numen_obj_t *p_db_numen = reinterpret_cast<db_numen_obj_t*>(p_offset);

        numen_obj_t numen_obj;
        numen_obj.numen_id = p_db_numen->numen_id; //rsp->numens[i].numen_id;
        numen_obj.numen_status = p_db_numen->numen_status; //rsp->numens[i].numen_status;
        memcpy(numen_obj.name, p_db_numen->name, max_nick_size);

        TRACE_LOG("numen ---> [%u %u %u]", numen_obj.numen_id, numen_obj.numen_status, p_db_numen->skill_cnt);
        for (uint32_t j = 0; j < p_db_numen->skill_cnt; j++) {
            TRACE_LOG("numen skill ---> [%u %u]", p_db_numen->skills[j].skill_id, p_db_numen->skills[j].skill_lv);
            numen_obj.skill_vec_.push_back(p_db_numen->skills[j]);
        }
        p->m_numens->push_back(numen_obj);

        p_offset += (sizeof(db_numen_obj_t) + p_db_numen->skill_cnt * sizeof(numen_skill_t));
    }

    //calc_and_save_player_attribute(p);

    int idx = sizeof(cli_proto_t);
    idx += pack_player_numen(p, pkgbuf + idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);	
}

/**
  * @brief callback for handling player invite a new numen
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_invite_numen_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    db_invite_numen_rsp_t* rsp = reinterpret_cast<db_invite_numen_rsp_t*>(body);

    numen_info_t* numen_info = get_numen_config_info(rsp->numen_type);
    if (numen_info == NULL) return -1;

    p->my_packs->del_item(p, rsp->call_item_id, 1, channel_string_other);

    numen_obj_t obj;
    obj.numen_id = rsp->numen_type;
    memcpy(obj.name, numen_info->name, max_nick_size);
    add_player_numen(p, &obj);

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->call_item_id, idx);
    pack(pkgbuf, rsp->numen_type, idx);
    pack(pkgbuf, static_cast<uint32_t>(0), idx);
    pack(pkgbuf, numen_info->name, max_nick_size, idx);
    pack(pkgbuf, static_cast<uint32_t>(numen_info->skill_vec_.size()), idx);
    for (uint32_t i = 0; i < numen_info->skill_vec_.size(); i++) {
        update_numen_skill(p, rsp->numen_type, NULL, &(numen_info->skill_vec_[i]));

        pack(pkgbuf, numen_info->skill_vec_[i].skill_id, idx);
        pack(pkgbuf, numen_info->skill_vec_[i].skill_lv, idx);
    }

    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);	
}

/**
  * @brief callback for handling player change numen status
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_change_numen_status_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    int idx = sizeof(cli_proto_t);

    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);	
}

/**
  * @brief callback for handling player change numen nick
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_change_numen_nick_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    db_change_numen_nick_rsp_t* rsp = reinterpret_cast<db_change_numen_nick_rsp_t*>(body);

    TRACE_LOG("numen nick ----> [%u][%u %s]", p->id, rsp->numen_type, rsp->name);

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->numen_type, idx);
    pack(pkgbuf, rsp->name, max_nick_size, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);	
}


/**
  * @brief callback for handling player update numen skill
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_make_numen_sex_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    db_make_numen_sex_rsp_t* rsp = reinterpret_cast<db_make_numen_sex_rsp_t*>(body);

    if (rsp->sex_item_id) {
        p->my_packs->del_item(p, rsp->sex_item_id, 1, channel_string_other);
    }

    numen_skill_t old_skill = {0};
    numen_skill_t new_skill = {0};

    old_skill.skill_id = rsp->old_id;
    old_skill.skill_lv = rsp->old_lv;
    new_skill.skill_id = rsp->new_id;
    new_skill.skill_lv = rsp->new_lv;

    TRACE_LOG("numen replace [%u][%u][%u][%u|%u]", p->id, rsp->numen_type, rsp->action, rsp->old_id, rsp->new_id);
    update_numen_skill(p, rsp->numen_type, old_skill.skill_id == 0 ? NULL : &old_skill, &new_skill);
    /*
    if (rsp->action == 1) {
    } else if (rsp->action == 2) {
    }*/

    int idx = sizeof(cli_proto_t);
    if (rsp->action == 5) {
        pack(pkgbuf, rsp->numen_type, idx);
        pack(pkgbuf, old_skill.skill_id, idx);
        //pack(pkgbuf, old_skill.skill_lv, idx);
        pack(pkgbuf, new_skill.skill_id, idx);
        pack(pkgbuf, new_skill.skill_lv, idx);
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
    }
    pack(pkgbuf, rsp->sex_item_id, idx);
    pack(pkgbuf, rsp->numen_type, idx);
    pack(pkgbuf, rsp->action, idx);
    pack(pkgbuf, new_skill.skill_id, idx);
    pack(pkgbuf, new_skill.skill_lv, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

//---------------------------------------------------------
// load config 
//---------------------------------------------------------
/**
  * @brief load numen skill configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_numen_skill(xmlNodePtr cur, numen_info_t* p_numen)
{
	uint32_t i = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Skill"))) {
            //numen_skill_t* p_skill = &(p_numen->skill_vec_[i]);
            numen_skill_t skill = {0};
			get_xml_prop(skill.skill_id, cur, "ID");
			get_xml_prop(skill.skill_lv, cur, "NeedLv");

            p_numen->skill_vec_.push_back(skill);
			i++;
		}
		cur = cur->next;
	}
	
	return 0;
}
/**
  * @brief load call numen odds configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_call_numen_odds(xmlNodePtr cur, call_numen_odds_t* p_numen)
{
	uint32_t i = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Numen"))) {
            numen_odds_t odds = {0};
            //numen_odds_t* p_odds = &(p_numen->odds_vec_[i]);
			get_xml_prop(odds.numen_id, cur, "ID");
			get_xml_prop(odds.odds, cur, "Odds");

            //p_numen->odds_vec_.push_back(odds);
			i++;
		}
		cur = cur->next;
	}
	
	return 0;
}
/**
  * @brief load numen configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_numens(xmlNodePtr cur)
{
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Numen"))) {
			numen_info_t numen_obj = {0};
			memset(&numen_obj, 0x00, sizeof(numen_info_t));
			get_xml_prop(numen_obj.numen_id, cur, "ID");
			get_xml_prop_raw_str(numen_obj.name, cur, "Name");

			// load configs for each Skill
			if (load_numen_skill(cur->xmlChildrenNode, &numen_obj) == -1) {
				throw XmlParseError(std::string("failed to load an skill"));
				return -1;
			}
			
			//log
			TRACE_LOG("numen info:[%u %s] ------>", numen_obj.numen_id, numen_obj.name);
			for (uint32_t i = 0; i < numen_obj.skill_vec_.size(); i++) {
				numen_skill_t* p_skill = &(numen_obj.skill_vec_[i]);
				TRACE_LOG("numen skill_%u[%u %u]",i, p_skill->skill_id, p_skill->skill_lv);
			}

			numens_map_.insert(std::map<uint32_t, numen_info_t>::value_type(numen_obj.numen_id, numen_obj));
		}
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("CallNumen"))) {
            call_numen_odds_t odds_obj = {0};
            //memset(&odds_obj, 0x00, sizeof(call_numen_odds_t));
            get_xml_prop(odds_obj.item_id, cur, "ItemID");
            get_xml_prop(odds_obj.god_id, cur, "GodID");

            /*
			// load configs for each Skill
			if (load_call_numen_odds(cur->xmlChildrenNode, &odds_obj) == -1) {
				throw XmlParseError(std::string("failed to load an odds"));
				return -1;
			}*/
			
			//log
			TRACE_LOG("numen info:[%u %u] ------>", odds_obj.item_id, odds_obj.god_id);
            /*
			for (uint32_t i = 0; i < odds_obj.odds_vec_.size(); i++) {
				numen_odds_t* p_odds = &(odds_obj.odds_vec_[i]);
				TRACE_LOG("numen odds_%u[%u %u]",i, p_odds->numen_id, p_odds->odds);
			}*/

            call_numen_rule_map_.insert(std::map<uint32_t, call_numen_odds_t>::value_type(odds_obj.item_id, odds_obj));
		}

		cur = cur->next;
	}
	return 0;
}

/**
  * @brief unload numen configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int unload_numens()
{
	numens_map_.clear();
	return 0;
}
