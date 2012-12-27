/**
 *
 *
 *
 *
 */

#include <libtaomee++/inet/pdumanip.hpp>
using namespace taomee;

extern "C" {
#include <libtaomee/project/utilities.h>
//#include <async_serv/mcast.h>
}

#include "player.hpp"
#include "utils.hpp"
#include "cli_proto.hpp"
#include "mcast_proto.hpp"
#include "title_system.hpp"
#include "kill_boss.hpp"
#include "global_data.hpp"
#include "common_op.hpp"
#include "player_attribute.hpp"


//global v
//title_condition_t g_title_condition;



/**
 * @brief as universal interface for platform of statstics
 */
void do_stat_log_title(uint32_t cmd, uint32_t id, uint32_t cnt1, uint32_t cnt2, uint32_t cnt3)
{
    uint32_t buf[3] = {0};
    buf[0] = cnt1;
	buf[1] = cnt2;
	buf[2] = cnt3;
    msglog(statistic_logfile, cmd + id, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [id=%d] [cnt: %d %d %d]", cmd, id, cnt1, cnt2, cnt3);
}

/**
 * @return the count of get title
 */
int CTitle::calc_title(player_t* p, uint8_t type)
{
    TitleMap::iterator it;
    TRACE_LOG("calc title UID=[%u] type=[%u]", p->id, type);
    switch (type) {
    case pve_title:
        return is_achieve_pve_title(p, type);
        break;
    case pvp_title:
        break;
    case enum_achieve_title:
        return is_finish_achieve_title(p, type);
        break;
    default:
        ERROR_LOG("CALC TITLE OUTOF ENUM");
        break;
    }
    return 0;
}

/**
 * @brief traverse g_title_condition and save last_title
 * @return the count of get title
 */
int CTitle::is_achieve_pve_title(player_t *p, uint8_t type) 
{
    if ( type != pve_title) {
        return 0;
    }
    last_title.clear();
    int title_cnt = 0;
    title_pve_condition_map::iterator g_it = g_title_condition.pve_map.begin();
    for (; g_it != g_title_condition.pve_map.end(); ++g_it) {
        //If player have title cotinue next
        if ( is_get_title(g_it->first) ) {
            continue;
        }
    
        TRACE_LOG("title --[%u %u]", g_it->first, g_it->second.achieve_id);
        //Here: for BADGE
        if ( g_it->second.achieve_id != 0 ) {
            if (is_player_achievement_data_exist(p, g_it->second.achieve_id)) {
                last_title.push_back(g_it->first);
                title_cnt++;
                KDEBUG_LOG(p->id, "GET TITLE BY ACHIEVE\t [%u %u] [%u] cnt=[%u]",pve_title, g_it->first, 
                    g_it->second.achieve_id, title_cnt);
            }
            continue;
        }

        uint32_t j = 0;
        for ( ; j < g_it->second.stageid.size(); j++) {
            uint32_t stageid = g_it->second.stageid[j];
            uint32_t difficulty = g_it->second.difficulty[j];
            uint32_t score = g_it->second.score[j];
            //if ( !is_player_finish_stage(p, stageid, difficulty)) {
            //    break;
            //}

            if ( !is_player_get_stage_score(p, stageid, difficulty, score) ) {
                break;
            }
        }
        if ( j == g_it->second.stageid.size() ) {
            uint32_t title_id = g_it->first;
            last_title.push_back(title_id);
            title_cnt++;
            KDEBUG_LOG(p->id, "GET PVE TITLE\t [%u %u] cnt=[%u]",pve_title, title_id, title_cnt);
        }

    }
    return title_cnt;
}

int CTitle::is_finish_achieve_title(player_t *p, uint8_t type)
{
    last_title.clear();
    int title_cnt = 0;
    title_achieve_map::iterator g_it = g_title_condition.achieve_map.begin();
    for (; g_it != g_title_condition.achieve_map.end(); ++g_it) {
        if ( is_get_title(g_it->first) ) {
            continue;
        }

        TRACE_LOG("title --[%u %u]", g_it->first, g_it->second.achieveid[0]);
        uint32_t j = 0;
        for (; j < g_it->second.achieveid.size(); j++) {
            if (!is_player_achievement_data_exist(p, g_it->second.achieveid[j])) {
                break;
            }
        }
        if (j == g_it->second.achieveid.size()) {
            last_title.push_back(g_it->first);
            title_cnt++;
            KDEBUG_LOG(p->id, "GET TITLE BY ACHIEVE\t[%u %u] cnt=[%u]",g_it->first,g_it->second.achieveid[0], title_cnt);
        }
    }
    return title_cnt;
}
/**
 * @brief get achieve title list
 */
int db_get_achieve_title_list(player_t* p)
{
    return send_request_to_db(p, p->id, p->role_tm, dbproto_get_achieve_title, 0, 0);
}
/**
  * @brief player performs periodical action : get achieve title
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_achieve_title_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    return db_get_achieve_title_list(p);
}

/**
  * @brief player performs periodical action : set player achieve title
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int set_player_achieve_title_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t title_id = 0;
    unpack(body, title_id, idx);

    if (title_id != 0) {
        //check title id available
        title_pve_condition_map::iterator g_pve_it = g_title_condition.pve_map.find(title_id);
        title_achieve_map::iterator g_ac_it = g_title_condition.achieve_map.find(title_id);
		title_other_map::iterator g_other_it = g_title_condition.other_map.find(title_id);
        if (g_pve_it == g_title_condition.pve_map.end() &&
            g_ac_it == g_title_condition.achieve_map.end() &&
            g_other_it == g_title_condition.other_map.end()) {
            return send_header_to_player(p, p->waitcmd, cli_err_achieve_title_not_exist, 1);
        }
        //check player is owner
        title_info_t* ptitle = p->pTitle->get_title(title_id);
        if (ptitle == NULL) {
            return send_header_to_player(p, p->waitcmd, cli_err_achieve_title_not_get, 1);
        }
    }
    idx = 0;
    pack_h(dbpkgbuf, title_id, idx);
    return send_request_to_db(p, p->id, p->role_tm, dbproto_set_player_achieve_title, dbpkgbuf, idx);
}

/**
 * @brief  db set achieve title
 * @param p the requester
 * @param type :1 pve; 2 pvp
 */
int db_set_achieve_title(player_t* p, uint8_t type, uint32_t tm)
{
    int idx = 0;
    uint32_t title_cnt = p->pTitle->last_title.size();
    TRACE_LOG("achieve title count ----- [%u]", title_cnt);
    pack_h(dbpkgbuf, type, idx);
    pack_h(dbpkgbuf, tm, idx);
    pack_h(dbpkgbuf, title_cnt, idx);
    for (uint32_t i = 0; i < title_cnt; i++){
        pack_h(dbpkgbuf, p->pTitle->last_title[i], idx);

        do_stat_log_title(stat_log_title_base_cmd, p->pTitle->last_title[i], 1, 0, 0);
        TRACE_LOG("achieve title [%u %u]", p->pTitle->last_title[i], tm);
    }
    p->pTitle->last_title.clear();
    return send_request_to_db(0, p->id, p->role_tm, dbproto_notify_achieve_title, dbpkgbuf, idx);
}

/**
  * @brief callback for handling adding periodical action returned from dbproxy
  * @param p the requester
  * @param uid id of the requester
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_get_achieve_title_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    uint32_t title_cnt = 0;
    int idx = 0;
    unpack_h(body, title_cnt, idx);
    CHECK_VAL_EQ(bodylen, title_cnt * sizeof(title_info_t) + 4);

    idx = sizeof(cli_proto_t);
    pack(pkgbuf, title_cnt, idx);
    title_info_t* title = reinterpret_cast<title_info_t*>((uint8_t*)(body) + 4);
    for (uint32_t i = 0; i < title_cnt; i++) {
        p->pTitle->save_title(title[i].type, title[i].subid, title[i].gettime);
        pack(pkgbuf, title[i].type, idx);
        pack(pkgbuf, title[i].subid, idx);
        pack(pkgbuf, title[i].gettime, idx);
        TRACE_LOG("achieve title callback [%u %u %u]", title[i].type, title[i].subid, title[i].gettime);
    }
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}
/**
  * @brief callback for handling adding periodical action returned from dbproxy
  * @param p the requester
  * @param uid id of the requester
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_set_player_achieve_title_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    uint32_t title_id = 0;
    int idx = 0;
    unpack_h(body, title_id, idx);
    TRACE_LOG("set player [%u] using achieve title[%u]", p->id, title_id);

    do_stat_log_title(stat_log_title_base_cmd, title_id, 0, 1, 0);
    do_stat_log_title(stat_log_title_base_cmd, p->using_achieve_title, 0, 0, 1);
    p->using_achieve_title = title_id;

    //calc_player_attr_ex(p);
    calc_and_save_player_attribute(p);
    
    idx = sizeof(cli_proto_t);
    pack(pkgbuf, id, idx);
    pack(pkgbuf, p->role_tm, idx);
    pack(pkgbuf, title_id, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    if (p->trade_grp) {
        send_to_player(p, pkgbuf, idx, 1);
    } else {
        send_to_map(p, pkgbuf, idx, 1);
    }
    return 0;
}

/**
 * @brief send achieve title to self
 */
int send_achieve_title_to_player(player_t* p)
{
    int idx = sizeof(cli_proto_t);
    CTitle* ptitle = p->pTitle;
    uint32_t title_cnt = ptitle->last_title.size();
    pack(pkgbuf, title_cnt, idx);
    for (uint32_t i = 0; i < title_cnt; i++) {
        pack(pkgbuf, ptitle->last_title[i], idx);
    }
    init_cli_proto_head(pkgbuf, p, cli_proto_notify_achieve_title, idx);
    return send_to_player(p, pkgbuf, idx, 0);
}
/**
 * @brief send single achieve title to player
 */
int send_one_achieve_title_to_player(player_t* p, uint32_t id, uint8_t type, uint32_t gettime)
{
    int idx = sizeof(cli_proto_t);
    pack(dbpkgbuf, type, idx);
    pack(dbpkgbuf, id, idx);
    pack(dbpkgbuf, gettime, idx);
    init_cli_proto_head(dbpkgbuf, p, cli_proto_notify_achieve_title, idx);
    return send_to_player(p, dbpkgbuf, idx, 0);
}

/**
 * @send player achieve title to world by title.xml
 */
int send_player_achieve_title_to_world(player_t* p, uint32_t titleid, char *name)
{
    

    char words[max_trade_mcast_size] = {0};
    int len = sprintf(words, "%s %s", g_title_condition.pve_note1, name);
    TRACE_LOG("mcast title msg[%s][%d]",words,len);

    player_mcast_t info = {0};
    memset(&info, 0x00, sizeof(player_mcast_t));
    info.type = 2; //:player_market 2:player_world
    info.front_id = 2;
    memcpy(&(info.nick), g_title_condition.pve_note2, sizeof(info.nick));
    memcpy(&(info.info), words, max_trade_mcast_size);

    notify_player_mcast_info(p, mcast_trade_message, &info);
/*
    int idx = sizeof(mcast_pkg_t);
    pack(pkgbuf, static_cast<uint8_t>(2), idx);//:player_market 2:player_world 
    pack(pkgbuf, "(系统)", max_nick_size, idx);
    pack(pkgbuf, static_cast<uint32_t>(2), idx);//front_id   
    pack(pkgbuf, words, max_trade_mcast_size, idx);
    init_cli_proto_head(pkgbuf, 0, cli_proto_trade_mcast_detail_info, idx);
    send_to_all(pkgbuf, idx);


    //send to other online
    idx = sizeof(mcast_pkg_t);
    pack(pkgbuf, static_cast<uint8_t>(2), idx);//:player_market 2:player_world
    pack(pkgbuf, "(系统)", max_nick_size, idx);
    pack(pkgbuf, static_cast<uint32_t>(2), idx);//front_id
    pack(pkgbuf, words, max_trade_mcast_size, idx);
    init_mcast_pkg_head(pkgbuf, mcast_trade_message, 0);
    send_mcast_pkg(pkgbuf, idx);
*/    return 0;
}
bool get_title_name(uint32_t titleid, uint8_t title_type, char* name)
{
    title_pve_condition_map::iterator g_pve_it;
    title_achieve_map::iterator g_ac_it;
	title_other_map::iterator g_other_it;
    switch (title_type) {
        case pve_title:
            g_pve_it = g_title_condition.pve_map.find(titleid);;
            if ( g_pve_it ==  g_title_condition.pve_map.end() ) {
                ERROR_LOG("WRONG in title.xml id=[%u]", titleid);
                //send_header_to_player(p, p->waitcmd, cli_err_achieve_title_not_exist, 0);
                return false;
            }
            memcpy(name, g_pve_it->second.title_name, sizeof(g_pve_it->second.title_name));
            break;
        case enum_achieve_title:
            g_ac_it = g_title_condition.achieve_map.find(titleid);;
            if ( g_ac_it ==  g_title_condition.achieve_map.end() ) {
                ERROR_LOG("WRONG in title.xml id=[%u]", titleid);
                //send_header_to_player(p, p->waitcmd, cli_err_achieve_title_not_exist, 0);
                return false;
            }
            memcpy(name, g_ac_it->second.title_name, sizeof(g_ac_it->second.title_name));
            break;
		case other_title:
			g_other_it = g_title_condition.other_map.find(titleid);;
            if ( g_other_it ==  g_title_condition.other_map.end() ) {
                ERROR_LOG("WRONG in title.xml id=[%u]", titleid);
                //send_header_to_player(p, p->waitcmd, cli_err_achieve_title_not_exist, 0);
                return false;
            }
            memcpy(name, g_other_it->second.title_name, sizeof(g_other_it->second.title_name));
            break;
        default:
            break;
    }
    return true;
}
/**
 * @brief notify player get achieve title
 */
int notify_player_get_achieve_title(player_t* p, uint8_t type, uint32_t gettime)
{
    p->pTitle->save_last_title(type, gettime);

    TRACE_LOG("SET title successfully, send to player");
    CTitle* p_title = p->pTitle;
    uint32_t title_id = 0, is_world = 0;
    title_pve_condition_map::iterator g_pve_it;
    title_achieve_map::iterator g_ac_it;
    title_other_map::iterator g_o_it;

    std::vector<uint32_t>::iterator v_it = p_title->last_title.begin();
    while (v_it != p_title->last_title.end()) {
        switch (type) {
            case pve_title:
                g_pve_it = g_title_condition.pve_map.find(*v_it);
                title_id = g_pve_it->first;
                is_world = g_pve_it->second.world;
                break;
            case enum_achieve_title:
                g_ac_it = g_title_condition.achieve_map.find(*v_it);
                title_id = g_ac_it->first;
                is_world = g_ac_it->second.world;
                break;
            case other_title:
                g_o_it = g_title_condition.other_map.find(*v_it);
                title_id = g_o_it->first;
                is_world = g_o_it->second.world;
                break;
            default:
                break;
        }
        send_one_achieve_title_to_player(p, title_id, type, gettime);

        if ( is_world == 1) {
            char name[max_title_len] = {0};
            if (get_title_name(title_id, type, name))
                send_player_achieve_title_to_world(p, title_id, name);
        }
        v_it++;
    }
    return 0;
}

int do_special_title_logic(player_t* p, uint32_t id, bool is_bcast)
{
    if (p->pTitle->is_get_title(id)) {
        return 0;
    }
    p->pTitle->last_title.push_back(id);
    uint32_t cur_time = time(NULL);
    if (is_bcast) {
        notify_player_get_achieve_title(p, other_title, cur_time);
    }
    if (db_set_achieve_title(p, other_title, cur_time) != 0) {
        ERROR_LOG("save UID=[%u] CNT=[%u] other title into db error.", p->id, id);
    }
    return 0;
}
/**
 * @brief Interface of add titile to player
 */
int add_title_interface(player_t* p, uint8_t type)
{
    int title_cnt = p->pTitle->calc_title(p, type);
    if (title_cnt > 0) {
        uint32_t cur_time = time(NULL);
        notify_player_get_achieve_title(p, type, cur_time);
        if (db_set_achieve_title(p, type, cur_time) != 0) {
            ERROR_LOG("save UID=[%u] CNT=[%u] achieve title into db error.", p->id, title_cnt);
        }
    }
    return 0;
}
/**
 * @brief convert str to uint32_t for title.xml
 * @example: "10|12-15" => "10 12 13 14 15"
 */
int my_str_to_uint32(char* srcstr, uint32_t* array)
{
    char* p = srcstr;
    if ( !is_digit(*p)) {
        return -1;
    }
    uint32_t tmp = 0;
    int idx = 0;
    while (*p != '\0') {
        while ( is_digit(*p) ) {
            tmp = tmp * 10 + (*p++ - 0x30);
        }
        array[idx] = tmp;
        tmp = 0;
        if (*p == '|') {
            p++;
            idx++;
        }

        if (*p == '-') {
            p++;
            while ( is_digit(*p) ) {
                tmp = tmp * 10 + (*p++ - 0x30);
            }
            uint32_t dig = array[idx];
            while (dig < tmp) {
                array[++idx] = ++dig;
            }
        }
    }
    return (idx + 1);
}

/**
 * @brief load pve title condition
 */
int load_pve_title(xmlNodePtr cur)
{
    int j = 0;
    uint32_t id = 0, world = 0;
    while (cur) {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Title"))) {
            get_xml_prop(id, cur, "ID");
            title_pve_condition_map::iterator g_it = g_title_condition.pve_map.find(id);
            if ( g_it != g_title_condition.pve_map.end() ) {
                ERROR_LOG("Duplicate title id = [%u]", id);
                return -1;
            }

            title_pve_condition_t tmp_t;
            get_xml_prop_raw_str_def(tmp_t.title_name, cur, "Name", "小侠士");
            get_xml_prop_def(world, cur, "World", 0);
            tmp_t.world = (uint8_t)(world);

            get_xml_prop_def(tmp_t.achieve_id, cur, "AchieveId", 0);

            uint32_t stage_arr[max_title_stage_num] = {0};
            int stage_num = 0;
#define STR_TO_UINT32 1
#if STR_TO_UINT32
            char id_str[max_title_stage_num] = {0};
            get_xml_prop_raw_str_def(id_str, cur, "StageId", "1");
            stage_num = my_str_to_uint32(id_str, stage_arr);
            if (stage_num < 1) {
                ERROR_LOG("Invalid StageId ID=%u", id);
                return -1;
            }
#else
            stage_num = get_xml_prop_arr_def(stage_arr, cur, "StageId", 0);
            if (stage_num < 1) {
                ERROR_LOG("Invalid StageId ID=!", id);
                return -1;
            }
#endif
            for (int i = 0; i < stage_num; i++) {
                tmp_t.stageid.push_back(stage_arr[i]);
            }

            uint32_t difficulty_arr[max_title_stage_num] = {0};
            memset(difficulty_arr, 1, max_title_stage_num);
            int difficulty_num = get_xml_prop_arr_def(difficulty_arr, cur, "StageDifficulty", 1);
            for (int i = 0; i < stage_num; i++) {
                if (difficulty_num == 0 || i < difficulty_num) {
                    tmp_t.difficulty.push_back( (uint8_t)(difficulty_arr[i]));
                } else {
                    tmp_t.difficulty.push_back( (uint8_t)(difficulty_arr[difficulty_num - 1]));
                }
            }

            uint32_t score_arr[max_title_stage_num] = {0};
            memset(score_arr, 6, max_title_stage_num);
            int score_num = get_xml_prop_arr_def(score_arr, cur, "StageScore", 6);
            for (int i = 0; i < stage_num; i++) {
                if (score_num == 0 || i < score_num) {
                    tmp_t.score.push_back((uint8_t)(score_arr[i]));
                } else {
                    //int index = score_num == 0 ? 0:(score_num - 1);
                    tmp_t.score.push_back((uint8_t)(score_arr[score_num - 1]));
                }
            }
            g_title_condition.pve_map.insert(title_pve_condition_map::value_type(id, tmp_t));
            //g_title_condition.pve_vector.push_back(tmp_t);

            /********************** dump pve title condition **********************************/
/*            TRACE_LOG("DUMP===================== title id=[%u] world=[%u]", id, tmp_t.world);
            for (int i = 0; i < stage_num; i++) {
                TRACE_LOG("title condition [%u %u %u]", tmp_t.stageid[i], tmp_t.difficulty[i], tmp_t.score[i]);
            }
*/
            /*
            TRACE_LOG("DUMP===================== title id = [%u]", g_title_condition.pve_vector[j].id);
            for (int i = 0; i < stage_num; i++) {
                TRACE_LOG("title condition [%u %u %u]", g_title_condition.pve_vector[j].stageid[i],
                    g_title_condition.pve_vector[j].difficulty[i], g_title_condition.pve_vector[j].score[i]);
            }*/
            j++;

        }
        cur = cur->next;
    }
    //====================== Dump pve title condition MAP ===========================
    TRACE_LOG("========================= [BEGIN DUMP] ===============================");
    title_pve_condition_map::iterator g_it = g_title_condition.pve_map.begin();
    for (; g_it != g_title_condition.pve_map.end(); ++g_it) {
        TRACE_LOG("DUMP=========================================== title id = [%u]", g_it->first);
        for (uint32_t i = 0; i < g_it->second.stageid.size(); i++) {
            TRACE_LOG("title condition [%u %u %u]", g_it->second.stageid[i], 
                g_it->second.difficulty[i], g_it->second.score[i]);
        }
    }
    TRACE_LOG("========================= [END DUMP] =================================");
    if (j == 0) {
        return -1;
    }
    return 0;
}
/**
 * @brief load achieve title condition
 */
int load_achieve_title(xmlNodePtr cur)
{
    //int j = 0;
    uint32_t id = 0, world = 0;
    while (cur) {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Title"))) {
            get_xml_prop(id, cur, "ID");
            title_achieve_map::iterator g_it = g_title_condition.achieve_map.find(id);
            if ( g_it != g_title_condition.achieve_map.end() ) {
                ERROR_LOG("Duplicate title id = [%u]", id);
                return -1;
            }

            title_achieve_t tmp_t;
            get_xml_prop_raw_str_def(tmp_t.title_name, cur, "Name", "小侠士");
            get_xml_prop_def(world, cur, "World", 0);
            tmp_t.world = (uint8_t)(world);

            uint32_t achieve_arr[max_title_stage_num] = {0};
            int achieve_num = 0;
#define STR_TO_UINT32 1
#if STR_TO_UINT32
            char id_str[max_title_stage_num] = {0};
            get_xml_prop_raw_str_def(id_str, cur, "AchieveId", "0");
            achieve_num = my_str_to_uint32(id_str, achieve_arr);
            if (achieve_num < 1) {
                ERROR_LOG("Invalid AchieveId ID=%u", id);
                return -1;
            }
#else
            achieve_num = get_xml_prop_arr_def(achieve_arr, cur, "AchieveId", 0);
            if (achieve_num < 1) {
                ERROR_LOG("Invalid AchieveId ID=!", id);
                return -1;
            }
#endif
            for (int i = 0; i < achieve_num; i++) {
                tmp_t.achieveid.push_back(achieve_arr[i]);
            }
            g_title_condition.achieve_map.insert(title_achieve_map::value_type(id, tmp_t));
            //j++;

        }
        cur = cur->next;
    }
    //====================== Dump pve title condition MAP ===========================
    TRACE_LOG("========================= [BEGIN DUMP] ===============================");
    title_achieve_map::iterator g_it = g_title_condition.achieve_map.begin();
    for (; g_it != g_title_condition.achieve_map.end(); ++g_it) {
        TRACE_LOG("DUMP=========================================== title id = [%u]", g_it->first);
        for (uint32_t i = 0; i < g_it->second.achieveid.size(); i++) {
            TRACE_LOG("title condition [%u]", g_it->second.achieveid[i]);
        }
    }
    TRACE_LOG("========================= [END DUMP] =================================");
    //if (j == 0) {
    //    return -1;
    //}
    return 0;
}

/**
 * @brief load achieve title condition
 */
int load_other_title(xmlNodePtr cur)
{
    int j = 0;
    uint32_t id = 0, world = 0;
    while (cur) {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Title"))) {
            get_xml_prop(id, cur, "ID");
            title_other_map::iterator g_it = g_title_condition.other_map.find(id);
            if ( g_it != g_title_condition.other_map.end() ) {
                ERROR_LOG("Duplicate title id = [%u]", id);
                return -1;
            }

            title_other_t tmp_t;
			tmp_t.title_id = id;
            get_xml_prop_raw_str_def(tmp_t.title_name, cur, "Name", "小侠士");
            get_xml_prop_def(world, cur, "World", 0);
            tmp_t.world = (uint8_t)(world);

            g_title_condition.other_map.insert(title_other_map::value_type(id, tmp_t));
            j++;

        }
        cur = cur->next;
    }
    //====================== Dump pve title condition MAP ===========================
    TRACE_LOG("========================= [BEGIN DUMP] ===============================");
    title_other_map::iterator g_it = g_title_condition.other_map.begin();
    for (; g_it != g_title_condition.other_map.end(); ++g_it) {
        TRACE_LOG("title condition [%u %s]", g_it->second.title_id, g_it->second.title_name);
    }
    TRACE_LOG("========================= [END DUMP] =================================");
    if (j == 0) {
        return -1;
    }
    return 0;
}


/**
 * @brief load achieve title info from conf/title.xml file
 * @param cur xml file point
 * @return 0 on success, -1 on error
 */
int load_title_config(xmlNodePtr cur)
{
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("PveTitle"))) {
            if ( !g_title_condition.pve_map.empty() ) {
                g_title_condition.pve_map.clear();
            }
			get_xml_prop_raw_str_def(g_title_condition.pve_note1, cur, "Note1", "");
			get_xml_prop_raw_str_def(g_title_condition.pve_note2, cur, "Note2", "");
			TRACE_LOG("%s %s", g_title_condition.pve_note1, g_title_condition.pve_note2);
            if (load_pve_title(cur->xmlChildrenNode) == -1) {
                ERROR_LOG("config pve title error!!!!!!!!");
                throw XmlParseError(std::string("config pve title error in title.xml"));
                return -1;
            }
        }
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("AchieveTitle"))) {
            if ( !g_title_condition.achieve_map.empty() ) {
                g_title_condition.achieve_map.clear();
            }
            if (load_achieve_title(cur->xmlChildrenNode) == -1) {
                ERROR_LOG("config achieve title error!!!!!!!!");
                throw XmlParseError(std::string("config achieve title error in title.xml"));
                return -1;
            }
		}

		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("OtherTitle"))) {
            if ( !g_title_condition.other_map.empty() ) {
                g_title_condition.other_map.clear();
            }
            if (load_other_title(cur->xmlChildrenNode) == -1) {
                ERROR_LOG("config achieve title error!!!!!!!!");
                throw XmlParseError(std::string("config other title error in title.xml"));
                return -1;
            }
		}
		cur = cur->next;
	}
	return 0;
}



