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
#include "summon_monster.hpp"
#include "apothecary.hpp"
#include "stat_log.hpp"
#include "trade.hpp"
#include "global_data.hpp"
#include "home.hpp"
#include "common_op.hpp"
//#include "pet_attribute.hpp"
//--------------------------------------------------------------------------------
// global 
//--------------------------------------------------------------------------------
/*! the all skills*/
//std::map<uint32_t, summon_info_t> summons_map_;
int summon_change_noti(player_t* p);
//------------------------------------------------------------------
// statics
//------------------------------------------------------------------
summon_info_t* get_summon(uint32_t summon_id)
{
	std::map<uint32_t, summon_info_t>::iterator it = summons_map_.find(summon_id);
	if (it != summons_map_.end()) {
		return &(it->second);
	}

	return 0;
}

uint32_t get_next_gradation_lv(uint32_t summon_id)
{
    summon_info_t *p_mon = get_summon(summon_id);
    if (p_mon) {
        return p_mon->next_lv;
    }
    return 0;
}

uint32_t get_summon_attr_type(uint32_t summon_id)
{
    summon_info_t *p_mon = get_summon(summon_id);
    if (p_mon) {
        return p_mon->attr_type;
    }
    return 0;
}

summon_skill_info_t* get_summon_super_skill(uint32_t mon_type)
{
    summon_info_t* p_mon = get_summon(mon_type);
    if ( p_mon == NULL || !is_summon_final_status(mon_type) ) {
        return NULL;
    }

    return &(p_mon->skills[3]); /* Hard code*/
}

summon_skill_scroll_t * get_scroll(uint32_t scrollid)
{
    std::map<uint32_t, summon_skill_scroll_t>::iterator it = sum_scroll_map_.find(scrollid);
    if (it != sum_scroll_map_.end()) {
        return &(it->second);
    }
    return 0;
}

bool check_odds_faild(summon_info_t* p_summ)
{
	uint32_t rd_num = random() % 100;
	if (rd_num > p_summ->odds) {
		return true;
	}
	return false;
}


summon_mon_t* get_player_summon(player_t* p, uint32_t mon_tm)
{
	for (uint32_t i = 0; i < p->summon_mon_num; i++) {
		if (p->summons[i].mon_tm == mon_tm) {
			return &(p->summons[i]);
		}
	}

	return 0;
}

uint32_t get_summon_skill_cnt(player_t* p, uint32_t mon_tm)
{
    for (uint32_t i = 0; i < p->summon_mon_num; i++) {
        if (p->summons[i].mon_tm == mon_tm) {
            return p->summons[i].skills_cnt;
        }
    }
    return 0;
}

bool is_summon_skill_exist(player_t* p, uint32_t mon_tm, uint32_t skill)
{
    summon_mon_t * p_mon = get_player_summon(p, mon_tm);
    if (p_mon) {
        for (uint32_t i = 0; i < p_mon->skills_cnt; i++) {
            if (p_mon->skills[i].skill_id == skill) {
                return true;
            }
        }
    }
    return false;
}

bool is_summon_add_new_skill(player_t* p, uint32_t mon_tm)
{
    //printf(" xxxx %u \n", get_summon_skill_cnt(p, mon_tm));
    uint32_t max_skill_limit = 5;
    if (p->power_user) {
        max_skill_limit += 1;
    }
    if (get_summon_skill_cnt(p, mon_tm) <= max_skill_limit) {
        return true;
    }
    return false;
}

bool is_summon_skill_lv_up(player_t* p, uint32_t mon_tm, uint32_t new_skill, uint32_t lv)
{
    /*
    summon_mon_t * p_mon = get_player_summon(p, mon_tm);
    if (p_mon) {
        for (uint32_t i = 0; i < p_mon->skills_cnt; i++) {
            if (p_mon->skills[i].skill_id == new_skill 
                && p_mon->skills[i].skill_lv < lv) {
                return true;
            }
        }
    }*/
    for (uint32_t i = 0; i < p->summon_mon_num; i++) {
        if (p->summons[i].mon_tm == mon_tm) {
            for (uint32_t j = 0; j < p->summons[i].skills_cnt; j++) {
                if (p->summons[i].skills[j].skill_id == new_skill 
                    && p->summons[i].skills[j].skill_lv < lv) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool  is_summon_role_type_exist(player_t* p, uint32_t role_type)
{
	for (uint32_t i = 0; i < p->summon_mon_num; i++)
	{
		if(p->summons[i].mon_type == role_type)
		{
			return true;
		}
	}
	return false;
}

bool is_summon_self_skill(uint32_t mon_type, uint32_t id)
{
    summon_info_t* p_mon = get_summon(mon_type);
    if ( p_mon == NULL) {
        return false;
    }

    for (uint32_t i = 0; i < max_summon_skills; i++) {
        if (p_mon->skills[i].skill_id == id) {
            return true;
        }
    }
    return false;
}

bool is_summon_final_status(uint32_t mon_type)
{
    uint32_t type = mon_type % 10;
    if ( type == 5 || type == 6 ) {
        return true;
    }
    return false;
}

static bool check_if_evolve(player_t *p, summon_mon_t* p_summon)

{
	const summon_info_t* mm = get_summon(p_summon->mon_type);
	
	if (mm && p_summon->lv >= mm->next_lv) {
		const summon_info_t* next_mm = get_summon(p_summon->mon_type + 1);
		if (!next_mm || (next_mm->evolve_need_item && !(p->my_packs->is_item_exist(next_mm->evolve_need_item)))) {
			return false;
		}
		if ((p_summon->mon_type + 1) % 10 == 6 )
		{
			if(is_vip_player(p))
			{
				return true;
			}
			return false;
		}

		return true;
	}
	return false;
}


bool check_summon_type_exist(player_t* p, uint32_t mon_type)
{
	for (uint32_t i = 0; i < p->summon_mon_num; i++) {
		if (p->summons[i].mon_type / 10 == mon_type / 10) {
			return true;
		}
	}
	return false;
}

uint32_t get_summon_tm_by_one_type(player_t* p, uint32_t mon_type)
{
	for (uint32_t i = 0; i < p->summon_mon_num; i++) {
			if (p->summons[i].mon_type / 10 == mon_type / 10) {
				return p->summons[i].mon_tm;
			}
		}
		return 0;
}

uint32_t get_summon_xml_skill_cnt(uint32_t mon_type)
{
    uint32_t tail_num = mon_type % 10;
    if (tail_num <= 5) {
        return tail_num - 1;
    }
    return 4;
}

uint32_t get_summon_type(player_t* p, uint32_t mon_tm)
{
    for (uint32_t i = 0; i < p->summon_mon_num; i++) {
        if (p->summons[i].mon_tm == mon_tm) {
            return p->summons[i].mon_type;
        }
    }
    return 0;
}

uint32_t get_summon_base_type(player_t* p, uint32_t mon_tm)
{
    for (uint32_t i = 0; i < p->summon_mon_num; i++) {
        if (p->summons[i].mon_tm == mon_tm) {
            return (p->summons[i].mon_type / 10 * 10 + 1);
        }
    }
    return 0;
}

bool is_sum_final_status_by_tm(player_t* p, uint32_t mon_tm)
{
    for (uint32_t i = 0; i < p->summon_mon_num; i++) {
        if (p->summons[i].mon_tm == mon_tm && 
            is_summon_final_status(p->summons[i].mon_type)) {
            return true;
        }
    }
    return false;
}

bool is_vip_valid_summon(player_t *p, uint32_t mon_type)
{
	if (mon_type % 10 == 6 && is_vip_player(p)) {
        return true;
	}
	return false;
}

bool is_valid_summon(player_t *p, summon_mon_t * p_summon)
{
	//summon_info_t * std_mon = get_summon(p_summon->mon_type);
	if (p_summon->mon_type % 10 == 6)
	{
		if(is_vip_player(p))
		{
			return true;
		}
		return false;
	}
	return true;
}

uint32_t del_summon(player_t *p, uint32_t mon_tm)
{
	uint32_t ch_flg = 0;

	for (uint16_t i = 0; i < p->summon_mon_num; i++) {
		if (p->summons[i].mon_tm == mon_tm) {
			if (i != (p->summon_mon_num - 1)) {
				for (uint16_t n = i; n < (p->summon_mon_num - 1); n++) {
					p->summons[n] = p->summons[n + 1];
				}
				//memcpy(&(p->summons[i]), &(p->summons[i + 1]), sizeof(summon_mon_t) * (p->summon_mon_num - 1));
			}
			p->summon_mon_num--;
			if (!ch_flg) {
				if (p->fight_summon) {
					if (p->summons[i].mon_tm == p->fight_summon->mon_tm) {
					
					} else {
						p->fight_summon--;
					}
				}
			}
			if (p->fight_summon && p->fight_summon->mon_tm == mon_tm) {
				p->fight_summon = &(p->summons[p->summon_mon_num - 1]);
			}
			return p->summon_mon_num;
		}
		if (p->fight_summon && p->summons[i].mon_tm == p->fight_summon->mon_tm) {
			ch_flg = 1;
		}
	}
	return p->summon_mon_num;
}

summon_mon_t* get_player_offline_summon(player_t* p)
{
	for (uint32_t i = 0; i < p->summon_mon_num; i++) {
		if (p->summons[i].call_flag == summon_in_offline) {
			return &(p->summons[i]);
		}
	}
	return 0;
}


inline void do_stat_log_hatch_summon_mon()
{
    uint32_t buf[1] = {1};
	msglog(statistic_logfile, stat_log_hatch_summon_mon, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [buf=%u]", stat_log_hatch_summon_mon, buf[0]);
}

/**
 * @brief add here for platform of statstics
 */
inline void do_stat_log_summon_level(uint32_t uid, uint32_t monid, uint32_t level)
{
    uint32_t cmd = 0;
    uint32_t buf[2] = {0};
    buf[0] = uid;
    buf[1] = level;
    switch (monid) {
        case 1001:
        case 1002:
        case 1003:
        case 1004:
        case 1005:
            cmd = stat_log_baiyuhu;
            break;
        case 1011:
        case 1012:
        case 1013:
        case 1014:
        case 1015:
            cmd = stat_log_yulingtu;
            break;
        case 1021:
        case 1022:
        case 1023:
        case 1024:
        case 1025:
            cmd = stat_log_liuguanglu;
            break;
        default:
            cmd = 0x09012000 + (monid - 1000) / 10 - 2;
            break;
    }
	msglog(statistic_logfile, cmd, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [buf=%u %u]", cmd, buf[0], buf[1]);
}

int pack_hatch_summon_mon(uint32_t faild_flg, uint32_t pre_mon_tm, uint32_t status, summon_mon_t* p_summ, void* buf)
{
	int idx = 0;
	pack(buf, faild_flg, idx);
	if (p_summ) {
		pack(buf, pre_mon_tm, idx);
		pack(buf, p_summ->mon_type, idx);
		pack(buf, p_summ->mon_tm, idx);
		pack(buf, p_summ->lv, idx);
		pack(buf, p_summ->exp, idx);
		pack(buf, p_summ->fight_value, idx);
		pack(buf, p_summ->nick, max_nick_size, idx);
		pack(buf, static_cast<uint8_t>(status), idx);
		//额外属性
		pack(buf, p_summ->attr_per, idx);
		pack(buf, p_summ->pet_attr->agility, idx);
		pack(buf, p_summ->pet_attr->strength, idx);
		pack(buf, p_summ->pet_attr->body_quality, idx);

		pack(buf, p_summ->pet_attr->accurate_value, idx);

		pack(buf, p_summ->pet_attr->hp, idx);

		pack(buf, p_summ->pet_attr->crit_value, idx);

		pack(buf, p_summ->pet_attr->attack, idx);

		pack(buf, p_summ->pet_attr->def_value, idx);

		pack(buf, p_summ->pet_attr->hit_value, idx);

		pack(buf, p_summ->pet_attr->dodge_value, idx);
	} 
	else 
	{
		char nick[max_nick_size] = "";
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, nick, max_nick_size, idx);
		pack(buf, static_cast<uint8_t>(0), idx);

		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
	}
	return idx;
}


//------------------------------------------------------------------
// Cmds
//------------------------------------------------------------------
/**
 * @brief player get a new summon monster
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int hatch_summon_mon_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t item_id = 0;
	uint32_t mon_type = 0;
	unpack(body, item_id, idx);
	
	const GfItem* itm = items->get_item(item_id);
	
	if (!itm || ! is_valid_montype(itm->summon_id()) ) {
		ERROR_LOG("summon mon type error![%u %u]", p->id, item_id);
		return -1;
	}

	mon_type = itm->summon_id();
	if (check_summon_type_exist(p, mon_type)) {
		return send_header_to_player(p, p->waitcmd, cli_err_same_summon_exist, 1);
	}

	summon_info_t* p_summon = get_summon(mon_type - 1);
	uint32_t pre_summon_tm = 0;
	
	if (p_summon->pre_summon_id) {
		
		//return send_header_to_player(p, p->waitcmd, cli_err_summon_cannot_evolve, 1);
		pre_summon_tm = get_summon_tm_by_one_type(p, p_summon->pre_summon_id);
		if (!pre_summon_tm) {
			return send_header_to_player(p, p->waitcmd, cli_err_summon_cannot_evolve, 1);
		}
	}
	
	if (check_odds_faild(p_summon)) {
		bool is_shop_item = itm->is_item_shop_sale();
		db_use_item_ex(0, p->id, p->role_tm, item_id, 1, is_shop_item);
		p->my_packs->del_item(p, item_id, 1, channel_string_other);
		
		int idx = sizeof(cli_proto_t);
		idx += pack_hatch_summon_mon(1, 0, 0, 0, pkgbuf + idx);

		
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);	
	}

	
	KDEBUG_LOG(p->id, "HATCH SUMMON\t[%u %u %u]", p->id, mon_type, pre_summon_tm);
	return db_hatch_summon_mon(p, item_id, mon_type, 0);
}

/**
 * @brief set nick name for a summon monster
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int set_summon_nick_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	body[bodylen - 1] = '\0';
	
	CHECK_DIRTYWORD(p, body);
	CHECK_INVALID_WORD(p, body);

	if (!(p->fight_summon) /*|| p->fight_summon->mon_tm != mon_tm*/) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_summon_mon, 1);
	}

    memcpy(p->fight_summon->nick, body, max_nick_size);
	return db_set_summon_nick(p);
}

/**
 * @brief feed a summon monster
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int feed_summon_mon_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	
	uint32_t mon_tm;
	uint32_t itm_id;
	
	unpack(body, mon_tm, idx);	
	unpack(body, itm_id, idx);

	summon_mon_t* p_pet = get_player_summon(p, mon_tm);
	if(!p_pet){
		 return send_header_to_player(p, p->waitcmd, cli_err_no_summon_mon, 1);
	}

	const GfItem* itm = items->get_item(itm_id);
	if (!itm) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
	}

	return db_feed_summon_mon(p,  mon_tm, itm_id, itm->fight_value());
}

/**
 * @brief set a summon monster can fight
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int set_fight_summon_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t mon_tm;
	unpack(body, mon_tm, idx);	

	summon_mon_t* p_mon = get_player_summon(p, mon_tm);
	if ( !p_mon || p_mon == p->fight_summon || p_mon->call_flag == summon_in_offline ) {
		return send_header_to_player(p, p->waitcmd, cli_err_cant_set_fight, 1);
	}

	const summon_info_t* mm = get_summon(p_mon->mon_type);
	if (mm && mm->need_vip_lv && (!is_vip_player(p) || p->vip_level < mm->need_vip_lv) ) {
		return send_header_to_player(p, p->waitcmd, cli_err_less_vip_lv, 1);
	}
	
	p->fight_summon = p_mon;
	p->fight_summon->call_flag = 1;

	db_set_fight_summon(p, mon_tm, 1);

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, mon_tm, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);
	return summon_change_noti(p);
}


int get_summons_attr_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->summon_mon_num, idx);
	for(uint16_t i = 0; i < p->summon_mon_num; i++)
	{
		summon_mon_t* p_mon = &(p->summons[i]);
		pack(pkgbuf, p_mon->mon_tm, idx);
		pack(pkgbuf, p_mon->mon_type, idx);
		pack(pkgbuf, p_mon->lv, idx);
		pack(pkgbuf, p_mon->exp, idx);
		pack(pkgbuf, p_mon->attr_per, idx); 	   //成长值
		pack(pkgbuf, p_mon->pet_attr->strength, idx);        //力量
		pack(pkgbuf, p_mon->pet_attr->agility,  idx);        //敏捷
		pack(pkgbuf, p_mon->pet_attr->body_quality, idx);    //体质
		pack(pkgbuf, p_mon->pet_attr->accurate_value, idx);  //精准
		pack(pkgbuf, p_mon->pet_attr->hp, idx);              //生命
		pack(pkgbuf, p_mon->pet_attr->crit_value, idx);      //致命一击
		pack(pkgbuf, p_mon->pet_attr->attack, idx);             //攻击
		pack(pkgbuf, p_mon->pet_attr->def_value, idx);       //防御
		pack(pkgbuf, p_mon->pet_attr->hit_value, idx);       //命中值
		pack(pkgbuf, p_mon->pet_attr->dodge_value, idx);     //躲闪值
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int pack_sum_new_skill_info(summon_mon_t* p_mon, void *buf) 
{
    int idx = 0;
    if (p_mon == NULL) {
        pack(buf, static_cast<uint32_t>(0), idx);
        return idx;
    }
    
    uint32_t xml_skill_cnt = get_summon_xml_skill_cnt(p_mon->mon_type);
    uint32_t cnt = p_mon->skills_cnt > xml_skill_cnt ? (p_mon->skills_cnt - xml_skill_cnt) : 0;

    pack(buf, cnt, idx);
    for (uint32_t i = xml_skill_cnt; i < p_mon->skills_cnt; i++) {
        TRACE_LOG(" new skill [%u %u]", p_mon->skills[i].skill_id, p_mon->skills[i].skill_lv);
        pack(buf, p_mon->skills[i].skill_id, idx);
        pack(buf, static_cast<uint32_t>(p_mon->skills[i].skill_lv), idx);
    }
    return idx;
}
/**
 * @brief get all summon monsters
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_summons_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->summon_mon_num, idx);
	for (uint16_t i = 0; i < p->summon_mon_num; i++) 
	{
		summon_mon_t* p_mon = &(p->summons[i]);
		pack(pkgbuf, p_mon->mon_tm, idx);
		pack(pkgbuf, p_mon->mon_type, idx);
		pack(pkgbuf, p_mon->nick, max_nick_size, idx);
		pack(pkgbuf, p_mon->lv, idx);
		pack(pkgbuf, p_mon->fight_value, idx);
		pack(pkgbuf, p_mon->exp, idx);
		pack(pkgbuf, p_mon->call_flag, idx);

        idx += pack_sum_new_skill_info(p_mon, pkgbuf + idx);

        uint16_t dis_skill_cnt = 0;
		pack(pkgbuf, dis_skill_cnt, idx);

		TRACE_LOG("%u %u %s %u %u %u", p->id, p_mon->mon_type, p_mon->nick, p_mon->lv, p_mon->call_flag, p_mon->skills_cnt);
	}
	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int get_friend_summon_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t uid;
	uint32_t role_tm;
	unpack(body, uid, idx);	
	unpack(body, role_tm, idx);	

	p->sesslen = 0;
	pack(p->tmp_session, uid, p->sesslen);
	pack(p->tmp_session, role_tm, p->sesslen);
	return db_get_summon_list(p, uid, role_tm);
}

/**
 * @brief call or take back summon monsters
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int call_summon_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint8_t	call_flag = 0;
	unpack(body, call_flag, idx);

	if (p->fight_summon) {
		p->fight_summon->call_flag = call_flag;
		db_set_fight_summon(p, p->fight_summon->mon_tm, call_flag);
	}

	if (p->trade_grp)
	{
		TRACE_LOG("player %u is in trade call summon", p->id);
		p->waitcmd = 0;
		int ifx = sizeof(tr_proto_t);
		pack_h(pkgbuf, call_flag, ifx);
		init_tr_proto_head(p, p->id, pkgbuf, ifx, tr_call_summon);
		send_to_trade_svr(p, pkgbuf, ifx, p->trade_grp->fd_idx);
		db_set_fight_summon(p, p->fight_summon->mon_tm, call_flag);
		return 0;
	}

	if(p->home_grp){
		p->waitcmd = 0;
		int ihx = sizeof(home_proto_t);
		pack_h(pkgbuf, call_flag, ihx);
		init_home_proto_head(p, p->id, pkgbuf, ihx, home_pet_follow);
		send_to_home_svr(p, pkgbuf, ihx, p->home_grp->fd_idx);
		db_set_fight_summon(p, p->fight_summon->mon_tm, call_flag);
		return 0;
	}	

	if (call_flag == summon_in_offline && get_player_offline_summon(p)){
		return -1;
	}
	
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, call_flag, idx);
	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_map(p, pkgbuf, idx, 1);	
	return 0;
}

/**
 * @brief evolve summon monster
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int evolve_summon_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t mon_tm;
	unpack(body, mon_tm, idx);

	if (!(p->fight_summon) || p->fight_summon->mon_tm != mon_tm) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_summon_mon, 1);
	}

	if (!check_if_evolve(p, p->fight_summon)) {
		ERROR_LOG("canot evolve summon:[%u %u %u]", p->id, p->fight_summon->mon_type, p->fight_summon->lv);
		return send_header_to_player(p, p->waitcmd, cli_err_summon_cannot_evolve, 1);
	}
	
	KDEBUG_LOG(p->id, "EVOLVE SUMMON\t[%u %u %u %u]", p->id, p->fight_summon->mon_tm, p->fight_summon->mon_type, p->fight_summon->lv);
	return db_evolve_summon(p);
}

/**
 * @brief disable summon monster skills
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int disable_summon_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t skill_id = 0;
	uint8_t	skill_idx = 0;
	uint8_t	flag = 0;
	unpack(body, skill_id, idx);
	//unpack(body, skill_idx, idx);
	unpack(body, flag, idx);
	TRACE_LOG("[%u %u %u %u]",p->id, skill_id, skill_idx, flag);

	//if (!(p->fight_summon) || skill_idx >= max_summon_skills || skill_idx == 0) {
    if ( !(p->fight_summon) ) {
		WARN_LOG("disable skill error:[%u %u]",p->id, skill_id);
		return send_header_to_player(p, p->waitcmd, cli_err_no_summon_mon, 1);
	}

	for (uint32_t i = 0; i < p->fight_summon->skills_cnt; i++) {
		if (p->fight_summon->skills[i].skill_id == skill_id) {
			p->fight_summon->skills[i].used_flag = flag;
		}
	}
/*
	p->fight_summon->disable_skills[skill_idx] = flag ? 0 : 1;
	p->fight_summon->skills[skill_idx].used_flag = flag;
*/
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, skill_id, idx);
	//pack(pkgbuf, skill_idx, idx);
	pack(pkgbuf, flag, idx);
	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
 * @brief use summon skills scroll
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int use_summon_skills_scroll_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t mon_tm = 0, scrollid = 0;
    unpack(body, mon_tm, idx);
    unpack(body, scrollid, idx);

    if ( !(p->my_packs->check_bag_item_count(scrollid, 1)) ) {
        return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
    }

    if (!is_sum_final_status_by_tm(p, mon_tm)) {
        return send_header_to_player(p, p->waitcmd, cli_err_summon_no_final_status, 1);
    }

    summon_skill_scroll_t * scrolls = get_scroll(scrollid);
    if (scrolls == NULL) {
        WARN_LOG("disable skill scroll error:[%u %u]",p->id, scrollid);
        return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
    }

    uint32_t new_skill = 0;
    uint32_t new_skill_lv = 0;
    uint32_t odds = rand() % 1000;
    uint32_t odds_flag = 0;
    for (uint32_t i = 0; i < scrolls->skill_vec.size(); i++) {
        odds_flag += scrolls->skill_vec[i].odds;
        if (odds < odds_flag) {
            new_skill = scrolls->skill_vec[i].skillid;
            new_skill_lv = scrolls->skill_vec[i].lv;
            break;
        }
    }

    TRACE_LOG("NEW SKILL uid[%u] montm[%u] [%u %u]", p->id, mon_tm, new_skill, new_skill_lv);
    uint32_t flag = 0;

    if ( is_summon_skill_exist(p, mon_tm, new_skill) ) {
        if ( is_summon_skill_lv_up(p, mon_tm, new_skill, new_skill_lv) ) {
            flag = 1;
        }
    } else {
        if ( is_summon_add_new_skill(p, mon_tm) ) {
            flag = 1;
        }
    }

    idx = 0;
    pack_h(dbpkgbuf, mon_tm, idx); 
    pack_h(dbpkgbuf, scrollid, idx); 
    pack_h(dbpkgbuf, flag, idx); 
    pack_h(dbpkgbuf, new_skill, idx); 
    pack_h(dbpkgbuf, new_skill_lv, idx); 
    return  send_request_to_db(p, p->id, p->role_tm, dbproto_use_sum_skills_scroll, dbpkgbuf, idx);
}

/**
 * @brief flag 1: replace 0: addition
 */
int summon_learn_skill(player_t* p, char* skill_buf, uint32_t mon_tm, 
                       uint32_t src_skill, uint32_t new_skill, int flag)
{
    int len = 0;
    //uint8_t skill_buf[max_summon_skills_len] = {0};

    summon_mon_t * p_mon = get_player_summon(p, mon_tm);
    if (!p_mon) {
        ERROR_LOG("renew summon skills error[%u %u]", p->id, mon_tm);
        return -1;
    }

    summon_skill_info_t * super_skill = get_summon_super_skill(p_mon->mon_type);
    if ( super_skill == NULL) {
        ERROR_LOG("renew summon no found super skills error[%u %u]", p->id, mon_tm);
        return -1;
    }

    uint32_t skills_id[max_summon_learn_skills] = {0};
    uint32_t pos = 0;
    for (uint32_t i = 1; i < max_summon_learn_skills && i < p_mon->skills_cnt; i++) {
        if (p_mon->skills[i].skill_id == super_skill->skill_id) { /*! skip super skill*/ 
            continue;
        }
        skills_id[pos++] = p_mon->skills[i].skill_id;
    }

    if (flag) {
        for (uint32_t i = 0; i < max_summon_learn_skills; i++) {
            if (skills_id[i] == src_skill) {
                skills_id[i] = new_skill;
                break;
            }
        }
    } else {
        skills_id[pos] = new_skill;
    }

    for (uint32_t i = 0; i < max_summon_learn_skills; i++) {
        len += sprintf((char*)skill_buf + len, "%u|", skills_id[i]);
    }

    if ((uint32_t)len >= max_summon_skills_len) {
        ERROR_LOG("summon skill too much [%u %u][%s]",p->id, mon_tm, (char*)skill_buf);
        return -1;
    }

    skill_buf[len - 1] = '\0';
    return 0;
}
/**
 * @brief renew summon monster skills
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int renew_summon_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t mon_tm = 0, skillid = 0, new_skill = 0, new_skill_lv = 0;
    unpack(body, mon_tm, idx);
    unpack(body, skillid, idx);
    unpack(body, new_skill, idx);
    unpack(body, new_skill_lv, idx);

    if ( !is_summon_skill_exist(p, mon_tm, skillid) ) {
        return send_header_to_player(p, p->waitcmd, cli_err_summon_no_learn_this_skill, 1);
    }
	
    if (!is_sum_final_status_by_tm(p, mon_tm)) {
        return send_header_to_player(p, p->waitcmd, cli_err_summon_no_final_status, 1);
    }

    TRACE_LOG(" %u %u %u %u ", mon_tm, skillid, new_skill, new_skill_lv);

    idx = 0;
    pack_h(dbpkgbuf, mon_tm, idx); 
    pack_h(dbpkgbuf, skillid, idx); 
    pack_h(dbpkgbuf, new_skill, idx); 
    pack_h(dbpkgbuf, new_skill_lv, idx); 
    return  send_request_to_db(p, p->id, p->role_tm, dbproto_renew_summon_skills, dbpkgbuf, idx);
}
/**
 * @brief notify the player change summon
 * @param p the request player 
 * @return 0 on success, -1 on error
 */
int summon_change_noti(player_t* p)
{
	if (!p->fight_summon) {
		return 0;
	}

	if (p->trade_grp) {
		TRACE_LOG("player %u summon change to trade svr", p->id);
		int idx = sizeof(tr_proto_t);
		pack_h(pkgbuf, p->fight_summon->mon_tm, idx);
		pack_h(pkgbuf, p->fight_summon->mon_type, idx);
		pack_h(pkgbuf, p->fight_summon->lv, idx);
		pack(pkgbuf, p->fight_summon->nick, max_nick_size, idx);
		init_tr_proto_head(p, p->id, pkgbuf, idx, tr_change_summon);
		send_to_trade_svr(p, pkgbuf, idx, p->trade_grp->fd_idx);
		return 0;
	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->fight_summon->mon_type, idx);
	pack(pkgbuf, p->fight_summon->lv, idx);
	pack(pkgbuf, p->fight_summon->nick, max_nick_size, idx);
	
	init_cli_proto_head(pkgbuf, p, cli_proto_summon_change_noti, idx);
	send_to_map_except_self(p, pkgbuf, idx);

	return 0;
}

int allocate_exp_to_summon_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	uint32_t mon_tm = 0;
	uint32_t allocated_exp = 0;
	int idx = 0;
	unpack(body, mon_tm, idx);
	unpack(body, allocated_exp, idx);
	summon_mon_t * p_mon = get_player_summon(p, mon_tm);
	KDEBUG_LOG(p->id, "user allocated exp to summon [%u %u %u]", p->id, mon_tm, allocated_exp);

	
	if(!p_mon) 
	{
		WARN_LOG("allocated exp noexit summon[%u %u]", p->id, mon_tm);
		return send_header_to_player(p, p->waitcmd,	cli_err_allocate_exp_summon_not_exit, 1); 
	}
	if (allocated_exp > p->allocator_exp)
	{
		WARN_LOG("allocated exp noenough exp[%u %u]", p->id, allocated_exp);
		return send_header_to_player(p, p->waitcmd, cli_err_allocate_exp_noenough_exp, 1);
	}
	if(p_mon->lv >= p->lv && allocated_exp >= 0)
	{
		return send_header_to_player(p, p->waitcmd, cli_err_allocate_exp_lv_over_range, 1);
	}

	
	summon_info_t * std_mon = get_summon(p_mon->mon_type);
	uint32_t lv_limit = std_mon->next_lv;  
	if (lv_limit >= p->lv)
	{
		lv_limit = p->lv;

	}

	if (p_mon->mon_type % 10 == 5) {
		lv_limit = p->lv;
	}

	if(p_mon->mon_type % 10 < 5 && p_mon->lv >= std_mon->next_lv && allocated_exp >= 0)
	{
		return send_header_to_player(p, p->waitcmd, cli_err_allocate_exp_need_evolve, 1);
	}



	uint32_t pre_exp = p_mon->exp;
	uint32_t pre_lv = p_mon->lv;
	calc_lv_add_exp(allocated_exp, p_mon->exp, p_mon->lv, lv_limit, p->role_type, p->id, true);  

	if (pre_lv != p_mon->lv) {
		attr_data * data = new attr_data;
	   	calc_summon_attr(data, std_mon->attr_type, p_mon->attr_per, p_mon->lv);
		set_summon_attr(p_mon, data);
	}

	allocated_exp = p_mon->exp - pre_exp;
	db_allocate_exp_to_summon(p, p_mon, allocated_exp);

//	calc_pet_attribute(p_mon);
	TRACE_LOG("allocated exp to summon[%u %u %u]", p->id, mon_tm, allocated_exp);
	return 0;
}

int get_allocator_exp_cmd(player_t *p, uint8_t *body = 0, uint32_t bodylen =0)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->allocator_exp, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_get_allocator_exp, idx);
	TRACE_LOG("usr get allocator exp[%u %u]", p->id, p->allocator_exp);
	return send_to_player(p, pkgbuf, idx, 1);
}


//------------------------------------------------------------------
// request to dbproxy
//------------------------------------------------------------------
/**
  * @brief player set a new summon monster to db
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_hatch_summon_mon(player_t* p, uint32_t item_id, uint32_t mon_type, uint32_t pre_summon_tm)
{
	int idx = 0;
	uint32_t status = 0;
	if (pre_summon_tm) {
		if (p->fight_summon && p->fight_summon->mon_tm == pre_summon_tm) {
			status = 1;
		}
	} else {
		status = p->fight_summon ? 0 : 1;
	}
	
	summon_info_t* p_summon = get_summon(mon_type - 1);
	if (!p_summon || p->lv < p_summon->hatch_lv) {
		WARN_LOG("summon mon type error[%u %u]:[%u %u]", p->id, mon_type, p->lv, p_summon->hatch_lv);
		return -1;
	}

	if (p_summon->need_vip_lv && is_vip_player(p) && p->vip_level < p_summon->need_vip_lv) {
		return send_header_to_player(p, p->waitcmd, cli_err_less_vip_lv, 1);
	}

	pack_h(dbpkgbuf, item_id, idx);
	pack_h(dbpkgbuf, mon_type, idx);
	pack_h(dbpkgbuf, pre_summon_tm, idx);
	pack_h(dbpkgbuf, status, idx);
	pack(dbpkgbuf, p_summon->name, max_nick_size, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_hatch_summon_mon, dbpkgbuf, idx);
}

/**
  * @brief player set summon monster nick name
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_set_summon_nick(player_t* p)
{
	int idx = 0;
	pack_h(dbpkgbuf, p->fight_summon->mon_tm, idx);
	pack(dbpkgbuf, p->fight_summon->nick, max_nick_size, idx);

	return send_request_to_db(p, p->id, p->role_tm, dbproto_set_summon_nick, dbpkgbuf, idx);
}

/**
  * @brief player set summon monster nick name
  * @param p the requester
  * @param itm_id the item id used
  * @param add_value the value add to fight_value
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_feed_summon_mon(player_t* p, uint32_t mon_tm, uint32_t itm_id, uint32_t add_value)
{
	int idx = 0;
	uint32_t max_value = max_fight_value;
	pack_h(dbpkgbuf, mon_tm, idx);
	pack_h(dbpkgbuf, add_value, idx);
	pack_h(dbpkgbuf, max_value, idx);
	pack_h(dbpkgbuf, itm_id, idx);

	return send_request_to_db(p, p->id, p->role_tm, dbproto_feed_summon_mon, dbpkgbuf, idx);
}

/**
  * @brief player set summon monster to fight
  * @param p the requester
  * @param mon_type of the requestee
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_set_fight_summon(player_t* p, uint32_t mon_type, uint32_t flag)
{
	int idx = 0;
	pack_h(dbpkgbuf, mon_type, idx);
	pack_h(dbpkgbuf, flag,     idx);
	return send_request_to_db(NULL,  p->id, p->role_tm, dbproto_set_fight_summon, dbpkgbuf, idx);
}

/**
  * @brief player set summon monster to fight
  * @param p the requester
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_get_summon_list(player_t* p, uint32_t uid, uint32_t role_tm)
{
	return send_request_to_db(p, uid, role_tm, dbproto_get_summon_list, 0, 0);
}

/**
  * @brief player evolve summon
  * @param p the requester
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_evolve_summon(player_t* p)
{
	int idx = 0;
	uint32_t new_type = p->fight_summon->mon_type + 1;
	const summon_info_t* next_mm = get_summon(p->fight_summon->mon_type + 1);
	uint32_t lv = p->fight_summon->lv;
	
	pack_h(dbpkgbuf, next_mm->evolve_need_item, idx);
	pack_h(dbpkgbuf, p->fight_summon->mon_tm, idx);
	pack_h(dbpkgbuf, new_type, idx);
	pack_h(dbpkgbuf, lv, idx);

	return send_request_to_db(p, p->id, p->role_tm, dbproto_evolve_summon, dbpkgbuf, idx);
}

/**
 * @brief pack p's summon info buf
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_player_summon(const player_t* p, void* buf)
{
	int idx = 0;
	uint16_t fight_cnt = 0;
	
	if (!(p->fight_summon)) {
		pack(buf, fight_cnt, idx);
	} else {
		fight_cnt = 1;

		summon_mon_t * p_mon = p->fight_summon;
		pack(buf, fight_cnt, idx);
		pack(buf, p_mon->mon_type, idx);
		pack(buf, p_mon->lv, idx);
		pack(buf, p_mon->nick, max_nick_size, idx);
		pack(buf, p_mon->call_flag, idx);
		pack(buf, p_mon->attr_per, idx);
		//计算该宠物的额外信息
		pack(buf, p_mon->pet_attr->agility, idx);

		pack(buf, p_mon->pet_attr->strength, idx);

		pack(buf, p_mon->pet_attr->body_quality, idx);
		
		pack(buf, p_mon->pet_attr->accurate_value, idx);
		
		pack(buf, p_mon->pet_attr->hp, idx);
		
		pack(buf, p_mon->pet_attr->crit_value, idx);
		
		pack(buf, p_mon->pet_attr->attack, idx);

		pack(buf, p_mon->pet_attr->def_value, idx);

		pack(buf, p_mon->pet_attr->hit_value, idx);

		pack(buf, p_mon->pet_attr->dodge_value, idx);

        idx += pack_sum_new_skill_info(p_mon, (uint8_t *)(buf) + idx);
	}

	return idx;
}

/**
 * @brief pack p's summon full info
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_player_summon_full_info(const player_t* p, void* buf)
{
	int idx = 0;
	uint16_t fight_cnt = 0;
	
	if (!(p->fight_summon)) {
		pack(buf, fight_cnt, idx);
	} else {
		fight_cnt = 1;
		pack(buf, fight_cnt, idx);
		pack(buf, p->fight_summon->mon_tm, idx);
		pack(buf, p->fight_summon->mon_type, idx);
		pack(buf, p->fight_summon->lv, idx);
		pack(buf, p->fight_summon->nick, max_nick_size, idx);
		pack(buf, p->fight_summon->fight_value, idx);
		pack(buf, p->fight_summon->exp, idx);
		pack(buf, p->fight_summon->call_flag, idx);
	}
	return idx;
}

bool has_summon_into_btl(const player_t * p, uint32_t battle_mode)
{
	if (battle_mode == btl_mode_lv_matching &&
			p->fight_summon && 
			p->fight_summon->call_flag == summon_fight_take) {

		if (p->battle_grp->pvp_btl_type == pvp_summon_mode || 
				p->battle_grp->pvp_btl_type == pvp_ghost_game || 
                p->battle_grp->pvp_btl_type == pvp_summon_mode_2 ||
				p->battle_grp->pvp_btl_type == pvp_dragon_ship ||
				p->battle_grp->pvp_btl_type == pvp_team_contest) {

			return true;
		}

	}
	return false;
}

/**
 * @brief pack p's summon info to battle
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_summon_to_btl(const player_t* p, void* buf, uint32_t battle_mode, bool take_flag)
{
	//int idx = 0;
	#pragma pack(1)
	struct btl_summon_info {
		uint32_t    attr_type; //属性模板
		uint32_t    attr_per; //成长系数
		uint32_t    mon_type;
		uint32_t    mon_tm;
		char        nick[max_nick_size];
		uint16_t    mon_lv;
		uint32_t    mon_exp;
		uint16_t    mon_fight_value;
		uint16_t    renascence_time;
		uint16_t	skills_cnt;
		summon_skill_t	skills[max_summon_skills];
	};
	#pragma pack()

	if (take_flag && battle_mode == btl_mode_pve && p->fight_summon && p->fight_summon->call_flag == 1) {
		int idx = 0;
		summon_info_t * sum = get_summon(p->fight_summon->mon_type);
		pack_h(buf, sum->attr_type, idx);
		pack_h(buf, p->fight_summon->attr_per, idx); 
		memcpy(((char*)buf) + 8, p->fight_summon, sizeof(btl_summon_info) - 8);
	} else if (has_summon_into_btl(p, battle_mode)) { 
		int idx = 0;
		summon_info_t * sum = get_summon(p->fight_summon->mon_type);
		pack_h(buf, sum->attr_type, idx);
		pack_h(buf, p->fight_summon->attr_per, idx); 
		memcpy(((char*)buf) + 8, p->fight_summon, sizeof(btl_summon_info) - 8);
	//	memset(buf, 0x00, sizeof(btl_summon_info));
	} else {
		memset(buf, 0x00, sizeof(btl_summon_info));
	}

	return sizeof(btl_summon_info);
}

int db_allocate_exp_to_summon(player_t *p, summon_mon_t* mon, uint32_t allocated_exp)
{
	int idx = 0;
	pack_h(dbpkgbuf, allocated_exp, idx);
	pack_h(dbpkgbuf, mon->mon_tm, idx);
	pack_h(dbpkgbuf, mon->exp, idx);
	pack_h(dbpkgbuf, (uint32_t)(mon->lv), idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_allocate_exp_to_summon, dbpkgbuf, idx); 
}


//------------------------------------------------------------------
// callback for handling package return from dbproxy
//------------------------------------------------------------------
int send_get_friend_summon_list_rsp(player_t* p, uint32_t friend_id, db_get_summon_list_rsp_t* rsp)
{
	int idx = 0;
	uint32_t uid;
	uint32_t role_tm;
	unpack(p->tmp_session, uid, idx);	
	unpack(p->tmp_session, role_tm, idx);	
	p->sesslen = 0;



	idx = sizeof(cli_proto_t);
 
	pack(pkgbuf, uid, idx);
	pack(pkgbuf, role_tm, idx);
	
	uint32_t mon_cnt = 0;
	int cnt_idx = idx;
	idx += 4;
	for (uint32_t i =0; i < rsp->mon_cnt; i++) {
		if (rsp->mons[i].fight_flag == 3) {
			pack(pkgbuf, rsp->mons[i].mon_tm, idx);
			pack(pkgbuf, rsp->mons[i].mon_type, idx);
			pack(pkgbuf, rsp->mons[i].lv, idx);
			pack(pkgbuf, rsp->mons[i].nick, sizeof(rsp->mons[i].nick), idx);
			mon_cnt++;
		}
	}	
	KDEBUG_LOG(p->id, "GET SUM\t[%u]", mon_cnt);
	pack(pkgbuf, mon_cnt, cnt_idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

void set_summon_skills(summon_mon_t* p_summon, uint32_t old_type)
{
	const summon_info_t* mm = get_summon(p_summon->mon_type);
	if (!mm) {
		WARN_LOG("no summon[%u]", p_summon->mon_type);
		return;
	}

    /*! Save summon old skill */
    sum_skill_t skill_info[max_summon_skills] = {{0},};
    uint32_t old_skill_cnt = get_summon_xml_skill_cnt(old_type);
    uint32_t learn_skill_cnt = p_summon->skills_cnt > old_skill_cnt ? (p_summon->skills_cnt - old_skill_cnt) : 0;
    for (uint32_t i = 0; i < learn_skill_cnt; i++) {
        skill_info[i].skillid = p_summon->skills[old_skill_cnt].skill_id;
        skill_info[i].skilllv = p_summon->skills[old_skill_cnt].skill_lv;
        old_skill_cnt++;
    }

    p_summon->skills_cnt = 0;
	for (uint32_t i = 0; i < max_summon_skills; i++) {
		uint32_t lv = 0;
		if (mm->skills[i].skill_id != 0 && p_summon->lv >= mm->skills[i].need_lv) {
			lv = p_summon->lv - mm->skills[i].need_lv + 1;
			lv = lv > mm->skills[i].top_skill_lv ? mm->skills[i].top_skill_lv : lv;
			
			p_summon->skills[i].skill_id = mm->skills[i].skill_id;
			p_summon->skills[i].skill_lv = lv;
			p_summon->skills[i].used_flag = p_summon->disable_skills[i] ? 0 : 1;
			p_summon->skills_cnt++;
		}
	}

    add_summon_new_skill(p_summon, learn_skill_cnt, skill_info);
}

void set_summon_skills_ex(summon_mon_t* p_summon)
{
	const summon_info_t* mm = get_summon(p_summon->mon_type);
	if (!mm) {
		WARN_LOG("no summon[%u]", p_summon->mon_type);
		return;
	}

    p_summon->skills_cnt = 0;
	for (uint32_t i = 0; i < max_summon_skills; i++) {
		uint32_t lv = 0;
		if (mm->skills[i].skill_id != 0 && p_summon->lv >= mm->skills[i].need_lv) {
			lv = p_summon->lv - mm->skills[i].need_lv + 1;
			lv = lv > mm->skills[i].top_skill_lv ? mm->skills[i].top_skill_lv : lv;
			
			p_summon->skills[i].skill_id = mm->skills[i].skill_id;
			p_summon->skills[i].skill_lv = lv;
			p_summon->skills[i].used_flag = p_summon->disable_skills[i] ? 0 : 1;
			p_summon->skills_cnt++;
		}
	}
}

void add_summon_new_skill(summon_mon_t* p_summon, uint32_t new_cnt, sum_skill_t *skill_data)
{
    if (new_cnt == 0 || skill_data == NULL) {
        return;
    }

    uint32_t idx = 0;
    for (uint32_t i = p_summon->skills_cnt; i < max_summon_skills && idx < new_cnt; i++) {
        p_summon->skills[i].skill_id  = skill_data[idx].skillid;
        p_summon->skills[i].skill_lv  = skill_data[idx].skilllv;
        p_summon->skills[i].used_flag = 1;
        p_summon->skills_cnt++;
        idx++;
    }
}

void set_summon_skill_lv(summon_mon_t* p_summon, uint32_t id, uint32_t lv)
{
    if (p_summon == NULL) return;

    for (uint32_t i = 0; i < max_summon_skills; i++) {
        if (p_summon->skills[i].skill_id == id) {
            p_summon->skills[i].skill_lv  = lv;
            p_summon->skills[i].used_flag = 1;
        }
    }
}

void renew_summon_new_skill(summon_mon_t* p_summon, uint32_t old_skill, sum_skill_t *skill_data)
{
    if (skill_data == NULL) {
        return ;
    }

    for (uint32_t i = 0; i < max_summon_skills; i++) {
        if (p_summon->skills[i].skill_id == old_skill) {
            p_summon->skills[i].skill_id  = skill_data->skillid;
            p_summon->skills[i].skill_lv  = skill_data->skilllv;
            p_summon->skills[i].used_flag = 1;

            break;
        }
    }
}

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
int db_get_summon_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	db_get_summon_list_rsp_t* rsp = reinterpret_cast<db_get_summon_list_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(db_get_summon_list_rsp_t) + rsp->mon_cnt * sizeof(summon_mon_info_t));

	if (p->waitcmd == cli_proto_get_friend_summon_list) {
		return send_get_friend_summon_list_rsp(p, id, rsp);
	}
	p->summon_mon_num = rsp->mon_cnt;
	if (rsp->mon_cnt > max_summon_num) {
		p->summon_mon_num = max_summon_num;
	}

	int  invalid = 0;
	db_re_evolve_t invalid_summon[max_summon_num];
	

	for (uint32_t i = 0; i < p->summon_mon_num; i++) 
	{
		summon_mon_t* p_mon = &(p->summons[i]);
		summon_mon_info_t* rsp_mon = &(rsp->mons[i]);
		p_mon->mon_type = rsp_mon->mon_type;
		p_mon->mon_tm = rsp_mon->mon_tm;
		memcpy(p_mon->nick, rsp_mon->nick, max_nick_size);
		p_mon->exp = rsp_mon->exp;
		p_mon->lv = rsp_mon->lv;
		p_mon->fight_value = rsp_mon->fight_value;
		p_mon->call_flag = rsp_mon->fight_flag;

		memset(p_mon->disable_skills, 0x00, sizeof(p_mon->disable_skills));
		p_mon->renascence_time = 0;
	//	p_mon->attr_per = rsp_mon->attr_per;

		const summon_info_t* pm = get_summon(p_mon->mon_type);
		if(pm)
		{
			p_mon->renascence_time = pm->renascence_time;
		}

		p_mon->attr_per = (uint32_t) (rsp_mon->attr_per + (int)pm->base_per);

		attr_data * data = new attr_data;
	   	calc_summon_attr(data, pm->attr_type, p_mon->attr_per, p_mon->lv);
		set_summon_attr(p_mon, data);

	//	calc_pet_attribute(p_mon, rsp_mon->attr_per);

		if(!is_valid_summon(p, p_mon))
		{
			invalid_summon[invalid].mon_type = p_mon->mon_type;
			invalid_summon[invalid].mon_tm = p_mon->mon_tm;
			invalid++;
		}
		//else 
		//{
			set_summon_skills_ex(p_mon);
            add_summon_new_skill(p_mon, rsp_mon->skill_cnt, rsp_mon->sum_skill);
		//}

		if (rsp_mon->fight_flag == summon_fight_notake || rsp_mon->fight_flag == summon_fight_take) {
			const summon_info_t* mm = get_summon(p_mon->mon_type);
			if (mm ) {
				p->fight_summon = p_mon;
			}
		}	
	}

	if (!invalid)
	{

		if (p->waitcmd == cli_proto_login ) {
			db_get_secondary_pro_list(p);
		}
	}
	else {
		int idx = 0;
		pack_h(dbpkgbuf, invalid, idx);
		for (int i = 0; i < invalid; i++)
		{
			pack_h(dbpkgbuf, invalid_summon[i].mon_tm, idx);
			pack_h(dbpkgbuf, invalid_summon[i].mon_type - 1, idx);
		}
		send_request_to_db(p, p->id, p->role_tm, dbproto_re_evolve_summon, dbpkgbuf, idx);
	}
	return 0;
}


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
int db_hatch_summon_mon_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	db_hatch_summon_rsp_t* rsp = reinterpret_cast<db_hatch_summon_rsp_t*>(body);
	int i = p->summon_mon_num++;
	p->summons[i].mon_type = rsp->summon_type;
	p->summons[i].mon_tm = rsp->summon_tm;
	p->summons[i].lv = rsp->summon_lv;
	p->summons[i].exp = 0;
	p->summons[i].fight_value = rsp->fight_value;
	p->summons[i].call_flag = 1;
	memcpy(p->summons[i].nick, rsp->nick, max_nick_size);
	set_summon_skills_ex(&(p->summons[i]));

	if (rsp->pre_summon_tm) {
		del_summon(p, rsp->pre_summon_tm);
	}

	//set fight
	if (rsp->status == 1) {
		p->fight_summon =&(p->summons[p->summon_mon_num - 1]);
		summon_change_noti(p);
	}

	//update  new summon attr
	const summon_info_t* pm = get_summon(rsp->summon_type);
	//the last sumon is the new one
	summon_mon_t  * p_mon = p->summons + p->summon_mon_num - 1;
	if(pm)
	{
		attr_data * data = new attr_data;
		p_mon->attr_per = pm->base_per;
	   	calc_summon_attr(data,
				        pm->attr_type, 
					   	p_mon->attr_per,
						p_mon->lv);
		set_summon_attr(p_mon, data);
	}


	//update my packs
	if (p->my_packs->del_item(p, rsp->used_itm_id, 1, channel_string_other) < 0) {
		WARN_LOG("del item error![%u %u]", p->id, p->my_packs->all_items_cnt());                
	}
 	do_stat_log_hatch_summon_mon();
	do_stat_log_hatch_summon_mon(p, rsp->summon_type);


	
	int idx = sizeof(cli_proto_t);
	idx += pack_hatch_summon_mon(0, rsp->pre_summon_tm, rsp->status, p_mon, pkgbuf + idx);

	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	proc_summon_achieve_logic(p);
	return 0;
}

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
int db_set_summon_nick_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->fight_summon->nick, max_nick_size, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

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
int db_feed_summon_mon_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	db_feed_summon_mon_rsp_t* rsp = reinterpret_cast<db_feed_summon_mon_rsp_t*>(body);


	summon_mon_t* p_pet = get_player_summon(p, rsp->mon_tm);
	if(p_pet)
	{
		p_pet->fight_value = rsp->total_value;
	}

	if ( p_pet && p_pet->fight_value > max_fight_value) {
		WARN_LOG("fight value too large:[%u %u %u]",p->id, p_pet->mon_tm, p_pet->fight_value);
		p_pet->fight_value = max_fight_value;
	}
	
	//update my packs
	if (p->my_packs->del_item(p, rsp->used_itm_id, 1, channel_string_other) < 0) {
		WARN_LOG("del item error![%u %u]", p->id, p->my_packs->all_items_cnt());                
	}

	uint16_t cur_value = rsp->total_value;


	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->mon_tm, idx);
	pack(pkgbuf, cur_value, idx);
	pack(pkgbuf, rsp->used_itm_id, idx);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

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
int db_set_fight_summon_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	db_set_fight_summon_rsp_t* rsp = reinterpret_cast<db_set_fight_summon_rsp_t*>(body);

	summon_mon_t* p_mon = get_player_summon(p, rsp->mon_tm);
	p->fight_summon = p_mon;
	p->fight_summon->call_flag = rsp->flag;

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->mon_tm, idx);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	//noti to map
	summon_change_noti(p);

	return 0;
}

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
int db_evolve_summon_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	db_evolve_summon_rsp_t* rsp = reinterpret_cast<db_evolve_summon_rsp_t*>(body);
	
	if (p->fight_summon) {

        uint32_t old_mon_type = p->fight_summon->mon_type;
		p->fight_summon->mon_type = rsp->mon_type;
        if (is_summon_final_status(old_mon_type)) {
            set_summon_skills(p->fight_summon, old_mon_type);
        } else {
            set_summon_skills_ex(p->fight_summon);
        }
		//for (uint32_t i = 0; i < p->fight_summon->skills_cnt; i++) {
		//	summon_skill_t* p_skill = &(p->fight_summon->skills[i]);
		//	TRACE_LOG("[%u]set summon skill[%u %u %u]", p->id, p->fight_summon->mon_type, p_skill->skill_id, p_skill->skill_lv );
		//}
		//update  new summon attr
		const summon_info_t* pm = get_summon(rsp->mon_type);
		const summon_info_t * od_mon = get_summon(rsp->mon_type - 1);
		if(pm && od_mon)
		{
			p->fight_summon->attr_per = p->fight_summon->attr_per
			   	+ pm->base_per - od_mon->base_per;
			attr_data * data = new attr_data;
		   	calc_summon_attr(data,
				        pm->attr_type, 
					   	p->fight_summon->attr_per,
						p->fight_summon->lv);
			set_summon_attr(p->fight_summon, data);
		}

	}
	do_stat_log_evolve_summon_mon(p, p->fight_summon->mon_type);
	
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->fight_summon->mon_tm, idx);
	pack(pkgbuf, p->fight_summon->mon_type, idx);

    idx += pack_sum_new_skill_info(p->fight_summon, pkgbuf + idx);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);
	proc_summon_achieve_logic(p);
	return 0;
}

int db_re_evolve_summon_callback(player_t * p, userid_t uid, void *body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t re_evolve_cnt = 0;
	int idx = 0;
	unpack_h(body, re_evolve_cnt, idx);
	CHECK_VAL_EQ(bodylen, sizeof(db_re_evolve_summon_rsp_t) + re_evolve_cnt * sizeof(db_re_evolve_t));

	db_re_evolve_summon_rsp_t * rsp = reinterpret_cast<db_re_evolve_summon_rsp_t*>(body);
    if (rsp->r_evolve_cnt)
    {
        for(uint32_t i = 0; i < rsp->r_evolve_cnt; i++)
        {
            summon_mon_t * p_mon = get_player_summon(p, (rsp->re_evolve[i]).mon_tm);
            if (!p_mon) {
                ERROR_LOG("db re_evolve summon error[%u %u]", p->id, (rsp->re_evolve[i]).mon_tm);
                return -1;
                //continue;
            }
            uint32_t old_mon_type = p_mon->mon_type;
            p_mon->mon_type = (rsp->re_evolve[i]).mon_type;

            TRACE_LOG("db re_evolve super summon[%u %u %u]", p->id, p_mon->mon_tm, p_mon->mon_type); 
            do_stat_log_summon_level(p->id, p_mon->mon_type, p_mon->lv);

            TRACE_LOG("db get summon:[%u %u %u %u %u %16s]", p_mon->mon_tm, p_mon->mon_type, p_mon->lv, 
                p_mon->fight_value, p_mon->exp, p_mon->nick);

            if (is_summon_final_status(old_mon_type)) {
                set_summon_skills(p_mon, old_mon_type);
            } else {
                set_summon_skills_ex(p_mon);
            }
            for (uint32_t i = 0; i < p_mon->skills_cnt; i++) {
                TRACE_LOG("[%u]set summon skill[%u %u %u]", p->id, p_mon->mon_type, 
                    p_mon->skills[i].skill_id, p_mon->skills[i].skill_lv );
            }
			const summon_info_t* pm = get_summon(p_mon->mon_type);
			const summon_info_t * od_mon = get_summon(p_mon->mon_type + 1);
			if(pm && od_mon)
			{
				p_mon->attr_per = p_mon->attr_per
				   	+ pm->base_per - od_mon->base_per;
				attr_data * data = new attr_data;
		   		calc_summon_attr(data,
					        pm->attr_type, 
						   	p_mon->attr_per,
							p_mon->lv);
				set_summon_attr(p_mon, data);
			}
		}

    }

    if (p->waitcmd == cli_proto_login ) {
        db_get_secondary_pro_list(p);
    }
	return 0;
}

int db_allocate_exp_to_summon_callback(player_t * p, userid_t uid, void * body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	db_allocate_exp_t * pkg = reinterpret_cast<db_allocate_exp_t *>(body);
	summon_mon_t * mon = get_player_summon(p, pkg->mon_tm);
	if (mon && mon->exp == pkg->mon_exp && p->allocator_exp >= pkg->allocator_exp)
	{
        //get_god_guard_mgr()->calc_group_quality_and_power(p, p->lv, p->add_quality);
        calc_and_save_player_attribute(p, 1);
//		KDEBUG_LOG(p->id, "db callback for summon allocate[%u %u %u %u]",  p->id, mon->exp, pkg->allocator_exp);
		p->allocator_exp = pkg->allocator_exp;
	//	mon->exp = pkg->mon_exp;
		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, p->allocator_exp, idx);
		pack(pkgbuf, mon->mon_tm, idx);
		pack(pkgbuf, mon->exp, idx);
		pack(pkgbuf, (uint32_t)(mon->lv), idx);
		init_cli_proto_head(pkgbuf, p, cli_proto_allocate_exp_to_summon, idx); 
		send_to_player(p, pkgbuf, idx, 1);
//		proc_summon_achieve_logic(p);
		return 0;
	}
	ERROR_LOG("[%u %u %u]allocated exp error", p->id, pkg->mon_tm, pkg->mon_exp);  
	return -1;
}

int db_use_sum_skills_scroll_callback(player_t * p, userid_t uid, void * body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    db_use_skills_scroll_t *rsp = reinterpret_cast<db_use_skills_scroll_t *>(body);
    
    p->my_packs->del_item(p, rsp->scrollid, 1, channel_string_other);

    summon_mon_t * p_mon = get_player_summon(p, rsp->mon_tm);
    if (!p_mon) {
        ERROR_LOG("db renew summon skills error[%u %u]", p->id, rsp->mon_tm);
        return -1;
    }

    if (rsp->flag == 1) {
        if ( is_summon_skill_exist(p, rsp->mon_tm, rsp->skill_info.skillid) 
            && is_summon_skill_lv_up(p, rsp->mon_tm, rsp->skill_info.skillid, rsp->skill_info.skilllv) )  {
            set_summon_skill_lv(p_mon, rsp->skill_info.skillid, rsp->skill_info.skilllv);
        } else {
            add_summon_new_skill(p_mon, 1, &(rsp->skill_info));
        }
    }
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->mon_tm, idx);
    pack(pkgbuf, rsp->scrollid, idx);
    pack(pkgbuf, rsp->skill_info.skillid, idx);
    pack(pkgbuf, rsp->skill_info.skilllv, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    send_to_player(p, pkgbuf, idx, 1); 
	proc_use_item_achieve_logic(p, 0, 0, summon_scroll_skill);
	return 0;
}

int db_fresh_summon_attr(player_t * p, uint32_t mon_type, uint32_t mon_tm, uint32_t item_id, int attr_per)
{
    uint32_t vip_sum = 0;
    uint32_t mon_lv = 0;
    uint32_t mon_exp = 0;
    if (is_vip_valid_summon(p, mon_type)) {
        vip_sum = 1;
        mon_lv = 45;
        if (mon_type == 1016) mon_lv = 50;
        mon_exp = calc_exp(mon_lv, 1);
    }
	int idx = 0;
	pack_h(dbpkgbuf, item_id, idx);
	pack_h(dbpkgbuf, mon_type, idx);
	pack_h(dbpkgbuf, mon_tm, idx);
	pack_h(dbpkgbuf, attr_per, idx);
	pack_h(dbpkgbuf, vip_sum, idx);
	pack_h(dbpkgbuf, mon_lv, idx);
	pack_h(dbpkgbuf, mon_exp, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_fresh_summon_attr, dbpkgbuf, idx);
}

int db_renew_summon_skills_callback(player_t * p, userid_t uid, void * body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    db_renew_summon_skills_t *rsp = reinterpret_cast<db_renew_summon_skills_t *>(body);
    
    summon_mon_t * p_mon = get_player_summon(p, rsp->mon_tm);
    if (!p_mon) {
        ERROR_LOG("db renew summon skills error[%u %u]", p->id, rsp->mon_tm);
        return -1;
    }

    renew_summon_new_skill(p_mon, rsp->src_id, &(rsp->skill_info));

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->mon_tm, idx);
    pack(pkgbuf, rsp->src_id, idx);
    pack(pkgbuf, rsp->skill_info.skillid, idx);
    pack(pkgbuf, rsp->skill_info.skilllv, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1); 
}


int fresh_summon_attr_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t mon_tm = 0;
	uint32_t mon_type = 0;
	uint32_t item_id = 0;
	unpack(body, item_id, idx);
	unpack(body, mon_tm, idx);
   	unpack(body, mon_type, idx);
	summon_mon_t * mon = get_player_summon(p, mon_tm);

	//for hatch item refresh summon attr
	uint32_t summon_id = 0;
	const GfItem* itm = items->get_item(item_id);
	if (itm) {
		summon_id = itm->summon_id();
	}

	if (itm && mon && summon_id && p->my_packs->get_item_cnt(item_id)) {

		if (((mon->mon_type) / 10) == ((itm->summon_id()) / 10)) {
			int new_attr_per =  get_attr_per(itm->pet_attr_id());
			return db_fresh_summon_attr(p, mon_type, mon_tm, item_id, new_attr_per);
		}  else {
			ERROR_LOG("ERROR HATCH ITEM REFESH %u %u %u", p->id, itm->summon_id(), mon->mon_type);

			return send_header_to_player(p, p->waitcmd, cli_err_summon_refresh_hatch_item, 1);
		}
	}  

	
	if (mon && mon->mon_type == mon_type 
			&& p->my_packs->get_item_cnt(item_id)) {

		int new_attr_per = get_attr_per(item_id);

		return db_fresh_summon_attr(p, mon_type, mon_tm, item_id, new_attr_per);

	} else {
		return -1;
	}
}


int db_fresh_summon_attr_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	db_fresh_attr_rsp_t * rsp = reinterpret_cast<db_fresh_attr_rsp_t*>(body);
	summon_mon_t * p_mon = get_player_summon(p, rsp->mon_tm);

	p->my_packs->del_item(p, rsp->item_id, 1, channel_string_other);
	
		//reset summon info
    if (rsp->vip_sum) {
        p_mon->lv = rsp->mon_lv;
        p_mon->exp = rsp->mon_exp;
    } else {
        p_mon->lv = 1;
        p_mon->exp = 0;
        if (p_mon->mon_type % 10 != 2) {
            p_mon->mon_type = (p_mon->mon_type / 10) * 10  + 2;
        }
    }
	memset(p_mon->skills, 0, sizeof(p_mon->skills));
	p_mon->skills_cnt = 0;
	memset(p_mon->disable_skills, 0x00, sizeof(p_mon->disable_skills));
	set_summon_skills_ex(p_mon);

	const summon_info_t* pm = get_summon(p_mon->mon_type);
	if (pm) {
		p_mon->attr_per = (uint32_t) (rsp->attr_per + pm->base_per);
		attr_data * data = new attr_data;
	   	calc_summon_attr(data, pm->attr_type, p_mon->attr_per, p_mon->lv);
		set_summon_attr(p_mon, data);

        //get_god_guard_mgr()->calc_group_quality_and_power(p, p->lv, p->add_quality);
        calc_and_save_player_attribute(p, 1);

		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, rsp->item_id, idx);
		pack(pkgbuf, p_mon->mon_tm, idx);
		pack(pkgbuf, p_mon->mon_type, idx);
		pack(pkgbuf, p_mon->lv, idx);
		pack(pkgbuf, p_mon->exp, idx);
		pack(pkgbuf, p_mon->attr_per, idx); 	   //成长值
		pack(pkgbuf, p_mon->pet_attr->strength, idx);        //力量
		pack(pkgbuf, p_mon->pet_attr->agility,  idx);        //敏捷
		pack(pkgbuf, p_mon->pet_attr->body_quality, idx);    //体质
		pack(pkgbuf, p_mon->pet_attr->accurate_value, idx);  //精准
		pack(pkgbuf, p_mon->pet_attr->hp, idx);              //生命
		pack(pkgbuf, p_mon->pet_attr->crit_value, idx);      //致命一击
		pack(pkgbuf, p_mon->pet_attr->attack, idx);             //攻击
		pack(pkgbuf, p_mon->pet_attr->def_value, idx);       //防御
		pack(pkgbuf, p_mon->pet_attr->hit_value, idx);       //命中值
		pack(pkgbuf, p_mon->pet_attr->dodge_value, idx);     //躲闪值
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx); 
		return send_to_player(p, pkgbuf, idx, 1);

	} else {
		return -1;
	}	
}



//---------------------------------------------------------------------

/**
  * @brief load summon skill scroll configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_summons_skills_scroll(xmlNodePtr cur)
{
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("scroll"))) {
			summon_skill_scroll_t scrolls;
			memset(&scrolls, 0x00, sizeof(summon_skill_scroll_t));
			get_xml_prop(scrolls.id, cur, "ID");
			get_xml_prop_def(scrolls.type, cur, "Type", 0);

            xmlNodePtr skill_node = cur->xmlChildrenNode;
            while(skill_node) {
                if( xmlStrcmp(skill_node->name, BAD_CAST"skills") == 0) {
                    scroll_enum_t scroll_enum;
                    memset(&scroll_enum, 0x00, sizeof(scroll_enum_t));

                    get_xml_prop_def(scroll_enum.skillid,  skill_node, "id", 0);
                    get_xml_prop_def(scroll_enum.lv,       skill_node, "Lv", 0);
                    get_xml_prop_def(scroll_enum.odds,     skill_node, "odds", 0);

                    scrolls.skill_vec.push_back(scroll_enum);
                }
                skill_node = skill_node->next;
            }

			//log
			TRACE_LOG("summon skill scroll info:[%u]", scrolls.id);
			for (uint32_t i = 0; i < scrolls.skill_vec.size(); i++) {
				TRACE_LOG("summon skill [%u %u]", scrolls.skill_vec[i].skillid, scrolls.skill_vec[i].odds);
			}

			sum_scroll_map_.insert(std::map<uint32_t, summon_skill_scroll_t>::value_type(scrolls.id, scrolls));
		}
		cur = cur->next;
	}
	return 0;
}
/**
  * @brief unload summon monsters configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int unload_summons_skills_scroll()
{
	sum_scroll_map_.clear();
	return 0;
}
/**
  * @brief load summmon skill configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_summon_skill(xmlNodePtr cur, summon_info_t* p_summon)
{
	uint32_t i = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Skill"))) {
			if (i >= max_summon_skills) {
				ERROR_LOG("summon skill more than max_summon_skills[%u %u]",p_summon->summon_id, i);
				return -1;
			}
			summon_skill_info_t* p_skill = &(p_summon->skills[i]);
			get_xml_prop(p_skill->skill_id, cur, "ID");
			get_xml_prop(p_skill->need_lv, cur, "NeedLv");
			get_xml_prop(p_skill->top_skill_lv, cur, "TopSkillLv");
			i++;
		}
		cur = cur->next;
	}
	
	return 0;
}

/**
  * @brief load summon monsters configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_summons(xmlNodePtr cur)
{
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Summon"))) {
			summon_info_t p_summon;
			memset(&p_summon, 0x00, sizeof(summon_info_t));
			get_xml_prop(p_summon.summon_id, cur, "ID");
			get_xml_prop(p_summon.lv, cur, "Lv");
			get_xml_prop(p_summon.next_lv, cur, "NextLv");
			get_xml_prop_def(p_summon.evolve_need_item, cur, "NeedItem", 0);
			get_xml_prop_def(p_summon.hatch_lv, cur, "HatchLv", 0);
			get_xml_prop_def(p_summon.need_vip_lv, cur, "NeedVipLv", 0);
			get_xml_prop_raw_str(p_summon.name, cur, "Name");
			get_xml_prop_def(p_summon.pre_summon_id, cur, "PreSummon", 0);
			get_xml_prop_def(p_summon.odds, cur, "Odds", 100);
			get_xml_prop_def(p_summon.renascence_time, cur, "Renascence", 10);
			get_xml_prop_def(p_summon.attr_type, cur,  "AttrType", 1);
			get_xml_prop_def(p_summon.base_per, cur, "BaseAttr", 100);

			// load configs for each Skill
			if (load_summon_skill(cur->xmlChildrenNode, &p_summon) == -1) {
				throw XmlParseError(std::string("failed to load an skill"));
				return -1;
			}
			
			//log
			TRACE_LOG("summon info:[%u %u %u %s]",p_summon.summon_id, p_summon.lv, p_summon.next_lv, p_summon.name);
			for (uint32_t i = 0; i < max_summon_skills; i++) {
				summon_skill_info_t* p_skill = &(p_summon.skills[i]);
				TRACE_LOG("summon skill_%u[%u %u %u]",i, p_skill->skill_id, p_skill->need_lv, p_skill->top_skill_lv);
			}

			summons_map_.insert(std::map<uint32_t, summon_info_t>::value_type(p_summon.summon_id, p_summon));
		}
		cur = cur->next;
	}
	return 0;
}

/**
  * @brief unload summon monsters configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int unload_summons()
{
	/*std::map<uint32_t,	summon_info_t*>::iterator it;
	for (it = summon_map_.begin(); it != summon_map_.end(); ++it) {
		delete *it;
	}*/
	summons_map_.clear();
	return 0;
}
