extern "C" {
#include <stdint.h>
#include <async_serv/mcast.h>
}

#include "achievement.hpp"
//#include "achievement_logic.hpp"
#include "utils.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "global_data.hpp"
#include "player.hpp"
#include "mcast_proto.hpp"
#include "app_log.hpp"
#include "common_op.hpp"
#include "rank_top.hpp"
#include "cachesvr.hpp"
#include "common_op.hpp"

using namespace taomee;

void proc_login_achieve_check(player_t * p);
void reset_player_achive_to_cachesvr(player_t * p);


int load_player_achievement_data_list(player_t* p, get_achievement_data_list_rsp_t* rsp)
{
	p->achieve_point = 0;
	for(uint32_t i =0; i< rsp->count; i++)
	{
		achievement_data_element* pdata = &(rsp->datas[i]);
		set_player_achievement_data(p, pdata->achieve_type, pdata->get_time);		
		achievement_data * data = get_achievement_data_mgr()->get_achievement_data_by_id(pdata->achieve_type);
		if (data) {
			p->achieve_point +=  data->add_point_;
		}
	}
	return 0;
}

int load_player_stat_data_list(player_t * p, db_player_stat_rsp_t * rsp)
{
	for (uint32_t i = 0; i < rsp->total_cnt; ++i) {
		player_stat_t data = rsp->stat_info[i];
		p->m_statdatas->insert(std::map<int, player_stat_t>::value_type(data.stat_id, data));
	}
	return 0;
}


bool achievement_data_mgr::init_ex(const char* xml)
{
	if(xml == NULL){
		return false;
	}
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr honor_node = NULL;
	xmlNodePtr item_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the glory xml file is not exist"));
		return false;
	}
	
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the glory xml file content is empty"));
		return false;
	}
	
	honor_node = root->xmlChildrenNode;
	while(honor_node)
	{
		if( xmlStrcmp(honor_node->name, BAD_CAST"Honor") == 0)
		{
			item_node = honor_node->xmlChildrenNode;
			while(item_node)
			{
				if( xmlStrcmp(item_node->name, BAD_CAST"Item") == 0)
				{
					uint32_t id = 0;
					uint32_t add_point = 0;
					uint32_t title = 0;
					uint32_t stat_id = 0;
					uint32_t stat_need = 0;

					get_xml_prop_def(id,  item_node, "id", 0);
					get_xml_prop_def(add_point,  item_node, "honorPoint", 0);
					get_xml_prop_def(title,  item_node, "titleID", 0);
					get_xml_prop_def(stat_id, item_node, "StatId", 0);
					if (stat_id) {
						get_xml_prop(stat_need, item_node, "StatNeed");
					}

					if(id == 0){
						throw XmlParseError(std::string("invalid id in the glory xml"));
						return false;
					}

					if( is_data_exist(id)){
						throw XmlParseError(std::string("id has existed in the glory xml"));
						return false;
					}

					achievement_data* pdata = new achievement_data;
					pdata->id_ = id;
					pdata->add_point_ = add_point;
					pdata->title_ = title;
					data_maps[id] = pdata;
					TRACE_LOG("LOAD ACHEIEVEMENT [%u %u %u]", pdata->id_, pdata->add_point_, pdata->title_);

					if (stat_id) {
						stat_data_t * data = new stat_data_t();
						data->stat_id = stat_id;
						data->need_cnt = stat_need;
						data->achieve_id = id;
						stat_maps[stat_id] = data;
					}
				}	
				item_node = item_node->next;
			}
		}

		honor_node = honor_node->next;
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	return true;
}

bool achievement_data_mgr::init(const char* xml)
{
	if(xml == NULL){
		return false;		    
	}
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr achievement_node = NULL;

	xmlKeepBlanksDefault(0);
	
	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the achievement xml file is not exist"));
		return false;
	}

	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the achievement xml file content is empty"));
		return false;
	}

	achievement_node = root->xmlChildrenNode;
	while( achievement_node )
	{
		uint32_t id = 0;
		uint32_t add_point = 0;

		get_xml_prop_def(id,  achievement_node, "id", 0);
		get_xml_prop_def(add_point,  achievement_node, "add_point", 0);

		if(id == 0){
			throw XmlParseError(std::string("invalid id in the achievement xml"));
			return false;
		}	

		if( is_data_exist(id)){
			ERROR_LOG("ERROR ACHIEVEMENT ID = %u EXISTED", id);
			throw XmlParseError(std::string("id has existed in the achievement xml"));
			return false;
		}

		achievement_data* pdata = new achievement_data;
		pdata->id_ = id;
		pdata->add_point_ = add_point;
		data_maps[id] = pdata;	

		achievement_node = achievement_node->next;
	}


	xmlFreeDoc(doc);
	xmlCleanupParser();
	return true;
}

bool achievement_data_mgr::final()
{
	std::map<uint32_t, achievement_data*>::iterator pItr = data_maps.begin();
	for(; pItr != data_maps.end(); ++pItr)
	{
		achievement_data* pdata = pItr->second;
		if(pdata == NULL)continue;
		delete pdata;
		pdata = NULL;
	}
	data_maps.clear();

    return true;
}


bool achievement_data_mgr::is_data_exist(uint32_t id)
{
	return data_maps.find(id) != data_maps.end();
}

achievement_data*  achievement_data_mgr::get_achievement_data_by_id(uint32_t id)
{
	std::map<uint32_t, achievement_data*>::iterator pItr = data_maps.find(id);
	if(pItr == data_maps.end())return NULL;
	return pItr->second;
}

uint32_t  achievement_data_mgr::get_add_achieve_point_by_id(uint32_t id)
{
	achievement_data* pdata = get_achievement_data_by_id(id);
	if( pdata == NULL)return 0;
	return pdata->add_point_;
}

stat_data_t * achievement_data_mgr::get_stat_data_by_id(uint32_t stat_id)
{
	std::map<uint32_t, stat_data_t*>::iterator pItr = stat_maps.find(stat_id);
	if(pItr == stat_maps.end())return NULL;
	return pItr->second;
}



bool init_player_achievement_data(player_t* p)
{
	p->m_achievedatas = new std::map<int, player_achievement_data>;
	p->achieve_point = 0;
	p->last_update_ap_tm = 0;

	p->m_statdatas = new std::map<int, player_stat_t>;
	return true;
}

bool final_player_achievement_data(player_t* p)
{
	if (p->m_achievedatas) {
		delete p->m_achievedatas;
		p->m_achievedatas = NULL;
	}
	p->achieve_point = 0;
	p->last_update_ap_tm = 0;
	if (p->m_statdatas) {
		delete p->m_statdatas;
		p->m_statdatas = NULL;
	}
	return true;
}

void set_player_achievement_data(player_t* p, int id, uint32_t get_time)
{
	player_achievement_data data(id, get_time);
	p->m_achievedatas->insert(std::map<int, player_achievement_data>::value_type(id, data));
}


bool is_player_achievement_data_exist(player_t* p, int id)
{
	std::map<int, player_achievement_data>::iterator it = p->m_achievedatas->find(id);
	if (it != p->m_achievedatas->end()) {
		return true;
	}
	return false;
}


bool init_achievement_callback_handles()
{
	return true;
}


int db_get_achievement_data_list(player_t* p, uint32_t id, uint32_t role_tm)
{
	return send_request_to_db(p, id, role_tm,  dbproto_get_achievement_data_list, 0, 0);
}

int db_replace_achievement_data(player_t* p, uint32_t achieve_type, uint32_t get_time, uint32_t add_achieve_point)
{
	int idx = 0;
	pack_h(dbpkgbuf, achieve_type, idx);
	pack_h(dbpkgbuf, get_time, idx);
	pack_h(dbpkgbuf, add_achieve_point, idx);
	return send_request_to_db(NULL, p->id, p->role_tm,  dbproto_replace_achievement_data, dbpkgbuf, idx);
}

int db_get_achievement_data_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_achievement_data_list_rsp_t *rsp = reinterpret_cast<get_achievement_data_list_rsp_t*>(body);
	CHECK_VAL_GE(bodylen, sizeof(get_achievement_data_list_rsp_t) + rsp->count*sizeof(achievement_data_element));
	uint32_t uid  = 0;
    uint32_t role_tm = 0;
	int idx = 0;
    unpack_h(p->session, uid, idx);
	unpack_h(p->session, role_tm, idx);
	//list self achievement
	if (uid == p->id && role_tm == p->role_tm) {	
		load_player_achievement_data_list(p, rsp);
		send_player_achievement_data_list(p);
		db_get_player_stat_info(p);
		//proc login achieve check
		proc_login_achieve_check(p);
	} else {
		idx = sizeof(cli_proto_t);
		pack(pkgbuf, uid, idx);
		pack(pkgbuf, role_tm, idx);
		pack(pkgbuf, rsp->count, idx);
		for (uint32_t i = 0; i < rsp->count; i++) {
			achievement_data_element* pdata = &(rsp->datas[i]);
			pack(pkgbuf, pdata->achieve_type, idx);
			pack(pkgbuf, pdata->get_time, idx);
		}
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		send_to_player(p, pkgbuf, idx, 1);
	}
	return 0;
}


void add_player_stat_cnt(player_t * p, uint32_t stat_id, uint32_t add_cnt)
{
	std::map<int, player_stat_t>::iterator it = p->m_statdatas->find(stat_id);
	if (it != p->m_statdatas->end()) {
		(it->second).stat_cnt += add_cnt;
	} else {
		player_stat_t data;
		data.stat_id = stat_id;
		data.stat_cnt = add_cnt;
		p->m_statdatas->insert(std::map<int, player_stat_t>::value_type(stat_id, data));
	}
	int idx = 0;
	pack_h(dbpkgbuf, stat_id, idx);
	pack_h(dbpkgbuf, add_cnt, idx);
	send_request_to_db(NULL, p->id, p->role_tm, dbproto_add_player_stat_info, dbpkgbuf, idx);
}

int get_player_stat_info(player_t * p, int stat_id)
{
	std::map<int, player_stat_t>::iterator it = p->m_statdatas->find(stat_id);
	if (it != p->m_statdatas->end()) {
		return (it->second).stat_cnt;
	}
	return 0;
}



void db_get_player_stat_info(player_t * p)
{
	send_request_to_db(p, p->id, p->role_tm, dbproto_get_player_stat_info, NULL, 0);
}

void pack_player_stat_info(player_t * p, void * buf, int & idx)
{
	uint32_t cnt = 0;
	int ifx = idx + 4;
	for (std::map<int, player_stat_t>::iterator it = p->m_statdatas->begin(); 
			it != p->m_statdatas->end(); ++it) {
		stat_data_t * data = get_achievement_data_mgr()->get_stat_data_by_id(it->first);
		if (data && data->need_cnt > (it->second).stat_cnt) {
			pack(buf, data->achieve_id, ifx);
			pack(buf, (it->second).stat_cnt, ifx);
			pack(buf, data->need_cnt, ifx);
			cnt ++;
		}
	}
	pack(buf, cnt, idx);
	idx = ifx;
}

int  send_player_stat_info(player_t * p)
{
	int idx = sizeof(cli_proto_t);
	pack_player_stat_info(p, pkgbuf, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_achievement_stat_info, idx);
	if (p->waitcmd == cli_proto_achievement_stat_info) {
		p->waitcmd = 0;
	} 
	return send_to_player(p, pkgbuf, idx, 0);
}

int achievement_stat_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	return send_player_stat_info(p);
}


int db_get_player_stat_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	db_player_stat_rsp_t *rsp = reinterpret_cast<db_player_stat_rsp_t*>(body);
	CHECK_VAL_GE(bodylen, sizeof(db_player_stat_rsp_t) + rsp->total_cnt*sizeof(player_stat_t));
	load_player_stat_data_list( p, rsp);
	reset_player_achive_to_cachesvr(p);
	return send_player_stat_info(p);
}

void pack_player_achievement_list(player_t * p, void * buf, int &idx)
{
	uint32_t count = p->m_achievedatas->size();
	pack(buf, p->id, idx);
	pack(buf, p->role_tm, idx);
	pack(buf, count, idx);
	for (std::map<int, player_achievement_data>::iterator it = p->m_achievedatas->begin();
			it != p->m_achievedatas->end(); ++it) {
		pack(buf, (it->second).id_, idx);
		pack(buf, (it->second).get_time_, idx);
	}
}




int get_achievement_data_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t other_id = 0;
	uint32_t role_tm = 0;
	unpack(body, other_id, idx);
	unpack(body, role_tm, idx);

	if (other_id == p->id && role_tm == p->role_tm) {
		idx = 0;
		pack_h(p->session, other_id, idx);
		pack_h(p->session, role_tm, idx);
		db_get_achievement_data_list(p, p->id, p->role_tm);
	} else {
		player_t * other_one =  get_player(other_id);
		if (other_one) {
			int idx = sizeof(cli_proto_t);
			pack_player_achievement_list(other_one, pkgbuf, idx);
			init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
			send_to_player(p, pkgbuf, idx, 1);
		} else {
			idx = 0;
			pack_h(p->session, other_id, idx);
			pack_h(p->session, role_tm, idx);
			db_get_achievement_data_list(p, other_id, role_tm);
		}
	}
	return 0;
}

int send_player_achievement_data_list(player_t* p)
{
	int idx = sizeof(cli_proto_t);
	pack_player_achievement_list(p, pkgbuf, idx);
	init_cli_proto_head(pkgbuf, p,  cli_proto_get_achievement_data_list, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int send_add_achievement_data(player_t* p, uint32_t achieve_type, uint32_t get_time, uint32_t achieve_point)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, achieve_type, idx);
	pack(pkgbuf, get_time, idx);
	pack(pkgbuf, achieve_point, idx);
	init_cli_proto_head(pkgbuf, p,  cli_proto_add_achievement_data, idx);
	return send_to_player(p, pkgbuf, idx, 0);
}


int broadcast_add_achievement(player_t* p, uint32_t achieve_type)
{
	uint32_t now = get_now_tv()->tv_sec;
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, p->role_tm, idx);
	pack(pkgbuf, p->nick, max_nick_size,  idx);
	pack(pkgbuf, achieve_type, idx);
	pack(pkgbuf, now, idx);
    init_cli_proto_head(pkgbuf, p, cli_proto_notify_achieve_get_mcast, idx);
	send_to_all(pkgbuf, idx);

	idx = sizeof(mcast_pkg_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, p->role_tm, idx);
	pack(pkgbuf, p->nick, max_nick_size,  idx);
	pack(pkgbuf, achieve_type, idx);
	pack(pkgbuf, get_now_tv()->tv_sec, idx);
    init_mcast_pkg_head(pkgbuf, 60007, 0);
    send_mcast_pkg(pkgbuf, idx);

	return 0;
	//send to other server
}

void player_gain_achieve(player_t * p, int achieve_id)
{
	if (is_player_achievement_data_exist(p, achieve_id)) {
		return;
	}
	achievement_data * data  = get_achievement_data_mgr()->get_achievement_data_by_id(achieve_id);
	if (data) {
		uint32_t now_time = get_now_tv()->tv_sec;
		p->add_achieve_point(data->add_point_, now_time);
		set_player_achievement_data(p, achieve_id, now_time);
		db_replace_achievement_data(p, data->id_, now_time, data->add_point_);
		send_add_achievement_data(p, data->id_, now_time, data->add_point_);
		KDEBUG_LOG(p->id, "PLAYER GAIN ACHIEVE [%u | %u | %u]", data->id_, data->add_point_, now_time); 
		if (data->title_) {
			do_special_title_logic(p, data->title_);
		}
		add_player_active_score(p, 12, data->add_point_);
		add_player_active_score(p, 13, data->add_point_);
		if (data->add_point_ == 10000) {
			broadcast_add_achievement(p, data->id_);
		}
	}
}

int trigger_achieve_event(player_t * p, achieve_event * event)
{
	lua_achieve_event_handle(p, event);
	return 0;
}

void process_pve_win_achieve_logic(player_t * p, int stageid, int btl_type, int stage_grade, int diff)
{
	achieve_event event(event_pass_stage, stageid, btl_type, stage_grade, diff);
	trigger_achieve_event(p, &event);
}

void proc_kill_monster_achieve_logic(player_t * p, int monster_id, int diff, int battle_killed_cnt)
{
	achieve_event event(event_kill_monster, monster_id, diff, battle_killed_cnt);
	trigger_achieve_event(p, &event);
}

void proc_task_achieve_logic(player_t * p, int task_type, int task_id)
{
	achieve_event event(event_task_finish, task_type, task_id);
	trigger_achieve_event(p, &event);
}

void proc_ranker_range_achieve_logic(player_t *p, int ranker_type, int range_id, int self_range)
{
	achieve_event event(event_ranker_range, ranker_type, range_id, self_range);
	trigger_achieve_event(p, &event);
}

void proc_use_item_achieve_logic(player_t *p, int item_id, int cnt, int channel, int swap_id)
{
	achieve_event event(event_use_item, item_id, cnt, channel, swap_id);
	trigger_achieve_event(p, &event);
}

void proc_gain_item_achieve_logic(player_t *p, int item_id, int cnt, int channle, int swap_id)
{
	achieve_event event(event_gain_item, item_id, cnt, channle, swap_id);
	trigger_achieve_event(p, &event);
}

void proc_gain_clothes_achieve_logic(player_t *p, int clothes_id, int lv, int channel, int swap_id)
{
	achieve_event event(event_gain_clothes, clothes_id, lv, channel, swap_id);
	trigger_achieve_event(p, &event);
}

void proc_pvp_win_achieve_logic(player_t *p, int pvp_lv, int win_status)
{
	achieve_event event(event_pvp_win, pvp_lv, win_status);
	trigger_achieve_event(p, &event);
}

void proc_summon_achieve_logic(player_t *p)
{
	achieve_event event(event_summon_info);
	trigger_achieve_event(p, &event);
}


void proc_player_team_achieve_logic(player_t *p)
{
	achieve_event event(event_team_info);
	trigger_achieve_event(p, &event);
}

void proc_player_clothes_logic(player_t * p)
{
	achieve_event event(event_clothes_info);
	trigger_achieve_event(p, &event);
}

void proc_login_achieve_check(player_t * p)
{
	proc_summon_achieve_logic(p);
//	proc_player_team_achieve_logic(p);
	proc_task_achieve_logic(p, 0, 0);
	proc_player_clothes_logic(p);
}


void reset_player_achive_to_cachesvr(player_t * p)
{
	uint32_t achieve_point = 0;
	for (std::map<int, player_achievement_data>::iterator it = p->m_achievedatas->begin(); 
			it != p->m_achievedatas->end(); ++it) {
		achievement_data * data  = get_achievement_data_mgr()->get_achievement_data_by_id((it->second).id_);
		if (data) {
			achieve_point += data->add_point_;
		}
	}

	int idx = 0;
	pack_h(dbpkgbuf, achieve_point, idx);
	pack_h(dbpkgbuf, p->role_type, idx);
	pack_h(dbpkgbuf, (int)(p->lv), idx);
	pack(dbpkgbuf, p->nick, max_nick_size, idx);
	send_request_to_cachesvr(p, p->id, p->role_tm, 	cache_reset_player_achieve,  dbpkgbuf, idx);
}


void proc_achieve_mcast(const void * body, uint32_t bodylen)
{
 	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, body, bodylen, idx);
    init_cli_proto_head(pkgbuf, 0, cli_proto_notify_achieve_get_mcast, idx);
    send_to_all(pkgbuf, idx);
}



