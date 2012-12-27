/**
 * ========================================================================
 * @file work_proc.cpp
 * @brief
 * @author henry（韩林），henry@taomee.com
 * @version 1.0.0
 * @date 2011-06-30
 * Modify $Date: $
 * Modify $Author: $
 * copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <new>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#include "work_proc.h"
#include "../role-info/role_info.h"
#include "../bag/bag.h"
#include "../room/room.h"
#include "../puzzle/puzzle.h"
#include "../friend/friend.h"
#include "../pinboard/pinboard.h"
#include "../plantation/plantation.h"
#include "../day-restrict/day_restrict.h"
#include "../pet/pet.h"
#include "../badge/badge.h"
#include "../game-level/game_level.h"
#include "../game-changed/game_changed.h"
#include "../npc-score/npc_score.h"
#include "../service/service.h"
#include "../task/task.h"
//#include "../denote/denote.h"
#include "../museum/museum.h"

#include "../activity/activity.h"
#include "../donate/donate.h"

using namespace std;

/**
 * @brief  创建work_proc进程的一个实例
 * @param  pp_instance
 * @return  0success -1 failed
 */
int create_work_proc_instance(i_work_proc **pp_instance)
{
    if(pp_instance == NULL)
    {
        return -1;
    }

    c_work_proc *p_instance = new (std::nothrow)c_work_proc();
    if(p_instance == NULL)
    {
        return -1;
    }
    else
    {
        *pp_instance = dynamic_cast<i_work_proc*>(p_instance);
        return 0;
    }
}

c_work_proc::c_work_proc()
{
    m_inited = 0;
    m_p_mysql_conn = NULL;
    m_errno = ERR_NO_ERR;
}

c_work_proc::~c_work_proc()
{
    uninit();
}

int c_work_proc::release()
{
    delete this;
    return 0;
}

int c_work_proc::init(int argc, char ** argv)
{
    if (m_inited)
    {
        ERROR_LOG("work proc inited");
        return -1;
    }

    //读取配置
    char *db_host = config_get_strval("db_host");
    if(db_host == NULL)
    {
        ERROR_LOG("read config(db_host) failed.");
        return -1;
    }

    char *db_name = config_get_strval("db_name");
    if(db_name == NULL)
    {
        ERROR_LOG("read config(db_name) failed.");
        return -1;
    }

    char *db_user = config_get_strval("db_user");
    if(db_user == NULL)
    {
        ERROR_LOG("read config(db_user) failed.");
        return -1;
    }

    char *db_pass = config_get_strval("db_pass");
    if(db_pass == NULL)
    {
        ERROR_LOG("read config(db_pass) failed.");
        return -1;
    }

    int db_port = config_get_intval("db_port", 3306);
    if (db_port <= 0 || db_port >= 65535)
    {
        ERROR_LOG("db_port must between 0 and 65535");
        return -1;
    }

    // 连接到数据库
    if (0 != create_mysql_iface_instance(&m_p_mysql_conn))
    {
        ERROR_LOG("create mysql iface failed.");
        return -1;
    }
	if (0 != m_p_mysql_conn->init(db_host, db_port, db_name, db_user, db_pass, "utf8"))
	{
		ERROR_LOG("mysql init failed (%s)", m_p_mysql_conn->get_last_errstr());
        m_p_mysql_conn->release();
        m_p_mysql_conn = NULL;
		return -1;
	}

    char *memcache_ip = config_get_strval("memcache_ip");
    if(memcache_ip == NULL)
    {
        ERROR_LOG("read config(memcache_ip) failed.");
        return -1;
    }

    int memcache_port = config_get_intval("memcache_port", 11311);
    if (memcache_port <= 0 || memcache_port >= 65535)
    {
        ERROR_LOG("memcache_port must between 0 and 65535");
        return -1;
    }

    if (m_memcached.init(memcache_ip, memcache_port)  < 0)
    {
		ERROR_LOG("memcache init failed");
        return -1;
    }

    init_msg_map();

    m_inited = 1;
    return 0;
}


int c_work_proc::uninit()
{
    if (!m_inited)
    {
        return -1;
    }

    if(m_p_mysql_conn)
    {
        m_p_mysql_conn->uninit();
        m_p_mysql_conn->release();
        m_p_mysql_conn = NULL;
    }

    m_memcached.uninit(1);

    m_inited = 0;
    return 0;
}

int c_work_proc::process(char * req, int req_len , char ** ack, int * ack_len)
{
    svr_msg_header_t * p_msg_header = (svr_msg_header_t *)req;
    uint32_t len = p_msg_header->len;
    m_seq_id = p_msg_header->seq_id;
    m_msg_type = p_msg_header->msg_type;
    m_user_id = p_msg_header->user_id;

    *ack = m_response_buffer;

    g_pack.begin(SEND_TO_SVR, (char *)*ack);
    g_pack.pack(m_seq_id);
    g_pack.pack(m_msg_type);
    g_pack.pack((uint32_t)0);
    g_pack.pack(m_user_id);

    KDEBUG_LOG(m_user_id, "[%u:%u] recv.len = %u", m_msg_type, m_seq_id, len);

    if(len != (uint32_t)req_len)
    {
        KCRIT_LOG(m_user_id, "msg_len:%u, req_len:%u", len, req_len);
        m_errno = ERR_MSG_LEN;
    }
    else
    {

        char *p_request_body = req + sizeof(svr_msg_header_t);

        m_errno = ERR_NO_ERR;

        map<uint16_t, MSG_PROCESS_FUNC>::iterator iter =  m_msg_func_map.find(m_msg_type);
        if (iter == m_msg_func_map.end())
        {
            KCRIT_LOG(m_user_id, "invalid msg type:%u", m_msg_type);
            m_errno = ERR_MSG_TYPE;
        }
        else
        {
            m_errno = iter->second(m_p_mysql_conn, &m_memcached, m_user_id, m_msg_type, p_request_body, len - sizeof(svr_msg_header_t));
        }
    }

    if (ERR_NO_ERR != m_errno)
    {
        g_pack.pack(m_errno, 10);
        g_pack.end(sizeof(svr_msg_header_t));
    }
    else
    {
        g_pack.end();
    }

    *ack_len = g_pack.get_msg_len();

    KDEBUG_LOG(m_user_id, "[%u:%u] send.len = %u\terrno = %u", m_msg_type, m_seq_id, *ack_len, m_errno);
    return 0;
}

int c_work_proc::init_msg_map()
{
    m_msg_func_map[svr_msg_db_add_role] = add_new_role;                         //53408
    m_msg_func_map[svr_msg_db_role_info] = query_role_info;                     //53409
    m_msg_func_map[svr_msg_db_get_bag_stuff] = get_bag_stuff;                   //53410
    m_msg_func_map[svr_msg_db_add_bag_stuff] = add_bag_stuff;                   //53411
    m_msg_func_map[svr_msg_db_update_bag_stuff] = update_bag_stuff;             //53412
    m_msg_func_map[svr_msg_db_room_num] = get_room_num;                         //53413
    m_msg_func_map[svr_msg_db_room_info] = get_room_info;                       //53414
    m_msg_func_map[svr_msg_db_add_room] = add_room;                             //53415
    m_msg_func_map[svr_msg_db_update_role] = update_role;                       //53416
    m_msg_func_map[svr_msg_db_update_room] = update_room;                       //53417
    m_msg_func_map[svr_msg_db_get_puzzle_info] = get_puzzle_info;               //53418
    m_msg_func_map[svr_msg_db_commit_puzzle] = commit_puzzle;                   //53419
    m_msg_func_map[svr_msg_db_add_role_value] = update_role_value;              //53420
    m_msg_func_map[svr_msg_db_get_friend_id] = get_friend_id;                   //53421
    m_msg_func_map[svr_msg_db_apply_for_friend] = apply_for_friend;             //53422
    m_msg_func_map[svr_msg_db_get_friend_apply] = get_friend_apply;             //53423
    m_msg_func_map[svr_msg_db_set_friend_status] = set_friend_status;           //53424
    m_msg_func_map[svr_msg_db_get_friend_info] = get_friend_info;               //53425
    m_msg_func_map[svr_msg_db_get_pinboard_info] = get_pinboard_info;           //53426
    m_msg_func_map[svr_msg_db_add_message] = add_message;                       //53427
    m_msg_func_map[svr_msg_db_update_message_status] = update_message_status;   //53428
    m_msg_func_map[svr_msg_db_del_friend] = del_friend;                         //53429
    m_msg_func_map[svr_msg_db_pet_op] = pet_op;                                 //53430
    m_msg_func_map[svr_msg_db_add_plant] = add_plant;                           //53431
    m_msg_func_map[svr_msg_db_del_plant] = del_plant;                           //53432
    m_msg_func_map[svr_msg_db_update_plant] = update_plant;                     //53433
    m_msg_func_map[svr_msg_db_interactive] = update_coins_from_interactive;                //32794
    m_msg_func_map[svr_msg_db_get_day_restrict] = get_day_restrict;             //53435
    m_msg_func_map[svr_msg_db_add_day_restrict] = add_day_restrict;             //53436
    m_msg_func_map[svr_msg_db_get_profile] = get_profile_info;                  //53437
    m_msg_func_map[svr_msg_db_eat_food] = eat_food;                             //53438
    m_msg_func_map[svr_msg_db_modify_sign] = modify_personal_sign;              //53439
    m_msg_func_map[svr_msg_db_other_role_info] = query_other_role_info;         //53440
    m_msg_func_map[svr_msg_db_update_login] = update_login_info;                //53441
    m_msg_func_map[svr_msg_db_get_all_pet] = get_all_pet;                	    //53442
    m_msg_func_map[svr_msg_db_get_real_message_count] = get_real_message_count; //32803
    m_msg_func_map[svr_msg_db_get_all_badge] = get_all_badge;                   //32804
    m_msg_func_map[svr_msg_db_set_unread_badge] = update_recent_badge;          //32805
    m_msg_func_map[svr_msg_db_update_badge] = update_a_badge;                   //32806
    m_msg_func_map[svr_msg_db_compose_stuff] = compose_stuff;                   //32807
    m_msg_func_map[svr_msg_db_add_game_day_restrict] = add_game_day_restrict;   //32808
    m_msg_func_map[svr_msg_db_get_game_day_restrict] = get_game_day_restrict;   //32809
    m_msg_func_map[svr_msg_db_add_game_level] = add_game_level;                 //32810
    m_msg_func_map[svr_msg_db_get_game_level] = get_game_level;                 //32811
    m_msg_func_map[svr_msg_db_modify_monster_name] = modify_monster_name;       //32812
    m_msg_func_map[svr_msg_db_get_hole_reward] =  get_all_hole_reward;          //32813
    m_msg_func_map[svr_msg_db_maintain_plant] = maintain_plant;                //32814
    m_msg_func_map[svr_msg_db_get_plant] = msg_get_plant;                       //32815
    m_msg_func_map[svr_msg_sun_reward_happy] = sun_reward_happy;                //32816
    m_msg_func_map[svr_msg_db_sun_reward_exp] = sun_reward_exp;                 //32817
    m_msg_func_map[svr_msg_db_sun_reward_coins] = sun_reward_coins;                 //32818
    m_msg_func_map[svr_msg_db_get_npc_score] = get_npc_score;   //32819
    m_msg_func_map[svr_msg_db_set_npc_score] = set_npc_score;   //32820

    m_msg_func_map[svr_msg_db_get_game_changed_stuff] = get_game_changed;   //32821
    m_msg_func_map[svr_msg_db_set_game_changed_stuff] = set_game_changed;   //32822

    m_msg_func_map[svr_msg_db_get_finished_task] = get_finished_task_list; //32823
    m_msg_func_map[svr_msg_db_finish_task] = finished_a_task;           //32824
    m_msg_func_map[svr_msg_db_factory_op] = factory_op;            //32825

    m_msg_func_map[svr_msg_db_enter_museum] = enter_museum;            //32826
    m_msg_func_map[svr_msg_db_get_museum_reward] = get_museum_reward;            //32827
    m_msg_func_map[svr_msg_db_commit_museum_game] = commit_museum_game;            //32828

	m_msg_func_map[svr_msg_db_request_update_wealth] = request_update_wealth; //32829

    m_msg_func_map[svr_msg_db_get_activity_reward] = get_activity_reward; //32830
    m_msg_func_map[svr_msg_db_get_open_reward] = get_open_act_reward;//32831

    m_msg_func_map[svr_msg_update_activity_status] = update_activity_status; //32832
	m_msg_func_map[svr_msg_db_get_invite_info] = activity_get_invite_info; //32833
	m_msg_func_map[svr_msg_db_request_prize_lottery] = prize_lottery; //32834
	m_msg_func_map[svr_msg_db_get_activity_reward_ex] = activity_get_activity_reward_ex; //32835
	m_msg_func_map[svr_msg_db_update_level_relative] = activity_update_invite_friend; //32836
	m_msg_func_map[svr_msg_db_get_dragon_boat_info] = activity_get_dragon_boat_info; //32837
	m_msg_func_map[svr_msg_db_update_dragon_boat] = activity_update_dragon_boat_info; //32838


    m_msg_func_map[svr_msg_db_insert_name] = insert_name;                       //33280
    m_msg_func_map[svr_msg_db_search_name] = search_by_name;                    //33281
    m_msg_func_map[svr_msg_db_encourage_guide] = encourage_guide;//33285

    m_msg_func_map[svr_msg_db_request_history_donate] = request_donate_history;//33286
    m_msg_func_map[svr_msg_db_request_cur_donate] = request_cur_donate ; //33287 查看当前届援助计划
    m_msg_func_map[svr_msg_db_request_donate_coins] = request_donate_coins; //33288 0x8028 捐助金币


    //客服查询信息
    m_msg_func_map[svr_msg_service_role_info] = query_role_info_service;
    m_msg_func_map[svr_msg_service_friends_info] = query_friend_service;
    m_msg_func_map[svr_msg_service_badge_info] = query_badge_service;
    m_msg_func_map[svr_msg_service_pinborad_info] = query_pinboard_service;
    m_msg_func_map[svr_msg_service_stuff_info] = query_stuff_service;
    m_msg_func_map[svr_msg_service_game_info] = query_game_service;
    m_msg_func_map[svr_msg_service_puzzle_info] = query_puzzle_service;

    m_msg_func_map[svr_msg_boke_role_info] = query_role_info_boke;

    return 0;
}
