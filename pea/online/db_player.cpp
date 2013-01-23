#include <libtaomee++/inet/pdumanip.hpp>
extern "C"
{
#include <arpa/inet.h>
#include <libtaomee/timer.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/project/utilities.h>
}

#include "proto.hpp"
#include "pea_common.hpp"
#include "cli_proto.hpp"
#include "db_player.hpp"
#include "utils.hpp"
#include "item_bag.hpp"
#include "login.hpp"
#include "prize.hpp"
#include "db_friends.hpp"

using namespace taomee;

int db_get_player(player_t* p)
{
    db_proto_get_player_in out;
    out.server_id = p->server_id;
    out.role_tm = p->role_tm;
    out.user_id = p->id;
	return send_to_db(p, db_proto_get_player_cmd, &out);
}


int db_proto_get_player_callback(DEFAULT_ARG)
{
    db_proto_get_player_out * p_in = P_IN;

	strncpy(p->nick, p_in->player_info.nick, MAX_NICK_SIZE);
	init_player_item_bag(p, p_in->player_info.max_bag_grid_count);
	
	p->set_player_attr_value(OBJ_ATTR_LEVEL, p_in->player_info.level); 
	p->set_player_attr_value(OBJ_ATTR_EXP, p_in->player_info.exp);
	p->set_player_attr_value(OBJ_ATTR_GOLD, p_in->player_info.gold);

	p->last_login_tm = p_in->player_info.last_login_tm;
	p->last_off_line_tm = p_in->player_info.last_off_line_tm;
	p->last_map_id = p_in->player_info.map_id;
	p->last_map_x  = p_in->player_info.map_x;
	p->last_map_y  = p_in->player_info.map_y;
	p->eye_model = p_in->player_info.model.eye_model;
	p->resource_id = p_in->player_info.model.resource_id;
	p->forbid_friends_me = p_in->player_info.forbid_friends_me;

	//TODO push offline msg to player?
	/*--add code here--*/
	std::vector<msg_t>::iterator it;
	for (it = p_in->offline_msg_list.msg_list.begin(); it != p_in->offline_msg_list.msg_list.end(); it++) {
		DEBUG_TLOG("push offline msg to user");
		send_to_player(p, it->msg, it->_msg_len, 0);
	}
	p->init_player_attr();
    return db_get_extra_info(p);
}

int db_get_extra_info(player_t * p)
{
    db_proto_get_extra_info_in in;
    p->export_db_user_id(&in.db_user_id);

    db_extra_info_t query;
    query.info_id = EXTRA_INFO_FREE_RAND_GIFT;
    query.info_value = 0;
    in.query_info.push_back(query);

    return send_to_db(p, db_proto_get_extra_info_cmd, &in);
}


int db_proto_get_extra_info_callback(DEFAULT_ARG)
{
    db_proto_get_extra_info_out * p_in = P_IN;

    for (uint32_t i = 0; i < p_in->query_info.size(); i++)
    {
        uint32_t info_id = p_in->query_info[i].info_id;
        uint32_t info_value = p_in->query_info[i].info_value;

        uint32_t * p_value = p->extra_info->id2p(info_id);
        if (NULL != p_value)
        {
            *p_value = info_value;
        }

    }

    return db_save_extra_info(p);
}

int db_save_extra_info(player_t * p)
{

    time_t cur_time = time(NULL);
    struct tm cur_tm;
    if (NULL == localtime_r(&cur_time, &cur_tm))
    {
        send_error_to_player(p, ONLINE_ERR_SYSTEM_FAULT);
        return 0;
    }

    cur_tm.tm_sec = 0;
    cur_tm.tm_min = 0;
    cur_tm.tm_hour = 0;
    time_t day_time = mktime(&cur_tm);


    db_proto_save_extra_info_in in;
    p->export_db_user_id(&in.db_user_id);

    if (p->last_login_tm < day_time)
    {
        // 说明上一次登录不是今天


        // 恢复free_rand_gift
        db_extra_info_t extra;
        extra.info_id = EXTRA_INFO_FREE_RAND_GIFT;
        extra.info_value = 3;
        in.extra_info.push_back(extra);

    }

    return send_to_db(p, db_proto_save_extra_info_cmd, &in);

}


int db_proto_save_extra_info_callback(DEFAULT_ARG)
{
    db_proto_save_extra_info_out * p_in = P_IN;

    for (uint32_t i = 0; i < p_in->extra_info.size(); i++)
    {
        uint32_t info_id = p_in->extra_info[i].info_id;
        uint32_t info_value = p_in->extra_info[i].info_value;

        uint32_t * p_value = p->extra_info->id2p(info_id);
        if (NULL != p_value)
        {
            *p_value = info_value;
        }

    }

    p->last_login_tm = get_now_tv()->tv_sec;

    p->set_module(MODULE_USER);

    return process_login(p);
}

int db_save_player(player_t* p)
{
    if (!p->check_module(MODULE_USER))
    {
        return 0;
    }

    db_proto_save_player_in out;
    db_player_info * p_info = &out.player_info;

    p_info->user_id = p->id;
    p_info->role_tm = p->role_tm;
	strncpy(p_info->nick, p->nick, MAX_NICK_SIZE);
    p_info->model.eye_model = p->eye_model;
    p_info->model.resource_id = p->resource_id;
    p_info->server_id = p->server_id;
    p_info->last_login_tm = p->last_login_tm;
    p_info->last_off_line_tm = p->last_off_line_tm;
    p_info->exp = p->get_player_attr_value(OBJ_ATTR_EXP);
    p_info->level = p->get_player_attr_value(OBJ_ATTR_LEVEL);

    if (!p->check_module(MODULE_BAG))
    {
        p_info->max_bag_grid_count = p->bag->max_grid_count;
    }

    p_info->map_id = p->cur_map ? p->cur_map->id : 10;
    p_info->map_x = p->x_pos;
    p_info->map_y = p->y_pos;
    p_info->gold = p->get_player_attr_value(OBJ_ATTR_GOLD);

	return send_to_db(p, db_proto_save_player_cmd, &out);
}

int db_proto_save_player_callback(DEFAULT_ARG)
{
    return 0;
}

int db_save_login_time(player_t * p)
{
    if (!p->check_module(MODULE_USER))
    {
        return 0;
    }

    db_proto_save_login_time_in in;
    p->export_db_user_id(&in.db_user_id);

    in.last_login_tm = p->last_login_tm;
    in.last_off_line_tm = p->last_off_line_tm;

    return send_to_db(p, db_proto_save_login_time_cmd, &in);
}

int db_proto_save_login_time_callback(DEFAULT_ARG)
{
    return 0;
}

int db_update_gold(const player_t *p, int32_t new_gold)
{
    db_proto_update_gold_in out;
    out.db_user_id.user_id = p->id;
    out.db_user_id.role_tm = p->role_tm;
    out.db_user_id.server_id = p->server_id;
    out.new_gold = new_gold;
    return send_to_db(p, db_proto_update_gold_cmd, &out);
}
int db_proto_update_gold_callback(DEFAULT_ARG)
{
    db_proto_update_gold_out *p_in = P_IN;
    p->set_player_attr_value(OBJ_ATTR_GOLD, p_in->new_gold);
    return 0;
}

int db_change_exp(const player_t *p, int32_t exp_change)
{
    db_proto_change_exp_in out;
    out.db_user_id.user_id = p->id;
    out.db_user_id.role_tm = p->role_tm;
    out.db_user_id.server_id = p->server_id;
    out.exp_change = exp_change;
    return send_to_db(p, db_proto_change_exp_cmd, &out);
}
int db_proto_change_exp_callback(DEFAULT_ARG)
{
    db_proto_change_exp_out *p_in = P_IN;

    uint32_t exp = p->get_player_attr_value(OBJ_ATTR_EXP);
    exp += p_in->exp_change;
    p->set_player_attr_value(OBJ_ATTR_EXP, exp);
   
    if (p->process_prize_state == true) {//如果是获取奖励增加经验，则回到奖励处理
        return get_prize(p, p->cache_prize_id, p->cache_prize_seq);
    }
    /*TODO(singku) 其他情况 add code below*/

    return 0;
}

int db_change_gold(const player_t *p, int32_t gold_change)
{
    db_proto_change_gold_in out;
    out.db_user_id.user_id = p->id;
    out.db_user_id.role_tm = p->role_tm;
    out.db_user_id.server_id = p->server_id;
    out.gold_change = gold_change;
    return send_to_db(p, db_proto_change_gold_cmd, &out);
}

int db_proto_change_gold_callback(DEFAULT_ARG)
{
    db_proto_change_gold_out *p_in = P_IN;
    //增加金币
    uint32_t gold = p->get_player_attr_value(OBJ_ATTR_GOLD);
    gold += p_in->gold_change;
    p->set_player_attr_value(OBJ_ATTR_GOLD, gold);

    if (p->process_prize_state == true) {//如果是获取奖励增加金币，回到奖励
        return get_prize(p, p->cache_prize_id, p->cache_prize_seq);
    }
    /*TODO(singku) 其他情况 add code below*/

    return 0;
}

int db_level_up(const player_t *p)
{
    db_proto_level_up_in out;
    out.db_user_id.user_id = p->id;
    out.db_user_id.role_tm = p->role_tm;
    out.db_user_id.server_id = p->server_id;
    return send_to_db(p, db_proto_level_up_cmd, &out);
}
int db_proto_level_up_callback(DEFAULT_ARG)
{
    return 0;
}
