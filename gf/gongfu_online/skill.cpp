#include <libtaomee++/inet/pdumanip.hpp>
#include <kf/item_impl.hpp>

using namespace taomee;
extern "C" {
#include <libtaomee/project/stat_agent/msglog.h>
}

#include "cli_proto.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "skill.hpp"
#include "fwd_decl.hpp"
#include "item.hpp"
#include "global_data.hpp"


//--------------------------------------------------------------------------------
// global 
//--------------------------------------------------------------------------------
/*! the all skills*/
//std::map<uint32_t, skills_t*> skills_map_;
/*! the all buff skills*/
//std::map<uint32_t, buff_skill_t> buff_skill_map_;

//--------------------------------------------------------------------------------
// static function
//--------------------------------------------------------------------------------
/**
  * @brief load skill configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
static int load_an_skill(xmlNodePtr cur, struct skills_t* p_skill);

/**
  * @brief get an skill_lv_info from skills_t
  * @return the skills get
  */
inline struct skill_info_t* get_skill_lv_info(skills_t* p_skill, uint32_t lv);

/**
  * @brief get an skill from skills_map_
  * @return the skills get
  */
inline static struct skills_t* get_skill(uint32_t skill_id);

/**
  * @brief get an skill from skills_map_ with role type
  * @return the skills get
  */
inline struct skills_t* get_role_skill(uint32_t role_type, uint32_t skill_id);

inline void do_stat_log_used_skillpoints(uint32_t cmdid, uint32_t id, uint32_t type, uint32_t cnt)
{
	uint32_t buf[1] = { cnt };
    uint32_t skid = (id - (type * 100000) - 200);
    uint32_t itype = ((type-1) << 12);
	msglog(statistic_logfile, cmdid + skid + itype, get_now_tv()->tv_sec, buf, sizeof(buf));
	TRACE_LOG("stat log cmd:[%x] id:[%u %x] type:[%u %x] cnt:%u", cmdid, id, skid, type, itype, cnt);
}

/**
  * @brief get an skill from all_skills
  * @return the skills get
  */
static bool check_skill_id(player_t*p, uint32_t skill_id);


//--------------------------------------------------------------------------------
// function
//--------------------------------------------------------------------------------
inline bool update_skill_bind_lv(player_t* p, uint32_t skill_id, uint32_t skill_lv)
{
	player_skill_bind_t* p_skill = p->skills_bind;
	for (uint32_t i = 0; i < p->skill_bind_num; i++) {
		if (p_skill->skill_id == skill_id) {
			p_skill->lv = skill_lv;
			return true;
		}
	}
	return false;
}

inline bool update_player_skill_lv(player_t* p, uint32_t skill_id, uint32_t skill_lv)
{
	PlayerSkillMap::iterator it = 
	p->player_skill_map->find(skill_id);
	if (it != p->player_skill_map->end()) {
		it->second.lv = skill_lv;
		return true;
	}	
	return false;
}

inline bool update_player_skill_bind(player_t* p, uint32_t skill_id, uint32_t key)
{
	PlayerSkillMap::iterator it = 
	p->player_skill_map->find(skill_id);
	if (it != p->player_skill_map->end()) {
		it->second.key = key;
		return true;
	}	
	return false;
}

inline bool clear_player_skill_bind(player_t* p)
{
	PlayerSkillMap::iterator it = p->player_skill_map->begin();

	for (; it != p->player_skill_map->end(); ++it) {
		it->second.key = 0;
	}
	return true;
}


inline bool add_player_skill(player_t* p, uint32_t skill_id, uint32_t skill_lv)
{
	player_skill_bind_t skill_tmp = { skill_id, skill_lv, 0 };
	
	(*p->player_skill_map)[skill_id] = skill_tmp;
	return true;
}

/**
  * @brief get an skill_lv_info from skills_t
  * @return the skills get
  */
inline struct skill_info_t* get_skill_lv_info(skills_t* p_skill, uint32_t lv)
{
	if (!p_skill || lv > p_skill->max_lv || lv < min_skill_level) {
		return NULL;
	}
	return &(p_skill->skill_info[lv]);
}

/**
  * @brief get an skill from skills_map_
  * @return the skills get
  */
inline struct skills_t* get_skill(uint32_t skill_id)
{
	std::map<uint32_t,struct skills_t*>::iterator it = skills_map_.find(skill_id);
	if ( it != skills_map_.end() ) {
		return it->second;
	}
	return NULL;
}

/**
  * @brief get an skill from skills_map_ with role type
  * @return the skills get
  */
inline struct skills_t* get_role_skill(uint32_t role_type, uint32_t skill_id)
{
	uint32_t type = skill_id / skill_id_boundary;
		
	if (type != role_type) {
		return NULL;
	}

	std::map<uint32_t,struct skills_t*>::iterator it = skills_map_.find(skill_id);
	if ( it != skills_map_.end() ) {
		return it->second;
	}
	return NULL;
}

inline bool is_have_buff_skill(uint32_t buff_id)
{
	if (buff_skill_map_.find(buff_id) != buff_skill_map_.end()) {
		return true;
	}
	return false;
}

const buff_skill_t* get_buff(uint32_t buff_id)
{
	std::map<uint32_t, buff_skill_t>::iterator it = buff_skill_map_.find(buff_id);
	if ( it != buff_skill_map_.end()) {
		return &(it->second);
	}
	return 0;
}

/**
  * @brief get an skill from all_skills
  * @return the skills get
  */
static bool check_skill_id(player_t*p, uint32_t skill_id)
{
	if (get_role_skill(p->role_type, skill_id)) {
		return true;
	}
	return false;
}

uint32_t get_used_skill_points(player_t* p, get_skills_rsp_t *rsp)
{
	uint32_t used_sp = 0;
	TRACE_LOG("%u %u", p->id, rsp->skills_cnt);
	for (uint32_t i = 0; i < rsp->skills_cnt; i++) {
		db_get_skill_elem_t * p_elem = &(rsp->skill[i]);
		skills_t * p_skill = get_skill(p_elem->skill_id);
		used_sp += p_elem->skill_points;

		TRACE_LOG("%u %u %u", i, p->id, p_elem->skill_lv);
		for (uint32_t j = 1; j < p_elem->skill_lv; j++) {
			used_sp += p_skill->skill_info[j].sp;
			TRACE_LOG("%u %u %u %u", j, p->id, used_sp, p_skill->skill_info[j].sp);
		}
		used_sp += p_skill->skill_info[p_elem->skill_lv].sp;
	}
	return used_sp;
}

int db_get_skills(player_t* p) 
{
	KDEBUG_LOG(p->id, "GET SKILLS\t%u", p->id);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_skills, 0, 0);
}

int reset_all_skills(player_t* p, uint32_t extera_point) 
{
	uint32_t total_sp = 0;
	PlayerSkillMap::iterator it = p->player_skill_map->begin();
	for (; it != p->player_skill_map->end(); ++it) {
		total_sp += it->second.lv;
	}
    total_sp += extera_point;
	KDEBUG_LOG(p->id, "RESET SKILLS\t%u leftsp:%u", p->id, total_sp);
	int idx = 0;
	pack_h(dbpkgbuf, total_sp, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_reset_skill, dbpkgbuf, idx);
}

int db_learn_new_skill(player_t* p, uint32_t skill_id, uint32_t item_id)
{
	int idx = 0;
	pack_h(dbpkgbuf, skill_id, idx);
	pack_h(dbpkgbuf, 1, idx);//skill default level
	pack_h(dbpkgbuf, 1, idx);//skill point needed
	pack_h(dbpkgbuf, item_id, idx);//item needed
	KDEBUG_LOG(p->id, "LEARN SKILL\t[uid=%u skill=%u sp_have=%u item=%u]",p->id, skill_id, p->skill_point, item_id);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_learn_new_skill, dbpkgbuf, idx);
}



/**
  * @brief pack the skill of the player have binded
  * @param p the player
  * @param buf
  * @return the idx
  */
int pack_player_skill_bind(player_t* p, void* buf)
{
	int idx = 0;
	pack(buf, p->skill_bind_num, idx);
	for ( uint32_t i = 0; i < p->skill_bind_num; i++ ) {
		pack(buf, p->skills_bind[i].skill_id, idx);
		pack(buf, p->skills_bind[i].lv, idx);
		pack(buf, p->skills_bind[i].key, idx);
		TRACE_LOG("pack bind skill[%u %u %u %u]",p->skill_bind_num,p->skills_bind[i].skill_id, p->skills_bind[i].lv, p->skills_bind[i].key);
	}
	
	return idx;
}

int send_get_buff_skill_rsp(player_t* p, add_buf_rsp_t* data)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, data->type, idx);
	pack(pkgbuf, data->duration, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	TRACE_LOG("uid=%u buffid=%u bufftm=%u",p->id, data->type, data->duration);
	return send_to_player(p, pkgbuf, idx, 1);
}

bool is_valid_skill(player_t* p, uint32_t skill_id, uint32_t limit_type, uint32_t limit_lv)
{
    if (limit_type == p->role_type && limit_lv <= p->lv) {
        if ((skill_id / 100) % 10 == 6 && p->power_user == 1) {
            return true;
        }

        if ((skill_id / 100) % 10 != 6 && p->power_user == 0) {
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------------------------
// commands
//--------------------------------------------------------------------------------

uint32_t get_learn_skill_need_item(player_t* p, uint32_t skill_id)
{
	//领悟技能
	if ( skill_id == 100607 || skill_id == 200607 || 
			skill_id == 300607 || skill_id == 400607) {
		if (p->role_type == 1) {
			return 2710001;
		} else if (p->role_type == 2) {
			return 2720001;
		} else if (p->role_type == 3) {
			return 2730001;
		} else if (p->role_type == 4) {
			return 2740001;
		}
	}
	return 0;
}

/**
* @brief player learn new skill
*/
int learn_new_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t skill_id = 0;
	uint32_t item_id = 0;
	int idx = 0;
	unpack(body, skill_id, idx);

	item_id = get_learn_skill_need_item(p, skill_id);
	if (item_id) {
		if (!(p->my_packs->get_item_cnt(item_id))) {
			return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
		}
	}
	if (skill_id && p->skill_point) {
		std::map<uint32_t, skills_t*>::iterator it = skills_map_.find(skill_id);
		PlayerSkillMap::iterator it_have = p->player_skill_map->find(skill_id);
		if (it != skills_map_.end() && it_have == p->player_skill_map->end()) {
			//player have not learn this skill before
			skills_t* p_new_skill = it->second;
			//if (p_new_skill->role_type == p->role_type && p->lv >= p_new_skill->skill_info[1].use_lv) {
            if ( is_valid_skill(p, skill_id, p_new_skill->role_type, p_new_skill->skill_info[1].use_lv) ) {
				//player have the same role type that skill needed
				//player's level is greater than skill's use-level
				for (uint32_t i = 0; i < p_new_skill->pre_skills_cnt; i++) {
					//check every preSkills
					PlayerSkillMap::iterator it = 
						p->player_skill_map->find(p_new_skill->pre_skills[i].skill_id);
					if (it == p->player_skill_map->end() || 
						it->second.lv < p_new_skill->pre_skills[i].skill_lv) {
						return send_header_to_player(p, p->waitcmd, cli_err_pre_skill_requst, 1);
					}
				}
				return db_learn_new_skill(p, skill_id, item_id);
			}
			return send_header_to_player(p, p->waitcmd, cli_err_less_lv, 1);
		}
	}
	ERROR_LOG("learn new skill err: %u sp_have=%u", skill_id, p->skill_point);
	return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
}


/**
  * @brief get the skill of the player have owned
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_skills_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return db_get_skills(p);
}

/**
  * @brief bind the skill of the player can use in the battle
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int skill_bind_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0, db_idx = 0;
	uint32_t cnt = 0;
	unpack(body, cnt, idx);
	TRACE_LOG("bind key number %u", cnt);

	CHECK_VAL_EQ(bodylen, 4 + cnt * 8);
	CHECK_VAL_LE(cnt, max_skills_bind);

	pack_h(dbpkgbuf, cnt, db_idx);
	for ( uint32_t i = 0; i < cnt; i++ ) {
		uint32_t skill_id = 0;
		uint32_t key = 0;
		unpack(body, skill_id, idx);
		unpack(body, key, idx);
		TRACE_LOG("skillid=%u, key=%u",skill_id, key);
		//TODO:check skill_id
		if ( !check_skill_id(p, skill_id) ) {
			WARN_LOG("player[%u] cannt bind this skill_id[%u]",p->id, skill_id);
			return send_header_to_player(p, p->waitcmd, cli_err_no_skill_id, 1);
		}

		pack_h(dbpkgbuf, skill_id, db_idx);
		pack_h(dbpkgbuf, key, db_idx);
	}

	return send_request_to_db(p, p->id, p->role_tm, dbproto_skill_bind, dbpkgbuf, db_idx);
}


/**
  * @brief update player's skill
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
/*
int skill_upgrade_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0, db_idx = 0;
	uint32_t skill_id = 0, skill_lv = 0, skill_point, to_skill_lv = 0;
	unpack(body, skill_id, idx);
	unpack(body, skill_lv, idx);
	unpack(body, skill_point, idx);
	to_skill_lv = skill_lv + 1;

	//check to_skill_lv
	if (to_skill_lv == min_skill_level) {
		WARN_LOG("impossable err,uid:%u skill lv\t[%u] to [%u] ", p->id, skill_lv, to_skill_lv);
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1); 
	}

	skills_t* p_skill = get_role_skill(p->role_type, skill_id);
	if (!p_skill) {
		WARN_LOG("skill not found ,uid:%u skillid\t[%u]", p->id, skill_id);
		return send_header_to_player(p, p->waitcmd, cli_err_no_skill_id, 1); 
	}
	skill_info_t* p_skill_lv = get_skill_lv_info(p_skill, to_skill_lv);
	if (!p_skill_lv) {
		TRACE_LOG("uid:%u skill lv max\t[%u %u] ",p->id, skill_id, skill_lv);
		return send_header_to_player(p, p->waitcmd, cli_err_skill_lv_max, 1); 
	}

	
	uint32_t need_skill_point = p_skill_lv->sp;
	uint32_t need_coins = p_skill_lv->coins;
	uint8_t role_lv_reach = 0;
	if (p->lv >= p_skill_lv->use_lv) {
		TRACE_LOG("uid:%u  LV:%u   need lv:%u", p->id, p->lv, p_skill_lv->use_lv);
		role_lv_reach = 1; 
	}

	//send to db
	pack_h(dbpkgbuf, skill_id, db_idx);
	pack_h(dbpkgbuf, skill_lv, db_idx);
	pack_h(dbpkgbuf, skill_point, db_idx);
	pack_h(dbpkgbuf, need_skill_point, db_idx);
	pack_h(dbpkgbuf, need_coins, db_idx);
	pack_h(dbpkgbuf, role_lv_reach, db_idx);
	KDEBUG_LOG(p->id, "SKILL UPGRADE\t[uid=%u skillid=%u lv=%u]",p->id, skill_id, skill_lv);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_skill_upgrade, dbpkgbuf, db_idx);
}
*/
int skill_upgrade_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0, db_idx = 0;
	uint32_t skill_id = 0, to_skill_lv = 0;
	unpack(body, skill_id, idx);
	unpack(body, to_skill_lv, idx);
	//to_skill_lv = skill_lv + 1;

	//check to_skill_lv
	if (to_skill_lv == min_skill_level || to_skill_lv > 20) {
		WARN_LOG("impossable err,uid:%u skill lv\t[%u] to [%u] ", p->id, to_skill_lv, to_skill_lv);
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1); 
	}

	skills_t* p_skill = get_skill(skill_id);
	if (!p_skill || p_skill->role_type != p->role_type) {
		WARN_LOG("skill not found ,uid:%u skillid\t[%u] %u %u", p->id, skill_id, p_skill->role_type, p->role_type);
		return send_header_to_player(p, p->waitcmd, cli_err_no_skill_id, 1); 
	}


	skill_info_t* p_skill_lv = get_skill_lv_info(p_skill, 1);
	if (!p_skill_lv) {
		TRACE_LOG("uid:%u skill lv max\t[%u %u] ",p->id, skill_id, to_skill_lv);
		return send_header_to_player(p, p->waitcmd, cli_err_skill_lv_max, 1); 
	}
	if (p->lv < p_skill_lv->use_lv) {
		TRACE_LOG("uid:%u  LV:%u   need lv:%u", p->id, p->lv, p_skill_lv->use_lv);
		return send_header_to_player(p, p->waitcmd, cli_err_less_lv, 1); 
	}

	PlayerSkillMap::iterator it = 
	p->player_skill_map->find(skill_id);
	if (it == p->player_skill_map->end() || 
		it->second.lv >= to_skill_lv) {
		TRACE_LOG("skill lv err uid=%u skillid=%u %u lv=%u",p->id, skill_id, it->second.lv, to_skill_lv);
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}	
	
	uint32_t need_skill_point = 1;//p_skill_lv->sp;
	uint32_t need_coins = 0;//p_skill_lv->coins;

	if (need_skill_point > p->skill_point) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_point, 1); 
	}

	if (need_coins > p->coins) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1); 
	}

	//send to db
	pack_h(dbpkgbuf, skill_id, db_idx);
	pack_h(dbpkgbuf, to_skill_lv, db_idx);
	pack_h(dbpkgbuf, need_skill_point, db_idx);
	pack_h(dbpkgbuf, need_coins, db_idx);
	KDEBUG_LOG(p->id, "SKILL UPGRADE\t[uid=%u skillid=%u lv=%u sp_have=%u]",p->id, skill_id, to_skill_lv, p->skill_point);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_skill_upgrade, dbpkgbuf, db_idx);
}


/**
* @brief player get buff skill
*/
int get_buff_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t buff_id;
	unpack(body, buff_id, idx);
	const buff_skill_t* p_buff = get_buff(buff_id);
	return db_add_buff(p, buff_id, p_buff->duration, 0);
}

int pack_all_buff_skill(player_t* p, uint8_t* buf)
{
	int idx = 0;
	BufSkillMap::iterator it = p->buf_skill_map->begin();
	for (; it != p->buf_skill_map->end(); ++it) {
		uint32_t left_duration = 0;
		
		if (it->second.buf_type < max_nor_buff_type) {
			left_duration = it->second.buf_duration;
		} else {
			left_duration = 
				get_now_tv()->tv_sec > (it->second.start_tm + it->second.buf_duration) ? 
				0 : (it->second.start_tm + it->second.buf_duration) - get_now_tv()->tv_sec;
		}
		pack(buf, it->second.buf_type, idx);
		pack(buf, left_duration, idx);
		TRACE_LOG("%u %u", it->second.buf_type, left_duration);
	}	
	return idx;
}

bool is_buff_alive(player_t* p, uint32_t buff_id)
{
	BufSkillMap::iterator it = p->buf_skill_map->find(buff_id);
	if (it != p->buf_skill_map->end()) {
		uint32_t left_duration = 
					get_now_tv()->tv_sec > (it->second.start_tm + it->second.buf_duration) ? 
					0 : (it->second.start_tm + it->second.buf_duration) - get_now_tv()->tv_sec;
		return left_duration;
	}
	return false;
}

/**
* @brief player get buff skill
*/
int get_buff_skill_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = sizeof(cli_proto_t);
	uint32_t cnt = p->buf_skill_map->size();
	
	pack(pkgbuf, cnt, idx);
	idx += pack_all_buff_skill(p, pkgbuf + idx);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1); 
}


//--------------------------------------------------------------------------------
// Requests to Dbproxy
//--------------------------------------------------------------------------------

int db_learn_new_skill_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	learn_new_skill_rsp_t *rsp = reinterpret_cast<learn_new_skill_rsp_t*>(body);
	
	add_player_skill(p, rsp->skill_id, rsp->skill_lv);
	p->skill_point = rsp->left_points;
	if (rsp->item_id) {
		p->my_packs->del_item(p, rsp->item_id, 1, channel_string_other);	
	}
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->skill_id, idx);
	//pack(pkgbuf, rsp->skill_lv, idx);
	pack(pkgbuf, rsp->left_points, idx);
	pack(pkgbuf, rsp->item_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	TRACE_LOG("uid=%u skillid=%u leftpt=%u",p->id, rsp->skill_id, rsp->left_points);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief callback for reset skill
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_reset_skill_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
/*
	reset_skill_rsp_t *rsp = reinterpret_cast<reset_skill_rsp_t*>(body);
	for ( uint32_t i = 0; i < p->skill_bind_num; i++ ) {
		p->skills_bind[i].lv = 1;
		TRACE_LOG("bind skill[%u %u %u]",p->skills_bind[i].skill_id, p->skills_bind[i].lv, p->skills_bind[i].key);
	}
	p->skill_point = rsp->left_skill_points;
	if (p->waitcmd == cli_proto_use_item) {
		uint32_t item_id = *(reinterpret_cast<uint32_t*>(p->session));
		return send_use_item_rsp(p, item_id, 1);
	}
	*/
	CHECK_DBERR(p, ret);
	reset_skill_rsp_t *rsp = reinterpret_cast<reset_skill_rsp_t*>(body);
	p->skill_bind_num = 0;
	p->player_skill_map->clear();
	p->skill_point = rsp->left_skill_points;
	if (p->waitcmd == cli_proto_use_item) {
		uint32_t item_id = *(reinterpret_cast<uint32_t*>(p->session));
		return send_use_item_rsp(p, item_id, 1);
	}
    if (p->waitcmd == cli_proto_up_power_user) {
    }
	return 0;
}

/**
  * @brief callback for getting skills
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_get_skills_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	get_skills_rsp_t *rsp = reinterpret_cast<get_skills_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(get_skills_rsp_t) + rsp->skills_cnt * sizeof(db_get_skill_elem_t));

//	if (p->waitcmd == cli_proto_use_item) {
//		uint32_t used_sp = get_used_skill_points(p, rsp);
//		return reset_all_skills(p, used_sp);
//	}
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->left_sp, idx);
	pack(pkgbuf, rsp->skills_cnt, idx);
	TRACE_LOG("get skill[%u %u %u]", p->id, rsp->left_sp, rsp->skills_cnt);
	for ( uint32_t i = 0; i < rsp->skills_cnt; i++ ) {
		db_get_skill_elem_t* elem = &(rsp->skill[i]);
		pack(pkgbuf, elem->skill_id, idx);
		pack(pkgbuf, elem->skill_lv, idx);
		pack(pkgbuf, elem->skill_points, idx);
		TRACE_LOG("get skill[%u %u %u]",elem->skill_id, elem->skill_lv, elem->skill_points);
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx , 1);

	return 0;
}

/**
  * @brief callback for binding skills
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_skill_bind_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	skill_bind_rsp_t *rsp = reinterpret_cast<skill_bind_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(skill_bind_rsp_t) + rsp->bind_cnt * sizeof(db_skill_bind_elem_t));

	if ( rsp->bind_cnt > max_skills_bind ) {
		WARN_LOG("too many skill bind, cnt=%u %u",rsp->bind_cnt, max_skills_bind);
		p->skill_bind_num = max_skills_bind;
	} else {
		p->skill_bind_num = rsp->bind_cnt;
	}
	clear_player_skill_bind(p);
	int idx = sizeof(cli_proto_t);
	//pack(pkgbuf, p->skill_bind_num, idx);
	for ( uint32_t i = 0; i < p->skill_bind_num; i++ ) {
		db_skill_bind_elem_t* elem = &(rsp->skill_bind[i]);
		p->skills_bind[i].skill_id = elem->skill_id;
		p->skills_bind[i].lv = elem->skill_lv;
		p->skills_bind[i].key = elem->bind_key;
		update_player_skill_bind(p, elem->skill_id, elem->bind_key);
		/*pack(pkgbuf, p->skills_bind[i].skill_id, idx);
		pack(pkgbuf, p->skills_bind[i].lv, idx);
		pack(pkgbuf, p->skills_bind[i].key, idx);
		TRACE_LOG("bind skill[%u %u %u]",p->skills_bind[i].skill_id, p->skills_bind[i].lv, p->skills_bind[i].key);*/
	}

	idx += pack_player_skill_bind(p, pkgbuf + idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx , 1);

	return 0;
}

/**
  * @brief callback for upgrading skills
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_skill_upgrade_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	skill_upgrade_rsp_t* rsp = reinterpret_cast<skill_upgrade_rsp_t*>(body);

	//TODO:set id,lv
	if (p->skill_point > rsp->left_point) {
		do_stat_log_used_skillpoints(stat_log_used_skillpoints, 
            rsp->skill_id, p->role_type, p->skill_point - rsp->left_point);
        uint32_t stat_id1 = ((p->role_type - 1) * 4) << 12;
        uint32_t stat_id2 = ( rsp->skill_id - (1200000 + (p->role_type - 1) * 200) ) << 8;
        do_stat_log_universal_interface_1(stat_log_learn_skill, stat_id1+stat_id2+rsp->skill_lv, id);
	}

//	do_stat_log_universal_interface_1(0x09810000, (p->role_type << 12) & 0xffffffff + rsp->skill_id % 100000, 1);
	p->coins = rsp->left_coins;
	p->skill_point = rsp->left_point;
	
	update_skill_bind_lv(p, rsp->skill_id, rsp->skill_lv);
	update_player_skill_lv(p, rsp->skill_id, rsp->skill_lv);
	int idx = sizeof(cli_proto_t);
	
	pack(pkgbuf, rsp->skill_id, idx);
	pack(pkgbuf, rsp->skill_lv, idx);
//	pack(pkgbuf, rsp->use_point, idx);
	pack(pkgbuf, p->skill_point, idx);
	pack(pkgbuf, p->coins, idx);
	
	KDEBUG_LOG(p->id, "UPGRADE SKILL CB\t[sid:%u, slv:%u, usp:%u, rsp:%u, coins:%u]", rsp->skill_id, rsp->skill_lv, rsp->use_point, p->skill_point, p->coins);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


//---------------------------------------------------------------------
/*
int load_role_skill(skills_t* p_skill, uint32_t role_type, xmlNodePtr cur)
{
	if (role_type > max_role_type) {
		throw XmlParseError(std::string("RoleType error"));
		return -1;
	}
	std::map<uint32_t role_skill_t>::iterator it = 
		role_skill_map_[role_type - 1].find(p_skill->id);
	if (it == role_skill_map_.end()) {
		ERROR_LOG("conflict skillid %u", p_skill->id);
		throw XmlParseError(std::string("conflict skillid"));
		return -1;
	}
	role_skill_t role_skill = { 0 };
	uint32_t pre_skill[max_pre_skill_cnt * 2] = { 0 };
	int arr_len = get_xml_prop_arr(pre_skill, "PreSkill", cur);
	if (arr_len > max_pre_skill_cnt * 2 || (arr_len % 2)) {
		ERROR_LOG("pre skill too many or arr_len=%u %u > %u", arr_len, (arr_len + 1) / 2, max_pre_skill_cnt);
		throw XmlParseError(std::string("pre skill too many"));
		return -1;
	}
	
	for (uint32_t i = 0; i < arr_len; ++i) {
		role_skill.pre_skill[role_skill.pre_skill_cnt].skill_id = pre_skill[i];
		i++;
		role_skill.pre_skill[role_skill.pre_skill_cnt].skill_lv = pre_skill[i];
		role_skill.pre_skill_cnt++;
	}
	role_skill_map_.insert(std::map<uint32_t role_skill_t>::value_type(p_skill->id, role_skill));
	return 0;
}
*/
int load_pre_skills(skills_t* p_skill, xmlNodePtr cur)
{
	char szPreSkills[max_pre_skill_cnt * 40] = "";
	get_xml_prop_raw_str_def(szPreSkills, cur, "PreSkill", "");

	std::string strSkills(szPreSkills);
	std::string sub_str;
	std::stringstream ss(strSkills);
	while(getline(ss, sub_str, '|')) {
		std::string strSkillID;
		std::string strSkillLv;
		std::stringstream sub_ss(sub_str);
		getline(sub_ss, strSkillID, ',');
		getline(sub_ss, strSkillLv, ',');

		p_skill->pre_skills[p_skill->pre_skills_cnt].skill_id = atoi(strSkillID.c_str());
		p_skill->pre_skills[p_skill->pre_skills_cnt].skill_lv = atoi(strSkillLv.c_str());
		
		TRACE_LOG("%u : %u, %u", p_skill->pre_skills_cnt, 
			p_skill->pre_skills[p_skill->pre_skills_cnt].skill_id,
			p_skill->pre_skills[p_skill->pre_skills_cnt].skill_lv);

		p_skill->pre_skills_cnt++;
	}
	return 0;
}

/**
  * @brief load skill configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_skills(xmlNodePtr cur)
{
	// load items from an xml file
	for (uint32_t i = 0; i < max_role_type; ++i) {
//		role_skill_map_[i].clear();
	}
	
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Skill"))) {
			struct skills_t* p_skill = reinterpret_cast<struct skills_t*>(malloc(sizeof(struct skills_t)));
			memset(p_skill, 0x00, sizeof(struct skills_t));
			get_xml_prop(p_skill->id, cur, "ID");
			//get_xml_prop(p_skill->type, cur, "Type");
			get_xml_prop_def(p_skill->role_type, cur, "Role_type", 0);
/*			
			if (p_skill->role_type) {
				if (load_role_skill(p_skill, role_type, cur) == -1) {
					return -1;
				}
			}
*/

			if (load_pre_skills(p_skill, cur) == -1) {
				return -1;
			}
			// load configs for each Skill
			if (load_an_skill(cur->xmlChildrenNode, p_skill) == -1) {
				throw XmlParseError(std::string("failed to load an skill"));
				return -1;
			}
			
			skills_map_.insert(std::map<uint32_t, struct skills_t*>::value_type(p_skill->id, p_skill));
		}
		cur = cur->next;
	}
	return 0;
}

/**
  * @brief load skill configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_an_skill(xmlNodePtr cur, struct skills_t* p_skill)
{
	uint32_t lv = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Level"))) {
			get_xml_prop(lv, cur, "Lv");
			if ( lv > max_skill_level) {
				ERROR_LOG("lv[%u] more than MAX_SKILL_LV",lv);
				return -1;	
			}	
			p_skill->skill_info[lv].lv = lv;
			get_xml_prop_def(p_skill->skill_info[lv].use_lv, cur, "UseLv", 0);
			get_xml_prop_def(p_skill->skill_info[lv].stars, cur, "Stars", 0);
			get_xml_prop_def(p_skill->skill_info[lv].sp, cur, "sp", 0);
			get_xml_prop_def(p_skill->skill_info[lv].coins, cur, "coin", 0);
			p_skill->max_lv++;
			if (p_skill->max_lv != lv) {
				ERROR_LOG("skillid[%u] lv[%u] should be[%u]  not continues", p_skill->id, lv, p_skill->max_lv);
				return -1;	
			}
			TRACE_LOG("skillid[%u %u %u %u %u]",p_skill->skill_info[lv].lv, p_skill->skill_info[lv].use_lv,
							 p_skill->skill_info[lv].stars, p_skill->skill_info[lv].sp, p_skill->skill_info[lv].coins);
		}
		
		cur = cur->next;
	}
	
	return 0;
}

/**
  * @brief load buff skill configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_buff_skills(xmlNodePtr cur)
{
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("buff"))) {
			buff_skill_t buff(0, 0);
			get_xml_prop(buff.buff_id, cur, "buff_id");
			get_xml_prop(buff.duration, cur, "duration_time");

			if (is_have_buff_skill(buff.buff_id)) {
				ERROR_LOG("buff_id duplicated %u", buff.buff_id);
				return -1;
			}
			buff_skill_map_.insert(std::map<uint32_t, buff_skill_t>::value_type(buff.buff_id, buff));
 			TRACE_LOG("buffid[%u %u]", buff.buff_id, buff.duration);
		}
		cur = cur->next;
	}
	
	return 0;
}


