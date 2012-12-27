#include "random_event.hpp"
#include "utils.hpp"
#include "mcast_proto.hpp"
#include "cli_proto.hpp"
#include "global_data.hpp"
#include "common_op.hpp"
#include "mail.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>


using namespace taomee;


bool is_same_summon(uint32_t id_1, uint32_t id_2)
{
    return (id_1 / 10) == (id_2 / 10);
}

bool is_random_event_enable(player_t* p, uint32_t swap_id)
{
    //TRACE_LOG(" player ol today [%u]", p->oltoday);
    //if (p->oltoday % 60 > 5) return false; 

    uint32_t swap_times = get_swap_action_times(p, swap_id);
    if (swap_times == 0) return false;

    swap_times = get_swap_action_times(p, 1204);
    if (swap_times >= 4) return false;
    
    uint32_t home_summon_cnt = get_sum_event_mgr()->get_home_summon_list(p);
    if (home_summon_cnt == 0) return false;
    return true;
}

void mail_summon_random_event(player_t* p, event_data_t * p_event)
{
    uint32_t duration = 0, lifetime = 0;
    char title[MAX_MAIL_TITLE_LEN + 1] = {0};
    char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
    std::vector<mail_item_enclosure> mail_item;
    std::vector<mail_equip_enclosure> mail_equips;

    memcpy(title, p_event->name_, sizeof(p_event->name_));
    memcpy(content, p_event->desc_, sizeof(p_event->desc_));

    for (uint32_t i = 0; i < p_event->item_vec.size(); i++) {
        if (p_event->item_vec[i].type == give_type_normal_item) {
            mail_item.push_back(mail_item_enclosure(p_event->item_vec[i].id, p_event->item_vec[i].cnt));
        } else if (p_event->item_vec[i].type == give_type_clothes) {
            const GfItem* itm = items->get_item(p_event->item_vec[i].id);
            duration = itm->duration * clothes_duration_ratio;
            lifetime = itm->lifetime();
            mail_equips.push_back(mail_equip_enclosure(p_event->item_vec[i].id,
                    get_now_tv()->tv_sec, 0, duration, lifetime));
        }

        if (mail_item.size() == 3) {
            db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_sum_event, 0, &mail_item, 0);
            mail_item.clear();
        }

        if (mail_equips.size() == 3) {
            db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_sum_event, 0, 0, &mail_equips);
            mail_equips.clear();
        }
    }

    db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_sum_event, 0, 
        mail_item.size() ? &mail_item : 0, mail_equips.size() ? &mail_equips : 0);
    mail_item.clear();
    mail_equips.clear();
}

void broadcast_unusual_event(player_t* p, event_data_t * p_event)
{
    if (p_event->is_bcast_ == 0) return;

    char words[max_trade_mcast_size] = {0};
    int len = snprintf(words, max_trade_mcast_size, "%s:", char_content[11].content);
    for (uint32_t i = 0; i < p_event->item_vec.size(); i++) {
        len += snprintf(words + len, max_trade_mcast_size, " %s", p_event->item_vec[i].name);
        if (len + sizeof(p_event->item_vec[i].name) >= max_trade_mcast_size) break;
    }

    TRACE_LOG("BROADCAST: %s", words);

    player_mcast_t info = {0};
    memset(&info, 0x00, sizeof(player_mcast_t));
    info.type = 2; //:player_market 2:player_world
    info.front_id = 2;
    memcpy(&(info.nick), char_content[12].content, sizeof(info.nick));
    memcpy(&(info.info), words, max_trade_mcast_size);

    notify_player_mcast_info(p, mcast_trade_message, &info);
}

/**
 * @brief only for example
 */
void broadcast_unusual_event_to_world(player_t* p, event_data_t * p_event)
{
    if (p_event->is_bcast_ == 0) return;

    char words[max_trade_mcast_size] = {0};
    int len = snprintf(words, max_trade_mcast_size, "(%s)%s:", p->nick, char_content[11].content);
    for (uint32_t i = 0; i < p_event->item_vec.size(); i++) {
        len += snprintf(words + len, max_trade_mcast_size, " %s", p_event->item_vec[i].name);
        if (len + sizeof(p_event->item_vec[i].name) >= max_trade_mcast_size) break;
    }

    notify_official_mcast_info(p->id, mcast_world_notice, words);
}

int do_random_event_logic(void* owner_, void* userdata)
{
    player_t* player = reinterpret_cast<player_t*>(owner_);
    TRACE_LOG("========== Player [%u] Time slice ==========", player->id);
    if (!is_random_event_enable(player, 1202)) {
        ADD_TIMER_EVENT(player, do_random_event_logic, 0, get_now_tv()->tv_sec + 600);
        return 0;
    }

    TRACE_LOG(" random logic uid=[%d]", player->id);
    //random
    event_data_t * event_obj = get_sum_event_mgr()->list_for_each_summon(player);
    if (event_obj != NULL) {
        //mail
        mail_summon_random_event(player, event_obj);

        add_swap_action_times(player, 1204);
        //broadcast
        broadcast_unusual_event(player, event_obj);
        //broadcast_unusual_event_to_world(player, event_obj);
    } else {
        TRACE_LOG(" No event !!!!!!!!");
    }
    ADD_TIMER_EVENT(player, do_random_event_logic, 0, get_now_tv()->tv_sec + 600);
    return 0;
}


void process_random_event()
{
    uint32_t cur_time = get_now_tv()->tv_sec;
    static uint32_t last_random_tm = 0;
    if (last_random_tm == 0) {
        last_random_tm = cur_time;
    }

    if (cur_time > last_random_tm && cur_time <= last_random_tm + 30) {
        return;
    } else {
        last_random_tm = cur_time;
    }

    TRACE_LOG("========== Time slice list all player ==========");
    //traverse_players(do_random_event_logic, 0);
}

void init_player_random_event(player_t* p)
{
    ADD_TIMER_EVENT(p, do_random_event_logic, 0, get_now_tv()->tv_sec + (600 - p->oltoday % 600)+60);
}

event_data_t * sum_event_mgr::list_for_each_summon(player_t* p)
{
    std::list<sum_odds_data_t>::iterator it = odds_list_.begin();
    for (; it != odds_list_.end(); ++it) {
        TRACE_LOG(" summon list summon type[%u]", it->sum_type);
        for ( std::vector<uint32_t>::iterator v_it = sum_vec_.begin(); v_it != sum_vec_.end(); ) {
            TRACE_LOG(" %u==%u", it->sum_type, *v_it);
            if (is_same_summon(it->sum_type, *v_it)) {
                v_it = sum_vec_.erase(v_it);
                uint32_t event_id = get_random_event_id(&(*it));
                TRACE_LOG(" random event id = [%u]", event_id);
                if (is_valid_event(p, event_id)) {
                    memcpy(summon_name, it->sum_name, max_nick_size);
                    return get_event_data(event_id);
                }
            } else {
                ++v_it;
            }
        }
    }
    return NULL;
}

bool sum_event_mgr::is_valid_event(player_t* p, uint32_t event_id)
{
    if (event_id == 0) return false;
    event_data_t * event_obj = get_event_data(event_id);
    if (p->lv < event_obj->min_lv_ || p->lv > event_obj->max_lv_) {
        return false;
    }

    if (event_obj->is_vip_ && !is_vip_player(p)) {
        return false;
    }

    return true;
}

uint32_t sum_event_mgr::get_random_event_id(sum_odds_data_t * odds_data)
{
    uint32_t odd_num = rand() % 1000;
#ifndef RELEASE_VER
    odd_num = odd_num / 2;
#endif
    uint32_t odds = 0;
    for (uint32_t i = 0; i < odds_data->odds_vec.size(); i++) {
        odds += odds_data->odds_vec[i].odds;
        if ( odd_num <= odds) {
            return odds_data->odds_vec[i].id;
        }
    }
    return 0;
}

bool sum_event_mgr::load_random_event(xmlNodePtr cur)
{
    while(cur) {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("event"))) {
            event_data_t event_elem;

            get_xml_prop_def(event_elem.event_id_, cur, "id",   0);
            get_xml_prop_def(event_elem.min_lv_, cur,   "min_lv",   0);
            get_xml_prop_def(event_elem.max_lv_, cur,   "max_lv",   0);
            get_xml_prop_def(event_elem.is_vip_, cur,   "is_vip",   0);
            get_xml_prop_def(event_elem.is_bcast_, cur, "broadcast",    0);

            get_xml_prop_raw_str_def(event_elem.name_, cur, "name", "NULL");
            get_xml_prop_raw_str_def(event_elem.desc_, cur, "desc", "NULL");

            if( event_elem.event_id_ == 0){
                throw XmlParseError(std::string("the random_event xml file event_id is 0"));
                return false;
            }
            xmlNodePtr event_node = cur->xmlChildrenNode;
            reward_item item = {0};
            while(event_node) {
                memset(&item, 0x00, sizeof(reward_item));
                if (!xmlStrcmp(event_node->name, reinterpret_cast<const xmlChar*>("item"))) {
                    get_xml_prop_def(item.type, event_node,  "give_type", 0);
                    get_xml_prop_def(item.id,   event_node,  "give_id", 0);
                    get_xml_prop_def(item.cnt,  event_node,  "count", 0);
                    get_xml_prop_raw_str_def(item.name, event_node, "name", "NULL");

                    event_elem.item_vec.push_back(item);
                }
                event_node = event_node->next;
            }

            TRACE_LOG(" %u %u %u", event_elem.event_id_, event_elem.is_vip_, event_elem.is_bcast_);
            add_event_to_map(&event_elem);
        } // end if event
        cur = cur->next;
    } // end while
    return true;
}

bool sum_event_mgr::load_random_summon(xmlNodePtr cur)
{
    while(cur) {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("limit"))) {
            sum_odds_data_t odds_elem;

            get_xml_prop_def(odds_elem.sum_type, cur,  "summon_id", 0);
            get_xml_prop_def(odds_elem.sum_prior, cur,  "priority", 0);

            get_xml_prop_raw_str_def(odds_elem.sum_name, cur, "summon_name", "NULL");

            if( odds_elem.sum_type == 0){
                throw XmlParseError(std::string("the random_event xml file sum_type is 0"));
                return false;
            }
            xmlNodePtr event_node = cur->xmlChildrenNode;
            event_odds_t odds = {0};
            while(event_node) {
                memset(&odds, 0x00, sizeof(event_odds_t));
                if (!xmlStrcmp(event_node->name, reinterpret_cast<const xmlChar*>("event"))) {
                    get_xml_prop_def(odds.id,   event_node, "id",   0);
                    get_xml_prop_def(odds.odds, event_node, "odds", 0);

                    odds_elem.odds_vec.push_back(odds);
                }
                event_node = event_node->next;
            }

            TRACE_LOG(" %u %u", odds_elem.sum_type, odds_elem.sum_prior);
            add_odds_data_to_list(&odds_elem);
        } // end if limit
        cur = cur->next;
    } // end while

    TRACE_LOG("========= Dump SUMMON LIST ====================");
    std::list<sum_odds_data_t>::iterator it = odds_list_.begin();
    for (; it != odds_list_.end(); ++it) {
        TRACE_LOG("random summon info [%u %u]", it->sum_type, it->sum_prior);
    }
    return true;
}

bool sum_event_mgr::init(const char* xml)
{
	if(xml == NULL){
		return false;
	}
	final();
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr cur = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the random_event xml file is not exist"));
		return false;
	}
	
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the random_event xml file content is empty"));
		return false;
	}

	cur = root->xmlChildrenNode;

	while(cur) {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Event"))) {
            if (load_random_event(cur->xmlChildrenNode) == -1) {
                ERROR_LOG("config random_event event error!!!!!!!!");
                throw XmlParseError(std::string("config event error in random_event.xml"));
                return -1;
            }
        }
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Summon"))) {
            if (load_random_summon(cur->xmlChildrenNode) == -1) {
                ERROR_LOG("config random_event summon error!!!!!!!!");
                throw XmlParseError(std::string("config summon error in random_event.xml"));
                return -1;
            }
        }
        cur = cur->next;
    }

	xmlFreeDoc(doc);
	xmlCleanupParser();
	return true;
}

bool sum_event_mgr::final()
{
    event_map_.clear();
    odds_list_.clear();
	return true;
}


//-----------------------------------------------------
//
//-----------------------------------------------------
void mail_random_reward(player_t* p, uint32_t userid, uint32_t roletm, uint32_t reward_id)
{
    char title[MAX_MAIL_TITLE_LEN + 1] = {0};
    char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
    std::vector<mail_item_enclosure> mail_item;

    memcpy(title, char_content[25].content, MAX_MAIL_TITLE_LEN);
    memcpy(content, char_content[26].content, MAX_MAIL_CONTENT_LEN);
/*
    uint32_t item_id = 0;
    if (reward_id == 1) item_id = 2500011;
    else if (reward_id == 2) item_id = 2500009;
    else if (reward_id == 4) item_id = 2500010;
    else if (reward_id == 5) item_id = 1500564;
    else if (reward_id == 6) item_id = 1500567;
    else if (reward_id == 7) item_id = 2400002;

    if (item_id) {
        mail_item.push_back(mail_item_enclosure(item_id, 1));
    }
  */

    db_send_system_mail(p, title, sizeof(title), content, sizeof(content), 999, 0, 
        mail_item.size() ? &mail_item : 0, 0);
    mail_item.clear();
}

/**
 * @brief only for example
 */
void broadcast_random_reward_to_world(player_t* p, uint32_t userid, uint32_t roletm, uint32_t reward_id)
{
    char words[max_trade_mcast_size] = {0};
    snprintf(words, max_trade_mcast_size, "(%s)%s%s:%s", p->nick, char_content[3].content, char_content[25].content, char_content[27].content);

    notify_official_mcast_info(p->id, mcast_world_notice, words);
}

void save_data_to_db(uint32_t userid, uint32_t roletm, uint32_t reward_id)
{
    int idx = 0;
    pack_h(dbpkgbuf, userid, idx);
    pack_h(dbpkgbuf, roletm, idx);
    pack_h(dbpkgbuf, reward_id, idx);
    send_request_to_db(0, userid, roletm, dbproto_save_random_reward, dbpkgbuf, idx);
}

int send_reward_player_rsp(player_t* p)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, g_reward_player_cache.cnt, idx);
	for (uint32_t i = 0; i < g_reward_player_cache.cnt; i++) {
		pack(pkgbuf, g_reward_player_cache.elem[i].id, idx);
		pack(pkgbuf, g_reward_player_cache.elem[i].uid, idx);
		pack(pkgbuf, g_reward_player_cache.elem[i].role_regtime, idx);
		pack(pkgbuf, g_reward_player_cache.elem[i].reward_id, idx);
		pack(pkgbuf, g_reward_player_cache.elem[i].reward_tm, idx);
		pack(pkgbuf, g_reward_player_cache.elem[i].reward_flag, idx);

    }
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int get_reward_player_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    uint32_t cur_time = get_now_tv()->tv_sec;
    if (cur_time < g_reward_player_cache.timestamp + 60) {
        send_reward_player_rsp(p);
    }

	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_reward_player_list, 0, 0);	
}

int db_get_reward_player_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    reward_player_cache_t* rsp = reinterpret_cast<reward_player_cache_t*>(body);

	KDEBUG_LOG(p->id, "GET reward player list\t[%u]", rsp->cnt);

	g_reward_player_cache.cnt = rsp->cnt;
    g_reward_player_cache.timestamp = get_now_tv()->tv_sec;
    
	for (uint32_t i = 0; i < rsp->cnt; i++) {
        g_reward_player_cache.elem[i].id = rsp->elem[i].id;
        g_reward_player_cache.elem[i].uid = rsp->elem[i].uid;
        g_reward_player_cache.elem[i].role_regtime = rsp->elem[i].role_regtime;
        g_reward_player_cache.elem[i].reward_id = rsp->elem[i].reward_id;
        g_reward_player_cache.elem[i].reward_tm = rsp->elem[i].reward_tm;
        g_reward_player_cache.elem[i].reward_flag = rsp->elem[i].reward_flag;
    }

    return send_reward_player_rsp(p);
}

int db_set_player_reward_flag(player_t* p, uint32_t key)
{
    uint32_t i = get_reward_player_rank(p, key);
    g_reward_player_cache.elem[i - 1].reward_flag = 1;

    int idx = 0;
    pack_h(dbpkgbuf, key, idx);
    return send_request_to_db(0, p->id, p->role_tm, dbproto_set_player_reward_flag, dbpkgbuf, idx);
}

uint32_t get_reward_player_rank(player_t* p, uint32_t key)
{
    uint32_t ret = 0;
    for (uint32_t i = 0; i < g_reward_player_cache.cnt; i++) {
        if (g_reward_player_cache.elem[i].uid == p->id
            && g_reward_player_cache.elem[i].role_regtime == p->role_tm
            && g_reward_player_cache.elem[i].id == key) {
            ret = i + 1;
            break;
        }
    }
    return ret;
}

bool is_player_can_get_reward(player_t* p, uint32_t key)
{
    uint32_t rank = get_reward_player_rank(p, key);

    if (rank > 0 && rank < 101) {
        if (g_reward_player_cache.elem[rank - 1].reward_flag == 0) {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------
