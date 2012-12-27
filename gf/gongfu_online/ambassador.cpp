#include <cstdio>

#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/utils.h>
#include <libtaomee/project/utilities.h>
}
using namespace taomee;

#include "common_op.hpp"
#include "utils.hpp" 
#include "task.hpp"
#include "item.hpp"
#include "ambassador.hpp"
#include "global_data.hpp"

//ambassador_reward_t g_ambassador_reward[max_ambassador_reward_num + 1];
//uint32_t			g_ambassador_reward_cnt;

//------------------------------------------------------------------------------------------------
//----- inline functions
//------------------------------------------------------------------------------------------------
bool inline can_receive_amb_reward(player_t* p, uint32_t pos)
{
	if (p->amb_status == 0 || p->amb_status == 2 || pos == 0 || pos > g_ambassador_reward_cnt || 
		!(g_ambassador_reward[pos].cnt)) {
		TRACE_LOG("%u %u %u ", p->amb_status, pos, g_ambassador_reward[pos].cnt);
		return false;
	}
	switch (pos) {
		case 1:
			break;
		case 2:
			if (p->lv < 5 || p->achieve_cnt < 5) {
				TRACE_LOG("%u %u ", p->lv, p->achieve_cnt);
				return false;
			}
			break;
		case 3:
			if (p->lv < 10 || p->achieve_cnt < 10) {
				TRACE_LOG("%u %u ", p->lv, p->achieve_cnt);
				return false;
			}
			break;
		case 4:
			if (p->lv < 20 || p->achieve_cnt < 20) {
				TRACE_LOG("%u %u ", p->lv, p->achieve_cnt);
				return false;
			}
			break;
		case 5:
			if (p->lv < 25 || p->achieve_cnt < 30) {
				TRACE_LOG("%u %u ", p->lv, p->achieve_cnt);
				return false;
			}
			break;
		default:
			TRACE_LOG("%u %u ", p->lv, p->achieve_cnt);
			return false;
			break;
	}
	return !test_bit_on(p->once_bit, flag_pos_ambassador + pos);
}

inline bool all_amb_reward_got(player_t* p)
{
	for (uint32_t i = 1; i <= g_ambassador_reward_cnt; ++i) {
		if (!test_bit_on(p->once_bit, flag_pos_ambassador + i)) {
			return false;
		}
	}
	return true;
}

inline int do_finish_get_reward(player_t* p)
{
	return db_set_user_flag(p, 1);
}

/**
 * @brief add here for platform of statstics
 */
inline void do_stat_log_get_reward(uint32_t userid, uint32_t pos)
{
    uint32_t buf[1] = {1};
	msglog(statistic_logfile, stat_log_get_amb_reward + pos, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [pos=%u] [buf=%u]",
        stat_log_get_amb_reward + pos, pos, buf[0]);
}

void set_amb_info_after_lv_up(player_t *p)
{
	if (p->lv == 5 && p->parentid && !is_achieve_amb_lv(p)) {
		db_add_amb_achieve_num(p->parentid);
		set_once_bit_on(p, flag_pos_achieve_lv);
		db_set_user_flag(p, 0);
		do_stat_log_achieve_child_num(p);
	}
}

//------------------------------------------------------------------------------------------------
//--- cmd processings
//------------------------------------------------------------------------------------------------
/**
  * @brief get set amb task flg : 0, never got; 1,nomor accept; 2,finished
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int set_amb_task_status_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint8_t flag = 0;
	int idx = 0;
	unpack(body, flag, idx);
	if ((flag == 2 && p->amb_status == 0) ||
		(p->amb_status == 2 && (flag == 0 || flag == 1))) {
		KDEBUG_LOG(p->id, "ERROR STATUS\t%u %u and %u", p->id, p->amb_status, flag);
		return send_header_to_player(p, p->waitcmd, cli_err_set_amb_status, 1);
	}
	return db_set_amb_task_status(p, flag);
}

/**
  * @brief get amb task info
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_amb_task_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->amb_status, idx);
	pack(pkgbuf, p->child_cnt, idx);
	pack(pkgbuf, p->achieve_cnt, idx);
	uint8_t amb_award_bit[4] = {0};
	amb_award_bit[0] = p->once_bit[0];
	pack(pkgbuf, amb_award_bit, 4, idx);
	TRACE_LOG("%u %u %u %u %x %x %x %x", p->id, p->amb_status, p->child_cnt, p->achieve_cnt, p->once_bit[0]
		, p->once_bit[1], p->once_bit[2], p->once_bit[3]);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx , 1);
}

/**
  * @brief get amb reward
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_amb_reward_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint8_t pos = 0;
	int idx = 0;
	unpack(body, pos, idx);
	if (p->battle_grp) {
		return send_header_to_player(p, p->waitcmd, cli_err_unable_in_battle, 1);
	}

	return db_amb_swap_item(p, pos);
}

//------------------------------------------------------------------------------------------------
/**
 * @brief update new_invitee_cnt in online and to db
 * @param p invitee
 */

//------------------------------------------------------------------------------------------------
//----- db operation functions
//------------------------------------------------------------------------------------------------

int db_amb_swap_item(player_t* p, uint8_t pos)
{
	if (!can_receive_amb_reward(p, pos)) {
		KDEBUG_LOG(p->id, "AMB REWARD ERR\t[%u %u %u %u]", p->id, pos, 
			(pos > 0 && pos <= max_ambassador_reward_num) ? test_bit_on(p->once_bit, pos) : 2, 
			p->achieve_cnt);
		return send_header_to_player(p, p->waitcmd, cli_err_get_amb_reward, 1);
	}
	
	//pack type in player's session
	p->sesslen = 0;
	pack_h(p->session, pos, p->sesslen);
	ambassador_reward_t* p_reward = &(g_ambassador_reward[pos]);
	single_reward_t* p_sigle = g_ambassador_reward[pos].reward_arr;
	int idx = 0;
	pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);
	uint32_t reward_cnt = p_reward->cnt;
	int idxtmp = idx;
	pack_h(dbpkgbuf, 0, idx);
	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);

	for (uint32_t i = 0; i < p_reward->cnt; i++) {
		uint32_t duration = 0, lifetime = 0;
		uint32_t item_max = 999;
		
		if (p_sigle->give_type == give_type_normal_item
				 || p_sigle->give_type == give_type_clothes) {
			const GfItem* itm = items->get_item(p_sigle->give_id);
			duration = itm->duration * clothes_duration_ratio;
			item_max = itm->max();
		}
		if (p->lv >= max_exp_lv && 
			p_sigle->give_type == give_type_player_attr && 
			p_sigle->give_id == give_type_exp) {
			reward_cnt--;
			p_sigle++;
			continue;
		}
		pack_h(dbpkgbuf, p_sigle->give_type, idx);
		pack_h(dbpkgbuf, p_sigle->give_id, idx);
		pack_h(dbpkgbuf, p_sigle->count, idx);
		pack_h(dbpkgbuf, duration, idx);
		pack_h(dbpkgbuf, lifetime, idx);
		pack_h(dbpkgbuf, item_max, idx);
		TRACE_LOG("reward item :[%u %u %u %u %u %u %u]",p->id, p_sigle->give_type, p_sigle->give_id,
								p_sigle->count, duration, lifetime, item_max);
		p_sigle++;
	}
	pack_h(dbpkgbuf, reward_cnt, idxtmp);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_swap_item, dbpkgbuf, idx);
}


int db_set_user_flag(player_t* p, uint8_t retflag)
{
	int idx = 0;
	pack(dbpkgbuf, p->once_bit, sizeof(p->once_bit), idx);
	TRACE_LOG("set_user: %x %x %x", p->once_bit[0], p->once_bit[1], p->once_bit[2]);
	return send_request_to_db(retflag ? p : 0, p->id, 0, dbproto_set_user_flag, dbpkgbuf, idx);
}

int db_set_amb_task_status(player_t* p, uint8_t status)
{
	int idx = 0;
	pack_h(dbpkgbuf, status, idx);
	KDEBUG_LOG(p->id, "SET AMB STATUS\t[ %u %u and %u ]", p->id, p->amb_status, status);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_set_amb_task_status, dbpkgbuf, idx);
}

//------------------------------------------------------------------------------------------------
//--- db operations callback processing 
//------------------------------------------------------------------------------------------------
/**
 * @brief callback for swap item
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */

int db_amb_swap_item_callback(player_t* p, swap_item_rsp_t* p_rsp, uint32_t bodylen)
{
	uint8_t pos = 0;
	int idx = 0;
	unpack_h(p->session, pos, idx);

	TRACE_LOG("%u", pos);
	ambassador_reward_t *p_reward = &(g_ambassador_reward[pos]);
	single_reward_t *p_single = g_ambassador_reward[pos].reward_arr;
	for (uint32_t i = 0; i < p_reward->cnt; i++) {
		if (p_single->give_type == give_type_normal_item) {
			//add to my packs
			p->my_packs->add_item(p, p_single->give_id, p_single->count, channel_string_other, true, system_reward);
		}
	}
	fin_amb_session_t* p_sess = reinterpret_cast<fin_amb_session_t *>(p->session);

	p_sess->clothes_cnt = p_rsp->clothes_cnt;
	for(uint32_t i = 0; i < p_rsp->clothes_cnt; i++) {
		p_sess->clothes[i].clothes_id = p_rsp->clothes[i].clothes_id;
		p_sess->clothes[i].unique_id = p_rsp->clothes[i].unique_id;
		TRACE_LOG("%u %u %u ", p_rsp->clothes_cnt, p_rsp->clothes[i].clothes_id, p_rsp->clothes[i].unique_id);
		//add to my packs
		p->my_packs->add_clothes(p, p_rsp->clothes[i].clothes_id, p_rsp->clothes[i].unique_id, 0, channel_string_other);
	}
	p->sesslen = 1 + sizeof(*p_sess);

	do_stat_log_get_reward(p->id, pos);
	set_once_bit_on(p, flag_pos_ambassador + pos);
	return do_finish_get_reward(p);
}


/**
 * @brief callback for user_flag
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_set_user_flag_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	if (p->waitcmd == cli_proto_get_amb_reward) {

		if (all_amb_reward_got(p)) {
			return db_set_amb_task_status(p, amb_task_finished);
		} else {
			int idx = 0;
			fin_amb_session_t* p_sess = reinterpret_cast<fin_amb_session_t *>(p->session);

			idx = sizeof(cli_proto_t);
			pack(pkgbuf, p_sess->pos, idx);
			pack(pkgbuf, p_sess->clothes_cnt, idx);
			p_sess->clothes_cnt = p_sess->clothes_cnt;
			for(uint32_t i = 0; i < p_sess->clothes_cnt; i++) {
				pack(pkgbuf, p_sess->clothes[i].clothes_id, idx);
				pack(pkgbuf, p_sess->clothes[i].unique_id, idx);
				TRACE_LOG("p:%u %u bit:%x clothes[%u %u]", p->id, p_sess->pos, p->once_bit[0], p_sess->clothes[i].clothes_id,
					p_sess->clothes[i].unique_id);
			}

			init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
			return send_to_player(p, pkgbuf, idx , 1);
		}
	}
	return 0;
}
/**
 * @brief callback for set amb task status
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_set_amb_task_status_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	if (p->waitcmd == cli_proto_get_amb_reward) {
		int idx = 0;
		fin_amb_session_t* p_sess = reinterpret_cast<fin_amb_session_t *>(p->session);

		idx = sizeof(cli_proto_t);
		pack(pkgbuf, p_sess->pos, idx);
		pack(pkgbuf, p_sess->clothes_cnt, idx);
		p_sess->clothes_cnt = p_sess->clothes_cnt;
		for(uint32_t i = 0; i < p_sess->clothes_cnt; i++) {
			pack(pkgbuf, p_sess->clothes[i].clothes_id, idx);
			pack(pkgbuf, p_sess->clothes[i].unique_id, idx);
			TRACE_LOG("p:%u bit:%x clothes[%u %u]", p->id, p->once_bit[0], p_sess->clothes[i].clothes_id,
				p_sess->clothes[i].unique_id);
		}

		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx , 1);
	}
	
	uint8_t status = 0;

	int idx = 0;
	unpack_h(body, status, idx);
	
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, status, idx);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx , 1);
}

//------------------------------------------------------------------------------------------------
//--- Parse Xml File
//------------------------------------------------------------------------------------------------
/**
  * @brief parse the "costs" and "rewards" sub node
  * @param cur sub node 
  * @param idx index to indicate certain restriction action
  * @return true on success, false on error
  */
bool load_ambassador_detail_reward(xmlNodePtr cur, uint32_t id)
{
	uint32_t cnt = 0;

	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("Item"))) {
				get_xml_prop(g_ambassador_reward[id].reward_arr[cnt].give_type, cur, "give_type");
				get_xml_prop(g_ambassador_reward[id].reward_arr[cnt].count, cur, "cnt");
				get_xml_prop(g_ambassador_reward[id].reward_arr[cnt].give_id, cur, "give_id");
				
				TRACE_LOG("detail:[%u %u %u]",(g_ambassador_reward[id]).reward_arr[cnt].give_type, 
					(g_ambassador_reward[id]).reward_arr[cnt].give_id, g_ambassador_reward[id].reward_arr[cnt].count);
				if (g_ambassador_reward[id].reward_arr[cnt].give_type == 1) {
					const GfItem* itm = items->get_item(g_ambassador_reward[id].reward_arr[cnt].give_id);
					if (!itm || !(items->is_clothes(itm->category()))) {
						ERROR_LOG("Parse ambassador detail rewards error! %u", g_ambassador_reward[id].reward_arr[cnt].give_id);
						throw XmlParseError(std::string("load_ambassador_detail_reward failed"));
					}
				}
				if (g_ambassador_reward[id].reward_arr[cnt].give_type == 2) {
					const GfItem* itm = items->get_item(g_ambassador_reward[id].reward_arr[cnt].give_id);
					if (!itm) {
						ERROR_LOG("Parse ambassador detail rewards error! %u", g_ambassador_reward[id].reward_arr[cnt].give_id);
						throw XmlParseError(std::string("load_ambassador_detail_reward failed"));
					}
				}				
				cnt++;
		}
		cur = cur->next;
	}

	g_ambassador_reward[id].cnt = cnt;
	TRACE_LOG("restr cnt[%u]",g_ambassador_reward[id].cnt);
	return true;
}



/**
  * @brief load ambassador configs from an xml file
  * @param cur xml node 
  * @return 0 on success, -1 on error
  */
int load_ambassador(xmlNodePtr cur)
{
	uint32_t id_test = 0;
	uint32_t id = 0;
	g_ambassador_reward_cnt = 0;
	cur = cur->xmlChildrenNode; 
	memset(g_ambassador_reward, 0, sizeof(g_ambassador_reward));
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Type"))) {
			id_test++;
			get_xml_prop(id, cur, "id");
			if (id != id_test || id > max_ambassador_reward_num) {
				ERROR_LOG("Id duplicated ambassador: id=%u", id);
				throw XmlParseError(std::string("id invaild"));
			}
			if (!load_ambassador_detail_reward(cur->xmlChildrenNode, id)) {
				ERROR_LOG("Parse ambassador detail rewards error!");
				throw XmlParseError(std::string("load_ambassador_detail_reward failed"));
			}
			g_ambassador_reward_cnt ++;
			if (g_ambassador_reward_cnt > max_ambassador_reward_num) {
				throw XmlParseError(std::string("load_ambassador_detail_reward failed too many lines"));
			}
		}
		cur = cur->next;
	}
	TRACE_LOG("total cnt: %u ", g_ambassador_reward_cnt);
	return 0;
}

