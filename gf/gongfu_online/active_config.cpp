#include "active_config.hpp"
#include "utils.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "cli_proto.hpp"
#include "global_data.hpp"
#include "common_op.hpp"
#include "stage_config.hpp"
#include "fight_team.hpp"
#include "mcast_proto.hpp"
using namespace taomee;

//static bool init_data_array( active_data_mgr* mgr );
static bool check_active_time(active_time_data* pdata);

bool active_data_mgr::init(const char* xml)
{
	if(xml == NULL){
		return false;
	}
	final();
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr active_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the active xml file is not exist"));
		return false;
	}
	
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the active xml file content is empty"));
		return false;
	}

	active_node = root->xmlChildrenNode;

	while(active_node)
	{
		active_data active_data_elem;
		active_data* pdata = &active_data_elem;
		
		
		get_xml_prop_def(pdata->active_id_, active_node,  "id", 0);
		get_xml_prop_def(pdata->min_lv_, active_node,  "min_lv", 0);
		get_xml_prop_def(pdata->max_lv_, active_node,  "max_lv", 0);
		get_xml_prop_def(pdata->is_team_, active_node,  "is_team", 0);
		get_xml_prop_def(pdata->team_member_, active_node,  "team_member", 0);
		get_xml_prop_def(pdata->team_coins_, active_node,  "team_coins", 0);
		get_xml_prop_def(pdata->coins_, active_node,  "coins", 0);
		get_xml_prop_def(pdata->broadcast_switch_, active_node,  "broadcast", 0);

        char fromstr[128] = "";
		get_xml_prop_raw_str_def(fromstr, active_node, "from", "NULL");
        pdata->from_ = get_utc_second_ex(fromstr);

        memset(fromstr, 0, sizeof(fromstr));
		get_xml_prop_raw_str_def(fromstr, active_node, "to", "NULL");
        pdata->to_ = get_utc_second_ex(fromstr);

		get_xml_prop_raw_str_def(pdata->word1, active_node, "word1", "NULL");
		get_xml_prop_raw_str_def(pdata->word2, active_node, "word2", "NULL");

		if( pdata->active_id_ == 0){
			throw XmlParseError(std::string("the active xml file active_id is 0"));
			return false;
		}

		xmlNodePtr limit_node = active_node->xmlChildrenNode;
		while(limit_node)
		{	
			active_time_data data;
			
			get_xml_prop_def(data.week_,   limit_node, "week",  -1);
			
			char begin_time[10] = { 0 };
			char end_time[10] = { 0 };
			get_xml_prop_raw_str_def(begin_time, limit_node, "begin_hour", "");
			get_xml_prop_raw_str_def(end_time, limit_node, "end_hour", "");

			bool ret = false;

			if(strlen(begin_time) != 0){
				ret = string2time(begin_time,  data.begin_hour_, data.begin_min_);
				if(!ret){
					throw XmlParseError(std::string("begin_hour error"));
				}
			}

			if(strlen(end_time) != 0){
				ret = string2time(end_time, data.end_hour_,  data.end_min_);
				if(!ret){
					throw XmlParseError(std::string("end_hour error"));
				}
			}

			TRACE_LOG("WEEK-%u [from-%u:%u to-%u:%u]", data.week_, 
                data.begin_hour_, data.begin_min_, data.end_hour_, data.end_min_);

			get_xml_prop_def(data.enter_flag_, limit_node, "enter_flag", 0);
			
			pdata->push_active_time_data(data);
			
			limit_node = limit_node->next;
		}

		add_active_data(pdata);
		active_node = active_node->next;
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	return true;
}

bool active_data_mgr::final()
{
    active_datas_.clear();
	return true;
}

bool active_data_mgr::add_active_data(active_data* pData)
{
	if(pData == NULL)return false;
	std::map<uint32_t, active_data>::iterator pItr = active_datas_.find(pData->active_id_);
	if(pItr == active_datas_.end()){
        //active_datas_.insert(map<uint32_t,  active_data>::value_type(pData->active_id_, *pData));
        active_datas_[pData->active_id_] = *pData;
		return true;
	}
	throw XmlParseError(std::string("add_data"));
	return false;
}


active_data* active_data_mgr::get_data_by_active_id(uint32_t active_id)
{
	std::map<uint32_t, active_data>::iterator pItr = active_datas_.find(active_id);
	if(pItr == active_datas_.end()){
		return NULL;
	}

    return &(pItr->second);
}

uint32_t check_player_can_join_active(player_t* p, uint32_t active_id, bool is_free)
{
    if (is_free) return 0;
    return check_player_enter_active(get_active_data_mgr(), p, active_id, is_free);
}

uint32_t is_in_active_time_section(uint32_t active_id)
{
    active_data* pdata = get_active_data_mgr()->get_data_by_active_id(active_id);
    if (pdata == NULL) {
        return 0;
    }

    uint32_t cur_time = get_now_tv()->tv_sec;
    if ( (pdata->from_ && cur_time < pdata->from_) 
        || (pdata->to_ && cur_time > pdata->to_) ) {
        return cli_err_swap_action_not_in_time;
    }

    for (uint32_t i = 0; i < pdata->time_datas_.size(); i++) {
        if ( check_active_time(&(pdata->time_datas_[i])) ) {
            return 0;
        }
    }
    return get_active_errcode(active_id);
}

uint32_t check_player_enter_active(active_data_mgr* mgr, player_t* p, uint32_t active_id, bool is_free)
{
    if (active_id == 0) {
        return 0;
    }

    active_data* pdata = mgr->get_data_by_active_id(active_id);
    if (pdata == NULL) {
        return 0;
    }

    uint32_t cur_time = get_now_tv()->tv_sec;
    if ( (pdata->from_ && cur_time < pdata->from_) 
        || (pdata->to_ && cur_time > pdata->to_) ) {
        return cli_err_swap_action_not_in_time;
    }

    if ( !(pdata->check_level(p)) ) {
        return cli_err_less_lv;
    }

    uint32_t err_id = pdata->check_team(p);
    if (err_id) {
        return err_id;
    }

    for (uint32_t i = 0; i < pdata->time_datas_.size(); i++) {
        if ( check_active_time(&(pdata->time_datas_[i])) ) {
            return check_and_reduce_coins(p, pdata->coins_);
        }
    }
    return get_active_errcode(active_id);
}

active_time_data * get_now_active_data_by_active_id(uint32_t active_id)
{
    active_data* pdata = get_active_data_mgr()->get_data_by_active_id(active_id);
	if (!pdata) {
		return 0;
	}
    uint32_t cur_time = get_now_tv()->tv_sec;
    if ( (pdata->from_ && cur_time < pdata->from_) 
        || (pdata->to_ && cur_time > pdata->to_) ) {
		return 0;
    }

	return pdata->get_cur_time_data();
}

bool check_active_time(active_time_data* ptm)
{
	time_t cur_time = time(NULL);		
	struct tm  *p_cur_tm = localtime(&cur_time);

    if (ptm->enter_flag_ == 0) return false;

    if (ptm->week_ < 0 || ptm->week_ > 6 || ptm->week_ != p_cur_tm->tm_wday) return false;

    bool begin_flag = false;
    if (ptm->begin_hour_ != -1) {
        if (ptm->begin_hour_ < p_cur_tm->tm_hour) {
            begin_flag = true;
        } else if (ptm->begin_hour_ == p_cur_tm->tm_hour
            && ptm->begin_min_ <= p_cur_tm->tm_min) {
            begin_flag = true;
        }
    }

    bool end_flag = false;
    if (ptm->end_hour_ != -1) {
        if (ptm->end_hour_ > p_cur_tm->tm_hour) {
            end_flag = true;
        } else if (ptm->end_hour_ == p_cur_tm->tm_hour
            && ptm->end_min_ > p_cur_tm->tm_min) {
            end_flag = true;
        }
    }

    if (begin_flag && end_flag) return true;

    return false;
}

int  get_active_errcode(uint32_t active_id)
{
	switch(active_id)
	{
		case 1:
		{
			return cli_err_swap_action_not_in_time;
		}
		break;
		default:
		{
			return cli_err_swap_action_not_in_time;
		}
		break;
	}
	return 0;
}

bool is_in_holiday()
{
   return (is_in_active_time_section(36) == 0);
}

void send_player_mail_fun(void * key, void* value, void* user_data)
{
    player_t* player = reinterpret_cast<player_t*>(value);

    //ERROR_LOG("xxxxxxxxxxxxxxxx [%u]", player->id);        
    if (player->home_grp) {
        mail_active_event(player, 42, 43, 999, 1500601, 5);
    }
}

void do_active_event_logic()
{
    time_t cur_time = get_now_tv()->tv_sec;
    struct tm _tm;
    localtime_r(&cur_time, &_tm);

    if (is_in_active_time_section(20) == 0) {
        traverse_players(send_player_mail_fun, 0);
    }
}

//added by cws 0612
void send_escort_reward_mail(player_t* p, const char *nick, uint32_t uid = 0, uint32_t roletm = 0, uint32_t itemid = 0)
{
    char title[MAX_MAIL_TITLE_LEN + 1] = {0};
    char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
    std::vector<mail_item_enclosure> mail_item;

    memcpy(title, char_content[50].content, MAX_MAIL_TITLE_LEN);
    snprintf(content, MAX_MAIL_CONTENT_LEN, "%s", char_content[51].content);
    //TRACE_LOG("cws title[%s] [%u]", title, itemid);
    if (itemid) {
        mail_item.push_back(mail_item_enclosure(itemid, 1));
    }

    if (p) {
        db_send_system_mail(p, title, sizeof(title), content, sizeof(content), 999, 0, mail_item.size() ? &mail_item : 0, 0);
    }else {
        db_send_system_mail(p, title, sizeof(title), content, sizeof(content), 999, 0, mail_item.size() ? &mail_item : 0, 0, uid, roletm);
    }
    mail_item.clear();
}

void do_send_escort_reward(void* key, void* player, void* userdata)
{
	player_t* p = reinterpret_cast<player_t*>(player);
	send_escort_reward_mail(p, p->nick,0,0,2500025);
}

bool is_celebration_goods_full()
{
    uint32_t left_cnt = 0;
    for (uint32_t swap_id = 1486; swap_id <= 1489; swap_id++) {
        global_limit_data_t* p_limit_data = g_limit_data_mrg.get_limit_data(swap_id);	
        if (p_limit_data) {
            left_cnt += p_limit_data->left_cnt;
        }
    }
    return left_cnt == 0;
}

void do_active_escort_reward_logic()
{
    if (is_in_active_time_section(37) == 0) {
        time_t cur_time = get_now_tv()->tv_sec;
        struct tm _tm;
        localtime_r(&cur_time, &_tm);
        //ERROR_LOG("cws now time is %d:%d:%d",_tm.tm_hour,_tm.tm_min,_tm.tm_sec);
        if(_tm.tm_hour == 20 && _tm.tm_min == 0){
            char loop_words[max_trade_mcast_size] = {0};
            if(is_celebration_goods_full()) {
                DEBUG_LOG("Trigger Reward ...");
                //mcast to all
                snprintf(loop_words, max_trade_mcast_size, "%s", char_content[48].content);
                //see in notify_official_mcast_info
                notify_official_mcast_info(0, cli_proto_mcast_official_notice, loop_words);
                //send type of fireworks
                notify_rebuild_gf_fireworks(1);

                traverse_players(do_send_escort_reward, 0);
                g_limit_data_mrg.reset_global_limit_data(7);

            } else {
                snprintf(loop_words, max_trade_mcast_size, "%s", 
                    char_content[49].content);
                notify_official_mcast_info(0, cli_proto_mcast_official_notice, loop_words);
            }
        }
    }
}
void do_active_breadcast_logic()
{
    time_t cur_time = get_now_tv()->tv_sec;
    std::map<uint32_t, active_data>::iterator pItr = get_active_data_mgr()->active_datas_.begin();
    for (; pItr != get_active_data_mgr()->active_datas_.end(); ++pItr) {
        active_data *p_active = &(pItr->second);
        if (p_active->broadcast_switch_ == 0)  continue;

        struct tm _tm;
        localtime_r(&cur_time, &_tm);

        if ( (p_active->from_ && cur_time < p_active->from_) 
            || (p_active->to_ && cur_time > p_active->to_) ) {
            continue;
        }

        for (uint32_t i = 0; i < p_active->time_datas_.size(); i++) {
            active_time_data *p_time = &(p_active->time_datas_[i]);
            if (p_time->enter_flag_ && p_time->week_ == _tm.tm_wday) {
                uint32_t start_sec = get_utc_second(_tm.tm_year+1900, _tm.tm_mon+1, _tm.tm_mday, p_time->begin_hour_, p_time->begin_min_, 0);
                //ERROR_LOG("active %u %u", start_sec, cur_time);
                if (cur_time + 600 > start_sec && cur_time + 600 < start_sec + 60) {
                    notify_official_mcast_info(0, mcast_world_notice, p_active->word1);
                    break;
                }
                if (cur_time + 300 > start_sec && cur_time + 300 < start_sec + 60) {
                    notify_official_mcast_info(0, mcast_world_notice, p_active->word2);
                    break;
                }

            }
        } // for vector

    } // for map
}


int init_server_config(void* owner, void* data)
{
	memset(&g_server_config, 0, sizeof(server_config_t) * max_server_config_id);

	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr active_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile("./conf/server_config.xml");
	if(doc == NULL){
		ERROR_LOG("the server_config xml file is not exist");
		ADD_TIMER_EVENT(&g_events, init_server_config, 0, get_now_tv()->tv_sec + 300);
		return false;
	}
	
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		ERROR_LOG("the server_config xml file content is empty");
		ADD_TIMER_EVENT(&g_events, init_server_config, 0, get_now_tv()->tv_sec + 300);
		return false;
	}

	active_node = root->xmlChildrenNode;

	while(active_node)
	{
		uint32_t id = 0;
		
		get_xml_prop(id, active_node,  "ID");
		if (id >= max_server_config_id) {
			continue;
		}
		g_server_config[id].id = id;
		get_xml_prop(g_server_config[id].value, active_node,  "Value");
		active_node = active_node->next;
	}
	for (uint32_t i = 1; i < max_server_config_id; i++) {
		if (!(g_server_config[i].id)) {
			break;
		}
		TRACE_LOG("id: %u  value: %u", g_server_config[i].id, g_server_config[i].value);
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();

	ADD_TIMER_EVENT(&g_events, init_server_config, 0, get_now_tv()->tv_sec + 300);
	return 0;
}


void init_server_config_timer()
{
  ADD_TIMER_EVENT(&g_events, init_server_config, 0, get_now_tv()->tv_sec + 1);
}


//---------------------------------------------------
//
bool event_trigger_mgr::init(const char* xml)
{
	if(xml == NULL){
		return false;
	}
	final();
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr event_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the event xml file is not exist"));
		return false;
	}
	
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the event xml file content is empty"));
		return false;
	}

	event_node = root->xmlChildrenNode;

	while(event_node)
	{
		event_node_t event_elem;
		event_node_t* pdata = &event_elem;
		
		get_xml_prop_def(pdata->event_id, event_node,  "id", 0);
		get_xml_prop_def(pdata->type, event_node,  "trigger_type", 0);
        
        get_xml_prop_arr_def(pdata->time_arr, event_node, "trigger_time", 0);

        if (pdata->type == 2) {
            for (uint32_t i = max_event_trigger_time_size - 1; i > 0; i--) {
                if (pdata->time_arr[i] != 0) {
                    if (pdata->time_arr[i] <= pdata->time_arr[i - 1]) {
                        ERROR_LOG("the event xml file trigger_type is error");
                        break;
                    }
                    pdata->time_arr[i] -= pdata->time_arr[i - 1];
                }
            }
        }

		if( pdata->event_id == 0){
			throw XmlParseError(std::string("the event xml file event_id is 0"));
			return false;
		}

		xmlNodePtr block_node = event_node->xmlChildrenNode;
		while(block_node)
		{	
			event_block_t data;
			
			get_xml_prop_def(data.stageid, block_node, "stage",  0);
			get_xml_prop_def(data.diff, block_node, "diff",  0);
			get_xml_prop_def(data.odds, block_node, "odds",  0);
		
            pdata->block_vec.push_back(data);
			
			block_node = block_node->next;
		}

        TRACE_LOG("event trigger --> %u", pdata->event_id);
        for (uint32_t i = 0; i < max_event_trigger_time_size; i++) {
            if (pdata->time_arr[i] == 0) break;
            TRACE_LOG("event trigger timer --> %u", pdata->time_arr[i]);
        }
		add_event_trigger(pdata);
		event_node = event_node->next;
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	return true;
}

bool event_trigger_mgr::final()
{
    event_trigger.clear();
	return true;
}

bool event_trigger_mgr::add_event_trigger(event_node_t* pData)
{
	if(pData == NULL)return false;
	std::map<uint32_t, event_node_t>::iterator pItr = event_trigger.find(pData->event_id);
	if(pItr == event_trigger.end()){
        //active_datas_.insert(map<uint32_t,  active_data>::value_type(pData->active_id_, *pData));
        event_trigger[pData->event_id] = *pData;
		return true;
	}
	throw XmlParseError(std::string("add_data"));
	return false;
}


event_node_t* event_trigger_mgr::get_data_by_event_id(uint32_t event_id)
{
	std::map<uint32_t, event_node_t>::iterator pItr = event_trigger.find(event_id);
	if(pItr == event_trigger.end()){
		return NULL;
	}

    return &(pItr->second);
}

event_block_t* event_trigger_mgr::rand_block_event(uint32_t event_id)
{
    event_node_t* pevent = get_data_by_event_id(event_id);
    if (pevent == NULL) return NULL;

    uint32_t value = rand() % 100;
    uint32_t range = 0;
    for (uint32_t i = 0; i < pevent->block_vec.size(); i++) {
        range += pevent->block_vec[i].odds;
        if (value < range) {
            return &(pevent->block_vec[i]);
        }
    }

    return NULL;
}

int event_trigger_handle(void* owner_, void* userdata)
{
    player_t* player = reinterpret_cast<player_t*>(owner_);

    uint32_t eventid = player->trigger_event;//*(reinterpret_cast<uint32_t*>(userdata));
    TRACE_LOG("========== Player [%u] Event [%u] Time slice ==========", player->id, eventid);
    event_node_t* pevent = get_event_trigger_mgr()->get_data_by_event_id(eventid);
    if (pevent) {
        player->trigger_times++;
        event_block_t* pblock = get_event_trigger_mgr()->rand_block_event(eventid);
        if (pblock) {
            player->trigger_block++;
            send_block_info_to_client(player, eventid, pblock);
        } else {
            if (pevent->time_arr[player->trigger_times] != 0) {
                uint32_t timelag = pevent->time_arr[player->trigger_times];
                player->trigger_timer = ADD_TIMER_EVENT(player, event_trigger_handle, 0, get_now_tv()->tv_sec + timelag);
                return 0;
            }
        }
    }
    player->trigger_timer = 0;
    return 0;
}

int do_event_trigger_logic(player_t* p, uint32_t event_id, uint32_t trigger_times, int reason)
{
    event_node_t* p_event_node = get_event_trigger_mgr()->get_data_by_event_id(event_id);
    if (p_event_node) {
        if (p_event_node->time_arr[trigger_times] != 0) {
            uint32_t timelag = p_event_node->time_arr[trigger_times];
            p->trigger_event = event_id;
            //Note: Here for cartoon
            del_event_trigger_timer(p);
            p->trigger_timer = ADD_TIMER_EVENT(p, event_trigger_handle, 0, get_now_tv()->tv_sec + timelag);
        }
        if (p->trigger_block > p->trigger_victory) {
            p->trigger_victory++;
        }
        TRACE_LOG("trigger data [%u] times[%u|%u] [%u %u]", 
            p->id, trigger_times, p_event_node->time_arr[trigger_times], p->trigger_block, p->trigger_victory);
    }
    return 0;
}

int del_event_trigger_timer( player_t *p)
{
    if (p->trigger_timer) {
        REMOVE_TIMER(p->trigger_timer);
        p->trigger_timer = 0;
    }
    return 0;
}

int send_block_info_to_client(player_t* p, uint32_t eventid, event_block_t* pblock)
{
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, eventid, idx);
    pack(pkgbuf, pblock->stageid, idx);
    pack(pkgbuf, pblock->diff, idx);
    init_cli_proto_head(pkgbuf, p, cli_proto_send_block_info, idx);
    return send_to_player(p, pkgbuf, idx, 0);
}

int send_block_result_to_client(player_t* p, uint32_t eventid, uint32_t trigger_times, uint32_t result)
{
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, result, idx);
    pack(pkgbuf, eventid, idx);
    pack(pkgbuf, trigger_times, idx);
    init_cli_proto_head(pkgbuf, p, cli_proto_send_block_result, idx);
    return send_to_player(p, pkgbuf, idx, 0);
}

