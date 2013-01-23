extern "C" {
#include <libtaomee/log.h>
}

#include <time.h>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>

#include "proto.hpp"
#include "cli_proto.hpp"
#include "pea_common.hpp"
#include "player.hpp"
#include "db_item.hpp"
#include "db_player.hpp"
#include "login.hpp"
#include "prize.hpp"

using namespace taomee;

std::map<uint32_t, prize_info_t*> prize_config_map;

prize_info_t *alloc_a_prize_info()
{
    prize_info_t *p = new prize_info_t();
    p->prizes = new prize_vec_t();
    p->prizes->clear();
    return p;
}

void dealloc_a_prize_info(prize_info_t *prize_info)
{
    delete prize_info->prizes;
    delete prize_info;
}

/**
 * @brief 单个奖励配置项的读取
 * @para item_node奖励项节点,id 奖励ID
 * @return 0 成功，-1读取错误
 */
int process_single_prize_conf(xmlNodePtr item_node, uint32_t id)
{
    prize_info_t *prize_info  = alloc_a_prize_info();
    prize_t aprize;
    uint32_t item_fix_op;//是否固定奖励
    uint32_t item_type;
    uint32_t item_id;
    uint32_t item_num;
    while (item_node) {
        get_xml_prop_def(item_fix_op, item_node, "fixed", 1);//默认是固定奖励
        if (item_fix_op != prize_fixed && item_fix_op != prize_optional) {
            dealloc_a_prize_info(prize_info);
            RT_ERROR_TLOG(-1, "invaid prize item abttr %u[must be 0 or 1]", item_fix_op);
        }
        aprize.fixed = (bool)item_fix_op;
        get_xml_prop_def(item_type, item_node, "type", 0);
        if (item_type <= prize_type_begin || item_type >= prize_type_end) {
            dealloc_a_prize_info(prize_info);
            RT_ERROR_TLOG(-1, "invalid prize item type %u(%u-%u)", 
                    item_type, prize_type_begin, prize_type_end);
        }
        aprize.type = item_type;

        get_xml_prop_def(item_id, item_node, "id", 0);
        if ((item_type == prize_type_item || item_type == prize_type_equip) 
            && item_data_mgr::get_instance()->is_item_data_exist(item_id) == false) {
            RT_ERROR_TLOG(-1, "invalid prize item id %u", item_id);
        }
        aprize.id = item_id;

        get_xml_prop_def(item_num, item_node, "num", 0);
        aprize.num = item_num;

        prize_info->prizes->push_back(aprize);
        item_node = item_node->next;
    }
    prize_info->prize_id = id;
    prize_config_map[id] = prize_info;
    return 0;
}

int init_prize_data(xmlNodePtr prize_node)
{
    prize_config_map.clear();
    prize_node = prize_node->xmlChildrenNode;
    uint32_t prize_id;
    while (prize_node) {
        if (xmlStrEqual(prize_node->name, reinterpret_cast<const xmlChar*>("prize"))) {
            get_xml_prop_def(prize_id, prize_node, "id", 0);
            if (prize_id == 0) {
                RT_ERROR_TLOG(-1, "invalid prize id[null or 0]");
            }
            if (prize_config_map.find(prize_id) != prize_config_map.end()) {
                RT_ERROR_TLOG(-1, "duplicate prize id! %u already exist", prize_id);
            }
            if (process_single_prize_conf(prize_node->xmlChildrenNode, prize_id)) {
                RT_ERROR_TLOG(-1, "process prize id %u faild", prize_id);
            }
        }
        prize_node = prize_node->next;
    }
    return 0;
}

void final_prize_data()
{
    std::map<uint32_t, prize_info_t*>::iterator iter;
    iter = prize_config_map.begin();
    for (; iter != prize_config_map.end(); iter++) {
        prize_info_t *p = iter->second;
        dealloc_a_prize_info(p);        
    }
    prize_config_map.clear();
}

int prepare_item_for_db(player_t *p, uint32_t item_id, uint32_t num, std::vector<db_add_item_request_t> &req)
{
    /*NOTICE(singku) 借用了db_item.cpp里的ericma实现的函数parse_db_add_item_request*/
    int32_t now, expire_time;
    item_data *p_item_data;
    now = (int32_t)time(NULL);
    p_item_data = item_data_mgr::get_instance()->get_item_data_by_id(item_id);
    if (p_item_data->duration_time_ == 0) {
        expire_time = (int32_t)p_item_data->end_time_;
    } else {
        expire_time = now + (int32_t)p_item_data->duration_time_;
    }

    bool ret = parse_db_add_item_request(p, item_id, num, expire_time, req);
    if (ret == false)
        return -1;
    return 0;
}

int prize_finished(player_t *p, uint32_t prize_id)
{
    uint32_t optional_pos = p->cache_optional_prize_pos;
    uint32_t tskid = p->cache_tskid;

    p->cache_prize_id = 0;
    p->cache_prize_seq = 1;
    p->cache_optional_prize_pos = 0;
    p->cache_tskid = 0;
    p->process_prize_state = false;//关闭奖励处理

    switch (p->waitcmd) {
    case cli_proto_finish_task_cmd :{
        //如果是完成任务的奖励
        cli_proto_finish_task_out out;
        out.tskid = tskid;
        out.optional_pos = optional_pos;
        return send_to_player(p, &out, p->waitcmd, 1);
    }
    case cli_proto_get_prize_cmd : {
        //如果是主动领取的奖励
        cli_proto_get_prize_out out;
        out.prize_id = prize_id;
        out.optional_pos = optional_pos;
        return send_to_player(p, &out, p->waitcmd, 1);
    }
    default :
        return 0;
    }
}

int get_prize(player_t *p, uint32_t prize_id, uint32_t seq)
{
    p->process_prize_state = true;
    std::map<uint32_t, prize_info_t*>::iterator it;
    it = prize_config_map.find(prize_id);
    if (it == prize_config_map.end()) {
        RT_ERROR_TLOG(-1, "invalid prize_id %u for player %u",prize_id, p->id);
    }

    prize_vec_t *p_prizes = it->second->prizes;
    if (seq >= p_prizes->size()) {//奖励完成
ok:     return prize_finished(p, prize_id);
    }
    
    //未完成，则更新缓存的奖励项
    p->cache_prize_seq = seq + 1;//下次调用该函数 就直接到下一个奖励项了

    std::vector<db_add_item_request_t> req;
    prize_t *p_prize;
    p_prize = &((*p_prizes)[seq - 1]);

    //如果可选奖励位置和seq不匹配则逐个跳过
    while (p_prize->fixed == prize_optional 
            && seq != p->cache_optional_prize_pos
            && seq < p_prizes->size()) {
        seq ++;
        p_prize = &((*p_prizes)[seq - 1]);//重新定位
    } 
    if (seq >= p_prizes->size()) {
        //几乎不可能到达这里,因为如果有可选奖励的话 
        //客户端一定要发送一个合法的可选奖励的位置过来
        //那么seq一定会定位到正确的位置而不会超过
        goto ok;
    }

    switch (p_prize->type) {
    case prize_type_exp :
        return db_change_exp(p, p_prize->num);
    case prize_type_gold :
        return db_change_gold(p, p_prize->num);
    case prize_type_item :
    case prize_type_equip :
        if (prepare_item_for_db(p, p_prize->id, p_prize->num, req) != 0) {
            RT_ERROR_TLOG(-1, "failed to add item %u of prize %u for user %u",
                    p_prize->id, prize_id, p->id);
        }
        return db_add_item(p, req);
    default :
        break;
    }
    DEBUG_TLOG("奖励结束");
    return 0;
}

/**
 * @brief 给用户p增加一个奖励为prize_id的奖励,所有发放奖励的地方都可以调用本函数
 * @para p 角色，prize_id 奖励ID
 * @notice 暂时只实现给客户端通知,DB存储可后续实现
 * @return 0 成功 其他失败
 */
int add_prize(player_t *p, uint32_t prize_id)
{
    /*TODO(singku) 存储未领取奖励列表到DB add code below*/
    return db_add_prize(p, prize_id);
}

int cli_proto_get_prize(DEFAULT_ARG)
{
    cli_proto_get_prize_in *p_in = P_IN;
    
    //奖励ID不存在
    std::map<uint32_t, uint32_t>::iterator itp;
    itp = p->player_prize.find(p_in->prize_id);
    if(itp == p->player_prize.end()) {
        send_error_to_player(p, ONLINE_ERR_PRIZE_ID_NOT_EXIST);
    }
    //可选奖励位置不合法
    std::map<uint32_t, prize_info_t*>::iterator it;
    it = prize_config_map.find(p_in->prize_id);
    if (p_in->optional_pos > it->second->prizes->size()) {
        send_error_to_player(p, ONLINE_ERR_PRIZE_OP_POS_INVALID);
    }
    prize_t *prize = &((*(it->second->prizes))[p_in->optional_pos - 1]);
    if (prize->fixed != prize_optional) {
        send_error_to_player(p, ONLINE_ERR_PRIZE_OP_POS_INVALID);
    }

    /*TODO(singku) 通知DB该奖励已领取 del prize add code below*/
    p->cache_prize_id = p_in->prize_id;
    p->cache_optional_prize_pos = p_in->optional_pos;
    return db_del_prize(p, p_in->prize_id);
}

int db_add_prize(player_t *p, uint32_t prize_id)
{
    int32_t now = (int32_t)time(NULL);
    db_proto_add_prize_in out;
    out.db_user_id.user_id = p->id;
    out.db_user_id.role_tm = p->role_tm;
    out.db_user_id.server_id = p->server_id;
    out.prize_id = prize_id;
    out.add_time = now;
    return send_to_db(p, db_proto_add_prize_cmd, &out);
}

int db_proto_add_prize_callback(DEFAULT_ARG)
{
    db_proto_add_prize_out *p_in = P_IN;
    cli_proto_prize_notify_out out;
    out.prize_id = p_in->prize_id;
    
    /*增加到用户奖励map中*/
    std::map<uint32_t, uint32_t>::iterator it;
    it = p->player_prize.find(p_in->prize_id);
    if (it != p->player_prize.end())  {
        it->second ++;
    } else {
        p->player_prize[p_in->prize_id] = 1;
    }
    /*通知用户有奖励可领取 用户领取奖励时，以player_prize中的信息作为依据*/
    return send_to_player(p, &out, cli_proto_prize_notify_cmd, 0);
}

int db_del_prize(player_t *p, uint32_t prize_id)
{
    db_proto_del_prize_in out;
    out.db_user_id.user_id = p->id;
    out.db_user_id.role_tm = p->role_tm;
    out.db_user_id.server_id = p->server_id;
    out.prize_id = prize_id;
    return send_to_db(p, db_proto_del_prize_cmd, &out);
}

int db_proto_del_prize_callback(DEFAULT_ARG)
{
    db_proto_del_prize_out *p_in = P_IN;
    std::map<uint32_t, uint32_t>::iterator itp;
    itp = p->player_prize.find(p_in->prize_id);
    //从map中删除记录
    itp->second --;
    if (itp->second == 0) {
        p->player_prize.erase(itp);
    }
    //增加奖励的具体内容给用户
    return get_prize(p, p_in->prize_id, prize_seq_start);
}

int db_get_prize_list(player_t *p)
{
    db_proto_get_prize_list_in out;
    out.db_user_id.user_id = p->id;
    out.db_user_id.role_tm = p->role_tm;
    out.db_user_id.server_id = p->server_id;
    return send_to_db(p, db_proto_get_prize_list_cmd, &out);
}   

int db_proto_get_prize_list_callback(DEFAULT_ARG)
{
    db_proto_get_prize_list_out *p_in = P_IN;
    std::vector<db_prize_t>::iterator it;
    it = p_in->prize_list.begin();
    for (; it != p_in->prize_list.end(); it++) {
        p->player_prize[it->prize_id] = it->count;
    }

    p->set_module(MODULE_PRIZE);
    return process_login(p);
}

int cli_proto_get_prize_list(DEFAULT_ARG)
{
    cli_proto_get_prize_list_out out;
    std::map<uint32_t, uint32_t>::iterator it;
    it = p->player_prize.begin();
    db_prize_t tmp;
    for (; it != p->player_prize.end(); it++) {
        tmp.prize_id = it->first;
        tmp.count = it->second;
        out.prize_list.push_back(tmp);
    }
    return send_to_player(p, &out, p->waitcmd, 1);
}
