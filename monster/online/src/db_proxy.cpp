/**
 * =====================================================================================
 *       @file  db_proxy.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/30/2011 02:27:31 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  henry (韩林), henry@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#include <assert.h>
#include <stdio.h>
#include <map>
#include <vector>

extern "C"
{
#include <libtaomee/conf_parser/config.h>
}

#include <libtaomee++/inet/pdumanip.hpp>
#include "message.h"
#include "stat.h"

#include "data.h"
#include "function.h"
#include "db_proxy.h"
#include "utility.h"
#include "plantation.h"
#include "stuff.h"
#include "cli_proto.h"
#include "badge.h"
#include "activity.h"

using std::map;
using std::vector;

void init_db_handle_funs()
{
    g_db_cmd_map.clear();

    g_db_cmd_map[svr_msg_db_add_role] = process_db_add_role_return;                             //32768---0x8000
    g_db_cmd_map[svr_msg_db_role_info] = process_db_role_info_return;                           //32769---0x8001
    g_db_cmd_map[svr_msg_db_get_bag_stuff] = process_db_get_bag_stuff_return;                   //32770---0x8002
    g_db_cmd_map[svr_msg_db_add_bag_stuff] = process_db_add_stuff_return;                       //32771---0x8003
    g_db_cmd_map[svr_msg_db_room_num] = process_db_get_room_num_return;                         //32773---0x8005
    g_db_cmd_map[svr_msg_db_room_info] = process_db_get_room_info_return;                       //32774---0x8006
    g_db_cmd_map[svr_msg_db_update_role] = process_db_update_role_return;                       //32776---0x8008
    g_db_cmd_map[svr_msg_db_update_room] = process_db_update_room_return;                       //32777---0x8009
    g_db_cmd_map[svr_msg_db_get_puzzle_info] = process_db_get_puzzle_return;                    //32778---0x800a
    g_db_cmd_map[svr_msg_db_commit_puzzle] = process_db_commit_puzzle_return;                   //32779---0x800b
    g_db_cmd_map[svr_msg_db_add_role_value] = process_db_add_role_value_return;                 //32780---0x800c
    g_db_cmd_map[svr_msg_db_get_friend_id] = process_db_get_friend_id_return;                   //32781---0x800d
    g_db_cmd_map[svr_msg_db_apply_for_friend] = process_db_apply_for_friend_return;             //32782---0x800e
    g_db_cmd_map[svr_msg_db_get_friend_apply] = process_db_get_friend_apply_return;             //32783---0x800f
    g_db_cmd_map[svr_msg_db_set_friend_status] = process_db_set_friend_status_return;           //32784---0x8010
    g_db_cmd_map[svr_msg_db_del_friend] = process_db_del_friend_return;                         //32789---0x8011
    g_db_cmd_map[svr_msg_db_update_plant] = process_db_update_plant_return;                     //32793---0x8012
    g_db_cmd_map[svr_msg_db_get_friend_info] = process_db_get_friend_info_return;               //32785---0x8013
    g_db_cmd_map[svr_msg_db_get_pinboard_info] = process_db_get_pinboard_return;                //32786---0x8014
    g_db_cmd_map[svr_msg_db_add_message] = process_db_add_message_return;                       //32787---0x8015
    g_db_cmd_map[svr_msg_db_update_message_status] = process_db_update_msg_status_return;       //32788---0x8016
    g_db_cmd_map[svr_msg_db_pet_op] = process_db_pet_op_return;                                 //32790---0x8017
    g_db_cmd_map[svr_msg_db_add_plant] = process_db_add_plant_return;                           //32791---0x8018
    g_db_cmd_map[svr_msg_db_del_plant] = process_db_del_plant_return;                           //32792---0x8019
    g_db_cmd_map[svr_msg_db_interactive] = process_db_interactive_return;                       //32794---0x801a
    g_db_cmd_map[svr_msg_db_get_day_restrict] = process_db_get_day_restrict_return;             //32795---0x801b
    g_db_cmd_map[svr_msg_db_add_day_restrict] = process_db_add_day_restrict_return;             //32796---0x801c
    g_db_cmd_map[svr_msg_db_get_profile] = process_db_get_profile_return;                       //32797---0x801d
    g_db_cmd_map[svr_msg_db_eat_food] = process_db_eat_food_return;                             //32798---0x801e
    g_db_cmd_map[svr_msg_db_modify_sign] = process_db_modify_sign_return;                       //32799---0x801f
    g_db_cmd_map[svr_msg_db_other_role_info] = process_db_other_role_info_return;               //32800---0x8020
    g_db_cmd_map[svr_msg_db_update_login] = process_db_update_login_return;                     //32801---0x8021
    g_db_cmd_map[svr_msg_db_get_all_pet] = process_db_get_pet_return;                           //32802---0x8022
    g_db_cmd_map[svr_msg_db_get_real_message_count] = process_db_get_real_message_count_return; //32803---0x8023
    g_db_cmd_map[svr_msg_db_get_all_badge] = process_db_get_all_badge_return;                   //32804---0x8024
    g_db_cmd_map[svr_msg_db_set_unread_badge] = process_db_set_unread_badge_return;             //32805---0x8025
    g_db_cmd_map[svr_msg_db_update_badge] = process_db_update_badge_return;                     //32806--0x8026
    g_db_cmd_map[svr_msg_db_compose_stuff] = process_db_compose_stuff_return;                   //32807--0x8027
    g_db_cmd_map[svr_msg_db_add_game_day_restrict] = process_db_add_game_day_restrict_return;   //32808--0x8028
    g_db_cmd_map[svr_msg_db_get_game_day_restrict] = process_db_get_game_day_restrict_return;   //32809--0x8029
    g_db_cmd_map[svr_msg_db_add_game_level] = process_db_add_game_level_return;                 //32810--0x802a
    g_db_cmd_map[svr_msg_db_get_game_level] = process_db_get_game_level_return;                 //32811--0x802b
    g_db_cmd_map[svr_msg_db_modify_monster_name] = process_modify_monster_name_return;          //32812--0x802c
    g_db_cmd_map[svr_msg_db_get_hole_reward] = process_get_hole_reward_return;                        //32813--0x802d
    g_db_cmd_map[svr_msg_db_maintain_plant] = process_maintain_plant_return;                    //32814 -- 0x802e
    g_db_cmd_map[svr_msg_db_get_plant] = process_get_plant_return;                              //32815 ---0x802f
    g_db_cmd_map[svr_msg_sun_reward_happy] = process_sun_reward_happy_return;                   //32816--0x8030
    g_db_cmd_map[svr_msg_db_sun_reward_exp] = process_sun_reward_exp_return;                //32817---0x8031
    g_db_cmd_map[svr_msg_db_sun_reward_coins] = process_sun_reward_coins_return;                //32818---0x8032
    g_db_cmd_map[svr_msg_db_get_npc_score] = process_get_npc_score_return;              //32819
    g_db_cmd_map[svr_msg_db_set_npc_score] = process_set_npc_score_return;              //32820
    g_db_cmd_map[svr_msg_db_get_game_changed_stuff] = process_get_game_changed_stuff_return;   //32821
    g_db_cmd_map[svr_msg_db_set_game_changed_stuff] = process_set_game_changed_stuff_return;   //32822
    g_db_cmd_map[svr_msg_db_get_finished_task] = process_get_finished_task_return;          //32823
    g_db_cmd_map[svr_msg_db_finish_task] = process_finish_task_return;                      //32824
    g_db_cmd_map[svr_msg_db_factory_op] = process_factory_op_return;                //32825
    g_db_cmd_map[svr_msg_db_enter_museum] = process_enter_museum_return;                //32826
    g_db_cmd_map[svr_msg_db_commit_museum_game] = process_commit_mesuem_game_return;                //32827
    g_db_cmd_map[svr_msg_db_get_museum_reward] = process_get_museum_reward_return;                //32828
    g_db_cmd_map[svr_msg_db_request_update_wealth] = process_update_wealth_return;                              //32829
    g_db_cmd_map[svr_msg_db_get_activity_reward] = process_get_activity_rewarded_return; //32830
    g_db_cmd_map[svr_msg_db_get_open_reward] = process_get_open_reward_return;//32831
    g_db_cmd_map[svr_msg_update_activity_status] = process_update_act_status_return;//32832
	g_db_cmd_map[svr_msg_db_get_invite_info] = process_get_invite_info_return;//32833
	g_db_cmd_map[svr_msg_db_request_prize_lottery] = process_prize_lottery_return;//32834
	g_db_cmd_map[svr_msg_db_get_activity_reward_ex] = process_get_activity_reward_ex_return;//32835
	g_db_cmd_map[svr_msg_db_get_dragon_boat_info] = process_get_dragon_boat_return;//33837
	g_db_cmd_map[svr_msg_db_update_dragon_boat] = process_update_dragon_boat_return;//33838

    g_db_cmd_map[svr_msg_db_search_name] = process_db_search_name_return;                       //33281
    g_db_cmd_map[svr_msg_db_encourage_guide] = process_encourage_guide_return;   			//33285

    g_db_cmd_map[svr_msg_db_request_history_donate] = process_request_history_donate_return;   			//33286
    g_db_cmd_map[svr_msg_db_request_cur_donate] = process_request_cur_donate_return;   			//33287
    g_db_cmd_map[svr_msg_db_request_donate_coins] = process_request_donate_coins_return;   			//33288

    g_db_cmd_map[account_msg_check_session] = process_db_check_session_return;                  //0xA024

    return;
}

void handle_db_return(svr_msg_header_t *p_db_pkg, int len)
{
    g_errno = ERR_NO_ERR;

    if (0 == p_db_pkg->seq_id)
    {
        KINFO_LOG(p_db_pkg->user_id, "db msg:%u, ret:%u", p_db_pkg->msg_type, p_db_pkg->result);
        return;
    }

    uint32_t counter = p_db_pkg->seq_id & 0xFFFF;
    uint32_t conn_fd  = p_db_pkg->seq_id >> 16;
    usr_info_t *p_user = g_user_mng.get_user(conn_fd);
    if (NULL == p_user || counter != p_user->counter || (int)p_db_pkg->len != len)
    {
        ERROR_LOG("CONNECTION HAD BEEN CLOSED OR DB TIMEOUT\t[cmd=%X %u cnt=%d %d uid=%u]", p_db_pkg->msg_type, p_user ? p_user->waitcmd : 0, counter, p_user ? p_user->counter : 0, p_user ? p_user->uid : 0);
        return;
    }

//    KDEBUG_LOG(p_user->uid, "DB R\t[uid=%u fd=%d cmd=0x%X ret=%u]", p_db_pkg->user_id, conn_fd, p_db_pkg->msg_type, p_db_pkg->result);

    REMOVE_TIMERS(p_user);

    map<uint16_t, DB_PROCESS_FUNC>::iterator cmd_iter = g_db_cmd_map.find(p_db_pkg->msg_type);
    if (cmd_iter == g_db_cmd_map.end())    //未知的协议
    {
        KERROR_LOG(p_user->uid, "[DB MSG:%u] invalid, seq:%u", p_db_pkg->msg_type, p_db_pkg->seq_id);
        close_client_conn(conn_fd);
    }
    else if (cmd_iter->second(p_user, p_db_pkg) != 0)
    {
        KCRIT_LOG(p_user->uid, "[DB MSG:%u] process failed, seq:%u", p_db_pkg->msg_type, p_db_pkg->seq_id);
        close_client_conn(p_user->session->fd);
        return;
    }
    else
    {
        // do nothing
    }

    if (g_errno != ERR_NO_ERR)
    {
        send_header_to_user(p_user, SYSTEM_ERR_NUM, g_errno);
    }

    return;
}

int process_db_role_info_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KERROR_LOG(p_db_pkg->user_id, "db login failed:%u", p_db_pkg->result);
        return -1;
    }

    KDEBUG_LOG(p_user->uid, "db_role_info return success");
    //将用户信息缓存起来
    const db_msg_get_role_rsp_t *p_db_rsp = (const db_msg_get_role_rsp_t *)p_db_pkg->body;
    if (!p_db_rsp->is_register) //用户没有注册
    {
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
	    uint32_t seq = uniform_rand(1, 10000);
        g_send_msg.pack(seq);
        p_user->seq = seq;
        g_send_msg.pack((uint8_t)0);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        return 0;
    }

    p_user->is_register = 1;

    //计算新的愉悦度
    time_t now = time(NULL);
    if (p_db_rsp->register_rsp.role.monster_happy > MAX_HAPPY_VALUE || p_db_rsp->register_rsp.role.monster_health > MAX_HEALTH_VALUE)
    {
        KEMERG_LOG(p_user->uid, "happy:%u, health:%u", p_db_rsp->register_rsp.role.monster_happy, p_db_rsp->register_rsp.role.monster_health);
    }
    uint32_t mon_happy = get_new_happy(now, p_db_rsp->register_rsp.role.last_login_time, p_db_rsp->register_rsp.role.monster_happy);
    uint32_t mon_health = get_monster_health_on_login(now, p_db_rsp->register_rsp.role.last_logout_time, p_db_rsp->register_rsp.role.monster_health);

    //缓存用户的登录信息
    role_cache_t role = {{{0}}};
    memcpy(&role, &p_db_rsp->register_rsp, sizeof(role_cache_t) - sizeof(uint32_t) - sizeof(uint16_t));
    role.role.monster_health = mon_health;
    role.role.monster_happy = mon_happy;
    role.role.last_login_time = now;
    role.last_update_health_time = now;

	//deal whether login first time today
	role.role.is_first_login = timestamp_equal_day(p_db_rsp->register_rsp.role.last_login_time, now) == 0?1:0;

    //缓存用户的精灵
    int idx = sizeof(db_msg_get_role_rsp_t);
    uint16_t pet_count = 0;
    taomee::unpack_h(p_db_rsp, pet_count, idx);
    if (pet_count > g_max_pet_id)
    {
        KERROR_LOG(p_user->uid, "following pet count:%u too large", pet_count);
        return -1;
    }
    for (int i = 0; i != pet_count; ++i)
    {
        pet_info_t pet_info;
        taomee::unpack_h(p_db_rsp, pet_info.pet_id, idx);
        taomee::unpack_h(p_db_rsp, pet_info.total_num, idx);
        taomee::unpack_h(p_db_rsp, pet_info.follow_num, idx);
        if (p_user->user_cache.add_pet(&pet_info) != 0)
        {
            KERROR_LOG(p_user->uid, "cache following pet");
            return -1;
        }
        KINFO_LOG(p_user->uid, "pet:%u, total:%u, follow:%u", pet_info.pet_id, pet_info.total_num, pet_info.follow_num);
    }

    db_msg_update_login_req_t *p_body = (db_msg_update_login_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->login_time = now;
    p_body->monster_happy = mon_happy;

    int past_time = now - role.role.last_logout_time;
    if (past_time >= 0)
    {
        p_body->offline_time = role.role.offline_time + past_time;
        role.role.offline_time = p_body->offline_time;
        if (p_body->offline_time >= CHENGMI_OFFLINE)    //如果离线时间到达5个小时，时间都清零
        {
            p_body->offline_time = 0;
            role.role.online_time = 0;
            role.role.offline_time = 0;
        }
    }
    else
    {
        p_body->offline_time = role.role.offline_time;
    }

    //缓存种植园里的植物
    uint8_t hole_count = 0;

    taomee::unpack_h(p_db_rsp, hole_count, idx);//解析获得实际种植的植物数量，实际并不用管这个字段

    hole_count = 0;

    taomee::unpack_h(p_db_rsp, hole_count, idx);//解析得到坑的数量
    if(hole_count != PLANTATION_NUM)
    {
        KCRIT_LOG(p_user->uid, "hole_count(%u) not equal to 3", hole_count);
        return -1;
    }

    char buffer[1024] = {0};
    all_hole_info_t *p_all_hole = (all_hole_info_t*)buffer;
    p_all_hole->plant_count = 0;
    p_all_hole->hole_count = hole_count;

    p_body->plant_count = hole_count;
    for (int i = 0; i < hole_count; ++i)
    {
        taomee::unpack_h(p_db_rsp, p_all_hole->hole[i].hole_id, idx);
        taomee::unpack_h(p_db_rsp, p_all_hole->hole[i].plant_id, idx);
        taomee::unpack_h(p_db_rsp, p_all_hole->hole[i].color, idx);
        taomee::unpack_h(p_db_rsp, p_all_hole->hole[i].growth, idx);
        taomee::unpack_h(p_db_rsp, p_all_hole->hole[i].last_growth_time, idx);
        taomee::unpack_h(p_db_rsp, p_all_hole->hole[i].last_add_extra_growth_time, idx);
        taomee::unpack_h(p_db_rsp, p_all_hole->hole[i].last_growth_value, idx);
        taomee::unpack_h(p_db_rsp, p_all_hole->hole[i].maintain, idx);
        taomee::unpack_h(p_db_rsp, p_all_hole->hole[i].next_maintain_time, idx);
        taomee::unpack_h(p_db_rsp, p_all_hole->hole[i].last_reward_id, idx);
        taomee::unpack_h(p_db_rsp, p_all_hole->hole[i].maintain_count, idx);


        KINFO_LOG(p_user->uid, "DB plant[hole_id:%u plant_id:%u growth:%u, last growth_time:%u, last_growth_value:%u, last_extra_time:%u, maintain:%u, next_maintain_time:%u, last_reward_id:%u, maintain_cout:%u]", p_all_hole->hole[i].hole_id, p_all_hole->hole[i].plant_id, p_all_hole->hole[i].growth, p_all_hole->hole[i].last_growth_time, p_all_hole->hole[i].last_growth_value,  p_all_hole->hole[i].last_add_extra_growth_time, p_all_hole->hole[i].maintain, p_all_hole->hole[i].next_maintain_time, p_all_hole->hole[i].last_reward_id, p_all_hole->hole[i].maintain_count);

        //计算植物增加的成长值
        p_body->plant_growth[i].hole_id = p_all_hole->hole[i].hole_id;
        if(p_all_hole->hole[i].plant_id == 0)
        {
            p_body->plant_growth[i].growth = 0;
            p_body->plant_growth[i].growth_time = 0;
            p_body->plant_growth[i].last_growth_value = 0;
            p_body->plant_growth[i].last_add_extra_growth_time = 0;
            p_body->plant_growth[i].new_maintain_type = NOT_MAINTAIN;
            p_body->plant_growth[i].next_maintain_time = 0;
        }
        else
        {
            p_all_hole->plant_count += 1;
            p_body->plant_growth[i].growth = get_plant_growth_add(mon_happy, p_all_hole->hole[i].last_growth_time);
            p_body->plant_growth[i].new_maintain_type = p_all_hole->hole[i].maintain;
            if(p_all_hole->hole[i].growth >= MATURITY_GROWTH_VALUE)
            {
                p_body->plant_growth[i].growth = 0;
                p_body->plant_growth[i].new_maintain_type = MAINTAIN_GET;
            }
            else if (p_body->plant_growth[i].growth + p_all_hole->hole[i].growth >= MATURITY_GROWTH_VALUE)
            {
                p_body->plant_growth[i].growth = MATURITY_GROWTH_VALUE - p_all_hole->hole[i].growth;
                p_body->plant_growth[i].new_maintain_type = MAINTAIN_GET;
             }

            p_body->plant_growth[i].growth_time = now;
            p_body->plant_growth[i].last_growth_value = 0;
            p_body->plant_growth[i].last_add_extra_growth_time = 0;
            p_body->plant_growth[i].next_maintain_time = p_all_hole->hole[i].next_maintain_time;

            p_all_hole->hole[i].maintain = p_body->plant_growth[i].new_maintain_type;
            p_all_hole->hole[i].last_growth_time = now;
            p_all_hole->hole[i].growth += p_body->plant_growth[i].growth;
        }
    }

    p_user->user_cache.cached_all_hole(p_all_hole);

    //封测大礼的活动
        if(g_activity_map.find(LIMIT_ACTIVITY_ID) != g_activity_map.end())
        {
            uint32_t time_now = (uint32_t)time(NULL);
            if(g_activity_map[LIMIT_ACTIVITY_ID].start_timestamp <= time_now && g_activity_map[LIMIT_ACTIVITY_ID].end_timestamp >= time_now)
            {//活动尚未结束
                   if(role.role.limit_reward == 0)
                   {//未领取
                        if(role.role.register_time <= LIMIT_END_TIMESTATMP)
                        {//是封测用户
                            if(role.role.monster_level >= 18)
                            {
                                role.role.limit_reward = 2;
                            }
                            else
                            {
                                role.role.limit_reward = 1;
                            }
                        }
                        else
                        {//不是封测用户
                                role.role.limit_reward = 0;
                        }
                   }
                   else
                   {//已领取
                                role.role.limit_reward = 0;
                   }
            }
            else
            {//活动已结束
                                role.role.limit_reward = 0;
            }
        }
        else
        {
                                role.role.limit_reward = 0;
        }

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_update_login, p_user->uid, (p_user->session->fd << 16) | p_user->counter, sizeof(db_msg_update_login_req_t) + p_body->plant_count * sizeof(plant_growth_t));

    p_body->limit_reward = role.role.limit_reward;//封测大礼,由db给用户stuff表添加上封测大礼对应的物品

	if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        ERROR_LOG("send to db proxy server failed.");
        return -1;
    }


    if (p_user->user_cache.add_role(&role))
    {
        KERROR_LOG(p_user->uid, "cache user role");
        return -1;
    }


    return 0;
}

int process_db_update_login_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KERROR_LOG(p_user->uid, "db update login failed:%u", p_db_pkg->result);
        return -1;
    }

    KINFO_LOG(p_user->uid, "db_update_login_return, get user badge list");


    all_badge_info_t *p_rsp = (all_badge_info_t *)p_db_pkg->body;

    //加入缓存
    KINFO_LOG(p_user->uid, "add badge to cached, badge_num:%u", p_rsp->badge_num);

    p_user->user_cache.add_all_badge(p_rsp);//将成就项加入缓存

    KINFO_LOG(p_user->uid, "get friend list from db.");
    //请求好友列表
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_id, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_friend_list_req_t));
    db_msg_friend_list_req_t *p_body = (db_msg_friend_list_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->begin = 0;
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        ERROR_LOG("send to db proxy server failed.");
        return -1;
    }

    db_msg_friend_list_req_t *p_req = (db_msg_friend_list_req_t *)p_user->buffer;
    p_req->begin = 0;

    return 0;

    //拉取用户的成就项列表
    //svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    //pack_svr_pkg_head(p_header, svr_msg_db_get_all_badge, p_user->uid, GET_SVR_SN(p_user), 0);

    //if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    //{
    //    g_errno = ERR_MSG_DB_PROXY;
    //    ERROR_LOG("send to db proxy server failed.");
    //    return -1;
    //}
    //return 0;
}

int process_db_add_role_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KERROR_LOG(p_user->uid, "db add role failed:%u", p_db_pkg->result);
        return -1;
    }

    p_user->is_register = 1;
    send_header_to_user(p_user, 0, ERR_NO_ERR);

    db_msg_friend_list_rsp_t *p_friend = (db_msg_friend_list_rsp_t *)p_user->buffer;
    p_friend->count = 1;
    p_friend->friend_info[0].friend_id = config_get_intval("default_friend_id", 0);
    p_friend->friend_info[0].type = FRIEND_DEFAULT;
    p_friend->friend_info[0].is_bestfriend = 0;
    //p_user->friend_tree.add_friend_list(p_friend);

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KERROR_LOG(p_user->uid, "get role failed");
        return -1;
    }
    p_role->role.friend_num = 1;
    p_role->role.approved_message_num = 1;
    p_role->role.unapproved_message_num= 0;
    //记下用户的名字对应的米米号，添加好友可以用名字搜索,不需要处理返回信息
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    db_msg_insert_name_req_t *p_body = (db_msg_insert_name_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, svr_msg_db_insert_name, p_user->uid, 0, sizeof(db_msg_insert_name_req_t));
    memcpy(p_body->name, p_role->role.name, sizeof(p_role->role.name));

    if (g_p_db_proxy_svr->send_data(NULL, g_send_buffer, p_header->len, NULL) != 0)
    {
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
    }

	if(p_role->role.invitor_id)
	{
		p_role->role.friend_num++;
		p_friend->count++;
		p_friend->friend_info[1].friend_id = p_role->role.invitor_id;
		p_friend->friend_info[1].type = FRIEND_DEFAULT;
		p_friend->friend_info[1].is_bestfriend = 0;

		db_msg_friend_apply_req_t *req = (db_msg_friend_apply_req_t *)(g_send_buffer+sizeof(svr_msg_header_t));
		req->peer_id = p_user->uid;
		req->type = FRIEND_DEFAULT;
		strcpy(req->remark, INVITE_FRIEND_REMARK);
		req->remark_count = strlen(req->remark);

	    pack_svr_pkg_head(p_header, svr_msg_db_apply_for_friend, p_role->role.invitor_id, 0, sizeof(db_msg_friend_apply_req_t)+req->remark_count);

	    if (g_p_db_proxy_svr->send_data(NULL, g_send_buffer, p_header->len, NULL) != 0)
	    {
	        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
	    }

		stat_one_param_t stat_data = {p_user->uid};
		msg_log(stat_register_throw_link, &stat_data, sizeof(stat_data));
	}

	p_user->friend_tree.add_friend_list(p_friend);
    //统计注册人数
    stat_one_param_t stat_data1 = {p_user->uid};
    msg_log(stat_register_num, &stat_data1, sizeof(stat_data1));
    //性别统计
    stat_two_param_t stat_data2 = {p_user->uid, p_role->role.gender == 1 ? 1 : 2};
    msg_log(stat_gender_distr, &stat_data2, sizeof(stat_data2));
    //年龄统计
    stat_two_param_t stat_data3 = {p_user->uid, get_timestamp(p_role->role.birthday)};
    msg_log(stat_age_distr, &stat_data3, sizeof(stat_data3));
    //怪兽数量统计
    stat_one_param_t stat_data4 = {1};
    msg_log(stat_monster_num + p_role->role.monster_id, &stat_data4, sizeof(stat_data4));

     //通知分享服务
     pack_as_pkg_header(p_user->uid, online_msg_login_user, 0, ERR_NO_ERR);
     g_send_msg.pack(p_role->role.gender);
     g_send_msg.pack(p_role->role.name, sizeof(p_role->role.name));
     g_send_msg.end();
     g_p_share_svr->send_data(NULL, g_send_msg.get_msg(), g_send_msg.get_msg_len(), NULL);

     //通知switch有用户登录
     pack_svr_pkg_head(p_header, svr_msg_login, p_user->uid, GET_SVR_SN(p_user), 0);
     //没有返回包，不需要加定时器
     g_p_switch_svr->send_data(NULL, g_send_buffer, p_header->len, NULL);

    return 0;
}

int process_db_get_bag_stuff_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KCRIT_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db get bag failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    const db_msg_get_bag_rsp_t *p_bag = (const db_msg_get_bag_rsp_t *)p_db_pkg->body;

    int num = 0;
    char buffer[MAX_SEND_PKG_SIZE] = {0};
    db_msg_get_bag_rsp_t *p_avail_stuff = (db_msg_get_bag_rsp_t *)buffer;

    for (int i = 0; i != p_bag->num; ++i)
    {
        if (p_bag->stuff[i].used_num < p_bag->stuff[i].stuff_num)                  //还可以使用的物品
        {
            p_avail_stuff->stuff[num].stuff_id = p_bag->stuff[i].stuff_id;
            p_avail_stuff->stuff[num].stuff_num = p_bag->stuff[i].stuff_num - p_bag->stuff[i].used_num;
            ++num;
        }
    }

    //将可以使用的物品返回给as
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack((uint16_t)num);
    for (int i = 0; i != num; ++i)
    {
#ifdef DEBUG
        KINFO_LOG(p_user->uid, "stuff:%u, num:%u", p_avail_stuff->stuff[i].stuff_id, p_avail_stuff->stuff[i].stuff_num);
#endif
        g_send_msg.pack(p_avail_stuff->stuff[i].stuff_id);
        g_send_msg.pack(p_avail_stuff->stuff[i].stuff_num);
        map<uint32_t, item_t>::iterator iter = g_item_map.find(p_avail_stuff->stuff[i].stuff_id);
        if (iter == g_item_map.end())
        {
            KCRIT_LOG(p_user->uid, "stuff:%u is not exists.", p_avail_stuff->stuff[i].stuff_id);
            return -1;
        }
        g_send_msg.pack(iter->second.category);
    }
    KINFO_LOG(p_user->uid, "get %u stuff from db", num);

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    //缓存背包物品
    if (p_user->user_cache.add_bag(p_bag) != 0)
    {
        KCRIT_LOG(p_user->uid, "add bag stuff to cache failed.");
        return -1;
    }

    return 0;
}

int process_db_get_room_info_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db get room failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    as_msg_get_room_req_t *p_req = (as_msg_get_room_req_t *)p_user->buffer;
    if (p_req->user_id != p_db_pkg->user_id)
    {
        KCRIT_LOG(p_user->uid, "ret user:%u is not %u", p_db_pkg->user_id, p_req->user_id);
        return -1;
    }
    int len = p_db_pkg->len - sizeof(svr_msg_header_t);

    //如果是获得的自己的房间信息，则放入缓存
    if (p_db_pkg->user_id == p_user->uid)
    {
        room_value_t room = {0};
        room.buf_len = len;
        memcpy(room.buf, p_db_pkg->body, len);
        if (ROOM_DEFAULT_ID == p_req->room_id)
        {
            if (p_user->user_cache.add_default_room(&room) != 0)
            {
                KCRIT_LOG(p_user->uid, "add default room cache");
                g_errno = ERR_ADD_ROOM_BUF;
                return 0;
            }
        }
        else if (p_user->user_cache.add_cur_room(p_req->room_id, &room) != 0)
        {
            KCRIT_LOG(p_user->uid, "add room:%u cache", p_req->room_id);
            g_errno = ERR_ADD_ROOM_BUF;
            return 0;
        }
    }
    KINFO_LOG(p_user->uid, "get user:%u room:%u from db", p_req->user_id, p_req->room_id);

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_db_pkg->body, len);
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_db_update_room_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KCRIT_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "update room buf failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    //db更新成功，更改缓存里的房间数据
    room_update_t *p_room_cache = (room_update_t *)p_user->buffer;
    p_user->user_cache.update_room(p_room_cache);

    //db更新成功，更改缓存里的背包数据
    stuff_change_t *p_change = (stuff_change_t *)(p_user->buffer + sizeof(room_update_t) + p_room_cache->buf_len);
    for (int i = 0; i != (int)p_change->num; ++i)
    {
        uint32_t stuff_id = p_change->stuff_flow[i].stuff_id;
        uint16_t stuff_num = p_change->stuff_flow[i].stuff_num;
        uint8_t stuff_flow = p_change->stuff_flow[i].stuff_flow;

        if (stuff_flow == flow_bag_to_room)
        {
            p_user->user_cache.add_stuff_used(stuff_id, stuff_num);
            KINFO_LOG(p_user->uid, "stuff:%u used add %u", stuff_id, stuff_num);
        }
        else
        {
            p_user->user_cache.desc_stuff_used(stuff_id, stuff_num);
            KINFO_LOG(p_user->uid, "stuff:%u used desc %u", stuff_id, stuff_num);
        }
    }

    send_header_to_user(p_user, 0, ERR_NO_ERR);

    return 0;
}

int process_db_get_room_num_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KCRIT_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "get all room id failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    db_msg_room_id_rsp_t *p_room = (db_msg_room_id_rsp_t *)p_db_pkg->body;

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_room->num);
    for (int i = 0; i != (int)p_room->num; ++i)
    {
        g_send_msg.pack(p_room->room_id[i]);
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    //缓存所有房间id
    p_user->user_cache.add_room_id(p_room->room_id, p_room->num);
    return 0;
}

int process_db_add_stuff_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "add bag stuff failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    commit_stuff_cache_t *p_cm_stuff = (commit_stuff_cache_t*)p_user->buffer;
    db_msg_add_stuff_req_t *p_add_stuff = (db_msg_add_stuff_req_t*)&(p_cm_stuff->add_stuff);


    //更新金币和愉悦值
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "can't get usr role");
        return -1;
    }
    p_role->role.coins -= p_add_stuff->coins;
    p_cm_stuff->old_happy = p_role->role.monster_happy;
    p_role->role.monster_happy += p_add_stuff->happy;
    if(p_add_stuff->level_up != 0)
    {
        p_role->role.monster_level += p_add_stuff->level_up;
        unlock_map_item(p_user, p_role->role.monster_level);
        if(p_role->role.monster_level >= 8)
        {
            activity_open(p_user, OPEN_ACTIVITY_ID, NULL);
        }
  		update_level_relative_info(p_user, INVITOR_ACTIVITY_ID);
    }
    p_role->role.monster_exp += p_add_stuff->reward_exp;


    KINFO_LOG(p_user->uid, "cost coins:%u, now_coins:%u, add happy:%u  add exp:%u add level:%u", p_add_stuff->coins, p_role->role.coins, p_add_stuff->happy, p_add_stuff->reward_exp, p_add_stuff->level_up);

    //去除统计项  2012-06-07
    //统计金币的减少
    //if (p_add_stuff->coins)
    //{
   //     stat_two_param_t stat_data = {p_user->uid, p_add_stuff->coins};
    //    msg_log(stat_gudong_buy_stuff, &stat_data, sizeof(stat_data));
  //  }

    //更新背包物品
    for (int i = 0; i != p_add_stuff->count; ++i)
    {
        p_user->user_cache.add_stuff(p_add_stuff->stuff[i].stuff_id, p_add_stuff->stuff[i].stuff_num);
    }

    if (p_user->waitcmd == add_new_unlock_map)
    {
        p_user->waitcmd = 0;
        return 0;
    }

//购买物品


    if(p_add_stuff->reward_exp == 0)
    {//没有奖励经验，不需要更新db和缓存的天限制
        return update_plant_info_after_buy_stuff(p_user, p_cm_stuff);
    }
    else
    {
        //统计经验值奖励
        stat_two_param_t stat_data = {p_user->uid, p_add_stuff->reward_exp};
        msg_log(stat_add_exp_buy_stuff, &stat_data, sizeof(stat_data));


        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;

        db_msg_add_day_restrict_req_t *p_body = (db_msg_add_day_restrict_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

        p_body->type = STRICT_STUFF_EXP;
        p_body->time = timestamp_begin_day();

        p_body->value = p_cm_stuff->add_stuff.reward_exp;
        p_body->count = 1;
        p_body->restrict_stuff[0].value_id = p_cm_stuff->add_stuff.stuff[0].stuff_id;
        p_body->restrict_stuff[0].value = p_cm_stuff->add_stuff.stuff[0].stuff_num;

        pack_svr_pkg_head(p_header, svr_msg_db_add_day_restrict, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_add_day_restrict_req_t) + p_body->count * sizeof(restrict_value_t));
        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
            return 0;
        }
        return 0;

    }

    return 0;
}

int process_db_get_puzzle_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "get puzzle failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);

    const db_msg_puzzle_rsp_t *p_puzzle_rsp = (const db_msg_puzzle_rsp_t *)p_db_pkg->body;
    g_send_msg.pack(p_puzzle_rsp->num);
    for (int i = 0; i != (int)p_puzzle_rsp->num; ++i)
    {
        g_send_msg.pack(p_puzzle_rsp->puzzle[i].type);
        uint8_t is_same_day = timestamp_equal_day(time(NULL), p_puzzle_rsp->puzzle[i].last_playtime);
        if (-1 == is_same_day)
        {
            KCRIT_LOG(p_user->uid, "get day from timestamp failed.");
            return -1;
        }

        g_send_msg.pack(is_same_day);
        g_send_msg.pack(p_puzzle_rsp->puzzle[i].max_score);
        uint16_t avg_score = p_puzzle_rsp->puzzle[i].score / p_puzzle_rsp->puzzle[i].num;
        g_send_msg.pack(avg_score);
    //    KINFO_LOG(p_user->uid, "puzzle:%u, play today:%u, max_score:%u, avg_score:%u from db", p_puzzle_rsp->puzzle[i].type, is_same_day, p_puzzle_rsp->puzzle[i].max_score, avg_score);
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    //将数据加入缓存
    if (p_user->user_cache.add_puzzle(p_puzzle_rsp) != 0)
    {
        KCRIT_LOG(p_user->uid, "add puzzle info to cache failed.");
        return -1;
    }

    return 0;
}

int process_db_commit_puzzle_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "commit puzzle failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    puzzle_cache_t *p_puzzle_cache = (puzzle_cache_t*)p_user->buffer;
    db_msg_puzzle_req_t *p_puzzle = &(p_puzzle_cache->puzzle_reward);
  //  int extra_coin = *(uint16_t *)(p_user->buffer + sizeof(db_msg_puzzle_req_t));

    //统计益智游戏咕咚果产出
    stat_two_param_t stat_data1 = {p_user->uid, p_puzzle->reward.coins};
    msg_log(stat_puzzle_gudong, &stat_data1, sizeof(stat_data1));

    //增加用户的奖励
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "get role info from cache failed.");
        return -1;
    }
    p_role->role.coins += p_puzzle->reward.coins;
    p_role->role.monster_exp += p_puzzle->reward.exp;
    if (p_puzzle->reward.level != 0)
    {
        p_role->role.monster_level += p_puzzle->reward.level;
        unlock_map_item(p_user, p_role->role.monster_level);
        if(p_role->role.monster_level >= 8)
        {
            activity_open(p_user, OPEN_ACTIVITY_ID, NULL);
        }
		update_level_relative_info(p_user, INVITOR_ACTIVITY_ID);
    }
    p_puzzle_cache->old_happy = p_role->role.monster_happy;
    p_role->role.monster_happy += p_puzzle->reward.happy;
    if (p_puzzle->puzzle.max_score)
    {
        if(p_puzzle->puzzle.type == DAILY_PUZZLE)
        {//每日挑战
        //每日挑战达到25分成就项
            p_role->role.max_puzzle_score = p_puzzle->puzzle.max_score;
            badge_step(p_user, badge_daily_puzzle_25_const, p_puzzle->puzzle.max_score);
//			encourage_guide_to_db(p_user, DAILY_TEST, p_puzzle->puzzle.score);
        }
        else
        {//谜题大全 突破5次自己的记录
            badge_step(p_user, badge_miti_5_const, 1);
        }
    }
    KINFO_LOG(p_user->uid, "add coins:%u, now_coins:%u exp:%u, happy:%u by puzzle", p_puzzle->reward.coins, p_role->role.coins,  p_puzzle->reward.exp, p_puzzle->reward.happy);

    //更改益智游戏的缓存
    puzzle_info_t *p_puzzle_info = p_user->user_cache.get_puzzle(p_puzzle->puzzle.type);
    if (p_puzzle_info != NULL)
    {
        if (p_puzzle->puzzle.last_playtime != 0)
        {
            p_puzzle_info->last_playtime = p_puzzle->puzzle.last_playtime;
        }
        if (p_puzzle->puzzle.max_score != 0)
        {
            p_puzzle_info->max_score = p_puzzle->puzzle.max_score;
        }
        p_puzzle_info->total_score += p_puzzle->puzzle.score;
        p_puzzle_info->total_num += p_puzzle->puzzle.num;
		p_puzzle_info->play_times++;
    }
    else
    {
        //增加益智游戏信息
        puzzle_info_t puzzle_info = {p_puzzle->puzzle.last_playtime, p_puzzle->puzzle.max_score, p_puzzle->puzzle.score, p_puzzle->puzzle.num, 1};
        p_user->user_cache.add_puzzle_type(p_puzzle->puzzle.type, &puzzle_info);
    }

    if(p_puzzle->reward.exp == 0)
    {//没有奖励经验，不需要更新db和缓存
        return update_plant_info_after_commit_puzzle_return(p_user, p_puzzle_cache);
    }
    else
    {
        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;

        db_msg_add_day_restrict_req_t *p_body = (db_msg_add_day_restrict_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->type = STRICT_PUZZLE_EXP;
        p_body->time = timestamp_begin_day();

        p_body->value = p_puzzle->reward.exp;
        p_body->count = 1;
        p_body->restrict_stuff[0].value_id = 0;
        p_body->restrict_stuff[0].value = 0;

        pack_svr_pkg_head(p_header, svr_msg_db_add_day_restrict, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_add_day_restrict_req_t) + p_body->count * sizeof(restrict_value_t));
        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
            return 0;
        }
        return 0;
    }

    return 0;

}

//每次怪兽增加愉悦值时都会进这个函数
int process_db_update_plant_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "update plant growth failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }


    switch (p_user->waitcmd)
    {
        case as_msg_commit_puzzle:
            {
                KINFO_LOG(p_user->uid, "update_plant after commit_puzzle add day_restrict exp");
                puzzle_cache_t *p_puzzle = (puzzle_cache_t *)p_user->buffer;

                if(update_plant_info_in_cache(p_user, &(p_puzzle->plant_change)) != 0)
                {
                    KCRIT_LOG(p_user->uid, "update plant_info_in_cache failed in as_msg_commit_puzzle return");
                }

                //返回益智游戏奖励信息
                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_puzzle->puzzle_reward.puzzle.type);
                g_send_msg.pack((uint16_t)(p_puzzle->puzzle_reward.reward.coins - p_puzzle->extra_coins));
                g_send_msg.pack(p_puzzle->puzzle_reward.reward.exp);
                g_send_msg.pack(p_puzzle->puzzle_reward.reward.happy);
                g_send_msg.pack(p_puzzle->extra_coins);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

                notify_as_current_user_info(p_user);

                if(p_puzzle->puzzle_reward.puzzle.type == DAILY_PUZZLE)
                {//每日挑战
			        encourage_guide_to_db(p_user, DAILY_TEST, p_puzzle->puzzle_reward.puzzle.score);
                }
        break;
            }
        case as_msg_get_sun_reward:
            {
                KINFO_LOG(p_user->uid, "update_plant after get_sun_reward");
                sun_reward_happy_cache_t *p_cache = (sun_reward_happy_cache_t*)p_user->buffer;
                if(update_plant_info_in_cache(p_user, &(p_cache->plant_change)) != 0)
                {
                    KCRIT_LOG(p_user->uid, "update plant_info_in_cache failed in as_msg_commit_puzzle return");
                }

                //给as回成功消息
                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_cache->sun_reward_happy.sun_reward.hole_id);
                g_send_msg.pack(p_cache->sun_reward_happy.sun_reward.reward_id);
                g_send_msg.pack(p_cache->sun_reward_happy.reward_happy);
                g_send_msg.pack((uint16_t)0);
                g_send_msg.pack((uint16_t)0);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                KINFO_LOG(p_user->uid, "update_plant  sun_reward_happy:reward_happy:%u", p_cache->sun_reward_happy.reward_happy);

                 notify_as_current_user_info(p_user);
                break;
            }
        case as_msg_eat_food:
            {
                db_msg_update_plant_req_t *p_change = (db_msg_update_plant_req_t *)p_user->buffer;
                update_plant_info_in_cache(p_user, p_change);
                send_header_to_user(p_user, 0, ERR_NO_ERR);

                notify_as_current_user_info(p_user);
                break;
            }
        case as_msg_buy_stuff:
            {
                KINFO_LOG(p_user->uid, "update_plant after buy_stuff add day_rest return ");
                commit_stuff_cache_t *p_stuff = (commit_stuff_cache_t *)p_user->buffer;

                if(update_plant_info_in_cache(p_user, &(p_stuff->plant_change)) != 0)
                {
                    KCRIT_LOG(p_user->uid, "update plant_info_in_cache failed in as_msg_commit_puzzle return");
                }


                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_stuff->add_stuff.stuff[0].stuff_id);
                g_send_msg.pack(p_stuff->category);
                g_send_msg.pack(p_stuff->add_stuff.reward_exp);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

                notify_as_current_user_info(p_user);
                break;
            }
        case as_msg_get_plant_info:
            {
                db_msg_update_plant_req_t *p_change = (db_msg_update_plant_req_t *)p_user->buffer;

                char buffer[200] = {0};
                all_hole_info_t *p_all_hole = (all_hole_info_t *)buffer;
                if (p_user->user_cache.get_all_hole(p_all_hole) == 1)
                {
                    KCRIT_LOG(p_user->uid, "get hole in cache failed");
                    return -1;
                }


                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_all_hole->hole_count);
                KINFO_LOG(p_user->uid, "all_hole:%u", p_all_hole->hole_count);

                uint8_t pet_flag = 0;
                int seed_match[PLANTATION_NUM] = {0};
                uint32_t pet_id = 0;

                for(int idx = 0; idx < (int)p_all_hole->hole_count; idx++)
                {
                    p_all_hole->hole[idx].growth += p_change->plant_growth[idx].growth;
                }

                if(all_plant_maturity(p_all_hole) == 1)
                {//全部成熟
                    //判断精灵匹配情况
                    pet_match(p_all_hole, &pet_id, seed_match);
                    for(int i = 0; i < (int)p_all_hole->hole_count; i++)
                    {
                        g_send_msg.pack(p_all_hole->hole[i].plant_id);
                        g_send_msg.pack(p_all_hole->hole[i].hole_id);
                        g_send_msg.pack(p_all_hole->hole[i].color);
                        g_send_msg.pack(p_all_hole->hole[i].last_growth_value);
                        g_send_msg.pack(p_all_hole->hole[i].growth);
                KINFO_LOG(p_user->uid, "send to as plant[hole_id:%u plant_id:%u growth:%u, last growth_time:%u, last_growth_value:%u, last_extra_time:%u, maintain:%u,next_maintain_time:%u, last_reward_id:%u, maintain_cout:%u]", p_all_hole->hole[i].hole_id, p_all_hole->hole[i].plant_id, p_all_hole->hole[i].growth, p_all_hole->hole[i].last_growth_time, p_all_hole->hole[i].last_growth_value,  p_all_hole->hole[i].last_add_extra_growth_time, p_all_hole->hole[i].maintain, p_all_hole->hole[i].next_maintain_time, p_all_hole->hole[i].last_reward_id, p_all_hole->hole[i].maintain_count);
                        if(pet_id == 0)
                        {//没有匹配成功，点击收获
                            g_send_msg.pack((uint8_t)MAINTAIN_GET);
                            pet_flag = PLANTATION_NOT_MATCH;
                        }
                        else
                        {//匹配成功，不需要维护
                            g_send_msg.pack((uint8_t)NOT_MAINTAIN);
                            pet_flag = PLANTATION_ATTRACT_PET;
                        }

			//获取所有奖励信息，返回给as即可
                 	char buf[2048] = {0};
                	single_hole_reward_t *p_hole_reward = (single_hole_reward_t*)buf;
                	if(-1 == p_user->user_cache.get_hole_reward(p_all_hole->hole[i].hole_id, p_hole_reward))
                	{
                    		KCRIT_LOG(p_user->uid, "get user hole %u reward failed", p_all_hole->hole[i].hole_id);
                    		g_send_msg.pack((uint32_t)0);
                	}
                	else
                	{
                    		g_send_msg.pack(p_hole_reward->reward_num);
                    		for(int j = 0; j < (int)p_hole_reward->reward_num; j++)
                    		{
                        		g_send_msg.pack(p_hole_reward->reward_id[j]);
                    		}
                	}
                    }
                }
                else
                {//没有全部成熟
                    for(int i = 0; i < (int)p_all_hole->hole_count; i++)
                    {
                        g_send_msg.pack(p_all_hole->hole[i].plant_id);
                        g_send_msg.pack(p_all_hole->hole[i].hole_id);
                        g_send_msg.pack(p_all_hole->hole[i].color);
                        g_send_msg.pack(p_all_hole->hole[i].last_growth_value);
                        g_send_msg.pack(p_all_hole->hole[i].growth);
                KINFO_LOG(p_user->uid, "send to as plant not matury[hole_id:%u plant_id:%u growth:%u, last growth_time:%u, last_growth_value:%u, last_extra_time:%u,  cache_maintain:%u, new_maintain_type:%u next_maintain_time:%u, last_reward_id:%u, maintain_cout:%u]", p_all_hole->hole[i].hole_id, p_all_hole->hole[i].plant_id, p_all_hole->hole[i].growth, p_all_hole->hole[i].last_growth_time, p_all_hole->hole[i].last_growth_value,  p_all_hole->hole[i].last_add_extra_growth_time, p_all_hole->hole[i].maintain, p_change->plant_growth[i].new_maintain_type, p_all_hole->hole[i].next_maintain_time, p_all_hole->hole[i].last_reward_id, p_all_hole->hole[i].maintain_count);
                        if(p_all_hole->hole[i].growth >= MATURITY_GROWTH_VALUE)
                        {//此颗植物已经成熟，只能点击收获
                            g_send_msg.pack((uint8_t)MAINTAIN_GET);
                        }
                        else
                        {//匹配成功，不需要维护
                            g_send_msg.pack((uint8_t)p_change->plant_growth[i].new_maintain_type);
                        }
                        //获取所有奖励信息，返回给as即可
                        char buf[2048] = {0};
                        single_hole_reward_t *p_hole_reward = (single_hole_reward_t*)buf;
                        if(-1 == p_user->user_cache.get_hole_reward(p_all_hole->hole[i].hole_id, p_hole_reward))
                        {
                            KCRIT_LOG(p_user->uid, "get user hole %u reward failed", p_all_hole->hole[i].hole_id);
                            g_send_msg.pack((uint32_t)0);
                        }
                        else
                        {
                            KINFO_LOG(p_user->uid, "reward_num:%u", p_hole_reward->reward_num);
                            g_send_msg.pack(p_hole_reward->reward_num);
                            for(int j = 0; j < (int)p_hole_reward->reward_num; j++)
                            {
                                g_send_msg.pack(p_hole_reward->reward_id[j]);
                            }
                        }
                    }
                    pet_flag = PLANTATION_UNATTRACT_PET;
                }


                //统计有没有匹配到精灵

                stat_one_param_t stat_data = {1};
                g_send_msg.pack(pet_flag);

                if(pet_flag ==  PLANTATION_NOT_MATCH)
                {
                    for (int i = 0; i < PLANTATION_NUM; ++i)
                    {
                        g_send_msg.pack((uint8_t)(seed_match[i] == 0 ? 0 : 1));
                    }
                    KINFO_LOG(p_user->uid, "not match pet");
                    msg_log(stat_not_attract_pet, &stat_data, sizeof(stat_data));
                }
                else if(pet_flag == PLANTATION_ATTRACT_PET)
                {
                        g_send_msg.pack(pet_id);
                        KINFO_LOG(p_user->uid, "db matched pet %u", pet_id);
                        p_user->user_cache.add_attract_pet(pet_id);
                        msg_log(stat_attract_pet, &stat_data, sizeof(stat_data));
                }


                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

                if(0 != update_plant_info_in_cache(p_user, p_change))
                {
                    KCRIT_LOG(p_user->uid, "update plant info in cache failed in as_msg_get_plant_info case.");
                }
                break;
            }
        default:
            KCRIT_LOG(p_user->uid, "msg:%u is invalid", p_user->waitcmd);
            return -1;
    }

    return 0;
}

int process_db_other_role_info_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "get role:%u info failed:%u", p_db_pkg->user_id, p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    const as_msg_register_rsp_t *p_db_rsp = (const as_msg_register_rsp_t *)p_db_pkg->body;
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    time_t now = time(NULL);
    uint32_t mon_happy = get_new_happy(now, p_db_rsp->role.last_login_time, p_db_rsp->role.monster_happy);
    uint32_t mon_health = get_other_monster_health(now, p_db_rsp->role.last_login_time, p_db_rsp->role.last_logout_time, p_db_rsp->role.monster_health);

    KINFO_LOG(p_user->uid, "get user:%u, happy:%u, health:%u, coins:%u, friend:%u, approved:%u, unapproved:%u", p_db_pkg->user_id, mon_happy, mon_health, p_db_rsp->role.coins, p_db_rsp->role.friend_num, p_db_rsp->role.approved_message_num, p_db_rsp->role.unapproved_message_num);

    g_send_msg.pack((uint8_t)1);
    g_send_msg.pack(p_db_rsp->role.name, sizeof(p_db_rsp->role.name));
    g_send_msg.pack(p_db_rsp->role.gender);
    g_send_msg.pack(p_db_rsp->role.country_id);
    g_send_msg.pack(p_db_rsp->role.user_type);
    g_send_msg.pack(p_db_rsp->role.birthday);
    g_send_msg.pack(p_db_rsp->role.mood);
    g_send_msg.pack(p_db_rsp->role.fav_color);
    g_send_msg.pack(p_db_rsp->role.fav_pet);
    g_send_msg.pack(p_db_rsp->role.fav_fruit);
    g_send_msg.pack(p_db_rsp->role.personal_sign, sizeof(p_db_rsp->role.personal_sign));
    g_send_msg.pack(p_db_rsp->role.room_num);
    g_send_msg.pack(p_db_rsp->role.pet_num);
    g_send_msg.pack(p_db_rsp->role.max_puzzle_score);
    g_send_msg.pack(p_db_rsp->role.register_time);
    g_send_msg.pack(p_db_rsp->role.coins);
    g_send_msg.pack(p_db_rsp->role.last_login_time);
	//whether fist login today
	g_send_msg.pack(p_db_rsp->role.is_first_login);
    g_send_msg.pack(p_db_rsp->role.monster_id);
    g_send_msg.pack(p_db_rsp->role.monster_name, sizeof(p_db_rsp->role.monster_name));
    g_send_msg.pack(p_db_rsp->role.monster_main_color);
    g_send_msg.pack(p_db_rsp->role.monster_ex_color);
    g_send_msg.pack(p_db_rsp->role.monster_eye_color);
    g_send_msg.pack(p_db_rsp->role.monster_exp);
    g_send_msg.pack(p_db_rsp->role.monster_level);
    g_send_msg.pack(mon_health);
    g_send_msg.pack(mon_happy);
    g_send_msg.pack(p_db_rsp->role.approved_message_num);
    g_send_msg.pack(p_db_rsp->role.unapproved_message_num);
    g_send_msg.pack(p_db_rsp->role.friend_num);
    g_send_msg.pack((uint8_t)(p_db_rsp->pending_req_num != 0 ? 1 : 0));
    g_send_msg.pack(p_db_rsp->role.thumb);
    g_send_msg.pack(p_db_rsp->role.visits);
    //这地方还需要打包一个最近来访未读数
    g_send_msg.pack(p_db_rsp->role.recent_unread_badge);
    g_send_msg.pack(p_db_rsp->role.npc_score);//打包npc评分

    int idx = sizeof(as_msg_register_rsp_t);


    uint16_t pet_count = 0;
    taomee::unpack_h(p_db_rsp, pet_count, idx);

    int loop_count = 3;
    int begin = 0;
    if(pet_count < 3)
    {
        loop_count = pet_count;
    }
    else
    {
        begin = uniform_rand(0, pet_count - 3);
    }

    g_send_msg.pack((uint8_t)loop_count);
    int pet_num = 0;
    for (int i = 0; i < pet_count; ++i)
    {
        uint32_t pet_id = 0;
        taomee::unpack_h(p_db_rsp, pet_id, idx);
        if(pet_count - begin > i)
        {
            g_send_msg.pack(pet_id);
            pet_num++;
            if(pet_num >= loop_count)
            {
                break;
            }
        }
    }
    g_send_msg.end();

    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    pack_as_pkg_header(p_user->uid, ucount_msg_add_visit, 0, ERR_NO_ERR);
    g_send_msg.pack(p_db_pkg->user_id);
    g_send_msg.end();

    cache_a_pkg(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_db_get_pinboard_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "get user:%u pinboard failed:%u", p_db_pkg->user_id, p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }
    pinboard_count_t *p_count = (pinboard_count_t *)p_user->buffer;
    if (p_count->peer_id != p_db_pkg->user_id)
    {
        KCRIT_LOG(p_user->uid, "get user:%u pinboard, return user:%u", p_count->peer_id, p_db_pkg->user_id);
        return -1;
    }
    p_count->count = 0;

    int idx = 0;
    uint8_t count = 0;
    uint8_t reverse_flag = 0;
    taomee::unpack_h(p_db_pkg->body, reverse_flag, idx);
    taomee::unpack_h(p_db_pkg->body, count, idx);

    pinboard_t *p_pinboard = (pinboard_t *)(p_user->buffer + sizeof(pinboard_count_t));
    p_pinboard->count = count;


	int i = 0;
    for (int ij = 0; ij < (int)count; ++ij)
    {
        if(reverse_flag)
        {
                i = ij;
        }
        else
        {
            i = count - ij - 1;
        }

        taomee::unpack_h(p_db_pkg->body, p_pinboard->message[i].message.id, idx);
        taomee::unpack_h(p_db_pkg->body, p_pinboard->message[i].message.peer_id, idx);
        taomee::unpack_h(p_db_pkg->body, p_pinboard->message[i].message.icon, idx);
        taomee::unpack_h(p_db_pkg->body, p_pinboard->message[i].message.color, idx);
        taomee::unpack_h(p_db_pkg->body, p_pinboard->message[i].message.status, idx);
        taomee::unpack_h(p_db_pkg->body, p_pinboard->message[i].message.create_time, idx);
        taomee::unpack_h(p_db_pkg->body, p_pinboard->message[i].message.message_count, idx);
        taomee::unpack(p_db_pkg->body, p_pinboard->message[i].message.message, p_pinboard->message[i].message.message_count, idx);
#ifdef DEBUG
        KINFO_LOG(p_user->uid, "message id:%u, peer:%u, icon:%u, color:%u, status:%u, create_time:%u", p_pinboard->message[i].message.id, p_pinboard->message[i].message.peer_id, p_pinboard->message[i].message.icon, p_pinboard->message[i].message.color, p_pinboard->message[i].message.status, p_pinboard->message[i].message.create_time);
#endif
    }

    KINFO_LOG(p_user->uid, "get %u message from db", count);
    if (0 == count)
    {
        if (p_count->peer_id == p_user->uid)
        {
            pinboard_t cache_pinboard = {0};
            if (p_user->user_cache.add_pinboard(p_count->page, &cache_pinboard) != 0)
            {
                KCRIT_LOG(p_user->uid, "add pinboard to cache");
                return -1;
            }
        }

        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(count);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

        return 0;
    }

    //向db请求第一个用户的信息
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_pinboard->message[0].message.peer_id, GET_SVR_SN(p_user), 0);
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_db_get_friend_info_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    switch (p_user->waitcmd)
    {
        case as_msg_get_pinboard:
            {
                pinboard_count_t *p_count = (pinboard_count_t *)p_user->buffer;
                pinboard_t *p_pinboard = (pinboard_t *)(p_user->buffer + sizeof(pinboard_count_t));
                if (p_db_pkg->result != 0)
                {
                    //某个用户信息出错，不影响留言板的打开
                    KCRIT_LOG(p_user->uid, "get user:%u info failed:%u", p_db_pkg->user_id, p_db_pkg->result);
                }
                else
                {
                    memcpy(&p_pinboard->message[p_count->count].user_info, p_db_pkg->body, sizeof(db_msg_get_friend_rsp_t));
                }

                p_count->count++;
                if (p_count->count == p_pinboard->count) //已经获得所有留言用户的信息
                {
                    if (p_count->peer_id == p_user->uid && p_count->page < PINBOARD_CACHE_NUM)
                    {
                        if (p_user->user_cache.add_pinboard(p_count->page, p_pinboard) != 0)
                        {
                            KCRIT_LOG(p_user->uid, "cache pinboard failed");
                            return -1;
                        }
                    }

                    //给用户回留言板消息
                    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                    uint8_t count = p_pinboard->count;
                    g_send_msg.pack(count);
                    KINFO_LOG(p_user->uid, "get %u message from db", count);
                    for (int i = 0; i != (int)count; ++i)
                    {
                        g_send_msg.pack(p_pinboard->message[i].message.id);
                        g_send_msg.pack(p_pinboard->message[i].message.icon);
                        g_send_msg.pack(p_pinboard->message[i].message.color);
                        g_send_msg.pack(p_pinboard->message[i].message.peer_id);
                        g_send_msg.pack(p_pinboard->message[i].user_info.name, sizeof(p_pinboard->message[i].user_info.name));
                        g_send_msg.pack(p_pinboard->message[i].user_info.user_type);
                        g_send_msg.pack(p_pinboard->message[i].user_info.monster_id);
                        g_send_msg.pack(p_pinboard->message[i].user_info.monster_level);
                        g_send_msg.pack(p_pinboard->message[i].user_info.monster_main_color);
                        g_send_msg.pack(p_pinboard->message[i].user_info.monster_ex_color);
                        g_send_msg.pack(p_pinboard->message[i].user_info.monster_eye_color);
                        g_send_msg.pack(p_pinboard->message[i].message.create_time);
                        g_send_msg.pack(p_pinboard->message[i].message.status);
                        uint16_t message_count = p_pinboard->message[i].message.message_count;
                        g_send_msg.pack(message_count);
                        g_send_msg.pack(p_pinboard->message[i].message.message, message_count);
#ifdef DEBUG
                        KINFO_LOG(p_user->uid, "pinboard id:%u, icon:%u, color:%u, peer_id:%u, user_type:%u, monster_id:%u, message_count:%u from db",
                                p_pinboard->message[i].message.id, p_pinboard->message[i].message.icon, p_pinboard->message[i].message.color, p_pinboard->message[i].message.peer_id,
                                p_pinboard->message[i].user_info.user_type, p_pinboard->message[i].user_info.monster_id, p_pinboard->message[i].message.message_count);
#endif
                    }
                    g_send_msg.end();
                    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

                    return 0;
                }

                //向db请求下一个用户的信息
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_pinboard->message[p_count->count].message.peer_id, GET_SVR_SN(p_user), 0);
                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
                    return 0;
                }

                break;
            }
        case as_msg_get_friend_list:
            {
                friend_cache_t *p_friend_cache = (friend_cache_t *)p_user->buffer;
                int count = p_friend_cache->cur_count;
                friend_list_t friend_list = {{0}};
                if (p_db_pkg->result != 0)
                {
                    KCRIT_LOG(p_user->uid, "get user:%u info failed:%u", p_db_pkg->user_id, p_db_pkg->result);
                }
                else
                {
                    if (p_db_pkg->user_id != p_friend_cache->friend_list.friend_info[count].friend_id)
                    {
                        KCRIT_LOG(p_user->uid, "ret user:%u is not %u", p_db_pkg->user_id, p_friend_cache->friend_list.friend_info[count].friend_id);
                        return -1;
                    }

                    friend_list.user_id = p_db_pkg->user_id;
                    friend_list.is_best_friend = p_friend_cache->friend_list.friend_info[count].is_bestfriend;
                    memcpy(&friend_list, p_db_pkg->body, sizeof(db_msg_get_friend_rsp_t));
                }

                p_friend_cache->cur_count = ++count;
                if (p_user->friend_tree.add_friend(p_friend_cache->peer_id, &friend_list) != 0)
                {
                    KCRIT_LOG(p_user->uid, "user:%u add friend:%u to cache failed.", p_friend_cache->peer_id, friend_list.user_id);
                    return -1;
                }
                KINFO_LOG(p_user->uid, "cur count:%u, total count:%u", count, p_friend_cache->friend_list.count);
                if (count == p_friend_cache->friend_list.count)    //好友信息拉取完了
                {
                    uint16_t all_friend_num = 0;
                    friend_rsp_t *p_rsp = (friend_rsp_t *)p_user->buffer;
                    if (p_user->friend_tree.get_friend_tree(p_friend_cache->peer_id, (p_friend_cache->page - 1 ) * p_friend_cache->page_num + 1, p_friend_cache->page * p_friend_cache->page_num, p_rsp, &all_friend_num) != 0)
                    {
                        KCRIT_LOG(p_user->uid, "get user:%u friend tree", p_friend_cache->peer_id);
                        return -1;
                    }
                    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                    g_send_msg.pack(all_friend_num);
                    g_send_msg.pack((uint8_t)p_rsp->count);
                    for (int i = 0; i != p_rsp->count; ++i)
                    {
                        g_send_msg.pack(p_rsp->friend_list[i].user_id);
                        g_send_msg.pack(p_rsp->friend_list[i].name, sizeof(p_rsp->friend_list[i].name));
                        g_send_msg.pack(p_rsp->friend_list[i].gender);
                        g_send_msg.pack(p_rsp->friend_list[i].country_id);
                        g_send_msg.pack(p_rsp->friend_list[i].birthday);
                        g_send_msg.pack(p_rsp->friend_list[i].user_type);
                        g_send_msg.pack(p_rsp->friend_list[i].mon_id);
                        g_send_msg.pack(p_rsp->friend_list[i].mon_level);
                        g_send_msg.pack(p_rsp->friend_list[i].mon_main_color);
                        g_send_msg.pack(p_rsp->friend_list[i].mon_ex_color);
                        g_send_msg.pack(p_rsp->friend_list[i].mon_eye_color);
                        g_send_msg.pack(p_rsp->friend_list[i].last_login_time);
                        g_send_msg.pack(p_rsp->friend_list[i].is_best_friend);
                    //    KINFO_LOG(p_user->uid, "get friend:%u, name:%s, user_type:%u, is_best_friend:%u.", p_rsp->friend_list[i].user_id, p_rsp->friend_list[i].name, p_rsp->friend_list[i].user_type, p_rsp->friend_list[i].is_best_friend);
                    }
                    g_send_msg.end();
                    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                }
                else
                {
                    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_friend_cache->friend_list.friend_info[count].friend_id, GET_SVR_SN(p_user), 0);
#ifdef DEBUG
                    KINFO_LOG(p_user->uid, "get user:%u info", p_friend_cache->friend_list.friend_info[count].friend_id);
#endif
                    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                    {
                        g_errno = ERR_MSG_DB_PROXY;
                        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
                        p_user->friend_tree.del_friend_tree(p_friend_cache->peer_id);
                        return 0;
                    }
                }

                break;
            }
        case as_msg_find_friend:
            {
                if (p_db_pkg->result != 0)
                {
                    if (ERR_ROLE_NOT_EXISTS == p_db_pkg->result)
                    {
                        KINFO_LOG(p_user->uid, "cann't find user:%u, use it as name.", p_db_pkg->user_id);
                        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                        pack_svr_pkg_head(p_header, svr_msg_db_search_name, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_search_name_req_t));
                        db_msg_search_name_req_t *p_body = (db_msg_search_name_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                        memset(p_body, 0, sizeof(db_msg_search_name_req_t));
                        sprintf((char *)p_body, "%u", p_db_pkg->user_id);
                        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                        {
                            g_errno = ERR_MSG_DB_PROXY;
                            KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                            return 0;
                        }
                    }
                    else
                    {
                        KCRIT_LOG(p_user->uid, "get user:%u info failed:%u", p_db_pkg->user_id, p_db_pkg->result);
                        g_errno = p_db_pkg->result;
                    }
                    return 0;
                }

                as_msg_find_frined_rsp_t *p_rsp = (as_msg_find_frined_rsp_t *)p_user->buffer;
                if (p_db_pkg->user_id != p_rsp->friend_info[p_rsp->current_count].user_id)
                {
                    KCRIT_LOG(p_user->uid, "db ret user:%u, expect:%u", p_db_pkg->user_id, p_rsp->friend_info[p_rsp->current_count].user_id);
                    return -1;
                }
                int idx = 0;
                taomee::unpack(p_db_pkg->body, p_rsp->friend_info[p_rsp->current_count].name, sizeof(db_msg_get_friend_rsp_t), idx);

#ifdef DEBUG
                KINFO_LOG(p_user->uid, "current get friend count:%u, total count:%u", p_rsp->current_count, p_rsp->total_count);
#endif
                ++p_rsp->current_count;
                if (p_rsp->current_count == p_rsp->total_count) //所有好友信息已经全部获得
                {
                    //查找好友的返回，直接给as回消息
                    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                    g_send_msg.pack(p_rsp->total_count);
                    KINFO_LOG(p_user->uid, "find %u user", p_rsp->total_count);
                    for (int i = 0; i != (int)p_rsp->total_count; ++i)
                    {
                        g_send_msg.pack(p_rsp->friend_info[i].user_id);
                        g_send_msg.pack(p_rsp->friend_info[i].name, sizeof(p_rsp->friend_info[i].name));
                        g_send_msg.pack(p_rsp->friend_info[i].gender);
                        g_send_msg.pack(p_rsp->friend_info[i].country_id);
                        g_send_msg.pack(p_rsp->friend_info[i].birthday);
                        g_send_msg.pack(p_rsp->friend_info[i].user_type);
                        g_send_msg.pack(p_rsp->friend_info[i].monster_id);
                        g_send_msg.pack(p_rsp->friend_info[i].monster_main_color);
                        g_send_msg.pack(p_rsp->friend_info[i].monster_ex_color);
                        g_send_msg.pack(p_rsp->friend_info[i].monster_eye_color);
                    }
                    g_send_msg.end();
                    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                    return 0;
                }

                //继续请求下一个用户的信息
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_rsp->friend_info[p_rsp->current_count].user_id, GET_SVR_SN(p_user), 0);
                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                    return 0;
                }

                break;
            }
        case as_msg_get_friend_apply:
            {
                friend_apply_rsp_t *p_apply = (friend_apply_rsp_t *)p_user->buffer;
                int count = p_apply->cur_count;
                if (p_db_pkg->result != 0)
                {
                    KCRIT_LOG(p_user->uid, "get user:%u info failed:%u", p_db_pkg->user_id, p_db_pkg->result);
                }
                else
                {
                    int idx = 0;
                    char *body = p_db_pkg->body;
                    taomee::unpack(body, p_apply->apply[count].name, sizeof(p_apply->apply[count].name), idx);
                    taomee::unpack_h(body, p_apply->apply[count].gender, idx);
                    taomee::unpack_h(body, p_apply->apply[count].country_id, idx);
                    taomee::unpack_h(body, p_apply->apply[count].user_type, idx);
                    taomee::unpack_h(body, p_apply->apply[count].birthday, idx);
                    idx += 4;
                    taomee::unpack_h(body, p_apply->apply[count].mon_id, idx);
                    taomee::unpack_h(body, p_apply->apply[count].mon_main_color, idx);
                    taomee::unpack_h(body, p_apply->apply[count].mon_ex_color, idx);
                    taomee::unpack_h(body, p_apply->apply[count].mon_eye_color, idx);
                }

                p_apply->cur_count = ++count;
                if (count == p_apply->count)
                {
                    //全部申请信息返回，给as会消息
                    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                    g_send_msg.pack(p_apply->flag);
                    g_send_msg.pack(p_apply->count);
                    KINFO_LOG(p_user->uid, "get [%u:%u] friend apply", p_apply->flag, p_apply->count);
                    for (int i = 0; i != (int)p_apply->count; ++i)
                    {
                        g_send_msg.pack(p_apply->apply[i].user_id);
                        g_send_msg.pack(p_apply->apply[i].name, sizeof(p_apply->apply[i].name));
                        g_send_msg.pack(p_apply->apply[i].gender);
                        g_send_msg.pack(p_apply->apply[i].country_id);
                        g_send_msg.pack(p_apply->apply[i].birthday);
                        g_send_msg.pack(p_apply->apply[i].user_type);
                        g_send_msg.pack(p_apply->apply[i].mon_id);
                        g_send_msg.pack(p_apply->apply[i].mon_main_color);
                        g_send_msg.pack(p_apply->apply[i].mon_ex_color);
                        g_send_msg.pack(p_apply->apply[i].mon_eye_color);
    //                    g_send_msg.pack(p_apply->apply[i].remark_count);
    //                    if (p_apply->apply[i].remark_count != 0)
    //                    {
     //                       g_send_msg.pack(p_apply->apply[i].remark, p_apply->apply[i].remark_count);
     //                   }
#ifdef DEBUG
                        KINFO_LOG(p_user->uid, "friend apply, user id:%u, mon_id:%u", p_apply->apply[i].user_id, p_apply->apply[i].mon_id);
#endif
                    }
                    g_send_msg.end();
                    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

                    return 0;
                }

                //继续请求下一个信息
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_apply->apply[count].user_id, GET_SVR_SN(p_user), 0);
                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send to db proxy failed.");
                    return 0;
                }

                break;
            }
        case as_msg_friend_apply_op:
            {
                if (p_db_pkg->result != 0)
                {
                    KCRIT_LOG(p_user->uid, "get user:%u info failed:%u", p_db_pkg->user_id, p_db_pkg->result);
                    g_errno = p_db_pkg->result;
                    return 0;
                }
                db_msg_get_friend_rsp_t friend_role = {{0}};
                memcpy((char *)&friend_role, p_db_pkg->body, sizeof(db_msg_get_friend_rsp_t));

                //给as回消息
                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_db_pkg->user_id);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());


                // 添加好友到缓存
                friend_list_t friend_list = {{0}};
                memcpy(friend_list.name, friend_role.name, sizeof(friend_role.name));
                friend_list.gender = friend_role.gender;
                friend_list.country_id = friend_role.country_id;
                friend_list.user_type = friend_role.user_type;
                friend_list.birthday = friend_role.birthday;
                friend_list.last_login_time = friend_role.last_login_time;
                friend_list.mon_id = friend_role.monster_id;
                friend_list.mon_main_color = friend_role.monster_main_color;
                friend_list.mon_ex_color = friend_role.monster_ex_color;
                friend_list.mon_eye_color = friend_role.monster_eye_color;
                friend_list.mon_level = friend_role.monster_level;
                friend_list.is_best_friend = 0;
                friend_list.user_id = p_db_pkg->user_id;
                p_user->friend_tree.add_friend(p_user->uid, &friend_list, true);

                //通知switch好友请求已经通过
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                pack_svr_pkg_head(p_header, svr_msg_switch_friend_apply, p_user->uid, GET_SVR_SN(p_user), sizeof(online_notify_t));

                online_notify_t *p_body = (online_notify_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                p_body->user_id = p_db_pkg->user_id;
                p_body->type = MESSAGE_APPROVED_FRIEND;
                KINFO_LOG(p_user->uid, "accept user:%u friend apply, notify switch", p_db_pkg->user_id);

                g_p_switch_svr->send_data(NULL, g_send_buffer, p_header->len, NULL);
                break;
            }
        case switch_msg_add_friend:
            {
                if (p_db_pkg->result != 0)
                {
                    KCRIT_LOG(p_user->uid, "get user:%u info failed:%u", p_db_pkg->user_id, p_db_pkg->result);
                    return 0;
                }
                db_msg_get_friend_rsp_t friend_role = {{0}};
                memcpy((char *)&friend_role, p_db_pkg->body, sizeof(db_msg_get_friend_rsp_t));

                role_cache_t *p_role = p_user->user_cache.get_role();
                if (p_role != NULL)
                {
                    p_role->role.friend_num += 1;
                }

                // 添加好友到缓存
                friend_list_t friend_list = {{0}};
                memcpy(friend_list.name, friend_role.name, sizeof(friend_role.name));
                friend_list.gender = friend_role.gender;
                friend_list.country_id = friend_role.country_id;
                friend_list.user_type = friend_role.user_type;
                friend_list.birthday = friend_role.birthday;
                friend_list.last_login_time = friend_role.last_login_time;
                friend_list.mon_id = friend_role.monster_id;
                friend_list.mon_main_color = friend_role.monster_main_color;
                friend_list.mon_ex_color = friend_role.monster_ex_color;
                friend_list.mon_eye_color = friend_role.monster_eye_color;
                friend_list.mon_level = friend_role.monster_level;
                friend_list.is_best_friend = 0;
                friend_list.user_id = p_db_pkg->user_id;
                p_user->friend_tree.add_friend(p_user->uid, &friend_list, true);
                KINFO_LOG(p_user->uid, "user:%u agree friend apply, add to friend list", p_db_pkg->user_id);
                p_user->waitcmd = 0;
                break;
            }
        case as_msg_latest_visit:
            {
                latest_visit_t *p_latest_visit = (latest_visit_t *)p_user->buffer;

                if (p_db_pkg->result != 0)
                {
                    KCRIT_LOG(p_user->uid, "get user:%u info failed:%u", p_db_pkg->user_id, p_db_pkg->result);
                }
                else
                {
                    if (p_db_pkg->user_id != p_latest_visit->visit_info[p_latest_visit->cur_index].user_id)
                    {
                        KCRIT_LOG(p_user->uid, "ret user:%u is not %u", p_db_pkg->user_id, p_latest_visit->visit_info[p_latest_visit->cur_index].user_id);
                        return -1;
                    }

                    db_msg_get_friend_rsp_t *p_friend_role = (db_msg_get_friend_rsp_t *)p_db_pkg->body;
                    p_latest_visit->visit_info[p_latest_visit->cur_index].mon_id = p_friend_role->monster_id;
                    p_latest_visit->visit_info[p_latest_visit->cur_index].main_color = p_friend_role->monster_main_color;
                    p_latest_visit->visit_info[p_latest_visit->cur_index].ex_color = p_friend_role->monster_ex_color;
                    p_latest_visit->visit_info[p_latest_visit->cur_index].eye_color = p_friend_role->monster_eye_color;
                    p_latest_visit->visit_info[p_latest_visit->cur_index].is_vip = p_friend_role->user_type;
                    memcpy(p_latest_visit->visit_info[p_latest_visit->cur_index].name, p_friend_role->name, sizeof(p_friend_role->name));
                }


                ++p_latest_visit->cur_index;
                if (p_latest_visit->cur_index == p_latest_visit->count) //所有信息拉取完了
                {
                    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                    g_send_msg.pack((uint16_t)p_latest_visit->count);
                    KINFO_LOG(p_user->uid, "get [%u] latest visit", p_latest_visit->count);
                    for (int i = 0; i != (int)p_latest_visit->count; ++i)
                    {
                        g_send_msg.pack(p_latest_visit->visit_info[i].status);
                        g_send_msg.pack(p_latest_visit->visit_info[i].type);
                        g_send_msg.pack(p_latest_visit->visit_info[i].timestamp);
                        g_send_msg.pack(p_latest_visit->visit_info[i].user_id);
                        g_send_msg.pack(p_latest_visit->visit_info[i].is_vip);
                        g_send_msg.pack(p_latest_visit->visit_info[i].name, sizeof(p_latest_visit->visit_info[i].name));
                        g_send_msg.pack(p_latest_visit->visit_info[i].mon_id);
                        g_send_msg.pack(p_latest_visit->visit_info[i].main_color);
                        g_send_msg.pack(p_latest_visit->visit_info[i].ex_color);
                        g_send_msg.pack(p_latest_visit->visit_info[i].eye_color);
#ifdef DEBUG
                        KINFO_LOG(p_user->uid, "latest visit, user id:%u, mon_id:%u, status:%u, type:%u", p_latest_visit->visit_info[i].user_id, p_latest_visit->visit_info[i].mon_id, p_latest_visit->visit_info[i].status, p_latest_visit->visit_info[i].type);
#endif
                    }
                    g_send_msg.end();
                    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

                    return 0;
                }

                //拉取下一个用户的信息
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_latest_visit->visit_info[p_latest_visit->cur_index].user_id, GET_SVR_SN(p_user), 0);
                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send to db proxy failed.");
                    return 0;
                }


                break;
            }
        default:
            KCRIT_LOG(p_user->uid, "msg:%u is invalid", p_user->waitcmd);
            return -1;
    }

    return 0;
}

int process_db_add_message_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "add message to user:%u failed:%u", p_db_pkg->user_id, p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    uint32_t msg_id = *(uint32_t*)p_db_pkg->body;


    if (p_db_pkg->user_id != p_user->uid) //给其他人留言，需要通知对方
    {
        //通知switch有新的留言
        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
        pack_svr_pkg_head(p_header, svr_msg_switch_friend_apply, p_user->uid, GET_SVR_SN(p_user), sizeof(online_notify_t));

        online_notify_t *p_body = (online_notify_t *)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->user_id = p_db_pkg->user_id;
        p_body->type = MESSAGE_NOTIFY;

        g_p_switch_svr->send_data(NULL, g_send_buffer, p_header->len, NULL);
    }
    else    //给自己留言，将自己的已审核留言数加1
    {
        role_cache_t *p_role = p_user->user_cache.get_role();
        if (NULL == p_role)
        {
            KCRIT_LOG(p_user->uid, "get user role failed");
            return -1;
        }
        p_role->role.approved_message_num += 1;
        badge_step(p_user, badge_message_50_const, p_role->role.approved_message_num);
    }

    //统计留言人数
    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_msg_num, &stat_data, sizeof(stat_data));


    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_db_pkg->user_id);
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());


    if(p_db_pkg->user_id == 12345)
    {
        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
        pinboard_message_t *p_body = (pinboard_message_t*)(g_send_buffer + sizeof(svr_msg_header_t));
        db_msg_add_message_req_t *p_cache_msg = (db_msg_add_message_req_t*)p_user->buffer;
        p_body->id = msg_id;
        p_body->peer_id = p_user->uid;
        p_body->icon = p_cache_msg->icon;
        p_body->color = p_cache_msg->color;
        p_body->status = p_cache_msg->type;
        p_body->create_time = p_cache_msg->create_time;
        p_body->message_count = p_cache_msg->message_count;
        memcpy(p_body->message, p_cache_msg->message, p_cache_msg->message_count);
        pack_svr_pkg_head(p_header, svr_msg_db_add_message, p_user->uid, GET_SVR_SN(p_user), sizeof(pinboard_message_t) + p_cache_msg->message_count);
        g_p_db_cache_svr->send_data(p_user, g_send_buffer, p_header->len, NULL);
    }


    return 0;
}

int process_db_update_msg_status_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "add message to user:%u failed:%u", p_db_pkg->user_id, p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    as_msg_message_status_req_t *p_req = (as_msg_message_status_req_t *)p_user->buffer;
    if (p_req->peer_id != p_db_pkg->user_id)
    {
        KCRIT_LOG(p_user->uid, "req user:%u, return user:%u", p_req->peer_id, p_db_pkg->user_id);
        return -1;
    }
    //统计删除留言次数
    if (MESSAGE_DELETE == p_req->type)
    {
        stat_one_param_t stat_data = {1};
        msg_log(stat_msg_del, &stat_data, sizeof(stat_data));
    }

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "get user role failed");
        return -1;
    }

    if (MESSAGE_APPROVED == p_req->type)
    {
        if (p_role->role.unapproved_message_num)
        {
            p_role->role.unapproved_message_num -= 1;
        }
        p_role->role.approved_message_num += 1;

        badge_step(p_user, badge_message_50_const, p_role->role.approved_message_num);

        pinboard_message_t *p_pinboard = p_user->user_cache.get_pinboard_by_id(p_req->id);
        if (p_pinboard != NULL)
        {
            p_pinboard->status = MESSAGE_APPROVED;
        }
    }
    else if (p_user->uid == p_db_pkg->user_id)
    {
        //删除用户的留言板缓存
        pinboard_message_t *p_pinboard = p_user->user_cache.get_pinboard_by_id(p_req->id);
        if (p_pinboard != NULL)
        {
            if (MESSAGE_APPROVED == p_pinboard->status && p_role->role.approved_message_num > 0) //删除的是已审核信息
            {
                p_role->role.approved_message_num -= 1;
            }
            else if (MESSAGE_UNAPPROVED == p_pinboard->status && p_role->role.unapproved_message_num > 0)
            {
                p_role->role.unapproved_message_num -= 1;
            }
        badge_step(p_user, badge_message_50_const, p_role->role.approved_message_num);
        }
        p_user->user_cache.del_pinboard();
    }

    //给as回消息
    send_header_to_user(p_user, 0, ERR_NO_ERR);

    return 0;
}

int process_db_add_role_value_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);

    switch (p_user->waitcmd)
    {
        case as_msg_rating_room:
            {
                if (p_db_pkg->result != 0)
                {
                    KCRIT_LOG(p_user->uid, "add role value failed:%u", p_db_pkg->result);
                    g_errno = p_db_pkg->result;
                    return 0;
                }

                KINFO_LOG(p_user->uid, "thumb user:%u successfully", p_db_pkg->user_id);

                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack((uint8_t)THUMB_ROOM_SUCCESS);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                break;
            }
        case ucount_msg_add_visit:
            {
                p_user->waitcmd = 0;
                if (p_db_pkg->result != 0)
                {
                    KCRIT_LOG(p_user->uid, "add role value failed:%u", p_db_pkg->result);
                    return 0;
                }

                KINFO_LOG(p_user->uid, "add user:%u visit success", p_db_pkg->user_id);
		        break;
            }
        case multi_puzzle_add_coin:
            {
                two_puzzle_req_t *p_result = (two_puzzle_req_t*)p_user->buffer;
                role_cache_t *p_role = p_user->user_cache.get_role();
                if (p_role == NULL)
                {
                    KCRIT_LOG(p_user->uid, "get role cache");
                    return -1;
                }
                p_role->role.coins += p_result->result.rewards_coins;

                KINFO_LOG(p_user->uid, "notice two_puzzle end, self_score:%u, opp_score:%u, result:%u(0:平局 1：胜利 2：失败), reward_coins:%u", p_result->result.self_score, p_result->result.opp_score, p_result->result.result, p_result->result.rewards_coins);

                if(p_result->result.result == two_puzzle_win)
                {//益智游戏比赛胜利
                    badge_step(p_user, badge_win_puzzle_50_const, 1);
                }

                pack_as_pkg_header(p_user->uid, p_result->msg_type, 0, ERR_NO_ERR);

                if (p_result->msg_type != as_msg_two_puzzle_result)
                {
                    g_send_msg.pack((uint8_t)both_finished);
                }
                g_send_msg.pack(p_result->result.self_score);
                g_send_msg.pack(p_result->result.opp_score);
                g_send_msg.pack(p_result->result.result);
                g_send_msg.pack(p_result->result.rewards_coins);
                g_send_msg.pack(p_result->result.is_restrict);

                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                notify_as_current_user_info(p_user);

                break;
            }
        case as_msg_enter_show:
            {
                msg_enter_show_return_t *p_mesrt = (msg_enter_show_return_t*)p_user->buffer;
                msg_next_of_enter_t *p_mnoet = (msg_next_of_enter_t*)(p_user->buffer + sizeof(msg_enter_show_return_t) + p_mesrt->user_num.user_num * sizeof(show_user_info_t));
                show_reward_t *p_srt = (show_reward_t*)(p_mnoet + 1);
                role_cache_t *p_role = p_user->user_cache.get_role();
                if (p_role == NULL)
                {
                    KCRIT_LOG(p_user->uid, "get role cache");
                    return -1;
                }
                p_role->role.coins += p_srt->reward_coins;
                KINFO_LOG(p_user->uid, "not noticed: show_id:%u show_status:%u user_num:%u total_num:%u reward_coins %u is_join %u", p_mesrt->show_id, p_mesrt->show_status, p_mesrt->user_num.user_num, p_mesrt->user_num.total_num,  p_srt->reward_coins, p_mnoet->is_join);
                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_mesrt->show_id);
                g_send_msg.pack(p_mesrt->show_status);
                g_send_msg.pack(p_mesrt->user_num.total_num);
                g_send_msg.pack(p_mesrt->user_num.user_num);
                for(uint8_t idx = 0; idx < p_mesrt->user_num.user_num; idx++)
                {
                    g_send_msg.pack(p_mesrt->user_info[idx].user_id);
                    g_send_msg.pack(p_mesrt->user_info[idx].user_name, 16);
                    g_send_msg.pack(p_mesrt->user_info[idx].mon_id);
                    g_send_msg.pack(p_mesrt->user_info[idx].mon_main_color);
                    g_send_msg.pack(p_mesrt->user_info[idx].mon_exp_color);
                    g_send_msg.pack(p_mesrt->user_info[idx].mon_eye_color);
                    g_send_msg.pack(p_mesrt->user_info[idx].npc_score);
                    g_send_msg.pack(p_mesrt->user_info[idx].npc_timestamp);
                    g_send_msg.pack(p_mesrt->user_info[idx].votes);
                    g_send_msg.pack(p_mesrt->user_info[idx].is_top);
                }
                g_send_msg.pack(p_mnoet->is_join);
                g_send_msg.pack((uint8_t)0);
                g_send_msg.pack(p_srt->unreward_show_id);
                g_send_msg.pack(p_srt->unreward_type);
                g_send_msg.pack(p_srt->reward_coins);

                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

                notify_as_current_user_info(p_user);
                break;
            }
        case as_msg_get_bobo_reward:
            {
                bobo_cache_t *p_cache = (bobo_cache_t*)p_user->buffer;
                role_cache_t *p_role = p_user->user_cache.get_role();
                if (p_role == NULL)
                {
                    KCRIT_LOG(p_user->uid, "get role cache");
                    return -1;
                }
                p_role->role.coins += p_cache->coins;
                p_role->role.last_paper_reward += p_cache->paper_term;

                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_cache->coins);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                notify_as_current_user_info(p_user);

                break;
            }
        default:
            KCRIT_LOG(p_user->uid, "msg:%u is invalid", p_user->waitcmd);
            return -1;
    }

    return 0;
}

int process_db_update_role_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "update role value failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    db_msg_update_login_t *p_change = (db_msg_update_login_t *)p_user->buffer;

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (p_role == NULL)
    {
        KCRIT_LOG(p_user->uid, "get role cache");
        return -1;
    }
    for (int i = 0; i != (int)p_change->count; ++i)
    {
        switch (p_change->field[i].type)
        {
            case FIELD_LOGIN_TIME:
                p_role->role.last_login_time = p_change->field[i].value;
                KINFO_LOG(p_user->uid, "update login time:%u", p_role->role.last_login_time);
                break;
            case FIELD_HAPPY:
                p_role->role.monster_happy = p_change->field[i].value;
                KINFO_LOG(p_user->uid, "update happy:%u", p_role->role.monster_happy);
                break;
            case FIELD_HEALTH:
                p_role->role.monster_health = p_change->field[i].value;
                KINFO_LOG(p_user->uid, "update health:%u", p_role->role.monster_health);
                break;
            case FIELD_LAST_VISIT_PLANTATION:
                p_role->role.last_visit_plantation_time = p_change->field[i].value;
                KINFO_LOG(p_user->uid, "update visit plantation:%u", p_role->role.last_visit_plantation_time);
                break;
            case FIELD_PUZZLE_MAX_SCORE:
                p_role->role.max_puzzle_score = p_change->field[i].value;
                break;
            case FIELD_LOGOUT_TIME:
                p_role->role.last_logout_time = p_change->field[i].value;
                break;
            case FIELD_FAV_COLOR:
                p_role->role.fav_color = p_change->field[i].value;
                break;
            case FIELD_FAV_FRUIT:
                p_role->role.fav_fruit = p_change->field[i].value;
                break;
            case FIELD_FAV_PET:
                p_role->role.fav_pet = p_change->field[i].value;
                break;
            case FIELD_MOOD:
                p_role->role.mood = p_change->field[i].value;
                break;
            case FIELD_FLAG1:
                {
                    stat_one_param_t stat_data = {1};
                    //统计触发了哪些剧情引导
                    switch (p_role->role.flag1 ^ p_change->field[i].value)
                    {
//                        case JUQING_OPEN_BOX:
//                            msg_log(stat_juqing_chuwuhe, &stat_data, sizeof(stat_data));
 //                           badge_step(p_user, badge_first_open_box, 1);
  //                          break;
                        case JUQING_ENTER_STREET:
                            msg_log(stat_juqing_jiedao, &stat_data, sizeof(stat_data));
                            break;
                        case JUQING_ENTER_STORE:
                            msg_log(stat_juqing_shangdian, &stat_data, sizeof(stat_data));
                            break;
                        case JUQING_ENTER_PLANTATION:
                            msg_log(stat_juqing_zhongzhiyuan, &stat_data, sizeof(stat_data));
                            break;
                    }
                    p_role->role.flag1 = p_change->field[i].value;

                    KINFO_LOG(p_user->uid, "update flag1:%u", p_role->role.flag1);
                    break;
                }
            case FIELD_GUIDE_FLAG:
                p_role->role.guide_flag = p_change->field[i].value;
                break;
	    case FIELD_COMPOSE_TIME:
                p_role->role.compose_time = p_change->field[i].value;
                KINFO_LOG(p_user->uid, "update compose time:%u", p_role->role.compose_time);
                break;
	    case FIELD_COMPOSE_ID:
                p_role->role.compose_id = p_change->field[i].value;
                KINFO_LOG(p_user->uid, "update compose id:%u", p_role->role.compose_id);
                break;
	    case FIELD_BOBO_READ:
                p_role->role.last_paper_read = p_change->field[i].value;
                KINFO_LOG(p_user->uid, "update bobo read:%u", p_role->role.last_paper_read);
                break;
        case FIELD_LAST_SHOW_ID:
                p_role->role.last_show_id = p_change->field[i].value;
                KINFO_LOG(p_user->uid, "update last_show_id %u", p_role->role.last_show_id);
            default:
                break;
        }
    }

    switch (p_user->waitcmd)
    {
	case as_msg_guess_show:
		{
			//dragon boat activity**
			uint32_t now_timestamp = time(NULL);
			map<uint32_t, activity_info_t>::iterator iter = g_activity_map.find(DRAGON_BOAT_ACTIVITY_ID);
			if(iter == g_activity_map.end())
			{//请求一个不存在的活动
				KCRIT_LOG(p_user->uid, "dragon boat activity_id not exist");
				return -1;
			}
			if(iter->second.start_timestamp <= now_timestamp && iter->second.end_timestamp >= now_timestamp)
			{
				KINFO_LOG(p_user->uid, "enter dragon boat process..");
				svr_msg_header_t *p_head = (svr_msg_header_t*)g_send_buffer;
				uint8_t *req = (uint8_t *)(g_send_buffer + sizeof(svr_msg_header_t));
				*req = 2;
				pack_svr_pkg_head(p_head, svr_msg_db_update_dragon_boat, p_user->uid, GET_SVR_SN(p_user), sizeof(uint8_t));
				if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
				{
					g_errno = ERR_MSG_DB_PROXY;
					KCRIT_LOG(p_user->uid, "send to db proxy failed");
				}
			}
			else
			{
				send_header_to_user(p_user, 0, ERR_NO_ERR);
			}
			break;
		}
        case as_msg_update_flag:        //no break
        case as_msg_update_guide_flag:        //no break
        case as_msg_update_profile:     //no break
        case as_msg_cancel_create:
        case as_msg_read_bobo_newspaper:
        case as_msg_join_show:
         {
             send_header_to_user(p_user, 0, ERR_NO_ERR);
             break;
         }
        default:
            KCRIT_LOG(p_user->uid, "msg:%u is invalid", p_user->waitcmd);
            return -1;
    }

    return 0;
}

int process_db_modify_sign_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "update role value failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (p_role == NULL)
    {
        KCRIT_LOG(p_user->uid, "get user cache");
        return -1;
    }
    memcpy(p_role->role.personal_sign, p_user->buffer, sizeof(p_role->role.personal_sign));
    send_header_to_user(p_user, 0, ERR_NO_ERR);

    return 0;
}

int process_db_eat_food_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "eat food failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    switch (p_user->waitcmd)
    {
        case as_msg_eat_food:
            {
                db_msg_eat_food_req_t *p_reward = (db_msg_eat_food_req_t *)p_user->buffer;
                if (p_user->user_cache.desc_stuff(p_reward->item_id, 1) != 0)
                {
                    KCRIT_LOG(p_user->uid, "desc stuff:%u from bag", p_reward->item_id);
                    return -1;
                }
                role_cache_t *p_role = p_user->user_cache.get_role();
                if (p_role == NULL)
                {
                    KCRIT_LOG(p_user->uid, "get role cache");
                    return -1;
                }
                uint32_t old_happy = p_role->role.monster_happy;
                p_role->role.monster_happy += p_reward->reward_happy;
                p_role->role.monster_health += p_reward->reward_health;
                KINFO_LOG(p_user->uid, "happy:%u, health:%u add by food", p_reward->reward_happy, p_reward->reward_health);

                if(p_reward->reward_happy == 0)
                {//愉悦值没有变化
                    send_header_to_user(p_user, 0, ERR_NO_ERR);
                    notify_as_current_user_info(p_user);
                    return 0;
                }

                db_msg_update_plant_req_t *p_change_cache = (db_msg_update_plant_req_t *)p_user->buffer;
                update_plant_info_after_eat_food(p_user, old_happy, p_change_cache);

                break;
            }
        case as_msg_trade_stuff:
            {
                stuff_trade_t *p_trade = (stuff_trade_t *)p_user->buffer;
                if (p_user->user_cache.desc_stuff(p_trade->item_id, 1) != 0)
                {
                    KCRIT_LOG(p_user->uid, "desc stuff:%u from bag", p_trade->item_id);
                    return -1;
                }
                role_cache_t *p_role = p_user->user_cache.get_role();
                if (p_role == NULL)
                {
                    KCRIT_LOG(p_user->uid, "get role cache");
                    return -1;
                }
                p_role->role.coins += p_trade->coins;
                KINFO_LOG(p_user->uid, "trade %u coins, now_coins:%u", p_trade->coins, p_role->role.coins);

                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_trade->percent);
                g_send_msg.pack(p_trade->coins);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                notify_as_current_user_info(p_user);

                //统计回收店产出的金币
                stat_two_param_t stat_data = {p_user->uid, p_trade->coins};
                msg_log(stat_game_huishou, &stat_data, sizeof(stat_data));

                break;
            }
        default:
            KCRIT_LOG(p_user->uid, "cmd:%u is invalid", p_user->waitcmd);
            return -1;
    }

    return 0;
}

//获取小游戏的天限制
int process_db_get_game_day_restrict_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "get game day restrict failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    int idx = 0;
    uint32_t restrict_item = 0;
    uint32_t restrict_coins = 0;
    taomee::unpack_h(p_db_pkg->body, restrict_coins, idx);
    taomee::unpack_h(p_db_pkg->body, restrict_item, idx);

    KINFO_LOG(p_user->uid, "get game_day_restrict return from db restrict_coins:%u item:%u", restrict_coins, restrict_item);
    game_t *p_game = (game_t *)p_user->buffer;

    uint32_t day_time = timestamp_begin_day();
    map<uint32_t, uint32_t>::iterator iter = g_game_map.find(p_game->game_id);
    if (iter == g_game_map.end())
    {
        KCRIT_LOG(p_user->uid, "game:%u is invalid", p_game->game_id);
        return -1;
    }

    restrict_key_t key_coins = {day_time, STRICT_COIN};
    restrict_key_t key_item = {day_time, STRICT_ITEM + p_game->game_id};
    p_user->user_cache.add_game_day_restrict(key_coins, restrict_coins, key_item, restrict_item);

   return do_game_restrict_and_star(p_user, p_game, restrict_coins, restrict_item, day_time);
}

int process_db_get_day_restrict_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "get day restrict failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    int idx = 0;
    uint32_t restrict_value = 0;
    taomee::unpack_h(p_db_pkg->body, restrict_value, idx);

    restrict_key_t key;
    key.time = timestamp_begin_day();
    switch(p_user->waitcmd)
    {
        case as_msg_commit_puzzle://提交益智游戏
            {
                KINFO_LOG(p_user->uid, "commit_puzzle get_exp_day_restrict:%u", restrict_value);
                key.type = STRICT_PUZZLE_EXP;
                p_user->user_cache.add_day_restrict(key, restrict_value);
                as_msg_puzzle_req_t *p_req = (as_msg_puzzle_req_t*)p_user->buffer;
                compute_reward_commit_to_db(p_user, p_req, restrict_value);
                break;
            }
        case as_msg_buy_stuff:
            {

                KINFO_LOG(p_user->uid, "buy_stuff get_exp_day_restrict:%u", restrict_value);
                key.type = STRICT_STUFF_EXP;
                p_user->user_cache.add_day_restrict(key, restrict_value);
                commit_stuff_cache_t *p_cm_stuff = (commit_stuff_cache_t*)p_user->buffer;
                process_buy_stuff_after_get_restrict(p_user, p_cm_stuff, restrict_value);
                break;
            }
        case as_msg_get_sun_reward:
            {
                KINFO_LOG(p_user->uid, "get_sun_reward get_exp_day_restrict:%u", restrict_value);
                key.type = STRICT_SUN_EXP;
                p_user->user_cache.add_day_restrict(key, restrict_value);
                sun_reward_exp_cache_t *p_cache = (sun_reward_exp_cache_t*)p_user->buffer;
                process_sun_reward_exp(p_user,p_cache, restrict_value);
                break;
            }
        default:
            KCRIT_LOG(p_user->uid, "get_day_restrict cmd %u is invalid", p_user->waitcmd);
            return -1;
    }
    return 0;

}

int process_db_add_game_day_restrict_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "add day restrict failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    game_t *p_game = (game_t *)p_user->buffer;
    map<uint32_t, uint32_t>::iterator iter = g_game_map.find(p_game->game_id);
    if (iter == g_game_map.end())
    {
        KCRIT_LOG(p_user->uid, "game:%u is invalid", p_game->game_id);
        return -1;
    }
    uint32_t day_time = timestamp_begin_day();
    //增加缓存里的金币值
    KINFO_LOG(p_user->uid, "add coins:%u by game", p_game->reward_coins);

    //统计小游戏增加的金币
    stat_two_param_t stat_data = {p_user->uid, p_game->reward_coins};
    msg_log(stat_game_gudong, &stat_data, sizeof(stat_data));

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (p_role == NULL)
    {
        KCRIT_LOG(p_user->uid, "get user cache");
        return -1;
    }
    p_role->role.coins += p_game->reward_coins;

    //更新缓存中的天限制
    restrict_key_t key_coins = {day_time, STRICT_COIN};
    restrict_key_t key_item = {day_time, STRICT_ITEM + p_game->game_id};
    p_user->user_cache.update_game_day_restrict(key_coins, p_game->reward_coins, key_item, p_game->reward_items);
    KINFO_LOG(p_user->uid, "add item day restrict:%u item_id:%u, reward_item_count:%u", p_game->value[0].value, p_game->value[0].value_id, p_game->reward_items);
    //增加背包里的物品
    for (int i = 0; i < (int)p_game->reward_items; ++i)
    {
        p_user->user_cache.add_stuff(p_game->value[i].value_id, p_game->value[i].value);
    }

    return send_game_lvl_to_db(p_user, p_game);

}

int process_db_add_day_restrict_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "add day restrict failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    restrict_key_t key;
    key.time = timestamp_begin_day();
    switch(p_user->waitcmd)
    {
        case as_msg_commit_puzzle://提交益智游戏
	{
            KINFO_LOG(p_user->uid, "commit_puzzle add day_rest exp return");
            puzzle_cache_t *p_puzzle_cache = (puzzle_cache_t*)p_user->buffer;
            key.type = STRICT_PUZZLE_EXP;
            p_user->user_cache.update_day_restrict(key, p_puzzle_cache->puzzle_reward.reward.exp);
            update_plant_info_after_commit_puzzle_return(p_user, p_puzzle_cache);
            break;
	 }
       case as_msg_buy_stuff:
        {
            KINFO_LOG(p_user->uid, "buy_stuff add_day_rest exp return");
            commit_stuff_cache_t *p_cm_stuff = (commit_stuff_cache_t*)p_user->buffer;
            key.type = STRICT_STUFF_EXP;
            p_user->user_cache.update_day_restrict(key, p_cm_stuff->add_stuff.reward_exp);
            update_plant_info_after_buy_stuff(p_user, p_cm_stuff);
        }
            break;
       case as_msg_get_sun_reward:
            {
                KINFO_LOG(p_user->uid, "get_sun_reward add_day_rest return");

                sun_reward_exp_cache_t *p_cache = (sun_reward_exp_cache_t*)p_user->buffer;
                key.type = STRICT_SUN_EXP;
                p_user->user_cache.update_day_restrict(key, p_cache->reward_exp);
                //给as回成功消息
                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_cache->sun_reward.hole_id);
                g_send_msg.pack(p_cache->sun_reward.reward_id);
                g_send_msg.pack((uint16_t)0);
                g_send_msg.pack(p_cache->reward_exp);
                 g_send_msg.pack((uint16_t)0);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                KINFO_LOG(p_user->uid, "add_day_restr after sun_reward_exp:%u", p_cache->reward_exp);

                notify_as_current_user_info(p_user);
            }
            break;
        default:
            KCRIT_LOG(p_user->uid, "get_day_restrict cmd %u is invalid", p_user->waitcmd);
            return -1;
    }
    return 0;


}

int process_db_del_plant_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "del plant failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    switch (p_user->waitcmd)
    {
        case as_msg_attract_pet:
            {
                p_user->user_cache.del_all_plant();
                //给as回成功消息
                send_header_to_user(p_user, 0, ERR_NO_ERR);
                break;
            }
        case as_msg_maintain_plant:
            {
                //统计铲除种子
                stat_one_param_t stat_data = {1};
                msg_log(stat_seeds_out, &stat_data, sizeof(stat_data));

                db_msg_del_plant_req_t *p_req = (db_msg_del_plant_req_t *)p_user->buffer;
                p_user->user_cache.del_plant_at_hole(p_req->hole_id);

                KINFO_LOG(p_user->uid, "del plant at hole %u returned", p_req->hole_id);
                //给as回成功消息
                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_req->hole_id);
                g_send_msg.pack((uint32_t)0);
                g_send_msg.pack((uint16_t)0);
                g_send_msg.pack((uint16_t)0);
                g_send_msg.pack((uint8_t)NOT_MAINTAIN);
                g_send_msg.pack((uint8_t)PLANTATION_UNATTRACT_PET);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());


                break;
            }
        default:
            KCRIT_LOG(p_user->uid, "msg:%u is invalid", p_user->waitcmd);
            return -1;
    }

    return 0;
}

int process_db_pet_op_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "pet op failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    int idx = 0;
    uint8_t num = 0;   //精灵已经存在的数量
    uint8_t op_type = 0;
    uint32_t pet_id = 0;
    taomee::unpack_h(p_db_pkg->body, num, idx);
    taomee::unpack_h(p_db_pkg->body, op_type, idx);
    taomee::unpack_h(p_db_pkg->body, pet_id, idx);

    db_msg_pet_req_t *p_pet = (db_msg_pet_req_t *)p_user->buffer;
    if(op_type == PET_ADD)
    {
        pet_info_t pet = {p_pet->id, 1, p_pet->status == PET_FOLLOWING ? 1 : 0};
        // 缓存里增加跟随的小怪兽
        if (p_user->user_cache.add_pet(&pet) != 0)
        {
            KCRIT_LOG(p_user->uid, "add following pet");
            return -1;
        }
        KINFO_LOG(p_user->uid, "add pet:%u to cache, follow:%u.", p_pet->id, p_pet->status);

        //增加小怪兽的数量
        role_cache_t *p_role = p_user->user_cache.get_role();
        if (p_role == NULL)
        {
            KCRIT_LOG(p_user->uid, "get role cache");
            return -1;
        }

        if (0 == num)   //增加精灵的类别
        {
            p_role->role.pet_num += 1;
            //增加成就项
            map<uint32_t, uint32_t>::iterator iter = g_pet_series_map.find(p_pet->id);
            if(iter != g_pet_series_map.end())
            {
                switch(iter->second)
                {
                    case tiaotiao_series:
                        badge_step(p_user, badge_get_tiaotiao_pet_const, 1);//跳跳系精灵增加一个
                        break;
                    case mengchong_series:
                        badge_step(p_user, badge_get_mengchong_pet_const, 1);//蒙宠系精灵增加一个
                        break;
                    case hero_series:
                        badge_step(p_user, badge_get_hero_pet_const, 1);//英雄系精灵增加一个
                        break;
                    case plant_series:
                        badge_step(p_user, badge_get_plant_pet_const, 1);//植物系精灵增加一个
                        break;
                    case dragon_series:
                        badge_step(p_user, badge_get_dragon_pet_const, 1);//神龙系精灵增加一个
                        break;
                    case seans_series:
                        badge_step(p_user, badge_get_seans_pet_const, 1);//海洋系精灵增加一个
                        break;
                    default:
                        KWARN_LOG(p_user->uid, "unknown pet_id %u not in g_pet_series", p_pet->id);
                        break;
                }
            }

        }

        //小精灵相关成就项
        badge_step(p_user, badge_get_one_monster_const, 1);//成功领养一只小精灵

        //num是这次领养之前拥有的数量
        badge_step(p_user, badge_get_two_same_monster_const, num + 1);//成功领养2只同样的小精灵


        //删掉吸引到的小怪兽
        p_user->user_cache.del_attract_pet();

        //删掉种植的所有植物
        p_user->user_cache.del_all_plant();

        //给as回成功消息
        send_header_to_user(p_user, 0, ERR_NO_ERR);

        return 0;
    }
    else if(op_type == PET_DROP)
    {

        p_user->user_cache.del_a_pet(pet_id);
        send_header_to_user(p_user, 0, ERR_NO_ERR);

    }
    return 0;
}

int process_db_add_plant_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "add plant failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    //统计播种人数
    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_seeds_down, &stat_data, sizeof(stat_data));

    db_msg_add_plant_req_t *p_plant = (db_msg_add_plant_req_t *)p_user->buffer;
    if (p_user->user_cache.update_plant_in_hole(p_plant) != 0)
    {
        KCRIT_LOG(p_user->uid, "update plant to cache failed");
        return -1;
    }
    if (p_user->user_cache.desc_stuff(p_plant->plant_id, 1) != 0)
    {
        KCRIT_LOG(p_user->uid, "desc stuff:%u", p_plant->plant_id);
        return -1;
    }

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);

    g_send_msg.pack(p_plant->hole_id);
    g_send_msg.pack(p_plant->maintain);
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    KINFO_LOG(p_user->uid, "use grow plant return from db and send to as: hole_id:%u maintain:%u", p_plant->hole_id, p_plant->maintain);

    return 0;
}

int process_db_get_friend_id_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);

    db_msg_friend_list_rsp_t *p_friend_list = (db_msg_friend_list_rsp_t *)p_db_pkg->body;
    switch (p_user->waitcmd)
    {
        case as_msg_get_friend_list:
            {
                friend_cache_t *p_friend_cache = (friend_cache_t *)p_user->buffer;
                if (p_db_pkg->result != 0)
                {
                    p_user->friend_tree.del_friend_tree(p_friend_cache->peer_id == p_user->uid);
                    KCRIT_LOG(p_user->uid, "get friend id list:%u", p_db_pkg->result);
                    g_errno = p_db_pkg->result;
                    return 0;
                }
                if (p_friend_cache->peer_id != p_db_pkg->user_id)
                {
                    p_user->friend_tree.del_friend_tree(p_friend_cache->peer_id == p_user->uid);
                    KCRIT_LOG(p_user->uid, "ret user:%u is not %u", p_db_pkg->user_id, p_friend_cache->peer_id);
                    return -1;
                }

                p_user->friend_tree.add_other_friend_list(p_friend_cache->peer_id, p_friend_list);
                p_friend_cache->begin_index += p_friend_list->count;
                p_friend_cache->friend_list.count += p_friend_list->count;

                if (!p_friend_list->is_end) //好友列表没有拉取完，继续拉取
                {
                    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_id, p_friend_cache->peer_id, GET_SVR_SN(p_user), sizeof(db_msg_friend_list_req_t));
                    db_msg_friend_list_req_t *p_body = (db_msg_friend_list_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                    p_body->begin = p_friend_cache->begin_index;
                    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                    {
                        g_errno = ERR_MSG_DB_PROXY;
                        KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                        return 0;
                    }
                }
                else    //好友列表拉取完成
                {
                    if (p_user->friend_tree.get_other_friend_list((p_friend_cache->page - 1 ) * p_friend_cache->page_num + 1, p_friend_cache->page * p_friend_cache->page_num, &p_friend_cache->friend_list) != 0)
                    {
                        KCRIT_LOG(p_user->uid, "cann't get user:%u friend list", p_friend_cache->peer_id);
                        return -1;
                    }

                    if (0 == p_friend_cache->friend_list.count)
                    {
                        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                        g_send_msg.pack((uint16_t)0);
                        g_send_msg.pack((uint8_t)0);
                        g_send_msg.end();
                        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                        return 0;
                    }

                    //向db请求第一个好友的信息
                    p_friend_cache->cur_count = 0;
                    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_friend_cache->friend_list.friend_info[0].friend_id, GET_SVR_SN(p_user), 0);

                    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                    {
                        g_errno = ERR_MSG_DB_PROXY;
                        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
                        return 0;
                    }
                }

                break;
            }
        case as_msg_login_online:
            {
                if (p_db_pkg->result != 0)
                {
                    KCRIT_LOG(p_user->uid, "get friend id list:%u", p_db_pkg->result);
                    return -1;
                }
                p_user->friend_tree.add_friend_list(p_friend_list);
                if (!p_friend_list->is_end) //好友列表没有拉取完
                {
                    db_msg_friend_list_req_t *p_req = (db_msg_friend_list_req_t *)p_user->buffer;

                    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_id, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_friend_list_req_t));
                    db_msg_friend_list_req_t *p_body = (db_msg_friend_list_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                    p_body->begin = p_req->begin + p_friend_list->count;
                    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                    {
                        ERROR_LOG("send to db proxy server failed.");
                        return -1;
                    }
                    p_req->begin += p_friend_list->count;
                }
                else
                {
                    //统计好友数量
                    stat_two_param_t stat_data = {p_user->uid, p_user->friend_tree.get_friend_num()};
                    msg_log(stat_friend_num, &stat_data, sizeof(stat_data));

                    badge_step(p_user, badge_friends_100_const, p_user->friend_tree.get_friend_num());

                    KINFO_LOG(p_user->uid, "get total %u friends", p_user->friend_tree.get_friend_num());

                    //获取未读的访客的条数
                    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                    pack_svr_pkg_head(p_header, svr_msg_get_unread_count, p_user->uid, GET_SVR_SN(p_user), 0);
                    if (g_p_ucount_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                    {
                        g_errno = ERR_MSG_UCOUNT;
                        KCRIT_LOG(p_user->uid, "send to ucount server failed.");
                        return 0;
                    }
                }

                break;
            }
        default:
            KCRIT_LOG(p_user->uid, "cmd:%u is invalid", p_user->waitcmd);
            return -1;
    }

    return 0;
}

int process_db_del_friend_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "del friend:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    switch (p_user->waitcmd)
    {
        case as_msg_friend_op:
            {
                as_msg_friend_op_req_t *p_req = (as_msg_friend_op_req_t *)p_user->buffer;
                if (p_user->uid == p_db_pkg->user_id)  //还需要删除对方的记录
                {
                    p_user->friend_tree.del_friend(p_req->friend_id);
                    role_cache_t *p_role = p_user->user_cache.get_role();
                    if (p_role == NULL)
                    {
                        KCRIT_LOG(p_user->uid, "get user role");
                        return -1;
                    }
                    if (p_role->role.friend_num > 0)
                    {
                        p_role->role.friend_num -= 1;
                    }

                    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                    pack_svr_pkg_head(p_header, svr_msg_db_del_friend, p_req->friend_id, GET_SVR_SN(p_user), sizeof(db_msg_del_friend_req_t));

                    db_msg_del_friend_req_t *p_body = (db_msg_del_friend_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                    p_body->friend_id = p_user->uid;

                    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                    {
                        g_errno = ERR_MSG_DB_PROXY;
                        KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                        return 0;
                    }
                }
                else //2跳记录都删完了，给as回消息
                {
                    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                    g_send_msg.pack(p_req->op_type);
                    g_send_msg.end();
                    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

                    //通知switch对方已经删除好友
                    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                    pack_svr_pkg_head(p_header, svr_msg_switch_friend_apply, p_user->uid, GET_SVR_SN(p_user), sizeof(online_notify_t));

                    online_notify_t *p_body = (online_notify_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                    p_body->user_id = p_db_pkg->user_id;
                    p_body->type = MESSAGE_DEL_FRIEND;
                    KINFO_LOG(p_user->uid, "del friend:%u, notify switch", p_db_pkg->user_id);

                    g_p_switch_svr->send_data(NULL, g_send_buffer, p_header->len, NULL);
                }

                break;
            }
        case as_msg_friend_apply_op:
            {
                as_msg_friend_apply_op_req_t *p_req = (as_msg_friend_apply_op_req_t *)p_user->buffer;
                if (p_user->uid != p_db_pkg->user_id)
                {
                    KCRIT_LOG(p_user->uid, "db ret:%u is not user:%u", p_db_pkg->user_id, p_req->user_id);
                    return -1;
                }

                KINFO_LOG(p_user->uid, "del %u req", p_req->user_id);
                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_req->user_id);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

                role_cache_t *p_role = p_user->user_cache.get_role();
                if (p_role != NULL && p_role->pending_req_num)
                {
                    p_role->pending_req_num -= 1;
                }

                break;
            }
        default:
            KCRIT_LOG(p_user->uid, "cmd:%u is invalid", p_user->waitcmd);
            return -1;
    }

    return 0;
}

int process_db_set_friend_status_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "set friend status:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return -1;
    }

    switch (p_user->waitcmd)
    {
        case as_msg_friend_op:
            {
                as_msg_friend_op_req_t *p_req = (as_msg_friend_op_req_t *)p_user->buffer;
                if (BEST_FRIEND == p_req->op_type || CANCEL_BEST_FRIEND == p_req->op_type)
                {
                    //操作完成，返回消息给as
                    if (p_user->friend_tree.set_best_friend(p_req->friend_id, p_req->op_type == BEST_FRIEND ? 1 : 0) != 0)
                    {
                        KCRIT_LOG(p_user->uid, "set friend:%u status failed", p_req->friend_id);
	                g_errno = ERR_SYSTEM_ERR;
                        return 0;
                    }
                    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                    g_send_msg.pack(p_req->op_type);
                    g_send_msg.end();
                    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                    KINFO_LOG(p_user->uid, "friend op finish");
                }
                else //屏蔽好友，需要将对方的记录也设为屏蔽状态
                {
                    if (p_user->uid == p_db_pkg->user_id)
                    {
                        //从缓存里删掉这个好友
                        if (p_user->friend_tree.del_friend(p_req->friend_id) != 0)
                        {
                            KCRIT_LOG(p_user->uid, "del friend:%u failed", p_req->friend_id);
                            return -1;
                        }
                        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                        pack_svr_pkg_head(p_header, svr_msg_db_set_friend_status, p_req->friend_id, GET_SVR_SN(p_user), sizeof(db_msg_set_friend_status_req_t));

                        db_msg_set_friend_status_req_t *p_body = (db_msg_set_friend_status_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                        p_body->friend_info.friend_id = p_user->uid;
                        p_body->friend_info.type = FRIEND_BLOCK;
                        const static uint8_t no_update = -1;
                        p_body->friend_info.is_bestfriend = no_update;

                        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                        {
                            g_errno = ERR_MSG_DB_PROXY;
                            KCRIT_LOG(p_user->uid, "send to db proxy server failed");
                            return 0;
                        }
                    }
                    else
                    {
                        //已经完成对方的屏蔽操作
                        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                        g_send_msg.pack(p_req->op_type);
                        g_send_msg.end();
                        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                    }
                }

                break;
            }
        case as_msg_friend_apply_op:
            {
                as_msg_friend_apply_op_req_t *p_req = (as_msg_friend_apply_op_req_t *)p_user->buffer;
                role_cache_t *p_role = p_user->user_cache.get_role();
                if (p_role == NULL)
                {
                    KCRIT_LOG(p_user->uid, "get role cached");
                    return -1;
                }
                if (p_role->pending_req_num)
                {
                    p_role->pending_req_num -= 1;
                }

                if (p_req->type == FRIEND_REQ_AGREE)   //好友申请成功，需要在对方的表里面也加入一条好友记录
                {
                    //统计同意好友申请
                    stat_one_param_t stat_data = {1};
                    msg_log(stat_friends_agree, &stat_data, sizeof(stat_data));
                    p_role->role.friend_num += 1;
                    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                    pack_svr_pkg_head(p_header, svr_msg_db_apply_for_friend, p_req->user_id, GET_SVR_SN(p_user), sizeof(db_msg_friend_apply_req_t));

                    db_msg_friend_apply_req_t *p_body = (db_msg_friend_apply_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                    p_body->peer_id = p_user->uid;
                    p_body->type = FRIEND_DEFAULT;
                    p_body->remark_count = 0;

                    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                    {
                        g_errno = ERR_MSG_DB_PROXY;
                        KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                        return 0;
                    }

                    return 0;
                }
                else
                {
                    //屏蔽好友申请，已完成,直接给as回消息
                    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                    g_send_msg.pack(p_req->user_id);
                    g_send_msg.end();
                    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                    return 0;
                }
                break;
            }
        default:
            KCRIT_LOG(p_user->uid, "cmd:%u is invalid", p_user->waitcmd);
            return -1;
    }

    return 0;
}

int process_db_search_name_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "search name failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    const db_msg_search_name_rsp_t *p_db_rsp = (const db_msg_search_name_rsp_t *)p_db_pkg->body;
    KINFO_LOG(p_user->uid, "get %u friends by name.", p_db_rsp->count);
    if (0 == p_db_rsp->count)
    {
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack((uint8_t)0);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        return 0;
    }

    as_msg_find_frined_rsp_t *p_as_rsp = (as_msg_find_frined_rsp_t *)p_user->buffer;
    p_as_rsp->current_count = 0;
    p_as_rsp->total_count = p_db_rsp->count;
    for (int i = 0; i != (int)p_db_rsp->count; ++i)
    {
        p_as_rsp->friend_info[i].user_id = p_db_rsp->user_id[i];
    }

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_as_rsp->friend_info[0].user_id, GET_SVR_SN(p_user), 0);
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
        return 0;
    }

    return 0;
}

int process_db_apply_for_friend_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        if(p_db_pkg->result != ERR_APPLY_FOR_FRIEND_AGAIN )
        {
             KCRIT_LOG(p_user->uid, "apply for friend failed:%u", p_db_pkg->result);
        }
        g_errno = p_db_pkg->result;
        return 0;
    }

    switch (p_user->waitcmd)
    {
        case as_msg_add_friend:
            {
                //统计好友请求数
                stat_one_param_t stat_data = {1};
                msg_log(stat_friends_req, &stat_data, sizeof(stat_data));
                send_header_to_user(p_user, 0, ERR_NO_ERR);
                //通知switch有好友请求
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                pack_svr_pkg_head(p_header, svr_msg_switch_friend_apply, p_user->uid, GET_SVR_SN(p_user), sizeof(online_notify_t));

                online_notify_t *p_body = (online_notify_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                p_body->user_id = p_db_pkg->user_id;
                p_body->type = FRIEND_NOTIFY;

                KINFO_LOG(p_user->uid, "notify user:%u has friend apply", p_body->user_id);
                g_p_switch_svr->send_data(NULL, g_send_buffer, p_header->len, NULL);
                break;
            }
        case as_msg_friend_apply_op:
            {
                as_msg_friend_apply_op_req_t *p_req = (as_msg_friend_apply_op_req_t *)p_user->buffer;
                if (p_req->user_id != p_db_pkg->user_id)
                {
                    KCRIT_LOG(p_user->uid, "db ret:%u is not user:%u", p_db_pkg->user_id, p_req->user_id);
                    return -1;
                }
                //请求这个好友的信息，把好友加入缓存
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_db_pkg->user_id, GET_SVR_SN(p_user), 0);
                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send to db proxy failed.");
                    return 0;
                }
                break;
            }
        default:
            KCRIT_LOG(p_user->uid, "cmd:%u is invalid", p_user->waitcmd);
            return -1;
    }

    return 0;
}

int process_db_get_friend_apply_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_user->uid != p_db_pkg->user_id)
    {
        KERROR_LOG(p_user->uid, "user:%u is not correct", p_db_pkg->user_id);
        return -1;
    }
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db ret friend apply failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    friend_apply_rsp_t *p_apply = (friend_apply_rsp_t *)p_user->buffer;

    int idx = 0;
    taomee::unpack_h(p_db_pkg->body, p_apply->count, idx);
#ifdef DEBUG
    KINFO_LOG(p_user->uid, "[get friend apply]%u apply", p_apply->count);
#endif

    p_apply->flag = 0;
    p_apply->cur_count = 0;
    if (p_apply->count == MAX_FRIEND_REQ)
    {
        p_apply->flag = 1;
    }
    else if (p_apply->count == 0)
    {
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack((uint8_t)0);
        g_send_msg.pack((uint8_t)0);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        return 0;
    }

    for (int i = 0; i != (int)p_apply->count; ++i)
    {
        taomee::unpack_h(p_db_pkg->body, p_apply->apply[i].user_id, idx);
       // taomee::unpack_h(p_db_pkg->body, p_apply->apply[i].remark_count, idx);
       // if (p_apply->apply[i].remark_count != 0)
       // {
       //     taomee::unpack(p_db_pkg->body, p_apply->apply[i].remark, p_apply->apply[i].remark_count, idx);
       // }
    }

    //向db-proxy请求第一个用户的信息
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_apply->apply[0].user_id, GET_SVR_SN(p_user), 0);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_db_get_profile_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    stranger_t *p_stranger = (stranger_t *)p_user->buffer;
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db get profile failed:%u", p_db_pkg->result);
    }
    else
    {
        db_msg_street_user_rsp_t *p_profile = (db_msg_street_user_rsp_t *)p_db_pkg->body;
//        KINFO_LOG(p_user->uid, "get user:%u profile, vip:%u, mon_id:%u", p_db_pkg->user_id, p_profile->profile.user_type, p_profile->profile.monster_id);

        memcpy(&p_stranger->stranger[p_stranger->cur_num].stranger_info, p_profile, sizeof(db_msg_street_user_rsp_t));
    }

    if (++p_stranger->cur_num >= p_stranger->count) //陌生人的profile信息都请求完了
    {

        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_stranger->count);
        for (int i = 0; i != (int)p_stranger->count; ++i)
        {
            g_send_msg.pack(p_stranger->stranger[i].user_id);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.name, sizeof(p_stranger->stranger[i].stranger_info.profile.name));
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.gender);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.country_id);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.user_type);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.birthday);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.monster_id);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.monster_name, sizeof(p_stranger->stranger[i].stranger_info.profile.monster_name));
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.monster_main_color);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.monster_ex_color);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.monster_eye_color);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.monster_exp);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.monster_level);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.monster_health);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.monster_happy);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.friend_num);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.visits);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.mood);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.fav_color);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.fav_pet);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.fav_fruit);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.room_num);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.pet_num);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.max_puzzle_score);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.pet_id);
            uint8_t sign_len = strlen(p_stranger->stranger[i].stranger_info.profile.personal_sign);
            g_send_msg.pack(sign_len);
            g_send_msg.pack(p_stranger->stranger[i].stranger_info.profile.personal_sign, sign_len);
        }

        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        return 0;
    }

    //继续请求下一个
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_profile, p_stranger->stranger[p_stranger->cur_num].user_id, GET_SVR_SN(p_user), 0);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
        return 0;
    }

    return 0;
}

int process_db_check_session_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "login session verify failed:%u", p_db_pkg->result);
        return -1;
    }

    KDEBUG_LOG(p_user->uid, "login session verify success.");
    //从db获得用户的登录信息
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_role_info, p_user->uid, (p_user->session->fd << 16) | p_user->counter, 0);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
        return -1;
    }

    return 0;
}

int process_db_get_pet_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db get pet failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    pet_t *p_rsp = (pet_t *)p_db_pkg->body;

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_rsp->count);
    for (int i = 0; i != p_rsp->count; ++i)
    {
        g_send_msg.pack(p_rsp->pet[i].pet_id);
        g_send_msg.pack(p_rsp->pet[i].total_num);
        g_send_msg.pack(p_rsp->pet[i].follow_num);
    }
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_db_interactive_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if(p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db get interactive return failed(%u)", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    db_msg_interactive_res_t *p_rsp = (db_msg_interactive_res_t*)p_db_pkg->body;
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    if(p_rsp->is_restrict != 0)
    {//达到每日限制
        g_send_msg.pack((uint8_t)1);
        g_send_msg.pack((uint16_t)0);
    }
    else
    {//未达到每日限制
        g_send_msg.pack((uint8_t)0);
        g_send_msg.pack(p_rsp->reward_coins);

        role_cache_t *p_role = p_user->user_cache.get_role();
        if (p_role != NULL)
        {
            p_role->role.coins += p_rsp->reward_coins;
        }
        else
        {
             KCRIT_LOG(p_user->uid, "Not get role info from user cache");
             g_errno = ERR_GET_ROLE_CACHE;
             return 0;
        }
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    if(p_rsp->is_restrict == 0)
    {
        notify_as_current_user_info(p_user);
    }
    KINFO_LOG(p_user->uid, "interactive result: is_restrict:%u, reward_coins:%u", p_rsp->is_restrict, p_rsp->reward_coins);
    return 0;
}

int process_db_get_real_message_count_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if(p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db get real message count return failed(%u)", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }
    uint32_t *p_real_count = (uint32_t *)p_db_pkg->body;

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(*p_real_count);
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    peer_real_msg_num_t peer_msg;
    peer_msg.peer_id = p_db_pkg->user_id;
    peer_msg.real_num = *p_real_count;
    p_user->user_cache.set_peer_real(&peer_msg);
    return 0;
}


int process_db_get_all_badge_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db get badge failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    all_badge_info_t *p_rsp = (all_badge_info_t *)p_db_pkg->body;

    if(p_user->waitcmd == as_msg_get_all_badge)
    {//请求别人的成就
      pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
      g_send_msg.pack(p_rsp->badge_num);
      for (int i = 0; i != p_rsp->badge_num; ++i)
      {
          g_send_msg.pack(p_rsp->badge[i].badge_id);
          g_send_msg.pack(p_rsp->badge[i].badge_status);
          g_send_msg.pack(p_rsp->badge[i].badge_progress);
      }
      g_send_msg.end();
      send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
      return 0;
    }
    else
    {
        KCRIT_LOG(p_user->uid, "logical error");
        return -1;
    }
}

int process_db_update_badge_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db update_badge failed:%u", p_db_pkg->result);
        //        g_errno = p_db_pkg->result;
        return -1;
    }

    badge_info_t *p_rsp = (badge_info_t *)p_db_pkg->body;

    uint16_t unread_badge = p_user->user_cache.update_badge_status(p_rsp);
    if(unread_badge == (uint16_t)-1)
    {
        return -1;
    }

    //if(p_rsp->badge_status == badge_acquired_no_reward)
    if(p_rsp->badge_status == badge_acquired_no_screen)
    {
        unread_badge++;
        //通知db将重新设置未读的成就数,db返回后将缓存中未读的成就数更新
        svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
        uint16_t *p_unread_badge = (uint16_t*)(g_send_buffer + sizeof(svr_msg_header_t));
        *p_unread_badge = unread_badge;
        pack_svr_pkg_head(p_header, svr_msg_db_set_unread_badge, p_user->uid, GET_SVR_SN(p_user), sizeof(uint16_t));
        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            // g_errno = ERR_MSG_DB_PROXY;
            ERROR_LOG("send to db proxy server failed.");
            return -1;
         }
    }
    else
    {
        p_user->waitcmd = 0;
    }


    return 0;
}

int process_db_set_unread_badge_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{//需要给AS回复
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db get pet failed:%u", p_db_pkg->result);
        return 0;
    }

    uint16_t *p_unread_badge_num = (uint16_t*)p_db_pkg->body;
    KINFO_LOG(p_user->uid, "db return , set unread_badge_num to %u", *p_unread_badge_num);
    p_user->user_cache.set_unread_badge(*p_unread_badge_num);

    if(p_user->waitcmd == as_msg_get_all_badge)
    {
        char buffer[MAX_SEND_PKG_SIZE] = {0};
        all_badge_info_t *p_all_badge = (all_badge_info_t*)buffer;
        if(p_user->user_cache.get_all_badge(p_all_badge) == 0)
        {//缓存里面有
            pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
            g_send_msg.pack(p_all_badge->badge_num);
            KINFO_LOG(p_user->uid, "all badge_num:%u", p_all_badge->badge_num);
            for(int i = 0; i < (int)p_all_badge->badge_num; i++)
            {
                g_send_msg.pack(p_all_badge->badge[i].badge_id);
                g_send_msg.pack(p_all_badge->badge[i].badge_status);
                g_send_msg.pack(p_all_badge->badge[i].badge_progress);
                KINFO_LOG(p_user->uid, "badge_id:%u, badge_status:%u(0:进行中 1：已获得未领奖杯 2:已获得未查看 3:已获得已领奖), badge_progress:%u", p_all_badge->badge[i].badge_id, p_all_badge->badge[i].badge_status, p_all_badge->badge[i].badge_progress);
            }
            g_send_msg.end();
            send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

            p_user->user_cache.set_all_badge_to_no_reward();//将所有未查看的成就项置为未领奖状态


        }
        else
        {
            g_errno = ERR_SYSTEM_ERR;
            KCRIT_LOG(p_user->uid, "db_proxy:It not possible to come here, we have get badge list at login time.");
        }
        return 0;
    }

    if(*p_unread_badge_num != 0)
    {//告知as有新的成就项获得
        pack_as_pkg_header(p_user->uid, as_msg_new_badge, 0, ERR_NO_ERR);
        g_send_msg.pack(*p_unread_badge_num);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }
    else
    {
        KINFO_LOG(p_user->uid, "It should not come here for theory");
        p_user->waitcmd = 0;
    }

    return 0;
}


int process_db_compose_stuff_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
	    g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db compose stuff failed:%u", p_db_pkg->result);
        return 0;
    }

    db_msg_compose_stuff_req_t *p_req = (db_msg_compose_stuff_req_t *)p_user->buffer;
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "get role info failed.");
        return -1;
    }

    KINFO_LOG(p_user->uid, "commit compose:%u db return success", p_req->stuff_id);
    p_role->role.compose_id = p_req->stuff_id;
    p_role->role.compose_time = p_req->finish_time;
    p_role->role.map_id = p_req->map_id;

    for (int i = 0; i != p_req->num; ++i)
    {
        p_user->user_cache.desc_stuff(p_req->item_cost[i].item_id, p_req->item_cost[i].item_num);
    }

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack((uint32_t)(p_req->finish_time - time(NULL)));
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_db_add_game_level_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db compose stuff failed:%u", p_db_pkg->result);
        return 0;
    }

    game_t *p_game = (game_t *)p_user->buffer;
    map<uint32_t, uint32_t>::iterator iter = g_game_map.find(p_game->game_id);
    if (iter == g_game_map.end())
    {
        KCRIT_LOG(p_user->uid, "game:%u is invalid", p_game->game_id);
        return -1;
    }

    p_user->user_cache.update_game_level(p_game);

    //给as回包
    uint8_t unlock_next_lvl = p_user->user_cache.unlock_next_level(p_game->game_id, p_game->level_id);
    if(unlock_next_lvl == -1)
    {//错误的情况
        KCRIT_LOG(p_user->uid, "It not possible ,not found gameid levelid");
        unlock_next_lvl = NOT_UNLOCK;//这种情况将其置为下一关不可解锁
    }

    KINFO_LOG(p_user->uid, "add_game_level_return game:%u level:%u reward-coin:%u star_num:%u unlock_next:%u reason:%u", p_game->game_id, p_game->level_id, p_game->reward_coins, p_game->star_num,  unlock_next_lvl, p_game->reason);

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_game->game_id);
    g_send_msg.pack(p_game->level_id);
    g_send_msg.pack(p_game->coin_restrict);
    g_send_msg.pack(p_game->reward_coins);
    g_send_msg.pack(p_game->star_num);
    g_send_msg.pack(unlock_next_lvl);
    g_send_msg.pack(p_game->reason);
    if(p_game->reason == GAME_GET_STUFF)
    {//奖励道具
        KINFO_LOG(p_user->uid, " add_game_level reward_stuff:%u num:%u", (uint8_t)(p_game->value[0].value), p_game->value[0].value_id);
        g_send_msg.pack((uint8_t)(p_game->value[0].value));
        g_send_msg.pack(p_game->value[0].value_id);
    }
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    if(p_game->reward_coins > 0)
    {
        notify_as_current_user_info(p_user);
    }
    return 0;
}

int process_db_get_game_level_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "db compose stuff failed:%u", p_db_pkg->result);
        return 0;
    }

    game_level_info_t *p_lvl = (game_level_info_t*)p_db_pkg->body;
    game_level_t *p_game = (game_level_t *)p_user->buffer;
    KINFO_LOG(p_user->uid, "get game %u level return", p_game->game_id);
    if(p_user->user_cache.add_game_level(p_game->game_id, p_game->total_level,  p_lvl) == 0)
    {//成功加入缓存
        KINFO_LOG(p_user->uid, "process_db_get_game_lvl_return unlock_next:%u, lvl_num:%u", p_lvl->unlock_next_level, p_lvl->level_num);
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_lvl->unlock_next_level);
        g_send_msg.pack(p_lvl->level_num);
        for(int i = 0; i < (int)p_lvl->level_num; i++)
        {
            g_send_msg.pack(p_lvl->level[i].level_id);
            g_send_msg.pack(p_lvl->level[i].max_score);
            g_send_msg.pack(p_lvl->level[i].star_num);
        }
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        return 0;
    }
    else
    {
        KCRIT_LOG(p_user->uid, "add game level to cache failed");
        return 0;
    }

    return 0;

}

int process_modify_monster_name_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db compose stuff failed:%u", p_db_pkg->result);
        return 0;
    }

    char *p_monster_name = (char *)p_user->buffer;
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "get role failed");
        return -1;
    }
    memcpy(p_role->role.monster_name, p_monster_name, 16);
    send_header_to_user(p_user, 0, ERR_NO_ERR);

    return 0;
}

int process_get_hole_reward_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get_hole_reward failed:%u", p_db_pkg->result);
        return 0;
    }

    db_all_hole_reward_rsp_t *p_all_reward = (db_all_hole_reward_rsp_t*)p_db_pkg->body;
    KINFO_LOG(p_user->uid, "all-reward_num:%u", p_all_reward->reward_num);

    p_user->user_cache.cache_all_hole_reward(p_all_reward);

    char buf[1024] = {0};
    all_hole_info_t *p_all_hole = (all_hole_info_t*)buf;

    if(p_user->user_cache.get_all_hole(p_all_hole) == 1)
    {
        KCRIT_LOG(p_user->uid, "hole not in cached, It should be cached at login time");
        return -1;
    }

    process_plantation_after_all_hole(p_user, p_all_hole);

    return 0;
}


int process_maintain_plant_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get_hole_reward failed:%u", p_db_pkg->result);
        return 0;
    }

    db_msg_maintain_req_t *p_req = (db_msg_maintain_req_t*)p_user->buffer;
    p_user->user_cache.update_hole_info_after_maintain(p_req);

    //给as回成功消息
     pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
     g_send_msg.pack(p_req->hole_id);
     g_send_msg.pack(p_req->last_reward_id);
     g_send_msg.pack((uint16_t)0);
     g_send_msg.pack(p_req->add_growth);
     g_send_msg.pack(p_req->new_maintain_type);



    char buffer[1024] = {0};
    all_hole_info_t *p_all_hole = (all_hole_info_t*)buffer;
    if(p_user->user_cache.get_all_hole(p_all_hole) == 1)
    {
        KCRIT_LOG(p_user->uid, "Not get hole info from cached");
        g_send_msg.pack((uint8_t)PLANTATION_UNATTRACT_PET);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        return 0;
    }

    if(all_plant_maturity(p_all_hole) == 1)
    {//植物全部成熟
        int seed_match[PLANTATION_NUM] = {0};
        uint32_t pet_id = 0;

        //判断精灵匹配情况
        pet_match(p_all_hole, &pet_id, seed_match);
        KINFO_LOG(p_user->uid, "all_plant_maturity get pet_id:%u", pet_id);
        stat_one_param_t stat_data = {1};
        if(pet_id == 0)
        {//没有获取到小精灵
            g_send_msg.pack((uint8_t)PLANTATION_NOT_MATCH);
            for (int i = 0; i < PLANTATION_NUM; ++i)
            {
                g_send_msg.pack((uint8_t)(seed_match[i] == 0 ? 0 : 1));
            }
            msg_log(stat_not_attract_pet, &stat_data, sizeof(stat_data));
        }
        else
        {
            g_send_msg.pack((uint8_t)PLANTATION_ATTRACT_PET);
            g_send_msg.pack(pet_id);
            p_user->user_cache.add_attract_pet(pet_id);
            msg_log(stat_attract_pet, &stat_data, sizeof(stat_data));
        }
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }
    else
    {
        KINFO_LOG(p_user->uid, "all_plant_not_maturity hole_id:%u, maintain_type:%u", p_req->hole_id, p_req->new_maintain_type);
        g_send_msg.pack((uint8_t)PLANTATION_UNATTRACT_PET);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }

    return 0;
}

/**
 * @brief process_get_plant_return 收获植物
 *
 * @param p_user
 * @param p_db_pkg
 *
 * @return
 */
int process_get_plant_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get_hole_reward failed:%u", p_db_pkg->result);
        return 0;
    }

    db_msg_get_plant_req_t *p_req = (db_msg_get_plant_req_t*)p_user->buffer;

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (p_role == NULL)
    {
        KCRIT_LOG(p_user->uid, "get user cache");
        return -1;
    }
    p_role->role.coins += p_req->reward_coins;

    p_user->user_cache.del_plant_at_hole(p_req->hole_id);

    //给as回成功消息
     pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
     g_send_msg.pack(p_req->hole_id);
     g_send_msg.pack((uint32_t)0);
     g_send_msg.pack(p_req->reward_coins);
     g_send_msg.pack((uint16_t)0);
     g_send_msg.pack((uint8_t)0);
     g_send_msg.pack((uint8_t)PLANTATION_UNATTRACT_PET);
     g_send_msg.end();
     send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
     KINFO_LOG(p_user->uid, "after get_plant:reward_coins:%u", p_req->reward_coins);

     notify_as_current_user_info(p_user);

    return 0;
}

/**
 * @brief process_sun_reward_happy_return 阳关奖励愉悦值
 *
 * @param p_user
 * @param p_db_pkg
 *
 * @return
 */
int process_sun_reward_happy_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get_hole_reward failed:%u", p_db_pkg->result);
        return 0;
    }

    sun_reward_happy_cache_t *p_cache = (sun_reward_happy_cache_t*)p_user->buffer;
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (p_role == NULL)
    {
        KCRIT_LOG(p_user->uid, "get user cache");
        return -1;
    }
    p_role->role.monster_happy += p_cache->sun_reward_happy.reward_happy;
    p_user->user_cache.del_hole_reward_i(p_cache->sun_reward_happy.sun_reward.hole_id, p_cache->sun_reward_happy.sun_reward.reward_id);

    char buffer[1024] = {0};
    all_hole_info_t *p_all_hole = (all_hole_info_t*)buffer;
    if(p_user->user_cache.get_all_hole(p_all_hole) != 0)
    {
        KCRIT_LOG(p_user->uid, "NOT find hole cache");
        //给as回成功消息
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_cache->sun_reward_happy.sun_reward.hole_id);
        g_send_msg.pack(p_cache->sun_reward_happy.sun_reward.reward_id);
        g_send_msg.pack(p_cache->sun_reward_happy.reward_happy);
        g_send_msg.pack((uint16_t)0);
        g_send_msg.pack((uint16_t)0);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        KINFO_LOG(p_user->uid, "after sun_reward_happy:reward_happy:%u", p_cache->sun_reward_happy.reward_happy);

        notify_as_current_user_info(p_user);
        return 0;
    }

    if(p_all_hole->plant_count == 0 || all_plant_maturity(p_all_hole) == 1)
    {
        //给as回成功消息
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_cache->sun_reward_happy.sun_reward.hole_id);
        g_send_msg.pack(p_cache->sun_reward_happy.sun_reward.reward_id);
        g_send_msg.pack(p_cache->sun_reward_happy.reward_happy);
        g_send_msg.pack((uint16_t)0);
        g_send_msg.pack((uint16_t)0);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        KINFO_LOG(p_user->uid, " plant empty or maturity after sun_reward_happy:reward_happy:%u", p_cache->sun_reward_happy.reward_happy);

        notify_as_current_user_info(p_user);
        return 0;
    }

    int change_num = update_plant_growth(p_user, p_cache->old_happy, p_all_hole, &(p_cache->plant_change), 0);
    if(change_num == 0)
    {
        //给as回成功消息
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_cache->sun_reward_happy.sun_reward.hole_id);
        g_send_msg.pack(p_cache->sun_reward_happy.sun_reward.reward_id);
        g_send_msg.pack(p_cache->sun_reward_happy.reward_happy);
        g_send_msg.pack((uint16_t)0);
        g_send_msg.pack((uint16_t)0);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        KINFO_LOG(p_user->uid, " change_num= 0 after sun_reward_happy:reward_happy:%u", p_cache->sun_reward_happy.reward_happy);

        notify_as_current_user_info(p_user);

    }

    return 0;
}

int process_sun_reward_exp_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get_hole_reward failed:%u", p_db_pkg->result);
        return 0;
    }

    sun_reward_exp_cache_t *p_cache = (sun_reward_exp_cache_t*)p_user->buffer;

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (p_role == NULL)
    {
        KCRIT_LOG(p_user->uid, "get user cache");
        return -1;
    }
    if(p_cache->level_up != 0)
    {
        p_role->role.monster_level += p_cache->level_up;
        unlock_map_item(p_user, p_role->role.monster_level);
        if(p_role->role.monster_level >= 8)
        {
            activity_open(p_user, OPEN_ACTIVITY_ID, NULL);
        }
  		update_level_relative_info(p_user, INVITOR_ACTIVITY_ID);
    }
    p_role->role.monster_exp += p_cache->reward_exp;

    p_user->user_cache.del_hole_reward_i(p_cache->sun_reward.hole_id, p_cache->sun_reward.reward_id);

    if(p_cache->reward_exp == 0)
    {//没有奖励经验，不需要更新db和缓存的天限制
        //给as回成功消息
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_cache->sun_reward.hole_id);
        g_send_msg.pack(p_cache->sun_reward.reward_id);
        g_send_msg.pack((uint16_t)0);
        g_send_msg.pack(p_cache->reward_exp);
        g_send_msg.pack((uint16_t)0);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        KINFO_LOG(p_user->uid, "reward_exp = 0 after sun_reward_exp");

        notify_as_current_user_info(p_user);
        return 0;
    }
    else
    {
        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;

        db_msg_add_day_restrict_req_t *p_body = (db_msg_add_day_restrict_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->type = STRICT_SUN_EXP;
        p_body->time = timestamp_begin_day();

        p_body->value = p_cache->reward_exp;
        p_body->count = 1;
        p_body->restrict_stuff[0].value_id = 0;
        p_body->restrict_stuff[0].value = 0;

        pack_svr_pkg_head(p_header, svr_msg_db_add_day_restrict, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_add_day_restrict_req_t) + p_body->count * sizeof(restrict_value_t));
        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
            return 0;
        }
        return 0;

    }
    return 0;
}

int process_sun_reward_coins_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get_hole_reward failed:%u", p_db_pkg->result);
        return 0;
    }

    sun_reward_coins_cache_t *p_cache = (sun_reward_coins_cache_t*)p_user->buffer;

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (p_role == NULL)
    {
        KCRIT_LOG(p_user->uid, "get user cache");
        return -1;
    }
    p_role->role.coins += p_cache->reward_coins;

    p_user->user_cache.del_hole_reward_i(p_cache->sun_reward.hole_id, p_cache->sun_reward.reward_id);
        //给as回成功消息
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_cache->sun_reward.hole_id);
        g_send_msg.pack(p_cache->sun_reward.reward_id);
        g_send_msg.pack((uint16_t)0);
        g_send_msg.pack((uint16_t)0);
        g_send_msg.pack(p_cache->reward_coins);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        KINFO_LOG(p_user->uid, "reward_coins after sun_reward_coins:%u", p_cache->reward_coins);

        notify_as_current_user_info(p_user);

    return 0;
}

int process_get_npc_score_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get_hole_reward failed:%u", p_db_pkg->result);
        return 0;
    }
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (p_role == NULL)
    {
        KCRIT_LOG(p_user->uid, "get user cache");
        return -1;
    }

    db_return_npc_score_t *p_db_ret = (db_return_npc_score_t*)p_db_pkg->body;

    if(p_db_ret->npc_num != NPC_SCORE_NUM)
    {
        KCRIT_LOG(p_user->uid, "npc_num:%u not equal to 3", p_db_ret->npc_num);
        return 0;
    }
    npc_score_info_t npc_score;
    npc_score.status = NPC_SCORED;//今天已经评过了
    npc_score.total_score = p_role->role.npc_score;
    npc_score.npc_num = p_db_ret->npc_num;
    memcpy(npc_score.npc, p_db_ret->npc, sizeof(npc_score.npc));

    p_user->user_cache.set_npc_score(&npc_score);

    send_npc_score_to_as(p_user, npc_score);

    return 0;
}

int process_set_npc_score_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get_hole_reward failed:%u", p_db_pkg->result);
        return 0;
    }
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (p_role == NULL)
    {
        KCRIT_LOG(p_user->uid, "get user cache");
        return -1;
    }

    uint32_t *p_day_time = (uint32_t*)p_db_pkg->body;
    p_role->role.npc_score_daytime = *p_day_time;


    npc_score_info_t npc_score;
    p_user->user_cache.get_npc_score(&npc_score);
    npc_score.status = NPC_SCORED;//将状态改为今天已经评过了
    p_role->role.npc_score = npc_score.total_score;

    p_user->user_cache.set_npc_score(&npc_score);

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(npc_score.total_score);
    g_send_msg.pack(npc_score.npc_num);
    KINFO_LOG(p_user->uid, "send_to_as after score: total_score:%u, npc_num:%u",  npc_score.total_score, npc_score.npc_num);
    for(int i = 0; i < npc_score.npc_num; i++)
    {
        g_send_msg.pack(npc_score.npc[i].npc_id);
        g_send_msg.pack(npc_score.npc[i].npc_score);
        KINFO_LOG(p_user->uid, "id:%u score:%u", npc_score.npc[i].npc_id, npc_score.npc[i].npc_score);
    }
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    badge_step(p_user, badge_npc_score_const, npc_score.total_score);

    encourage_guide_to_db(p_user, ROOM_SCORE, npc_score.total_score);

    return 0;
}

int process_get_game_changed_stuff_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get_changed_stuff failed:%u", p_db_pkg->result);
        return 0;
    }


    db_msg_changed_stuff_t *p_changed_stuff = (db_msg_changed_stuff_t*)p_db_pkg->body;
    KINFO_LOG(p_user->uid, "get game %u changed_stuff from db returnd, changed_num:%u", p_changed_stuff->game_id, p_changed_stuff->changed_stuff_num);

    p_user->user_cache.set_game_changed(p_changed_stuff);

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_changed_stuff->game_id);
    g_send_msg.pack(p_changed_stuff->changed_stuff_num);
    for(uint16_t i = 0; i < p_changed_stuff->changed_stuff_num; i++)
    {
        g_send_msg.pack(p_changed_stuff->changed_stuff[i]);
    }
    g_send_msg.end();

    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}


int process_set_game_changed_stuff_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get_changed_stuff failed:%u", p_db_pkg->result);
        return 0;
    }


    db_msg_get_changed_stuff_t *p_changed_stuff = (db_msg_get_changed_stuff_t*)p_db_pkg->body;
    KINFO_LOG(p_user->uid, "set game %u changed_stuff %u repeat %u from db returnd, period:%u", p_changed_stuff->game_id, p_changed_stuff->stuff_id, p_changed_stuff->repeat,  p_changed_stuff->period_id);

    if(p_changed_stuff->repeat == 0)
    {
        if(p_user->user_cache.update_game_changed(p_changed_stuff) != 0)
        {
            g_errno = ERR_SYSTEM_ERR;
            KCRIT_LOG(p_user->uid, "set game db returned, but not found game %u in cached.", p_changed_stuff->game_id);
            return 0;
        }
    }
    //更新用户背包物品
    p_user->user_cache.add_stuff(p_changed_stuff->stuff_id, 1);


    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}


int process_get_finished_task_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get_finished_task_list failed:%u", p_db_pkg->result);
        return 0;
    }

    db_msg_finished_task_rsp_t *p_finished_task = (db_msg_finished_task_rsp_t*)p_db_pkg->body;
    KINFO_LOG(p_user->uid, "get finished_task_list from db return,finished_num:%u", p_finished_task->finished_task_num);

    p_user->user_cache.add_finished_task(p_finished_task);


    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_finished_task->finished_task_num);
    for(uint16_t i = 0; i < p_finished_task->finished_task_num; i++)
    {
        g_send_msg.pack(p_finished_task->finished_task[i].task_id);
    }
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());


    return 0;
}

int process_finish_task_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db finish_task failed:%u", p_db_pkg->result);
        return 0;
    }

    db_msg_task_reward_t *p_body = (db_msg_task_reward_t*)(p_db_pkg->body);
    KINFO_LOG(p_user->uid, "finish task %u from db return,reward_num:%u, reward_time:%u", p_body->task_id,  p_body->reward_num, p_body->reward_time);

    p_user->user_cache.update_task_list(p_body->task_id, p_body->reward_time);

    //给as回复
    //
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_body->task_id);
    g_send_msg.pack(p_body->reward_num);
    role_cache_t *p_role = p_user->user_cache.get_role();
    for(uint8_t i = 0; i < p_body->reward_num; i++)
    {
        g_send_msg.pack(p_body->reward[i].reward_id);
        g_send_msg.pack(p_body->reward[i].reward_num);
        KINFO_LOG(p_user->uid, "task reward_id:%u num:%u", p_body->reward[i].reward_id, p_body->reward[i].reward_num);
        if(p_body->reward[i].reward_id == TASK_REWARD_COIN)
        {//奖励金币
            p_role->role.coins += p_body->reward[i].reward_num;
        }
        else if(p_body->reward[i].reward_id == TASK_REWARD_EXP)
        {
            if(p_body->level_up != 0)
            {
                p_role->role.monster_level += p_body->level_up;
            }
            p_role->role.monster_exp += p_body->reward[i].reward_num;

        }
        else if(p_body->reward[i].reward_id >= ITEM_ID_BEGIN && p_body->reward[i].reward_id <= ITEM_ID_END)
        {//奖励道具
            p_user->user_cache.add_stuff(p_body->reward[i].reward_id, p_body->reward[i].reward_num );
        }
    }
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    notify_as_current_user_info(p_user);

    if(p_body->level_up != 0)
    {//解锁新的图纸
       unlock_map_item(p_user, p_role->role.monster_level);
    }

    return 0;
}

/*
int process_select_donate_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db select_donate failed:%u", p_db_pkg->result);
        return 0;
    }

    msg_db_select_donate_t *p_body = (msg_db_select_donate_t*)(p_db_pkg->body);
    KINFO_LOG(p_user->uid, "select donate from db return,last_unreward_donate:%u, last_reward:%u, donate_id:%u, cur_donate_num:%u, donater_num:%u", p_body->last_unreward_donate, p_body->last_reward_id, p_body->donate_id, p_body->cur_donate_num, p_body->donater_num);

    if(p_body->last_reward_id != 0)
    {//上次的奖励还未领取,更新背包
            p_user->user_cache.add_stuff(p_body->last_reward_id, 1);
    }

    if(

            g_donate_map.find(p_body->donate_id) == g_donate_map.end())
    {
        p_body->donate_id = 0;//暂无新的援助计划
    }
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_body->last_unreward_donate);
    g_send_msg.pack(p_body->donate_id);
    g_send_msg.pack(p_body->cur_donate_num);
    g_send_msg.pack(p_body->donater_num);

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    KINFO_LOG(p_user->uid, "send cur donate to AS: last_unreward_donate:%u, donate_id:%u, cur_donate_num:%u, donater_num:%u", p_body->last_unreward_donate, p_body->donate_id, p_body->cur_donate_num, p_body->donater_num);
    return 0;
}


int process_select_history_donate_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db select_donate failed:%u", p_db_pkg->result);
        return 0;
    }

    msg_history_donate_t *p_body = (msg_history_donate_t*)(p_db_pkg->body);
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_body->donate_total);
    g_send_msg.pack(p_body->helped_pet);
    g_send_msg.pack(p_body->total_participater);
    g_send_msg.pack(p_body->history_num);

    KINFO_LOG(p_user->uid, "send history donate to AS: donate_total:%u, helped_pet:%u, total_part:%u, history_num:%u", p_body->donate_total, p_body->helped_pet, p_body->total_participater, p_body->history_num);

    for(uint16_t idx = 0; idx < p_body->history_num; idx++)
    {
        g_send_msg.pack(p_body->donate[idx].donate_id);
        g_send_msg.pack(p_body->donate[idx].end_time);
        KINFO_LOG(p_user->uid, "idx:%u, donate_id:%u, end_time:%u", idx, p_body->donate[idx].donate_id, p_body->donate[idx].end_time);
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_donate_coins_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db select_donate failed:%u", p_db_pkg->result);
        return 0;
    }

    db_msg_donate_response_t *p_res = (db_msg_donate_response_t*)p_db_pkg->body;

    KINFO_LOG(p_user->uid, "donate_coins db return:id:%u flag:%u, num:%u, reward:%u new_donate:%u", p_res->donate_id, p_res->flag, p_res->donate_num, p_res->reward_id, p_res->new_donate_id);



    if(p_res->flag == DENOTE_SUCCESS_BUT_NOT_REACHED || p_res->flag == DENOTE_REACHED_LIMIT)
    {
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_res->donate_id);
        g_send_msg.pack(p_res->flag);

        KINFO_LOG(p_user->uid, "send donate_coins result to as: donate_id:%u, flag:%u", p_res->donate_id, p_res->flag);

        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }
    else if(p_res->flag == DENOTE_SUCCESS_AND_REACHED || p_res->flag == DENOTE_FAIL_AND_REACHED)
    {//募捐达成
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_res->donate_id);
        g_send_msg.pack(p_res->flag);
        g_send_msg.pack(p_res->donate_id);
        if(g_donate_map.find(p_res->new_donate_id) == g_donate_map.end())
        {
            p_res->new_donate_id = 0;
        }
        g_send_msg.pack(p_res->new_donate_id);


        KINFO_LOG(p_user->uid, "send donate_coins result to as: donate_id:%u, flag:%u unreward_donate_id:%u new_donate_id:%u", p_res->donate_id, p_res->flag, p_res->donate_id, p_res->new_donate_id);

        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }

    if(p_res->flag == DENOTE_SUCCESS_BUT_NOT_REACHED || p_res->flag == DENOTE_SUCCESS_AND_REACHED)
    {//募捐成功，但计划尚未达成，扣除金币
        role_cache_t *p_role = p_user->user_cache.get_role();
        if (NULL == p_role)
        {
            KERROR_LOG(p_user->uid, "get role failed");
            return -1;
        }
        if(p_role->role.coins >= p_res->donate_num)
        {
            p_role->role.coins -= p_res->donate_num;
        }
        else
        {
            p_role->role.coins = 0;
        }

    stat_three_param_t stat_data3 = {p_user->uid, 1, p_res->donate_num};
    msg_log(stat_donate_base, &stat_data3, sizeof(stat_data3));

    stat_two_param_t stat_data2 = {p_user->uid, 1};
    msg_log(stat_donate_base + p_res->donate_id, &stat_data2, sizeof(stat_data2));

        p_user->user_cache.add_stuff(p_res->reward_id, 1);

                notify_as_current_user_info(p_user);

    }
    return 0;
}
*/
int process_factory_op_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (p_role == NULL)
    {
        KCRIT_LOG(p_user->uid, "get role cache");
        return -1;
    }
    p_role->role.compose_time = 0;
    p_role->role.compose_id = 0;

    factory_op_get_stuff_t *p_body = (factory_op_get_stuff_t*)p_user->buffer;
    //更新背包物品
    for (int i = 0; i < p_body->count; ++i)
    {
        p_user->user_cache.add_stuff(p_body->stuff[i].stuff_id, p_body->stuff[i].stuff_num);
    }

    send_header_to_user(p_user, 0, ERR_NO_ERR);
    return 0;
}

int process_enter_museum_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
	assert(p_user!=NULL && p_db_pkg!=NULL);
	if(p_db_pkg->result != 0)
	{
		g_errno = p_db_pkg->result;
		KCRIT_LOG(p_user->uid, "db get mesuem level failed:%u!", p_db_pkg->result);
		return 0;
	}

	museum_info_t *p_museum = (museum_info_t *)(p_db_pkg->body);

	uint32_t cur_time = timestamp2day(time(NULL));
	if(cur_time > p_museum->timestamp)
	{
		p_museum->timestamp = cur_time;
		p_museum->level_id = 0;
		p_museum->reward_flag = HAVE_GET_REWARD;
		p_museum->next_level_id= 1;
	}
	else
	{
		uint8_t level_count = (g_museum_map.find(p_museum->museum_id))->second;
		p_museum->next_level_id = p_museum->level_id == level_count ? level_count : p_museum->level_id + 1;
	}

	p_user->user_cache.add_museum_item(p_museum);

	pack_as_pkg_header(p_user->uid,p_user->waitcmd,0,ERR_NO_ERR);
	g_send_msg.pack(p_museum->museum_id);
	g_send_msg.pack(p_museum->reward_flag);
	uint8_t level_id = p_museum->reward_flag == HAVE_GET_REWARD?p_museum->next_level_id:p_museum->level_id;
	g_send_msg.pack(level_id);
	g_send_msg.end();
	send_data_to_user(p_user,g_send_msg.get_msg(),g_send_msg.get_msg_len());

	KINFO_LOG(p_user->uid, "send musuem to AS: museum_id:%u flag:%d,level:%d",p_museum->museum_id, p_museum->reward_flag, p_museum->next_level_id);
	return 0;
}

int process_commit_mesuem_game_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
	assert(p_user!=NULL && p_db_pkg!=NULL);
	if(p_db_pkg->result != 0)
	{
		g_errno = p_db_pkg->result;
		KCRIT_LOG(p_user->uid, "db get mesuem level failed:%u!", p_db_pkg->result);
		return 0;
	}

	db_request_commit_museum_t *p_db_ret = (db_request_commit_museum_t *)(p_db_pkg->body);
	museum_info_t *p_museum_cache = p_user->user_cache.get_museum_item(p_db_ret->museum_id);
      if(p_museum_cache == NULL)
      {
            KCRIT_LOG(p_user->uid,"commit museum game return:museum item is NULL!");
            return -1;
      }
	uint8_t level_count = (g_museum_map.find(p_museum_cache->museum_id))->second;
	p_museum_cache->level_id = p_db_ret->level_id;
	p_museum_cache->reward_flag = HAVENOT_GET_REWARD;
	p_museum_cache->next_level_id = p_museum_cache->level_id == level_count ? level_count : p_museum_cache->level_id + 1;

	pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
	g_send_msg.pack((uint8_t)0);
	g_send_msg.pack((uint8_t)0);
	g_send_msg.end();
	send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

	KINFO_LOG(p_user->uid, "send musuem to AS:flag:0,level:%d", p_db_ret->level_id);
	return 0;
}

int process_get_museum_reward_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
	assert(p_user!=NULL && p_db_pkg!=NULL);
	if(p_db_pkg->result != 0)
	{
		g_errno = p_db_pkg->result;
		KCRIT_LOG(p_user->uid, "db get mesuem level failed:%u!", p_db_pkg->result);
		return 0;
	}

    db_msg_add_museum_reward_t *p_db_ret = (db_msg_add_museum_reward_t*)(p_db_pkg->body);

	museum_info_t *museum_item = p_user->user_cache.get_museum_item(p_db_ret->museum_id);
	if(museum_item == NULL)
	{
		KCRIT_LOG(p_user->uid,"museum reward:museum cache is NULL!");
		return -1;
	}

	museum_item->reward_flag = HAVE_GET_REWARD;

	uint8_t level_count = (g_museum_map.find(p_db_ret->museum_id))->second;
	museum_item->next_level_id = museum_item->level_id == level_count?level_count:museum_item->level_id+1;

	switch(p_db_ret->reward_id)
	{
		case TASK_REWARD_NONE:
			break;
		case TASK_REWARD_COIN:
			(p_user->user_cache.get_role())->role.coins += p_db_ret->reward_num;
			break;
		case TASK_REWARD_EXP:
			break;
		default:
			p_user->user_cache.add_stuff(p_db_ret->reward_id, p_db_ret->reward_num);
	}

	pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
	g_send_msg.pack(p_db_ret->reward_id);
	g_send_msg.pack(p_db_ret->reward_num);
	g_send_msg.pack(museum_item->next_level_id);
	g_send_msg.end();
	send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

	KINFO_LOG(p_user->uid, "send musuem to AS:level:%d", museum_item->next_level_id);

	if(p_db_ret->reward_id == TASK_REWARD_COIN)
	{
		notify_as_current_user_info( p_user);
	}
	return 0;
}

int process_encourage_guide_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    return 0;
	assert(p_user!=NULL && p_db_pkg!=NULL);
	if(p_db_pkg->result != 0)
	{
		g_errno = p_db_pkg->result;
		KCRIT_LOG(p_user->uid, "db encourage guide failed:%u!", p_db_pkg->result);
		return 0;
	}

	db_return_encourage_t *p_db_ret = (db_return_encourage_t *)(p_db_pkg->body);
	KINFO_LOG(p_user->uid, "send encourage to AS:win:%d total:%d", p_db_ret->win_num,p_db_ret->total_num);

	pack_as_pkg_header(p_user->uid, as_msg_encourage_guide, 0, ERR_NO_ERR);
	g_send_msg.pack(p_db_ret->type);
	g_send_msg.pack(p_db_ret->score);
	g_send_msg.pack(p_db_ret->win_num);
	g_send_msg.pack(p_db_ret->total_num);
	g_send_msg.pack(p_db_ret->user_id);
	g_send_msg.end();
	send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), false);


	return 0;
}


int process_request_history_donate_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db request_history_donate failed:%u", p_db_pkg->result);
        return 0;
    }

    msg_history_donate_t *p_body = (msg_history_donate_t *)(p_db_pkg->body);

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_body->donate_total);
    g_send_msg.pack(p_body->helped_pet);
    g_send_msg.pack(p_body->total_participater);
    g_send_msg.pack(p_body->history_num);

    KINFO_LOG(p_user->uid, "send history donate to AS: donate_total:%u, helped_pet:%u, total_pariticapter:%u, history_num:%u", p_body->donate_total, p_body->helped_pet, p_body->total_participater, p_body->history_num);

    for(uint16_t i = 0; i < p_body->history_num; i++)
    {
        g_send_msg.pack(p_body->donate[i].donate_id);
        g_send_msg.pack(p_body->donate[i].end_time);
        KINFO_LOG(p_user->uid, "idx:%u, donate_id:%u, end_time:%u", i, p_body->donate[i].donate_id, p_body->donate[i].end_time);
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;

}

int process_request_cur_donate_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db request_cur_donate failed:%u", p_db_pkg->result);
        return 0;
    }

    db_return_cur_donate_t *p_body = (db_return_cur_donate_t*)(p_db_pkg->body);

	if(g_donate_map.find(p_body->donate_id) == g_donate_map.end())
	{
			p_body->donate_id = 0;
	}

    if(p_body->reward_donate_id)
    {//有未领奖的
        map<uint32_t, donate_info_t>::iterator iter = g_donate_map.find(p_body->reward_donate_id);
        if(iter == g_donate_map.end())
        {
            KCRIT_LOG(p_user->uid, " request_cur_donate return reward_donate_id %u not exist", p_body->reward_donate_id);
            return -1;
        }
        p_body->reward_id = iter->second.reward_id;

        iter = g_donate_map.find(p_body->donate_id);
        if(iter == g_donate_map.end())
        {
            KINFO_LOG(p_user->uid, "request_cur_donate return donate_id %u not exist", p_body->donate_id);
            p_body->donate_id = 0;
        }

        update_wealth_to_db(p_user, 0 , p_body->reward_id , 1, (char *)p_body, sizeof(db_return_cur_donate_t));

    }
    else
    {
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);

        g_send_msg.pack(p_body->reward_donate_id);
        g_send_msg.pack(p_body->donate_id);
        g_send_msg.pack(p_body->donate_num);
        g_send_msg.pack(p_body->donor_count);

        KINFO_LOG(p_user->uid, "send cur donate to AS: reward_donate_id:%u, donate_id:%u, donate_num:%u, donor_count:%u", p_body->reward_donate_id, p_body->donate_id, p_body->donate_num, p_body->donor_count);

        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    }

    return 0;
}

int process_request_donate_coins_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db donate coins failed:%u", p_db_pkg->result);
        return 0;
    }

    db_return_donate_coins_t *p_body = (db_return_donate_coins_t *)(p_db_pkg->body);
	if(g_donate_map.find(p_body->next_donate_id) == g_donate_map.end())
	{
			p_body->next_donate_id = 0;
	}

    if(p_body->reward_donate_id || p_body->flag == DENOTE_SUCCESS_BUT_NOT_REACHED)
    {
        update_wealth_to_db(p_user, (uint32_t)(p_body->money), p_body->reward_id, 1, (char *)p_body, sizeof(db_return_donate_coins_t));
    }
    else
    {
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);

        g_send_msg.pack(p_body->donate_id);
        g_send_msg.pack(p_body->flag);
		if(p_body->flag == DENOTE_FAIL_AND_REACHED)
		{
			g_send_msg.pack(p_body->reward_donate_id);
			g_send_msg.pack(p_body->next_donate_id);
		}
		KINFO_LOG(p_user->uid, "send donate coins to AS: donate_id:%u flag:%u", p_body->donate_id, p_body->flag);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }

        return 0;
}

int process_update_wealth_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db select_donate failed:%u", p_db_pkg->result);
        return 0;
    }

    db_request_update_wealth_t *p_body = (db_request_update_wealth_t *)(p_db_pkg->body);

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    switch(p_user->waitcmd)
    {
	case as_msg_interactive_element:
         {
             p_user->user_cache.add_stuff(p_body->stuff_id, 1);
    			KINFO_LOG(p_user->uid, "send interactive element to AS after update wealth:stuff id=%u", p_body->stuff_id);
			g_send_msg.end();
			send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
			break;
         }
     case as_msg_commit_game_change:
         {
             p_user->user_cache.add_stuff(p_body->stuff_id, 1);
    			KINFO_LOG(p_user->uid, "send commit game change  to AS after update wealth:stuff id=%u", p_body->stuff_id);
			g_send_msg.end();
			send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
			break;
         }
     case as_msg_donate_info:
         {
             db_return_cur_donate_t *res_cur = (db_return_cur_donate_t *)(p_body->buf);
             p_user->user_cache.add_stuff(res_cur->reward_id, 1);

             g_send_msg.pack(res_cur->reward_donate_id);
             g_send_msg.pack(res_cur->donate_id);
             g_send_msg.pack(res_cur->donate_num);
             g_send_msg.pack(res_cur->donor_count);
     KINFO_LOG(p_user->uid, "send cur donate info  to AS after gained reward: reward_donate_id:%u, donate_id:%u, donate_num:%u, donor_count:%u", res_cur->reward_donate_id, res_cur->donate_id, res_cur->donate_num, res_cur->donor_count);
             g_send_msg.end();
             send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
             break;
         }
        case as_msg_donate:
            {//需要通知as最新的金币数
                db_return_donate_coins_t *res_coins = (db_return_donate_coins_t *)(p_body->buf);

                if(res_coins->flag == DENOTE_SUCCESS_AND_REACHED || res_coins->flag == DENOTE_SUCCESS_BUT_NOT_REACHED)
                {
                    role_cache_t *p_role = p_user->user_cache.get_role();
                    if (NULL == p_role)
                    {
                        KERROR_LOG(p_user->uid, "get role failed");
                        return -1;
                    }
                    if(p_role->role.coins >= res_coins->money)
                    {
                        p_role->role.coins -= res_coins->money;
                    }
                    else
                    {
                        p_role->role.coins = 0;
                    }
                }

                if(res_coins->reward_id != 0)
                {
                    p_user->user_cache.add_stuff(res_coins->reward_id, 1);
                }

                g_send_msg.pack(res_coins->donate_id);
                g_send_msg.pack(res_coins->flag);
                if(res_coins->flag == DENOTE_SUCCESS_AND_REACHED || res_coins->flag == DENOTE_FAIL_AND_REACHED)
                {
                    g_send_msg.pack(res_coins->reward_donate_id);
                    g_send_msg.pack(res_coins->next_donate_id);
                }
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

                KINFO_LOG(p_user->uid, "send donate coins to AS after update wealth: donate_id:%u flag:%u", res_coins->donate_id, res_coins->flag);

                stat_three_param_t stat_data3 = {p_user->uid, 1, res_coins->money};
                msg_log(stat_denote_base, &stat_data3, sizeof(stat_data3));

                //去除这个统计项 2012-06-07
    //            stat_two_param_t stat_data2 = {p_user->uid, 1};
    //            msg_log(stat_denote_base + res_coins->donate_id, &stat_data2, sizeof(stat_data2));


                notify_as_current_user_info(p_user);


                break;
            }
    }


    return 0;
}

int process_get_activity_rewarded_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get activity rewarded failed:%u", p_db_pkg->result);
        return 0;
    }

   single_activity_t *p_act = (single_activity_t*)p_db_pkg->body;
   if(p_act->kind_num == 0)
   {

       char buffer[2048] = {0};
       p_act = (single_activity_t*)buffer;
	   p_act->activity_id = OPEN_ACTIVITY_ID;
        if(get_actinfo_from_conf(p_act) == -1)
        {
            KCRIT_LOG(p_user->uid, "err act_id:%u", p_act->activity_id);
            return -1;
        }
   }

        p_user->user_cache.set_cached_open_act(p_act);


       activity_open(p_user, OPEN_ACTIVITY_ID, p_act);

        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_act->activity_id);
        g_send_msg.pack(p_act->kind_num);
        for(uint16_t idx = 0; idx < p_act->kind_num; idx++)
        {
            g_send_msg.pack(p_act->reward[idx].reward_id);
            g_send_msg.pack(p_act->reward[idx].reward_num);
            g_send_msg.pack(p_act->reward[idx].reward_status);
        }

        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_get_open_reward_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get activity rewarded failed:%u", p_db_pkg->result);
        return 0;
    }

    db_msg_get_open_reward_t *p_ret = (db_msg_get_open_reward_t*)p_db_pkg->body;

    p_user->user_cache.update_cached_open_act(p_ret->reward_id);//更新缓存置为已领奖

    //统计人数
    stat_one_param_t stat_data1 = {p_user->uid};
    if(p_ret->reward_id == OPEN_ACT_REWARD1)
    {//窗户
        msg_log(stat_open_window, &stat_data1, sizeof(stat_data1));
    }
    else if(p_ret->reward_id == OPEN_ACT_REWARD2)
    {
        msg_log(stat_open_floor, &stat_data1, sizeof(stat_data1));
    }
    else if(p_ret->reward_id == OPEN_ACT_REWARD3)
    {
        msg_log(stat_open_door, &stat_data1, sizeof(stat_data1));
    }
    else if(p_ret->reward_id == OPEN_ACT_REWARD4)
    {
        msg_log(stat_open_wallpaper, &stat_data1, sizeof(stat_data1));
    }

    p_user->user_cache.add_stuff(p_ret->reward_id, p_ret->reward_num);

        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_ret->act_id);
        g_send_msg.pack(p_ret->reward_id);
        g_send_msg.pack((uint32_t)ACTIVITY_STATUS_RUNNING);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_update_act_status_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        g_errno = p_db_pkg->result;
        KCRIT_LOG(p_user->uid, "db get activity rewarded failed:%u", p_db_pkg->result);
        return 0;
    }

    single_activity_t *p_ret = (single_activity_t*)p_db_pkg->body;
    p_user->user_cache.set_cached_open_act(p_ret);
    p_user->waitcmd = 0;

    return 0;
}


int process_prize_lottery_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
	assert(p_user != NULL && p_db_pkg != NULL);
	if (p_db_pkg->result != 0)
	{
	  g_errno = p_db_pkg->result;
	  KCRIT_LOG(p_user->uid, "db request prize_lottery failed:%u", p_db_pkg->result);
	  return 0;
	}

	db_request_prize_lottery_t *p_body = (db_request_prize_lottery_t *)(p_db_pkg->body);
	p_user->user_cache.add_stuff(p_body->stuff_b_id, 1);
	p_user->user_cache.desc_stuff(p_body->stuff_a_id,p_body->cost);
	uint16_t stuff_a_num = p_user->user_cache.get_stuff_num(p_body->stuff_a_id);

	pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
	g_send_msg.pack(p_body->stuff_a_id);
	g_send_msg.pack(stuff_a_num);
	g_send_msg.pack(p_body->stuff_b_id);
	g_send_msg.pack(p_body->category);
	g_send_msg.end();
	KINFO_LOG(p_user->uid, "send prize lottery to AS: lottery_num:%u", stuff_a_num);
	send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

	return 0;
}
