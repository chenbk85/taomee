/**
 * =====================================================================================
 *       @file  cli_proto.cpp
 *      @brief  as协议相关的处理函数
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/30/2011 10:04:54 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  henry(韩林), henry@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <glib.h>
#include <assert.h>
#include <math.h>
#include <map>
#include <vector>
#include <list>
#include <arpa/inet.h>
#include <queue>
#include <libtaomee++/inet/pdumanip.hpp>
extern "C"
{
#include <async_serv/net_if.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
#include <libtaomee/conf_parser/config.h>
}

#include "message.h"
#include "data_structure.h"
#include "constant.h"
#include "stat.h"

#include "cli_proto.h"
#include "data.h"
#include "function.h"
#include "plantation.h"
#include "stuff.h"
#include "utility.h"
#include "online_constant.h"
#include "badge.h"
#include "activity.h"

using namespace taomee;
using std::map;
using std::list;
using std::queue;
using std::vector;

void init_cli_handle_funs()
{
    g_cli_cmd_map.clear();

    g_cli_cmd_map[as_msg_acquire_userinfo] = process_register;                      //40101
    g_cli_cmd_map[as_msg_login_online] = process_login;                             //40102
    g_cli_cmd_map[as_msg_bag_stuff] = process_get_bag_stuff;                        //40103
    g_cli_cmd_map[as_msg_room_info] = process_get_room_info;                        //40104
    g_cli_cmd_map[as_msg_room_buf] = process_update_room_buf;                       //40105
    g_cli_cmd_map[as_msg_room_num] = process_get_room_num;                          //40106
    g_cli_cmd_map[as_msg_buy_stuff] = process_buy_stuff;                            //40108
    g_cli_cmd_map[as_msg_get_puzzle] = process_get_puzzle_info;                     //40109
    g_cli_cmd_map[as_msg_commit_puzzle] = process_commit_puzzle;                    //40110
    g_cli_cmd_map[as_msg_get_friend_list] = process_get_friend_list;                //40111
    g_cli_cmd_map[as_msg_get_user_info] = process_get_user_info;                    //40112
    g_cli_cmd_map[as_msg_friend_op] = process_friend_op;                            //40113
    g_cli_cmd_map[as_msg_find_friend] = process_find_friend;                        //40114
    g_cli_cmd_map[as_msg_add_friend] = process_add_friend;                          //40115
    g_cli_cmd_map[as_msg_get_friend_apply] = process_get_friend_apply;              //40116
    g_cli_cmd_map[as_msg_friend_apply_op] = process_friend_apply_op;                //40117
    g_cli_cmd_map[as_msg_get_pinboard] = process_get_pinboard;                      //40118
    g_cli_cmd_map[as_msg_add_message] = process_add_message;                        //40119
    g_cli_cmd_map[as_msg_update_message_status] = process_update_message_status;    //40120
    g_cli_cmd_map[as_msg_rating_room] = process_rating_user_room;                   //40121
    g_cli_cmd_map[as_msg_get_plant_info] = process_get_plant_info;                  //40122
    g_cli_cmd_map[as_msg_grow_plant] = process_grow_plant;                          //40123
    g_cli_cmd_map[as_msg_maintain_plant] = process_maintain_plant;                 //40124
    g_cli_cmd_map[as_msg_attract_pet] = process_attract_pet;                        //40125
    g_cli_cmd_map[as_msg_get_stranger] = process_get_stranger;                      //40126
    g_cli_cmd_map[as_msg_commit_game] = process_commit_game;                        //40127
    g_cli_cmd_map[as_msg_eat_food] = process_eat_food;                              //40128
    g_cli_cmd_map[as_msg_update_profile] = process_update_profile;                  //40129
    g_cli_cmd_map[as_msg_shop_item] = process_get_shop_item;                        //40130
    g_cli_cmd_map[as_msg_update_flag] = process_update_flag;                        //40131
    g_cli_cmd_map[as_msg_trade_stuff] = process_trade_stuff;                        //40132
    g_cli_cmd_map[as_msg_get_all_pet] = process_get_pet;                            //40133
    g_cli_cmd_map[as_msg_enter_two_puzzle] = process_enter_two_puzzle;              //40134
    g_cli_cmd_map[as_msg_start_two_puzzle] = process_start_two_puzzle;              //40135
    g_cli_cmd_map[as_msg_answer_two_puzzle] = process_answer_two_puzzle;            //40136
    g_cli_cmd_map[as_msg_interactive_element] = process_interactive_element;        //40137
    g_cli_cmd_map[as_msg_stat_data] = process_stat_data;                            //40138
    g_cli_cmd_map[as_msg_latest_visit] = process_get_latest_visit;                  //40139
    g_cli_cmd_map[as_msg_get_all_badge] = process_get_all_badge;                    //40140
    g_cli_cmd_map[as_msg_get_real_message] = process_get_real_message_count;        //40142
    g_cli_cmd_map[as_msg_get_unread_visit] = process_get_unread_visit;              //40143
    g_cli_cmd_map[as_msg_get_game_level] = process_get_game_level;                  //40144


    g_cli_cmd_map[as_msg_get_factory] = process_get_factory;                        //40145
    g_cli_cmd_map[as_msg_create_stuff] = process_create_stuff;                      //40146
    g_cli_cmd_map[as_msg_cancel_create] = process_factory_op;                       //40147
    g_cli_cmd_map[as_msg_dirty_word] = process_check_dirty_word;                    //40148
    g_cli_cmd_map[as_msg_modify_name] = process_modify_monster_name;                //40149
    g_cli_cmd_map[as_msg_get_random_name] = process_get_random_name;                    //40151
    g_cli_cmd_map[as_msg_get_bobo_reward] = process_get_bobo_reward;                //40152
    g_cli_cmd_map[as_msg_get_sun_reward] = process_get_sun_reward;                  //40153
    g_cli_cmd_map[as_msg_read_bobo_newspaper] = process_read_bobo_newspaper;        //40154
    g_cli_cmd_map[as_msg_enter_npc_score] = process_enter_npc_score;                  //40155
    g_cli_cmd_map[as_msg_npc_score] = process_npc_score;                  //40156
    g_cli_cmd_map[as_msg_get_game_change] = process_get_game_change;             //40157
    g_cli_cmd_map[as_msg_game_change] = process_game_change;                     //40158
    g_cli_cmd_map[as_msg_get_finished_task] = process_get_finished_task;        //40159
    g_cli_cmd_map[as_msg_finish_task] = process_finish_task;                //40160


    g_cli_cmd_map[as_msg_donate_info] = process_cur_donate_info;            //40161
    g_cli_cmd_map[as_msg_donate] = process_donate_coins;                //40162
    g_cli_cmd_map[as_msg_history_donate] = process_history_donate;      //40163



    g_cli_cmd_map[as_msg_enter_museum] = process_enter_museum;      //40164
    g_cli_cmd_map[as_msg_get_museum_reward] = process_get_museum_reward;      //40165
    g_cli_cmd_map[as_msg_answer_museum_end] = process_commit_museum_game;      //40166
    g_cli_cmd_map[as_msg_commit_game_change] = process_commit_game_change_score;      //40167
    g_cli_cmd_map[as_msg_update_guide_flag] = process_update_guide_flag;      //40168

    g_cli_cmd_map[as_msg_get_a_activity_info] = process_get_a_activity_info; //40169
    g_cli_cmd_map[as_msg_get_activity_reward] = process_get_activity_reward;//40170
    g_cli_cmd_map[as_msg_get_cur_activity] = process_get_activity_list;// 40171
    g_cli_cmd_map[as_msg_get_activity_reward_ex] = process_get_activity_reward_ex;//40172
    g_cli_cmd_map[as_msg_prize_lottery] = process_prize_lottery;// 40173
    g_cli_cmd_map[as_msg_enter_show] = process_enter_show;// 40174
	g_cli_cmd_map[as_msg_join_show] = process_join_show;// 40175
	g_cli_cmd_map[as_msg_guess_show] = process_guess_show;// 40176
    g_cli_cmd_map[as_msg_history_show] = process_history_show;//40177


    //online自己缓存的包的处理，不从as获得
    g_cli_cmd_map[switch_msg_add_friend] = process_add_friend_to_cache;             //43001
    g_cli_cmd_map[ucount_msg_add_visit] = process_ucount_add_visit;                 //43002
    g_cli_cmd_map[multi_puzzle_add_coin] = process_multi_puzzle_add_coin;           //43003
    g_cli_cmd_map[badge_update_status] = process_update_badge_status;               //43004
    g_cli_cmd_map[add_new_unlock_map] = process_add_new_unlock_map;                 //43005
    g_cli_cmd_map[activity_update_status] = process_update_activity_status;         //43006


    return;
}

int dispatch(void *p_data, int len, fdsession_t *fdsess, bool first_tm)
{
    as_msg_header_t *p_pkg = (as_msg_header_t *)p_data;
    as_msg_header_t pkg_header = {0};
    pkg_header.len = taomee::bswap(p_pkg->len);
    pkg_header.msg_type = taomee::bswap(p_pkg->msg_type);
    pkg_header.user_id = taomee::bswap(p_pkg->user_id);
    pkg_header.result = taomee::bswap(p_pkg->result);
    pkg_header.err_no = taomee::bswap(p_pkg->err_no);

    if (pkg_header.msg_type == as_msg_check_online)
    {
        return send_pkg_to_client(fdsess, p_pkg, pkg_header.len);
    }

    if(pkg_header.msg_type == msg_reload_conf_file)
    {
        return process_reload_conf(fdsess, p_pkg->body, pkg_header.len - sizeof(as_msg_header_t));
    }

    map<uint16_t, PROTO_PROCESS_FUNC>::iterator cmd_iter;
    int ret = 0;
    usr_info_t *p_user = NULL;
    g_errno = ERR_NO_ERR;

    p_user = g_user_mng.get_user(fdsess->fd);
    //错误的数据包
    if ((!p_user && pkg_header.msg_type != as_msg_login_online) ||
        (p_user && pkg_header.msg_type == as_msg_login_online) ||
        (p_user && p_user->uid != pkg_header.user_id) ||
        (p_user && pkg_header.msg_type != as_msg_acquire_userinfo && pkg_header.msg_type != as_msg_get_random_name && pkg_header.msg_type != as_msg_dirty_word && 0 == p_user->is_register) ||
        len != (int)pkg_header.len || len > MAX_RECV_PKG_SIZE || 0 == pkg_header.user_id)
    {
        if (!p_user)
        {
            KERROR_LOG(pkg_header.user_id, "pkg err, cmd:%u, fd:%u, len:%u,%u.", pkg_header.msg_type, fdsess->fd, len, pkg_header.len);
        }
        else
        {
            KERROR_LOG(pkg_header.user_id, "pkg err, user_id:%u, cmd:%u, fd:%u, len:%u,%u.", p_user->uid, pkg_header.msg_type, fdsess->fd, len, pkg_header.len);
        }
        return -1;
    }

    //first_tm为true表示是as发过来的信息
    if (first_tm && pkg_header.msg_type > switch_msg_add_friend)
    {
        KERROR_LOG(pkg_header.user_id, "cmd:%u invalid", pkg_header.msg_type);
        return -1;
    }

    if(first_tm)
    {
        KDEBUG_LOG(pkg_header.user_id, "AS\t[msg:%u, len:%u, seq:%u errno:%u]", pkg_header.msg_type, pkg_header.len, pkg_header.result, pkg_header.err_no);
        if (pkg_header.msg_type != as_msg_login_online)
        {
            //校验序列号
            uint32_t seq = get_new_seq(p_user->seq, len, pkg_header.msg_type);
            if (seq != pkg_header.result)
            {
                KCRIT_LOG(pkg_header.user_id, "pkg seq:%u is not expect:%u.", pkg_header.result, seq);
                return -1;
            }
            p_user->seq = seq;


            uint32_t body_sum = check_body_sum((uint8_t*)(p_pkg->body), pkg_header.len - sizeof(as_msg_header_t));
            //校验包体
            if(body_sum != pkg_header.err_no)
            {
                KCRIT_LOG(pkg_header.user_id, "pkg err_no:%u not expect %u", pkg_header.err_no, body_sum);
                return -1;
            }
        }
    }
    else
    {
        KDEBUG_LOG(pkg_header.user_id, "CACHE\t[msg:%u, len:%u]", pkg_header.msg_type, pkg_header.len);
    }


    if (first_tm && p_user && p_user->waitcmd != 0) //有包还没处理完，需要缓存在命令链表里
    {
        if (p_user->cached_pkg_queue.size() >= MAX_REQ_PER_USER)
        {
            KERROR_LOG(pkg_header.user_id, "too many cmd in process");
            g_errno = ERR_TOO_MANY_PKG;
            goto EXIT;
        }

        KDEBUG_LOG(p_user->uid, "waitcmd = %u, cache this pkg(msg_type:%u)", p_user->waitcmd, pkg_header.msg_type);

        if (cache_a_pkg(p_user, (char *)p_pkg, pkg_header.len) != 0)
        {
            KERROR_LOG(p_user->uid, "cache pkg failed");
            return -1;
        }
        return 0;
    }

    //登录协议
    if (NULL == p_user)
    {
        //在一台online上重复登录，关掉前面的连接
        fdsession_t *p_last_fdsess = g_user_mng.get_fd(pkg_header.user_id);
        if (p_last_fdsess != NULL)
        {
            pack_as_pkg_header(pkg_header.user_id, as_msg_level_up, 0, ERR_REPEAT_LOGIN);
            g_send_msg.end();
    	    send_pkg_to_client(p_last_fdsess, g_send_msg.get_msg(), g_send_msg.get_msg_len());
            close_client_conn(p_last_fdsess->fd);
            KERROR_LOG(pkg_header.user_id, "login repeatly, close last conn");
        }

        p_user = g_user_mng.alloc_user(fdsess->fd, pkg_header.user_id, fdsess);
        if (NULL == p_user)
        {
            KERROR_LOG(pkg_header.user_id, "alloc user failed");
            return -1;
        }
    }

    assert(p_user != NULL);
    //当前请求可以马上处理,设置正在处理的协议号
    cmd_iter = g_cli_cmd_map.find(pkg_header.msg_type);
    //未知的协议
    if (cmd_iter == g_cli_cmd_map.end())
    {
        KERROR_LOG(pkg_header.user_id, "invalid msg:%u", pkg_header.msg_type);
        return -1;
    }

    KDEBUG_LOG(p_user->uid, "process, waitcmd = %u", p_user->waitcmd);
    p_user->waitcmd = pkg_header.msg_type;
    ret = cmd_iter->second(p_user, p_pkg->body, pkg_header.len - sizeof(as_msg_header_t));
    if (ret != 0)
    {
    	KCRIT_LOG(p_user->uid, "process msg:%u faield", pkg_header.msg_type);
        return -1;
    }
    KDEBUG_LOG(p_user->uid, "process finish, waitcmd = %u, errno:%u", p_user->waitcmd, g_errno);

EXIT:
    if (g_errno != 0) //协议处理失败，给客户端回失败消息
    {
        if(g_errno == ERR_TOO_MANY_PKG)
        {
            pack_as_pkg_header(p_user->uid, pkg_header.msg_type, SYSTEM_ERR_NUM, g_errno);
            g_send_msg.end();
    	    send_pkg_to_client(p_user->session, g_send_msg.get_msg(), g_send_msg.get_msg_len());
            //命令缓存满了，直接给客户端回errno,
        }
        else
        {
            send_header_to_user(p_user, SYSTEM_ERR_NUM, g_errno);
        }
    }

    return 0;
}

int cache_a_pkg(usr_info_t *p_user, const char *buf, uint32_t buf_len, bool clear_cmd)
{
    uint16_t len = sizeof(cached_pkg_t) + buf_len;
    cached_pkg_t *cache_buf = reinterpret_cast<cached_pkg_t *>(g_slice_alloc(len));
    if (NULL == cache_buf)
    {
        KCRIT_LOG(p_user->uid, "g slice alloc failed");
        return -1;
    }

    if(clear_cmd)
    {
	    p_user->waitcmd = 0;
    }

    cache_buf->len = len;
    memcpy(cache_buf->pkg, buf, buf_len);
    p_user->cached_pkg_queue.push(cache_buf);
    if (p_user->waiting_flag == 0)   //用户不在等待链表里面，需要加入
    {
        g_waiting_user_list.insert(g_waiting_user_list.end(), p_user->session->fd);
        p_user->waiting_flag = 1;
    }
    return 0;
}

void proc_cached_pkgs()
{
    for (list<int>::iterator iter = g_waiting_user_list.begin(); iter != g_waiting_user_list.end();)    //遍历所有待处理的用户
    {
        usr_info_t *p_user = g_user_mng.get_user(*iter);
        if (NULL == p_user) //用户已经断开连接,将用户从链表里面删除
        {
            list<int>::iterator cur_iter = iter;
            ++iter;
            g_waiting_user_list.erase(cur_iter);
            continue;
        }

        queue<cached_pkg_t *> &cmd_queue = p_user->cached_pkg_queue;
        while (0 == p_user->waitcmd)   //当前用户没有命令再处理，则从命令队列里去命令处理
        {
            KDEBUG_LOG(p_user->uid, "proc cached pkg, waitcmd:%u", p_user->waitcmd);
            cached_pkg_t *p_cached_cmd = cmd_queue.front();
            if(p_cached_cmd == NULL)
            {
                CRIT_LOG("user %u cmd_queue is empty.", p_user->uid);
                break;
            }

            int err = dispatch(p_cached_cmd->pkg, p_cached_cmd->len - sizeof(cached_pkg_t), p_user->session, false);
            usr_info_t *p_tmp_usr = g_user_mng.get_user(*iter);
            if(p_tmp_usr == NULL)
            {
                CRIT_LOG("user %u deleted already.", p_user->uid);
                break;
            }

            g_slice_free1(p_cached_cmd->len, p_cached_cmd);
            cmd_queue.pop();
            if (0 == err)
            {
                if (cmd_queue.empty())
                {
                    list<int>::iterator cur_iter = iter;
                    ++iter;
                    g_waiting_user_list.erase(cur_iter);
                    p_user->waiting_flag = 0;
                    break;
                }
            }
            else
            {
                close_client_conn(p_user->session->fd);
                break;
            }
        }

        ++iter;
    }
}

int process_login(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, (int)msg_body_len, (int)sizeof(as_msg_login_req_t), -1);

    as_msg_login_req_t login_req = {{0}};
    int idx = 0;
    taomee::unpack(p_msg_body, login_req.session, sizeof(login_req.session), idx);

    //先向account平台验证session
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    account_msg_check_session_t *p_body = (account_msg_check_session_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, account_msg_check_session, p_user->uid, GET_SVR_SN(p_user), sizeof(account_msg_check_session_t));

    p_body->game_id = ACCOUNT_GAME_ID;
    memcpy(p_body->session, login_req.session, sizeof(login_req.session));
    p_body->del_session_flag = 1;           //验证完后删除session

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        KERROR_LOG(p_user->uid, "send data to account failed.");
        return -1;
    }

    return 0;
}

int process_register(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, (int)msg_body_len, (int)sizeof(as_msg_register_req_t), 0);

    int idx = 0;
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;

    db_msg_add_role_req_t *p_body = (db_msg_add_role_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    //设置请求包的数据
    taomee::unpack(p_msg_body, p_body->name, sizeof(p_body->name), idx);
    char name[sizeof(p_body->name) + 1] = {0};
    memcpy(name, p_body->name, sizeof(p_body->name));
    if (tm_dirty_check(0, name) != 0)
    {
        g_errno = ERR_DIRTY_WORDS;
        KCRIT_LOG(p_user->uid, "dirty name");
        return 0;
    }
    taomee::unpack(p_msg_body, p_body->gender, idx);
    taomee::unpack(p_msg_body, p_body->country_id, idx);
    taomee::unpack(p_msg_body, p_body->birthday, idx);
    p_body->register_time = time(NULL);
    p_body->coins = INIT_COINS;
    p_body->last_login_time = p_body->register_time;
    taomee::unpack(p_msg_body, p_body->monster_id, idx);
    taomee::unpack(p_msg_body, p_body->monster_name, sizeof(p_body->monster_name), idx);
    memset(name, 0, sizeof(name));
    memcpy(name, p_body->monster_name, sizeof(p_body->name));
    if (tm_dirty_check(0, name) != 0)
    {
        g_errno = ERR_DIRTY_WORDS;
        return 0;
    }
    taomee::unpack(p_msg_body, p_body->monster_main_color, idx);
    taomee::unpack(p_msg_body, p_body->monster_ex_color, idx);
    taomee::unpack(p_msg_body, p_body->monster_eye_color, idx);
    taomee::unpack(p_msg_body, p_body->invitor_id, idx);
    p_body->monster_exp = INIT_EXP;
    p_body->monster_level = get_level_from_exp(INIT_EXP);
    p_body->monster_health = INIT_HEALTH;
    p_body->monster_happy = INIT_HAPPY;

    if (!(p_body->monster_id > 0 && p_body->monster_id < 7))
    {
        KCRIT_LOG(p_user->uid, "register mon_id:%d", p_body->monster_id);
        return -1;
    }

    KINFO_LOG(p_user->uid, "register, gender:%u, birthday:%u, monster:%u, main color:%u, ex color:%u, eye color:%u",
            p_body->gender, p_body->birthday, p_body->monster_id, p_body->monster_main_color, p_body->monster_ex_color, p_body->monster_eye_color);

    //初始room_buf
    user_room_buf_t init_room_buf = {0};
    init_room_buf.buf_len = taomee::bswap((uint16_t)sizeof(user_room_buf_t));
    init_room_buf.stuff_num = 0;


        p_body->wallpaper_id = WALLPAPER_DEFAULT_ID;
        p_body->floor_id = FLOOR_DEFAULT_ID;
        p_body->window_id = WINDOWS_DEFAULT_ID;
        p_body->door_id = DOOR_DEFAULT_ID;

        init_room_buf.wallpaper_id = taomee::bswap((uint32_t)WALLPAPER_DEFAULT_ID);
        init_room_buf.door_id = taomee::bswap((uint32_t)DOOR_DEFAULT_ID);
        init_room_buf.floor_id = taomee::bswap((uint32_t)FLOOR_DEFAULT_ID);
        init_room_buf.windows_id = taomee::bswap((uint32_t)WINDOWS_DEFAULT_ID);



    p_body->room_id = ROOM_DEFAULT_ID;

    init_room_buf.buf_len = bswap((uint16_t)sizeof(user_room_buf_t));
    memcpy((char *)&p_body->room_buf, &init_room_buf, sizeof(user_room_buf_t));

    //确定赠送的种子

    p_body->seed1 = (uint32_t)90004;
    p_body->seed2 = (uint32_t)0;
    p_body->seed3 = (uint32_t)0;

    //int num = g_seed_reward.size();
    //if (num > 0)
    //{
    //    int index = uniform_rand(0, num - 1);
    //    p_body->seed1 = g_seed_reward[index].seed1;
    //    p_body->seed2 = g_seed_reward[index].seed2;
    //    p_body->seed3 = g_seed_reward[index].seed3;
    //}
    //else
    //{
    //    p_body->seed1 = 0;
    //    p_body->seed2 = 0;
    //    p_body->seed3 = 0;
    //}

    //默认好友
    p_body->default_friend = config_get_intval("default_friend_id", 12345);
    //赠送的物品
    p_body->stuff_count = 5;

    p_body->stuff_id[0] = 20026;

    if (FEMALE == p_body->gender)
    {//女性
        p_body->stuff_id[1] = (uint32_t)FEMALE_WALLPAPER_DEFAULT_ID;
        p_body->stuff_id[2] = (uint32_t)FEMALE_FLOOR_DEFAULT_ID;
        p_body->stuff_id[3] = (uint32_t)FEMALE_WINDOWS_DEFAULT_ID;
        p_body->stuff_id[4] = (uint32_t)FEMALE_DOOR_DEFAULT_ID;

    }
    else
    {
        p_body->stuff_id[1] = (uint32_t)MALE_WALLPAPER_DEFAULT_ID;
        p_body->stuff_id[2] = (uint32_t)MALE_FLOOR_DEFAULT_ID;
        p_body->stuff_id[3] = (uint32_t)MALE_WINDOWS_DEFAULT_ID;
        p_body->stuff_id[4] = (uint32_t)MALE_DOOR_DEFAULT_ID;
    }
    pack_svr_pkg_head(p_header, svr_msg_db_add_role, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_add_role_req_t) + p_body->stuff_count * sizeof(uint32_t));

    //给db发请求新增一个用户
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
        return 0;
    }

    //缓存用户角色信息
    role_cache_t role_cache = {{{0}}};
    memcpy(role_cache.role.name, p_body->name, sizeof(p_body->name));
    role_cache.role.gender = p_body->gender;
    role_cache.role.country_id = p_body->country_id;
    role_cache.role.birthday = p_body->birthday;
    role_cache.role.register_time = p_body->register_time;
    role_cache.role.last_login_time = p_body->last_login_time;
    role_cache.role.coins = p_body->coins;
    role_cache.role.monster_id = p_body->monster_id;
    memcpy(role_cache.role.monster_name, p_body->monster_name, sizeof(p_body->monster_name));
    role_cache.role.monster_main_color = p_body->monster_main_color;
    role_cache.role.monster_ex_color = p_body->monster_ex_color;
    role_cache.role.monster_eye_color = p_body->monster_eye_color;
    role_cache.role.monster_exp = p_body->monster_exp;
    role_cache.role.monster_level = p_body->monster_level;
    role_cache.role.monster_health = p_body->monster_health;
    role_cache.role.monster_happy = p_body->monster_happy;
    role_cache.last_update_health_time = p_body->last_login_time;
    role_cache.role.room_num = 1;
    role_cache.role.recent_unread_badge = 0;
	role_cache.role.invitor_id = p_body->invitor_id;

    //缓存用户的登录信息
    if (p_user->user_cache.add_role(&role_cache) != 0)
    {
        KCRIT_LOG(p_user->uid, "cache user role");
        g_errno = ERR_ADD_ROLE_CACHE;
        return 0;
    }
    room_value_t default_room = {0};

    default_room.buf_len = sizeof(user_room_buf_t);
    memcpy(default_room.buf, &init_room_buf, sizeof(user_room_buf_t));

    if (p_user->user_cache.add_default_room(&default_room) != 0)
    {
        KCRIT_LOG(p_user->uid, "add default room to cache");
        g_errno = ERR_ADD_ROOM_BUF;
        return 0;
    }

    //缓存用户的成就信息
    char buffer[10] = {0};
    all_badge_info_t *p_badge = (all_badge_info_t*)buffer;
    p_badge->badge_num = 0;

    p_user->user_cache.add_all_badge(p_badge);

    //缓存种植园
    char hole_buf[1024] = {0};
    all_hole_info_t *p_all_hole = (all_hole_info_t*)hole_buf;
    p_all_hole->hole_count = 3;
    p_all_hole->hole[0].hole_id = 1;
    p_all_hole->hole[1].hole_id = 2;
    p_all_hole->hole[2].hole_id = 3;

    p_user->user_cache.cached_all_hole(p_all_hole);
    return 0;
}

int process_get_bag_stuff(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, (int)msg_body_len, 0, 0);


    //先看缓存有没有
    char buffer[MAX_SEND_PKG_SIZE] = {0};
    db_msg_get_bag_rsp_t *p_bag = (db_msg_get_bag_rsp_t *)buffer;

    if (p_user->user_cache.get_bag(p_bag) == 0)
    {
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack((uint16_t)0);   //先占位,物品的数量
        uint16_t num = 0;
        for (int i = 0; i != (int)p_bag->num; ++i)
        {
            uint32_t stuff_id = p_bag->stuff[i].stuff_id;
            map<uint32_t, item_t>::iterator iter = g_item_map.find(stuff_id);
            if (iter == g_item_map.end())
            {
                KCRIT_LOG(p_user->uid, "stuff:%u is not exists.", stuff_id);
                continue;
            }

            uint16_t stuff_num = p_bag->stuff[i].stuff_num;
            uint16_t used_num = p_bag->stuff[i].used_num;

            if (stuff_num > used_num)
            {
                g_send_msg.pack(stuff_id);
                g_send_msg.pack((uint16_t)(stuff_num - used_num));
                g_send_msg.pack(iter->second.category);
                ++num;
#ifdef DEBUG
                KINFO_LOG(p_user->uid, "stuff:%u, num:%u", stuff_id, stuff_num - used_num);
#endif
            }
        }

        //更新物品的个数
        KINFO_LOG(p_user->uid, "get %u stuff from cache", num);
        g_send_msg.pack(num, sizeof(as_msg_header_t));
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        return 0;
    }

    //缓存没有，从数据库获得
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;

    pack_svr_pkg_head(p_header, svr_msg_db_get_bag_stuff, p_user->uid, GET_SVR_SN(p_user), 0);
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_ACCOUNT;
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_get_room_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, (int)msg_body_len, (int)sizeof(as_msg_get_room_req_t), 0);

    //获得包体
    as_msg_get_room_req_t *p_req = (as_msg_get_room_req_t *)p_user->buffer;
    int idx = 0;
    taomee::unpack(p_msg_body, p_req->user_id, idx);
    taomee::unpack(p_msg_body, p_req->room_id, idx);

    if (p_req->user_id == p_user->uid)  //获得的是自己的房屋信息，先在缓存里找
    {
        room_value_t *p_room = NULL;
        if (ROOM_DEFAULT_ID == p_req->room_id)
        {
            p_room = p_user->user_cache.get_default_room();    //默认房屋
        }
        else if (p_user->user_cache.get_cur_room_id() == p_req->room_id)
        {
            p_room = p_user->user_cache.get_cur_room();    //最后一次拉取的房屋
        }

        //缓存里有数据，直接给客户端回消息
        if (p_room != NULL)
        {
            KINFO_LOG(p_user->uid, "get room:%u info from cache", p_req->room_id);
            pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
            g_send_msg.pack(p_room->buf, p_room->buf_len);
            g_send_msg.end();
            send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
            return 0;
        }
    }

    //pack请求包
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    db_msg_get_room_req_t *p_body = (db_msg_get_room_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, svr_msg_db_room_info, p_req->user_id, GET_SVR_SN(p_user), sizeof(db_msg_get_room_req_t));

    p_body->room_id = p_req->room_id;
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_update_room_buf(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL_GE(p_user->uid, (int)msg_body_len, (int)sizeof(user_room_buf_t), 0);

    int idx = 0;
    char buffer_req[MAX_SEND_PKG_SIZE] = {0};
    as_msg_update_room_req_t *p_req = (as_msg_update_room_req_t *)buffer_req;

    taomee::unpack(p_msg_body, p_req->room_id, idx);
    int buf_len = msg_body_len - sizeof(as_msg_update_room_req_t);

    KINFO_LOG(p_user->uid, "update room:%u, buf len:%u", p_req->room_id, buf_len);
    CHECK_VAL_LE(p_user->uid, buf_len, MAX_ROOM_STUFF * 32, 0);

    taomee::unpack(p_msg_body, p_req->buf, buf_len, idx);

    //XXX 注意，如果解析出来的物品超过1000多个，这个地方会core掉
    char new_room_buf[MAX_SEND_PKG_SIZE] = {0};
    room_stuff_t *p_new_room = (room_stuff_t *)new_room_buf;
    //获得要更新的房间物品的id
    if (parse_room_buf(p_req->buf, buf_len, p_new_room) != 0)
    {
        KCRIT_LOG(p_user->uid, "parse room buf failed.");
        g_errno = ERR_MSG_PARSE_ROOM_BUF;
        return 0;
    }

    uint32_t new_room_stuff_num = p_new_room->stuff_num;
    KINFO_LOG(p_user->uid, "new_room_stuff_num=%u", new_room_stuff_num);

    room_value_t *p_room = NULL;
    if (ROOM_DEFAULT_ID == p_req->room_id)
    {
        p_room = p_user->user_cache.get_default_room();
    }
    else if (p_user->user_cache.get_cur_room_id() == p_req->room_id)
    {
        p_room = p_user->user_cache.get_cur_room();
    }
    if (NULL == p_room)
    {
        KCRIT_LOG(p_user->uid, "room:%u buf is not in cache.", p_req->room_id);
        g_errno = ERR_MSG_CACHE_ROOM_BUF;
        return 0;
    }
    char old_room_buf[MAX_SEND_PKG_SIZE] = {0};
    room_stuff_t *p_old_room = (room_stuff_t *)old_room_buf;
    //获得老的房间物品的id
    if (parse_room_buf(p_room->buf, p_room->buf_len, p_old_room) != 0)
    {
        KCRIT_LOG(p_user->uid, "parse current room:%u buf failed.", p_req->room_id);
        g_errno = ERR_MSG_PARSE_ROOM_BUF;
        return 0;
    }



    //进行buf的比较，确定物品的变化
    stuff_change_t stuff_change = {0};
    if (cmp_room_buf(p_user, p_old_room, p_new_room, &stuff_change) != 0)
    {
        KCRIT_LOG(p_user->uid, "compare buffer change failed.");
        g_errno = ERR_MSG_CHANGE_ROOM_BUF;
        return 0;
    }

    room_update_t *p_update_buf = (room_update_t *)p_user->buffer;

    p_update_buf->room_id = p_req->room_id;
    p_update_buf->buf_len = buf_len;
    memcpy(p_update_buf->buf, p_req->buf, buf_len);

    //保存背包物品的变化，数据库操作成功后，更新用户的物品缓存
    memcpy(p_update_buf->buf + buf_len, &stuff_change, sizeof(uint16_t) + stuff_change.num * sizeof(stuff_flow_t));

    //向db-proxy发更新请求
    g_send_msg.begin(SEND_TO_SVR);

    g_send_msg.pack((uint32_t)GET_SVR_SN(p_user));
    g_send_msg.pack((uint16_t)svr_msg_db_update_room);
    g_send_msg.pack((uint32_t)ERR_NO_ERR);
    g_send_msg.pack(p_user->uid);

    g_send_msg.pack(stuff_change.num);
    for (int i = 0; i != (int)stuff_change.num; ++i)
    {
        g_send_msg.pack(stuff_change.stuff_flow[i].stuff_id);
        g_send_msg.pack(stuff_change.stuff_flow[i].stuff_num);
        g_send_msg.pack(stuff_change.stuff_flow[i].stuff_flow);
#ifdef DEBUG
        KINFO_LOG(p_user->uid, "stuff:%u, num:%u, flow:%u", stuff_change.stuff_flow[i].stuff_id, stuff_change.stuff_flow[i].stuff_num, stuff_change.stuff_flow[i].stuff_flow);
#endif
    }

    g_send_msg.pack(p_req->room_id);
    g_send_msg.pack((uint16_t)buf_len);
    g_send_msg.pack(p_req->buf, buf_len);

    g_send_msg.end();
    if (g_p_db_proxy_svr->send_data(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_get_room_num(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, (int)msg_body_len, 0, 0);

    uint32_t room_id[MAX_ROOM_NUM] = {0};
    int room_num = 0;
    if (p_user->user_cache.get_all_room_id(room_id, &room_num) == 0)  //缓存里有
    {
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack((uint16_t)room_num);
        for (int i = 0; i != room_num; ++i)
        {
            g_send_msg.pack((uint32_t)room_id[i]);
        }
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

        return 0;
    }

    //pack请求包
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_room_num, p_user->uid, GET_SVR_SN(p_user), 0);
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_buy_stuff(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, (int)msg_body_len, (int)sizeof(as_msg_buy_stuff_req_t), 0);

    int idx = 0;
    as_msg_buy_stuff_req_t buy_stuff = {0};
    taomee::unpack(p_msg_body, buy_stuff.stuff_id, idx);
    taomee::unpack(p_msg_body, buy_stuff.stuff_num, idx);

    if(buy_stuff.stuff_num != 1)
    {//一次只能购买一个物品
        KCRIT_LOG(p_user->uid, "Just can buy one stuff(stuff_id:%u, stuff_num:%u)", buy_stuff.stuff_id, buy_stuff.stuff_num);
        return -1;
    }

    if(g_item_npc_map.find(buy_stuff.stuff_id) == g_item_npc_map.end())
    {
        KCRIT_LOG(p_user->uid, "can not buy stuff %u not in shops.", buy_stuff.stuff_id);
        return -1;
    }


    //验证购买商品的限制
    map<uint32_t, item_t>::iterator iter = g_item_map.find(buy_stuff.stuff_id);
    if (iter == g_item_map.end())
    {
        g_errno = ERR_STUFF_NOT_EXIST;
        KCRIT_LOG(p_user->uid, "stuff:%u is not exist.", buy_stuff.stuff_id);
        return 0;
    }

    uint32_t total_coins = buy_stuff.stuff_num * iter->second.price;
    KINFO_LOG(p_user->uid, "buy stuff:%u, num:%u", buy_stuff.stuff_id, buy_stuff.stuff_num);

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        g_errno = ERR_GET_ROLE_CACHE;
        KCRIT_LOG(p_user->uid, "get role info failed.");
        return 0;
    }

    if (p_role->role.coins < total_coins || p_role->role.monster_level < iter->second.level)
    {
        g_errno = ERR_BUY_STUFF;
        KCRIT_LOG(p_user->uid, "cann't buy stuff:%u, now have coins:%u, level:%u.", buy_stuff.stuff_id, p_role->role.coins, p_role->role.monster_level);
        return 0;
    }


    commit_stuff_cache_t *p_cm_stuff = (commit_stuff_cache_t*)p_user->buffer;

    p_cm_stuff->category = iter->second.category;

    p_cm_stuff->add_stuff.count = 1;
    p_cm_stuff->add_stuff.stuff[0].stuff_id = buy_stuff.stuff_id;
    p_cm_stuff->add_stuff.stuff[0].stuff_num = buy_stuff.stuff_num;
    p_cm_stuff->add_stuff.coins = total_coins;
    if (ITEM_FRUNITURE == iter->second.category)
    {
        p_cm_stuff->add_stuff.happy = iter->second.happy;
        if (p_role->role.monster_happy + p_cm_stuff->add_stuff.happy > MAX_HAPPY_VALUE)
        {
            p_cm_stuff->add_stuff.happy = MAX_HAPPY_VALUE > p_role->role.monster_happy ? MAX_HAPPY_VALUE - p_role->role.monster_happy : 0;
        }

    }
    else
    {
        p_cm_stuff->add_stuff.happy = 0;
    }

    uint32_t day_time = timestamp_begin_day();
    restrict_key_t exp_key = {day_time, STRICT_STUFF_EXP};
    uint32_t day_exp = 0;
    //获得今天已经获得的经验值
    if(p_user->user_cache.get_day_restrict(exp_key, &day_exp) == 0)
    {//缓存中有
        return process_buy_stuff_after_get_restrict(p_user, p_cm_stuff, day_exp);
    }
    else
    {//缓存中没有
          svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
          pack_svr_pkg_head(p_header, svr_msg_db_get_day_restrict, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_get_day_restrict_req_t));

          db_msg_get_day_restrict_req_t  *p_req = (db_msg_get_day_restrict_req_t*)(g_send_buffer + sizeof(svr_msg_header_t));
          p_req->type = STRICT_STUFF_EXP;
          p_req->time = day_time;
          if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
          {
                g_errno = ERR_MSG_DB_PROXY;
                KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
                return 0;
          }
      }

    return 0;
}

int process_get_puzzle_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, (int)msg_body_len, 0, 0);

    //判断缓存里面有没有
    db_msg_puzzle_rsp_t puzzle = {0};

    if (p_user->user_cache.get_all_puzzle(&puzzle) == 0)
    {
        KINFO_LOG(p_user->uid, "get puzzle from cache");
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(puzzle.num);
        for (int i = 0; i != (int)puzzle.num; ++i)
        {
            g_send_msg.pack(puzzle.puzzle[i].type);
            int is_same_day = timestamp_equal_day(time(NULL), puzzle.puzzle[i].last_playtime);
            if (-1 == is_same_day)
            {
                KCRIT_LOG(p_user->uid, "get day from timestamp failed.");
                g_errno = ERR_SYSTEM_ERR;
                return 0;
            }
            else
            {
                g_send_msg.pack((uint8_t)is_same_day);
            }
            g_send_msg.pack(puzzle.puzzle[i].max_score);
            uint16_t avg_score = puzzle.puzzle[i].score / puzzle.puzzle[i].num;
            g_send_msg.pack(avg_score);
        }

        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

        return 0;
    }

    KINFO_LOG(p_user->uid, "request puzzle info");

    //pack请求包
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_puzzle_info, p_user->uid, GET_SVR_SN(p_user), 0);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_commit_puzzle(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_puzzle_req_t), 0);

    int idx = 0;
    as_msg_puzzle_req_t req = {0};
    taomee::unpack(p_msg_body, req.type, idx);
    taomee::unpack(p_msg_body, req.total_num, idx);
    taomee::unpack(p_msg_body, req.right_num, idx);

    KINFO_LOG(p_user->uid, "commit puzzle:%u, total_num:%u, right_num:%u", req.type, req.total_num, req.right_num);

    //验证合法性
    if (req.type > MAX_PUZZLE_TYPE_NUM)//益智游戏的最大类别
    {
        KCRIT_LOG(p_user->uid, "puzle type:%u is not exist.", req.type);
        g_errno = ERR_PUZZLE_TYPE_INVALID;
        return 0;
    }

    if (req.right_num > req.total_num || req.right_num > PUZZLE_MAX_RIGHT_NUM)//益智游戏最大的答题正确数
    {
        KCRIT_LOG(p_user->uid, "puzzle right num can't greater than total num or max num.");
        g_errno = ERR_PUZZLE_NUM;
        return 0;
    }


    uint32_t day_time = timestamp_begin_day();
    restrict_key_t exp_key = {day_time, STRICT_PUZZLE_EXP};
    uint32_t day_exp = 0;
    //获得今天已经获得的经验值
    if(p_user->user_cache.get_day_restrict(exp_key, &day_exp) == 0)
    {//缓存中有
        return compute_reward_commit_to_db(p_user, &req, day_exp);//计算益智游戏的奖励并提交数据库
    }
    else
    {//缓存中没有
        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
        pack_svr_pkg_head(p_header, svr_msg_db_get_day_restrict, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_get_day_restrict_req_t));

        db_msg_get_day_restrict_req_t  *p_req = (db_msg_get_day_restrict_req_t*)(g_send_buffer + sizeof(svr_msg_header_t));
        p_req->type = STRICT_PUZZLE_EXP;
        p_req->time = day_time;
        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
            return 0;
        }
        memcpy(p_user->buffer,(char*)&req, sizeof(as_msg_puzzle_req_t));
    }

    return 0;

}

int process_get_user_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_get_user_req_t), 0);

    int idx = 0;
    as_msg_get_user_req_t req = {0};
    taomee::unpack(p_msg_body, req.user_id, idx);

    if (req.user_id == p_user->uid)  //拉取的是自己的信息，从缓存读取
    {
        role_cache_t *p_role = p_user->user_cache.get_role();
        if (p_role == NULL)
        {
            KCRIT_LOG(p_user->uid, "get user cached role info failed.");
            return -1;
        }

        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack((uint8_t)1);
        g_send_msg.pack(p_role->role.name, sizeof(p_role->role.name));
        g_send_msg.pack(p_role->role.gender);
        g_send_msg.pack(p_role->role.country_id);
        g_send_msg.pack(p_role->role.user_type);
        g_send_msg.pack(p_role->role.birthday);
        g_send_msg.pack(p_role->role.mood);
        g_send_msg.pack(p_role->role.fav_color);
        g_send_msg.pack(p_role->role.fav_pet);
        g_send_msg.pack(p_role->role.fav_fruit);
        g_send_msg.pack(p_role->role.personal_sign, sizeof(p_role->role.personal_sign));
        g_send_msg.pack(p_role->role.room_num);
        g_send_msg.pack(p_role->role.pet_num);
        g_send_msg.pack(p_role->role.max_puzzle_score);
        g_send_msg.pack(p_role->role.register_time);
        g_send_msg.pack(p_role->role.coins);
        g_send_msg.pack(p_role->role.last_login_time);
		//whether fist login today
		g_send_msg.pack(p_role->role.is_first_login);
        g_send_msg.pack(p_role->role.monster_id);
        g_send_msg.pack(p_role->role.monster_name, sizeof(p_role->role.monster_name));
        g_send_msg.pack(p_role->role.monster_main_color);
        g_send_msg.pack(p_role->role.monster_ex_color);
        g_send_msg.pack(p_role->role.monster_eye_color);
        g_send_msg.pack(p_role->role.monster_exp);
        g_send_msg.pack(p_role->role.monster_level);

        uint32_t now = time(NULL);
        uint32_t mon_health =  get_now_health(p_role->role.monster_health, p_role->last_update_health_time, now);
        g_send_msg.pack(mon_health);
        //计算新的愉悦度
        uint32_t mon_happy = get_new_happy(now, p_role->role.last_login_time, p_role->role.monster_happy);
        g_send_msg.pack(mon_happy);
        g_send_msg.pack(p_role->role.approved_message_num);
        g_send_msg.pack(p_role->role.unapproved_message_num);
        g_send_msg.pack(p_role->role.friend_num);
        g_send_msg.pack((uint8_t)(p_role->pending_req_num != 0 ? 1 : 0));
        g_send_msg.pack(p_role->role.thumb);
        g_send_msg.pack(p_role->role.visits);

        g_send_msg.pack(p_role->role.recent_unread_badge);
        g_send_msg.pack(p_role->role.npc_score);//打包npc评分
        KINFO_LOG(p_user->uid, "[CACHE]happy:%u, health:%u, coins:%u, friend:%u, approved:%u, unapproved:%u, unread_badge:%u, visits:%u, online_time:%u, offline_time:%u, npc_score:%u", mon_happy, mon_health, p_role->role.coins, p_role->role.friend_num, p_role->role.approved_message_num, p_role->role.unapproved_message_num, p_role->role.recent_unread_badge, p_role->unread_visits, p_role->role.online_time, p_role->role.offline_time, p_role->role.npc_score);

        char buf[4096] = {0};
        following_pet_t *p_pet = (following_pet_t *)buf;
        p_user->user_cache.get_following_pet(p_pet);

        //随机产生三个跟随的精灵
        int loop_count = 3;
        int idx = 0;
        if(p_pet->count < 3)
        {
            loop_count = p_pet->count;
            idx = 0;
        }
        else
        {
             idx = uniform_rand(0, p_pet->count - 3);
        }

       g_send_msg.pack((uint8_t)loop_count);
        for (int i = 0; i < loop_count; ++i)
        {
            g_send_msg.pack(p_pet->pet_id[i + idx]);
            KINFO_LOG(p_user->uid, "follow pet:%u", p_pet->pet_id[i+ idx]);
        }

        g_send_msg.pack(p_role->role.flag1);
        g_send_msg.pack(p_role->unread_visits);
        if (p_role->role.last_paper_read == g_current_paper_term)   //当前bobo报已读
        {
            if (p_role->role.last_paper_reward == g_current_paper_term)   //当前bobo报已领教
            {
                g_send_msg.pack((uint8_t)BOBO_REWARDED);
            }
            else
            {
                g_send_msg.pack((uint8_t)BOBO_UNREWARD);
            }
        }
        else
        {
            g_send_msg.pack((uint8_t)BOBO_UNREAD);   //当前卜卜报未读
        }
        g_send_msg.pack(p_role->role.online_time);   //防沉迷的上线时间
        g_send_msg.pack(p_role->role.limit_reward);

        p_role->role.limit_reward = 0;//置为已领奖


        g_send_msg.end();

        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

        badge_step(p_user, badge_daily_puzzle_25_const, p_role->role.max_puzzle_score);
        badge_step(p_user, badge_visits_500_const, p_role->role.visits);
        badge_step(p_user, badge_thumb_50_const, p_role->role.thumb);
        badge_step(p_user, badge_message_50_const, p_role->role.approved_message_num);

        activity_open(p_user, OPEN_ACTIVITY_ID, NULL);
        return 0;
    }

    stat_one_param_t stat_data = {1};
    msg_log(stat_visit_room, &stat_data, sizeof(stat_data));

    //不是自己信息，从db获取
    KINFO_LOG(p_user->uid, "get user:%u info from db", req.user_id);

   // if(check_for_depversion(p_user, req.user_id))
   // {//检测是否为涉及版署号的操作
   //     return 0;
   // }

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_other_role_info, req.user_id, GET_SVR_SN(p_user), 0);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_ACCOUNT;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }
    //清掉上一个人的好友树缓存
    p_user->friend_tree.del_friend_tree(false);

    return 0;
}

int process_get_pinboard(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_pinboard_req_t), 0);

    int idx = 0;
    as_msg_pinboard_req_t req = {0};
    taomee::unpack(p_msg_body, req.user_id, idx);
    taomee::unpack(p_msg_body, req.page, idx);
    taomee::unpack(p_msg_body, req.page_num, idx);
#ifdef DEBUG
    KINFO_LOG(p_user->uid, "get pinboard, user_id:%u, page:%u, page_num:%u", req.user_id, req.page, req.page_num);
#endif

    if (0 == req.page)
    {
        g_errno = ERR_PAGE_ZERO;
        KCRIT_LOG(p_user->uid, "pinboard page cann't be zero.");
        return 0;
    }
    if (req.page_num > MAX_MESSAE_PER_PAGE)
    {
        g_errno = ERR_PINBORAD_PAGE;
        KCRIT_LOG(p_user->uid, "pinboard page num is larger than max.");
        return 0;
    }


    if (req.page <= PINBOARD_CACHE_NUM && req.user_id == p_user->uid)
    {
        pinboard_t *p_pinboard = p_user->user_cache.get_pinboard(req.page);
        if (p_pinboard != NULL)
        {
            pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
            g_send_msg.pack(p_pinboard->count);
            KINFO_LOG(p_user->uid, "get %u message in page:%u from cache", p_pinboard->count, req.page);

            for (int i = 0; i != (int)p_pinboard->count; ++i)
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
                KINFO_LOG(p_user->uid, "pinboard id:%u, icon:%u, color:%u, peer_id:%u, user_type:%u, monster_id:%u, message_count:%u",
                        p_pinboard->message[i].message.id, p_pinboard->message[i].message.icon, p_pinboard->message[i].message.color, p_pinboard->message[i].message.peer_id,
                        p_pinboard->message[i].user_info.user_type, p_pinboard->message[i].user_info.monster_id, p_pinboard->message[i].message.message_count);
#endif
            }
            g_send_msg.end();
            send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

            return 0;
        }
    }

    KINFO_LOG(p_user->uid, "request for pinboard page:%u", req.page);




    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    db_msg_pinboard_req_t *p_body = (db_msg_pinboard_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, svr_msg_db_get_pinboard_info, req.user_id, GET_SVR_SN(p_user), sizeof(db_msg_pinboard_req_t));

    p_body->page = req.page;
    p_body->page_num = req.page_num;
    p_body->peer_id = p_user->uid;

    if (p_user->uid == req.user_id)  //拉取的是自己的留言板
    {
        p_body->status = MESSAGE_UNAPPROVED | MESSAGE_APPROVED;
        p_body->real_num = -1;

    }
    else
    {
        p_body->status = MESSAGE_APPROVED;
        p_body->real_num = p_user->user_cache.get_peer_real(req.user_id);
    }

    if(req.user_id == 12345 && p_user->uid != 12345)
    {//其他人拉取麦咭的留言板
        if(g_p_db_cache_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            ERROR_LOG("send to db cache server failed.");
            return 0;
        }
    }
    else
    {
        //给db发请求获取对应的留言
        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            ERROR_LOG("send to db proxy failed.");
            return 0;
        }
    }
    pinboard_count_t *p_count = (pinboard_count_t *)p_user->buffer;
    p_count->count = (uint8_t)0;
    p_count->page = req.page;
    p_count->peer_id = req.user_id;
    return 0;
}

int process_add_message(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    int message_count = msg_body_len - sizeof(as_msg_add_message_req_t);
    //留言字数必须大于0，小于MAX_MESSAGE_BYTE
    CHECK_VAL_LE(p_user->uid, message_count, MAX_MESSAGE_BYTE, 0);
    CHECK_VAL_GE(p_user->uid, message_count, 0, 0);

    int idx = 0;
    memset(p_user->buffer, 0, sizeof(p_user->buffer));
    as_msg_add_message_req_t *p_req = (as_msg_add_message_req_t *)p_user->buffer;
    taomee::unpack(p_msg_body, p_req->peer_id, idx);
    taomee::unpack(p_msg_body, p_req->icon, idx);
    taomee::unpack(p_msg_body, p_req->color, idx);
    taomee::unpack(p_msg_body, p_req->message, message_count, idx);
    if (tm_dirty_replace(p_req->message) == -1)
    {
        g_errno = ERR_SYSTEM_ERR;
        KCRIT_LOG(p_user->uid, "dirty replace failed:%s", p_req->message);
        return 0;
    }


    //给自己留言，清楚自己留言板的缓存
    if (p_user->uid == p_req->peer_id)
    {
        p_user->user_cache.del_pinboard();
    }

   // if(check_for_depversion(p_user, p_req->peer_id))
   // {//检测是否为涉及版署号的操作
   //     return 0;
   // }


    //给db发增加留言的请求
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    db_msg_add_message_req_t *p_body = (db_msg_add_message_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, svr_msg_db_add_message, p_req->peer_id, GET_SVR_SN(p_user), sizeof(db_msg_add_message_req_t) + message_count);

    p_body->peer_id = p_user->uid;
    p_body->icon = p_req->icon;
    p_body->color = p_req->color;


    if (p_req->peer_id == p_user->uid || p_req->peer_id == 12345)  //给自己的留言以及给麦咭留言不需要审核
    {
        p_body->type = MESSAGE_APPROVED;
    }
    else
    {
        p_body->type = MESSAGE_UNAPPROVED;
    }
    p_body->create_time = (uint32_t)time(NULL);
    KINFO_LOG(p_user->uid, "add message, peer_id:%u, icon:%u, color:%u, create_time:%u", p_req->peer_id, p_req->icon, p_req->color, p_body->create_time);


    p_body->message_count = message_count;
    memcpy(p_body->message, p_req->message, message_count);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
        return 0;
    }

    if(p_req->peer_id == 12345)
    {//给麦咭流言
        memcpy(p_user->buffer, p_body, p_header->len - sizeof(svr_msg_header_t));
    }

    return 0;
}

int process_update_message_status(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_message_status_req_t), 0);

    int idx = 0;
    as_msg_message_status_req_t *p_req = (as_msg_message_status_req_t *)p_user->buffer;
    taomee::unpack(p_msg_body, p_req->peer_id, idx);
    taomee::unpack(p_msg_body, p_req->id, idx);
    taomee::unpack(p_msg_body, p_req->type, idx);
    KINFO_LOG(p_user->uid, "update user:%u message to type:%u", p_req->peer_id, p_req->type);

    if (!(MESSAGE_DELETE == p_req->type || MESSAGE_UNAPPROVED == p_req->type || MESSAGE_APPROVED == p_req->type || MESSAGE_REPORT == p_req->type))
    {
        g_errno = ERR_MSG_TYPE_INVALID;
        KCRIT_LOG(p_user->uid, "[update msg status] type:%u is invalid.", p_req->type);
        return 0;
    }

    //不是自己的留言，只能举报
    if (p_user->uid != p_req->peer_id && p_req->type != MESSAGE_REPORT)
    {
        g_errno = ERR_CAN_REPORT_ONLY;
        KCRIT_LOG(p_user->uid, "can't operate msg that is not oneself.");
        return -1;
    }

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    db_msg_message_status_req_t *p_body = (db_msg_message_status_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, svr_msg_db_update_message_status, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_message_status_req_t));

    p_body->id = p_req->id;
    p_body->type = p_req->type;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_rating_user_room(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_thumb_user_req_t), 0);

    int idx = 0;
    as_msg_thumb_user_req_t *p_req = (as_msg_thumb_user_req_t *)p_user->buffer;
    taomee::unpack(p_msg_body, p_req->user_id, idx);
    KINFO_LOG(p_user->uid, "thumb user:%u", p_req->user_id);

    if (p_user->uid == p_req->user_id)
    {
        KCRIT_LOG(p_user->uid, "cann't thumb oneself.");
        g_errno = ERR_SYSTEM_ERR;
        return 0;
    }


    //向ucount查询当前用户有没有顶过
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    ucount_msg_req_t *p_body = (ucount_msg_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, svr_msg_ucount_thumb, p_user->uid, GET_SVR_SN(p_user), sizeof(ucount_msg_req_t));
    p_body->user_id = p_req->user_id;

    if (g_p_ucount_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_UCOUNT;
        KCRIT_LOG(p_user->uid, "send to ucount server failed.");
        return 0;
    }

    return 0;
}

int process_update_guide_flag(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_update_flag_req_t), 0);

    int idx = 0;
    as_msg_update_flag_req_t *p_req = (as_msg_update_flag_req_t *)p_user->buffer;
    taomee::unpack(p_msg_body, p_req->flag1, idx);
    KINFO_LOG(p_user->uid, "set flag1:%u", p_req->flag1);

    svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_update_role, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_update_login_t) + sizeof(field_t));

    db_msg_update_login_t *p_body = (db_msg_update_login_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->count = 1;
    p_body->field[0].type = FIELD_GUIDE_FLAG;
    p_body->field[0].value = p_req->flag1;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    memcpy(p_user->buffer, p_body, sizeof(db_msg_update_login_t) + sizeof(field_t));

    return 0;
}
int process_update_flag(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_update_flag_req_t), 0);

    int idx = 0;
    as_msg_update_flag_req_t *p_req = (as_msg_update_flag_req_t *)p_user->buffer;
    taomee::unpack(p_msg_body, p_req->flag1, idx);
    KINFO_LOG(p_user->uid, "set flag1:%u", p_req->flag1);

    svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_update_role, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_update_login_t) + sizeof(field_t));

    db_msg_update_login_t *p_body = (db_msg_update_login_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->count = 1;
    p_body->field[0].type = FIELD_FLAG1;
    p_body->field[0].value = p_req->flag1;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    memcpy(p_user->buffer, p_body, sizeof(db_msg_update_login_t) + sizeof(field_t));

    return 0;
}

int process_update_profile(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);

    int idx = 0;
    uint8_t type = 0;
    taomee::unpack(p_msg_body, type, idx);
    if (PROFILE_SIGN == type)   //个性签名
    {
        //验证包长
        int sign_len = msg_body_len - sizeof(type);
        CHECK_VAL_LE(p_user->uid, sign_len, MAX_SIGN_BYTE, 0);

        svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
        pack_svr_pkg_head(p_head, svr_msg_db_modify_sign, p_user->uid, GET_SVR_SN(p_user), sign_len);
        char *p_body = (char *)(g_send_buffer + sizeof(svr_msg_header_t));
        taomee::unpack(p_msg_body, p_body, sign_len, idx);

        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy failed.");
            return 0;
        }
        //缓存值的变化，DB成功后更新缓存的值
        memset(p_user->buffer, 0, sizeof(p_user->buffer));
        memcpy(p_user->buffer, p_body, sign_len);
    }
    else
    {
        CHECK_VAL_LE(p_user->uid, msg_body_len, (int)(sizeof(uint8_t) + sizeof(uint32_t)), 0);
        uint32_t value = 0;
        taomee::unpack(p_msg_body, value, idx);

        //更新数据库的值
        svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
        pack_svr_pkg_head(p_head, svr_msg_db_update_role, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_update_login_t) + sizeof(field_t));

        db_msg_update_login_t *p_body = (db_msg_update_login_t *)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->count = 1;
        switch (type)
        {
            case PROFILE_MOOD:
                p_body->field[0].type = FIELD_MOOD;
                break;
            case PROFILE_COLOR:
                p_body->field[0].type = FIELD_FAV_COLOR;
                break;
            case PROFILE_PET:
                p_body->field[0].type = FIELD_FAV_PET;
                break;
            case PROFILE_FRUIT:
                p_body->field[0].type = FIELD_FAV_FRUIT;
                break;
        }
        p_body->field[0].value = value;

        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy failed.");
            return 0;
        }
        //缓存值的变化，DB成功后更新缓存的值
        memcpy(p_user->buffer, p_body, sizeof(db_msg_update_login_t) + sizeof(field_t));
    }

    return 0;
}

int process_eat_food(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint32_t), 0);

    int idx = 0;
    uint32_t item_id = 0;
    taomee::unpack(p_msg_body, item_id, idx);
    map<uint32_t, item_t>::iterator iter =  g_item_map.find(item_id);
    if (iter == g_item_map.end())
    {
        KCRIT_LOG(p_user->uid, "food id:%u is not exists.", item_id);
        return -1;
    }
    //判断物品是不是食物
    if (iter->second.category != ITEM_FOOD)
    {
        KCRIT_LOG(p_user->uid, "item id:%u is not food.", item_id);
        return -1;
    }
    //判断背包里有没有这个物品
    int stuff_num = p_user->user_cache.get_stuff_num(item_id);
    if (stuff_num < 0)
    {
        KCRIT_LOG(p_user->uid, "get stuff:%u num failed", item_id);
        return -1;
    }
    if (0 == stuff_num)
    {
        KCRIT_LOG(p_user->uid, "There is no stuff %u in bag", item_id);
       // p_user->waitcmd = 0;    //前端请求太快，物品数量不够，直接丢弃掉这个请求
        g_errno = ERR_NOT_ENOUGH_FOOD;
        return 0;
    }

    KINFO_LOG(p_user->uid, "eat food:%u", item_id);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    db_msg_eat_food_req_t *p_body = (db_msg_eat_food_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, svr_msg_db_eat_food, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_eat_food_req_t));
    p_body->item_id = item_id;
    p_body->reward_health = iter->second.health;
    p_body->reward_happy = iter->second.happy;
    p_body->reward_coins = 0;

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "get role cache");
        return -1;
    }

    time_t now = time(NULL);
    uint32_t now_happy = get_new_happy(now, p_role->role.last_login_time, p_role->role.monster_happy);
    if (now_happy + p_body->reward_happy > MAX_HAPPY_VALUE)
    {
        p_body->reward_happy = MAX_HAPPY_VALUE > now_happy ? MAX_HAPPY_VALUE - now_happy : 0;
    }

    p_role->role.monster_health = get_now_health(p_role->role.monster_health, p_role->last_update_health_time, now);
    p_role->last_update_health_time = now;
    if (p_role->role.monster_health + p_body->reward_health > MAX_HEALTH_VALUE)
    {
        p_body->reward_health = MAX_HEALTH_VALUE > p_role->role.monster_health ? MAX_HEALTH_VALUE - p_role->role.monster_health : 0;
    }

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        ERROR_LOG("send to db proxy server failed.");
        return -1;
    }

    memcpy(p_user->buffer, p_body, sizeof(db_msg_eat_food_req_t));

    return 0;
}

int process_commit_game(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_game_req_t), 0);

    int idx = 0;
    game_t *p_game = (game_t *)p_user->buffer;
    memset(p_game, 0, sizeof(game_t));
    taomee::unpack(p_msg_body, p_game->game_id, idx);
    taomee::unpack(p_msg_body, p_game->level_id, idx);
    taomee::unpack(p_msg_body, p_game->reward_coins, idx);
    taomee::unpack(p_msg_body, p_game->score, idx);

    p_game->reward_coins = get_current_gains(p_game->reward_coins, p_user);//防成谜

    char game_key_buffer[32] ={0};
    sprintf(game_key_buffer, "%u%u", p_game->game_id, p_game->level_id);
    if(strlen(game_key_buffer) > 9)
    {
        KCRIT_LOG(p_user->uid, "error game_id and level_id: game_key_buffer:%s, game_id:%u level_id:%u", game_key_buffer, p_game->game_id, p_game->level_id);
        return -1;
    }

    p_game->game_key = atoi(game_key_buffer);

    map<uint32_t, game_level_item_t>::iterator iter = g_game_level_map.find(p_game->game_key);
    if (iter == g_game_level_map.end())
    {
        KCRIT_LOG(p_user->uid, "game:%u level:%u is invalid, game_key=%u", p_game->game_id, p_game->level_id, p_game->game_key);
        return -1;
    }

    uint8_t need_update_db = 0;//默认0不需要    1需要更新数据库
    uint32_t badge_add = 0;
    //判断是否解锁
       if(p_user->user_cache.level_unlocked(p_game->game_id, p_game->game_key, p_game->level_id, p_game->score, iter->second.unlock_cond, &need_update_db, &badge_add) == 0)
       {
           KINFO_LOG(p_user->uid, "game_id:%u level_id:%u not unlocked", p_game->game_id, p_game->level_id);
           g_errno = ERR_NOT_UNLOCK_LEVEL;
           return -1;
       }

       //得到奖励的星星数
    get_game_star(p_game, iter->second.one_star_score, iter->second.two_star_score, iter->second.three_star_score, iter->second.finished_score);

    p_game->reward_item_score = iter->second.reward_item_score;
    p_game->need_update_db = need_update_db;
    p_game->max_coins = g_max_game_score;//小游戏每天的金币限制

    //添加成就项
    if(p_game->game_id == 3)
    {
        badge_step(p_user, badge_heixia_5000_const, badge_add);
        //为了记录6.6-6.30第一个跳跳小黑峡达10000分的用户
        if(p_game->score >= 10000)
        {
            KWARN_LOG(p_user->uid, "xiaoheixia score %u time %u", p_game->score, (uint32_t)time(NULL));
        }
    }
    else if(p_game->game_id == 2)
    {
        badge_step(p_user, badge_kuaigongfang_12000_const, badge_add);
    }
    else if(p_game->game_id == 1)
    {
        badge_step(p_user, badge_lianliankan_4000_const, badge_add);
    }

    //统计小游戏次数
    time_t now = time(NULL);
    if (p_user->stat_time > 0 && now >= p_user->stat_time)
    {
        stat_four_param_t stat_data = {p_user->uid, 1, now - p_user->stat_time, p_game->score};
        msg_log(stat_leave_picture_matching + p_game->game_id - 1, &stat_data, sizeof(stat_data));
        p_user->stat_time = 0;
    }

    KINFO_LOG(p_user->uid, "game:%u, level:%u get coins:%u, score:%u", p_game->game_id, p_game->level_id,  p_game->reward_coins, p_game->score);

    uint32_t day_time = timestamp_begin_day();
    restrict_key_t key_coins = {day_time, STRICT_COIN};
    restrict_key_t key_item = {day_time, STRICT_ITEM + p_game->game_id};
    uint32_t restrict_coins = 0;
    uint32_t restrict_item = 0;
    if(p_user->user_cache.get_game_day_restrict(key_coins, key_item, &restrict_coins, &restrict_item) == 0)
    {//金币和道具有缓存
        KINFO_LOG(p_user->uid, "get day_restrict from cache");
        return do_game_restrict_and_star(p_user, p_game, restrict_coins, restrict_item, day_time);
    }
    else
    {//没有缓存, 从db拉取天限制
        //缓存里没有，从db获得今天已经获得的金币
        KINFO_LOG(p_user->uid, "get day_restrict from db");
        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
        pack_svr_pkg_head(p_header, svr_msg_db_get_game_day_restrict, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_get_day_restrict_req_t));

        db_msg_get_day_restrict_req_t *p_body = (db_msg_get_day_restrict_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->type = STRICT_ITEM + p_game->game_id;
        p_body->time = day_time;

        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
            return 0;
        }
    }
    return 0;
}

int process_attract_pet(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_attract_pet_req_t), 0);

    int idx = 0;
    as_msg_attract_pet_req_t req = {0};
    taomee::unpack(p_msg_body, req.type, idx);
    taomee::unpack(p_msg_body, req.pet_id, idx);

    if (!(PET_ADD == req.type || PET_DEL == req.type || PET_DROP == req.type))
    {
        g_errno = ERR_ATTRACT_PET_OP;
        KCRIT_LOG(p_user->uid, "attract pet operate type:%u is invalid.", req.type);
        return 0;
    }

    KINFO_LOG(p_user->uid, "attract pet op:%u, pet_id:%u", req.type, req.pet_id);
    uint32_t attract_pet_id = p_user->user_cache.get_attract_pet();
    if (0 == req.pet_id)
    {
        g_errno = ERR_NO_ATTRACT_PET;
        KCRIT_LOG(p_user->uid, "user has no attract pet.");
        return 0;
    }

    switch(req.type)
    {
        case PET_DEL:
            //放弃吸引到的精灵
            {
                if(req.pet_id != attract_pet_id)
                {
                    g_errno = ERR_PET_ID;
                    KCRIT_LOG(p_user->uid, "server_pet_id:%u client_pet_id:%u", attract_pet_id, req.pet_id);
                    return 0;
                }
                p_user->user_cache.del_attract_pet();
                //删掉种植的所有植物
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                db_msg_del_plant_req_t *p_body = (db_msg_del_plant_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

                pack_svr_pkg_head(p_header, svr_msg_db_del_plant, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_del_plant_req_t));
                p_body->hole_id = 0;    //删掉所有植物

                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                    return 0;
                }
            }
            break;
        case PET_ADD:
            //收养吸引到的精灵
            {
                char buf[1024] = {0};
                following_pet_t *p_pet = (following_pet_t *)buf;
                p_user->user_cache.get_following_pet(p_pet);
                if(p_pet->count >= OWNED_PET_NUM)
                {
                    g_errno = ERR_PET_TOO_MANY;
                    KCRIT_LOG(p_user->uid, "user have owned %u pet, must del one", p_pet->count);
                    return 0;
                }

                if(req.pet_id != attract_pet_id)
                {
                    g_errno = ERR_PET_ID;
                    KCRIT_LOG(p_user->uid, "server_pet_id:%u client_pet_id:%u", attract_pet_id, req.pet_id);
                    return 0;
                }

                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                db_msg_pet_req_t *p_body = (db_msg_pet_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                pack_svr_pkg_head(p_header, svr_msg_db_pet_op, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_pet_req_t));

                p_body->type = req.type;
                p_body->id = req.pet_id;
                p_body->status = PET_FOLLOWING;

                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                    return 0;
                }
                memcpy(p_user->buffer, p_body, sizeof(db_msg_pet_req_t));
            }
            break;
        case PET_DROP:
            //删除已有的一只小精灵
            {
                if(p_user->user_cache.is_pet_exist(req.pet_id) != 0)
                {//不存在的精灵
                    g_errno = ERR_PET_ID;
                    KCRIT_LOG(p_user->uid, "del client_pet_id:%u not exist", req.pet_id);
                    return 0;
                }

                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                db_msg_pet_req_t *p_body = (db_msg_pet_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                pack_svr_pkg_head(p_header, svr_msg_db_pet_op, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_pet_req_t));

                p_body->type = req.type;
                p_body->id = req.pet_id;
                p_body->status = PET_FOLLOWING;

                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                    return 0;
                }
                memcpy(p_user->buffer, p_body, sizeof(db_msg_pet_req_t));

            }
            break;
        default:
            KCRIT_LOG(p_user->uid, "Not possible to come here");
            return -1;
    }

    return 0;
}

int process_maintain_plant(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_maintain_plant_req_t), 0);

    int idx = 0;
    as_msg_maintain_plant_req_t req;
    taomee::unpack(p_msg_body, req.hole_id, idx);
    taomee::unpack(p_msg_body, req.maintain_type, idx);
    KINFO_LOG(p_user->uid, "user maintain hole:%u type:%u", req.hole_id, req.maintain_type);
    if(req.hole_id <=0 || req.hole_id > PLANTATION_NUM)
    {
        g_errno = ERR_HOLE_ID;
        KCRIT_LOG(p_user->uid, "Not valid hole id (%u)", req.hole_id);
        return 0;
    }

    if(g_maintain_map.find(req.maintain_type) == g_maintain_map.end())
    {
        g_errno = ERR_MAINTAIN_TYPE;
        KCRIT_LOG(p_user->uid, "Not valid maintain type (%u)", req.maintain_type);
        return 0;
    }

    char buf[1024] = {0};
    all_hole_info_t *p_all_hole = (all_hole_info_t*)buf;
    if(p_user->user_cache.get_all_hole(p_all_hole) == 1)
    {
        KCRIT_LOG(p_user->uid, "plant hole not in cache");
        return -1;
    }

    if(p_all_hole->hole[req.hole_id - 1].plant_id == 0)
    {
        KCRIT_LOG(p_user->uid, "hole:%u has no plant.", req.hole_id);
        g_errno = ERR_PLANT_NOT_EXIST;
        return 0;
    }

    if((p_all_hole->hole[req.hole_id - 1].maintain == MAINTAIN_GET && req.maintain_type == MAINTAIN_DEL)|| (req.maintain_type != MAINTAIN_DEL && p_all_hole->hole[req.hole_id - 1].maintain != req.maintain_type))
    {
        KCRIT_LOG(p_user->uid, "Not invalid user, maintaintype not consistent(hole_id:%u serv:%u cli:%u)", req.hole_id,  p_all_hole->hole[req.hole_id - 1].maintain, req.maintain_type);
        g_errno = ERR_MAINTAIN_TYPE_NOT_CONSISTENT;
        return 0;
    }

    switch(req.maintain_type)
    {
        case MAINTAIN_WATER:
        case MAINTAIN_MUSIC:
        case MAINTAIN_MATERIAL:
            {
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                db_msg_maintain_req_t *p_body = (db_msg_maintain_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

                pack_svr_pkg_head(p_header, svr_msg_db_maintain_plant, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_maintain_req_t));
                p_body->hole_id = req.hole_id;
                p_body->add_growth = g_maintain_map[req.maintain_type].reward_growth;
                if(p_body->add_growth + p_all_hole->hole[req.hole_id - 1].growth >= MATURITY_GROWTH_VALUE)
                {//代表加上本次的成长值后植物就成熟了
                    p_body->add_growth = MATURITY_GROWTH_VALUE - p_all_hole->hole[req.hole_id - 1].growth;
                    p_body->new_maintain_type = MAINTAIN_GET;
                    p_body->maintain_count = 0;

                }
                else
                {
                    uint8_t maintain_count = p_all_hole->hole[req.hole_id - 1].maintain_count;
                    p_body->new_maintain_type = get_loop_maintain(maintain_count);
                    if(maintain_count >= MAX_MAINTAIN_COUNT_PER_QUARTER)
                    {
                     p_body->maintain_count = maintain_count;
                    }
                    else
                    {
                     p_body->maintain_count = maintain_count + 1;

                    }
                }
                p_body->add_growth_time = time(NULL);
                p_body->last_reward_id = (p_all_hole->hole[req.hole_id - 1].last_reward_id + 1)%MAX_REWARD_ID;
                if(p_body->last_reward_id == 0)
                {
                    p_body->last_reward_id = 1;
                }
                p_body->reward_happy = g_maintain_map[req.maintain_type].reward_happy;
                p_body->reward_exp = g_maintain_map[req.maintain_type].reward_exp;
                p_body->reward_coins = g_maintain_map[req.maintain_type].reward_coins;

                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                    return 0;
                }

                memcpy(p_user->buffer, p_body, sizeof(db_msg_maintain_req_t));
            }
            break;
        case MAINTAIN_GET://收获
            {
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                db_msg_get_plant_req_t *p_body = (db_msg_get_plant_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

                pack_svr_pkg_head(p_header, svr_msg_db_get_plant, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_get_plant_req_t));
                p_body->hole_id = req.hole_id;
                p_body->reward_coins = g_maintain_map[req.maintain_type].reward_coins;

                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                    return 0;
                }

                memcpy(p_user->buffer, p_body, sizeof(db_msg_get_plant_req_t));
            }
            break;
        case MAINTAIN_DEL://铲除
            {
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                db_msg_del_plant_req_t *p_body = (db_msg_del_plant_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

                pack_svr_pkg_head(p_header, svr_msg_db_del_plant, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_del_plant_req_t));
                p_body->hole_id = req.hole_id;

                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                    return 0;
                }
                memcpy(p_user->buffer, p_body, sizeof(db_msg_del_plant_req_t));
            }
            break;
        default:
            break;
    }

    return 0;
}

/**
 * @brief process_grow_plant 种植园种植物
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_grow_plant(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_grow_plant_req_t), 0);

    int idx = 0;
    as_msg_grow_plant_req_t *p_req = (as_msg_grow_plant_req_t *)p_user->buffer;
    taomee::unpack(p_msg_body, p_req->plant_id, idx);
    taomee::unpack(p_msg_body, p_req->hole_id, idx);


    if (!(p_req->hole_id > 0 && p_req->hole_id <= PLANTATION_NUM))
    {
        KCRIT_LOG(p_user->uid, "hole id:%u is not exists.", p_req->hole_id);
        g_errno =  ERR_HOLE_NOT_EXIST;
        return 0;
    }

    //判断物品id是否是种子
    map<uint32_t, item_t>::iterator iter =  g_item_map.find(p_req->plant_id);
    if (iter == g_item_map.end())
    {
        KCRIT_LOG(p_user->uid, "plant id:%u is not exists.", p_req->plant_id);
        g_errno = ERR_ITEM_NOT_EXIST;
        return -1;
    }
    if (iter->second.category != ITEM_SEED)
    {
        KCRIT_LOG(p_user->uid, "item id:%u is not seed.", p_req->plant_id);
        g_errno = ERR_ITEM_NOT_SEED;
        return 0;
    }

    //判断背包里有没有足够的种子
    int seed_num = p_user->user_cache.get_stuff_num(p_req->plant_id);
    if (-1 == seed_num)
    {
        KCRIT_LOG(p_user->uid, "get user bag from cache failed.");
        return -1;
    }
    else if (seed_num < 1)
    {
        g_errno = ERR_NOT_ENOUGH_SEED;
        KCRIT_LOG(p_user->uid, "not enough seed to plant.");
        return 0;
    }

    hole_info_t *p_hole = p_user->user_cache.get_hole_plant(p_req->hole_id);
    if (p_hole == NULL || p_hole->plant_id != 0)
    {
        g_errno = ERR_PLANT_EXIST_IN_HOLE;
        if(p_hole != NULL)
        {
            KCRIT_LOG(p_user->uid, "hole:%u has plant (%u).", p_req->hole_id, p_hole->plant_id);
        }
        else
        {
            KCRIT_LOG(p_user->uid, "hole is NULL");
        }
        return 0;
    }

    //随机生成一种颜色
    int color = uniform_rand(1, PLANT_COLOR_NUM);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    db_msg_add_plant_req_t *p_body = (db_msg_add_plant_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, svr_msg_db_add_plant, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_add_plant_req_t));

    p_body->plant_id = p_req->plant_id;
    p_body->hole_id = p_req->hole_id;
    p_body->color = color;
    time_t now = time(NULL);
    p_body->last_extra_growth_time = now;
    p_body->last_grown_time = now;
    p_body->maintain = get_new_maintain();
    p_body->maintain_time = now + g_maintain_plant_time * 60;

    KINFO_LOG(p_user->uid, "user grow plant hole:%u, plant:%u maintain:%u", p_req->hole_id, p_req->plant_id, p_body->maintain);
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
        return 0;
    }

    //保存种的植物的信息，db操作成功后更新缓存
    memcpy(p_user->buffer, p_body, sizeof(db_msg_add_plant_req_t));
    return 0;
}

int process_get_plant_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);

    char buf[1024] = {0};
    all_hole_info_t *p_all_hole = (all_hole_info_t*)buf;

    if(p_user->user_cache.get_all_hole(p_all_hole) == 1)
    {//未做缓存，错误情况 ，应该在登录时就缓存好了
        KCRIT_LOG(p_user->uid, "get hole in cache failed");
        return -1;
    }
    //做了缓存


    //统计种植园访问人数人次
    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_plantation_visit, &stat_data, sizeof(stat_data));



    if(p_user->user_cache.is_hole_reward_cached() == 0)
    {//奖励没做缓存，从数据库取
        KINFO_LOG(p_user->uid, "hole reward not cached, get from db");
        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
        pack_svr_pkg_head(p_header, svr_msg_db_get_hole_reward, p_user->uid, GET_SVR_SN(p_user), 0);

        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
            return 0;
        }

    }
    else
    {
        KINFO_LOG(p_user->uid, "hole reward cached");
        return process_plantation_after_all_hole(p_user, p_all_hole);
    }

    return 0;
}


int process_get_friend_list(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_get_friend_req_t), 0);

    int idx = 0;
    friend_cache_t *p_friend_cache = (friend_cache_t *)p_user->buffer;
    taomee::unpack(p_msg_body, p_friend_cache->page, idx);
    taomee::unpack(p_msg_body, p_friend_cache->page_num, idx);
    if (p_friend_cache->page_num > 100)
    {
        KCRIT_LOG(p_user->uid, "page num:%u is too large.", p_friend_cache->page_num);
        return -1;
    }
    taomee::unpack(p_msg_body, p_friend_cache->peer_id, idx);


    p_friend_cache->cur_count = 0;

    uint16_t all_friend_num = 0;
    char buffer[MAX_SEND_PKG_SIZE] = {0};
    friend_rsp_t *p_friend_rsp = (friend_rsp_t *)buffer;
    //缓存里面有数据
    if (p_user->friend_tree.get_friend_tree(p_friend_cache->peer_id, (p_friend_cache->page - 1 ) * p_friend_cache->page_num + 1, p_friend_cache->page * p_friend_cache->page_num, p_friend_rsp, &all_friend_num) == 0)
    {
        KINFO_LOG(p_user->uid, "get %u friend tree from cache.", p_friend_cache->peer_id);
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);

        g_send_msg.pack(all_friend_num);
        g_send_msg.pack((uint8_t)p_friend_rsp->count);
        for (int i = 0; i != (int)p_friend_rsp->count; ++i)
        {
            g_send_msg.pack(p_friend_rsp->friend_list[i].user_id);
            g_send_msg.pack(p_friend_rsp->friend_list[i].name, sizeof(p_friend_rsp->friend_list[i].name));
            g_send_msg.pack(p_friend_rsp->friend_list[i].gender);
            g_send_msg.pack(p_friend_rsp->friend_list[i].country_id);
            g_send_msg.pack(p_friend_rsp->friend_list[i].birthday);
            g_send_msg.pack(p_friend_rsp->friend_list[i].user_type);
            g_send_msg.pack(p_friend_rsp->friend_list[i].mon_id);
            g_send_msg.pack(p_friend_rsp->friend_list[i].mon_level);
            g_send_msg.pack(p_friend_rsp->friend_list[i].mon_main_color);
            g_send_msg.pack(p_friend_rsp->friend_list[i].mon_ex_color);
            g_send_msg.pack(p_friend_rsp->friend_list[i].mon_eye_color);
            g_send_msg.pack(p_friend_rsp->friend_list[i].last_login_time);
            g_send_msg.pack(p_friend_rsp->friend_list[i].is_best_friend);
#ifdef DEBUG
            KINFO_LOG(p_user->uid, "get friend:%u, name:%s, user_type:%u, mon_id:%u, is_best_friend:%u",
                    p_friend_rsp->friend_list[i].user_id, p_friend_rsp->friend_list[i].name, p_friend_rsp->friend_list[i].user_type,
                    p_friend_rsp->friend_list[i].mon_id, p_friend_rsp->friend_list[i].is_best_friend);
#endif
        }
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

        if(p_user->uid == p_friend_cache->peer_id)
        {//获取自己的好友列表
            badge_step(p_user, badge_friends_100_const, all_friend_num);
        }
        return 0;
    }

    KINFO_LOG(p_user->uid, "get friend tree from db");

    //好友列表存不存在
    bool friend_list_exist = false;
    if (p_friend_cache->peer_id == p_user->uid)
    {
        if (p_user->friend_tree.get_friend_list((p_friend_cache->page - 1 ) * p_friend_cache->page_num + 1, p_friend_cache->page * p_friend_cache->page_num, &p_friend_cache->friend_list) == 0)
        {
            friend_list_exist = true;
        }
    }
    else
    {
        if (p_user->friend_tree.get_other_friend_list((p_friend_cache->page - 1 ) * p_friend_cache->page_num + 1, p_friend_cache->page * p_friend_cache->page_num, &p_friend_cache->friend_list) == 0)
        {
            friend_list_exist = true;
        }
    }
    if (friend_list_exist)
    {
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
        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
        pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_friend_cache->friend_list.friend_info[0].friend_id, GET_SVR_SN(p_user), 0);

        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy failed.");
            return 0;
        }

        return 0;
    }

    //向db请求好友列表
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_id, p_friend_cache->peer_id, GET_SVR_SN(p_user), sizeof(db_msg_friend_list_req_t));
    db_msg_friend_list_req_t *p_body = (db_msg_friend_list_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->begin = 0;
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
        return 0;
    }
    p_friend_cache->begin_index = 0;
    p_friend_cache->friend_list.count = 0;

    return 0;
}

int process_friend_op(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_friend_op_req_t), 0);

    int idx = 0;
    const static uint8_t no_update = -1;
    as_msg_friend_op_req_t *p_req = (as_msg_friend_op_req_t *)p_user->buffer;
    taomee::unpack(p_msg_body, p_req->friend_id, idx);
    taomee::unpack(p_msg_body, p_req->op_type, idx);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;

    if (p_req->op_type == DEL_FRIEND)
    {
        KINFO_LOG(p_user->uid, "del friend:%u", p_req->friend_id);
        pack_svr_pkg_head(p_header, svr_msg_db_del_friend, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_del_friend_req_t));

        db_msg_del_friend_req_t *p_body = (db_msg_del_friend_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->friend_id = p_req->friend_id;
    }
    else
    {
        pack_svr_pkg_head(p_header, svr_msg_db_set_friend_status, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_set_friend_status_req_t));

        db_msg_set_friend_status_req_t *p_body = (db_msg_set_friend_status_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->friend_info.friend_id = p_req->friend_id;

        if (BEST_FRIEND == p_req->op_type)
        {
            KINFO_LOG(p_user->uid, "set friend:%u to best friend", p_req->friend_id);
            p_body->friend_info.type = no_update;
            p_body->friend_info.is_bestfriend = 1;
        }
        else if (CANCEL_BEST_FRIEND == p_req->op_type)
        {
            KINFO_LOG(p_user->uid, "cancel friend:%u from best friend", p_req->friend_id);
            p_body->friend_info.type = no_update;
            p_body->friend_info.is_bestfriend = 0;
        }
        else if (BLOCK_FRIEND == p_req->op_type)
        {
            KINFO_LOG(p_user->uid, "block friend:%u", p_req->friend_id);
            p_body->friend_info.type = FRIEND_BLOCK;
            p_body->friend_info.is_bestfriend = no_update;
        }
        else
        {
            KCRIT_LOG(p_user->uid, "[msg:%u] op type:%u is invalid.", p_user->waitcmd, p_req->op_type);
            return -1;
        }
    }

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_find_friend(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL_GE(p_user->uid, msg_body_len, 0, 0);
    CHECK_VAL_LE(p_user->uid, msg_body_len, (int)sizeof(as_msg_find_friend_req_t), 0);

    int idx = 0;
    char buffer[sizeof(as_msg_find_friend_req_t) + 1] = {0};
    as_msg_find_friend_req_t *p_req = (as_msg_find_friend_req_t *)buffer;
    taomee::unpack(p_msg_body, p_req->buffer, msg_body_len, idx);
    if (strlen(p_req->buffer) == 0)
    {
        KCRIT_LOG(p_user->uid, "find user name or user_id cann't be null.");
        g_errno = ERR_MSG_LEN;
        return 0;
    }
    KINFO_LOG(p_user->uid, "find friend:%s", p_req->buffer);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    uint32_t user_id = 0;
    if (string_is_digit(p_req->buffer))            //是米米号
    {
        user_id = atoi(p_req->buffer);
    }
    if (user_id != 0)            //是米米号
    {
        as_msg_find_frined_rsp_t *p_rsp = (as_msg_find_frined_rsp_t *)p_user->buffer;
        p_rsp->current_count = 0;
        p_rsp->total_count = 1;
        p_rsp->friend_info[0].user_id = user_id;

        pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_rsp->friend_info[0].user_id, GET_SVR_SN(p_user), 0);
        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
            return 0;
        }
    }
    else
    {
        pack_svr_pkg_head(p_header, svr_msg_db_search_name, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_search_name_req_t));
        db_msg_search_name_req_t *p_body = (db_msg_search_name_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
        memcpy(p_body, p_req->buffer, sizeof(p_req->buffer));
        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
            return 0;
        }
    }

    return 0;
}

int process_add_friend(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL_LE(p_user->uid, msg_body_len, (int)(sizeof(as_msg_add_friend_req_t) + MAX_MESSAGE_BYTE), 0);
    CHECK_VAL_GE(p_user->uid, msg_body_len, (int)sizeof(as_msg_add_friend_req_t), 0);

    //保证好友数量不超过限制
    int friend_num = p_user->friend_tree.get_friend_num();
    if (friend_num < 0)
    {
        KCRIT_LOG(p_user->uid, "get friend num failed.");
        return -1;
    }
    if (friend_num >= MAX_FRIEND_NUM)
    {
        g_errno = ERR_MAX_FRIEND_NUM;
        KINFO_LOG(p_user->uid, "friend num:%u is too large", friend_num);
        return 0;
    }

    int idx = 0;
    as_msg_add_friend_req_t *p_req = (as_msg_add_friend_req_t *)p_user->buffer;
    taomee::unpack(p_msg_body, p_req->user_id, idx);
    taomee::unpack(p_msg_body, p_req->remark_count, idx);
    taomee::unpack(p_msg_body, p_req->remark, p_req->remark_count, idx);
    KINFO_LOG(p_user->uid, "apply for friend:%u", p_req->user_id);

   // if(check_for_depversion(p_user, p_req->user_id))
   // {//检测是否为涉及版署号的操作
   //     return 0;
   // }

    if (p_req->user_id == p_user->uid)   //加自己为好友
    {
        g_errno = ERR_ADD_SELF_FRIEND;
        KINFO_LOG(p_user->uid, "add oneself as friend.");
        return 0;
    }
    if (p_user->friend_tree.is_friend(p_req->user_id) == 1)
    {
        g_errno = ERR_ALREADY_FRIEND;
        KINFO_LOG(p_user->uid, "user:%u is already friend", p_req->user_id);
        return 0;
    }

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_apply_for_friend, p_req->user_id, GET_SVR_SN(p_user), sizeof(db_msg_friend_apply_req_t) + p_req->remark_count);

    db_msg_friend_apply_req_t *p_body = (db_msg_friend_apply_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->peer_id = p_user->uid;
    p_body->type = FRIEND_PENDING;
    p_body->remark_count = p_req->remark_count;
    memcpy(p_body->remark, p_req->remark, p_req->remark_count);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
        return 0;
    }

    return 0;
}

int process_get_friend_apply(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_apply, p_user->uid, GET_SVR_SN(p_user), 0);
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
        return 0;
    }

    return 0;
}

int process_friend_apply_op(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_friend_apply_op_req_t), 0);

    int idx = 0;
    as_msg_friend_apply_op_req_t *p_req = (as_msg_friend_apply_op_req_t *)p_user->buffer;
    taomee::unpack(p_msg_body, p_req->user_id, idx);
    taomee::unpack(p_msg_body, p_req->type, idx);

    KINFO_LOG(p_user->uid, "friend:%u req, op:%u", p_req->user_id, p_req->type);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;

    if (FRIEND_REQ_REFUSE == p_req->type) //拒绝好友申请
    {
        pack_svr_pkg_head(p_header, svr_msg_db_del_friend, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_del_friend_req_t));

        db_msg_del_friend_req_t *p_body = (db_msg_del_friend_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->friend_id = p_req->user_id;
    }
    else
    {
        pack_svr_pkg_head(p_header, svr_msg_db_set_friend_status, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_set_friend_status_req_t));

        db_msg_set_friend_status_req_t *p_body = (db_msg_set_friend_status_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

        p_body->friend_info.friend_id = p_req->user_id;

        if (FRIEND_REQ_AGREE == p_req->type) //同意好友申请
        {
            if (p_user->friend_tree.is_friend(p_req->user_id) > 0)
            {
                KINFO_LOG(p_user->uid, "%u is already friend", p_req->user_id);

                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(p_req->user_id);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                return 0;
            }
            int friend_num = p_user->friend_tree.get_friend_num();
            if (friend_num < 0)
            {
                KCRIT_LOG(p_user->uid, "get friend num failed");
                return -1;
            }
            if (friend_num >= MAX_FRIEND_NUM)
            {
                KCRIT_LOG(p_user->uid, "friend reach max, cann't accpet apply");
                g_errno = ERR_MAX_FRIEND_NUM;
                return 0;
            }
            p_body->friend_info.type = FRIEND_DEFAULT;
        }
        else if (FRIEND_REQ_BLOCK == p_req->type)  //屏蔽好友申请
        {
            p_body->friend_info.type = FRIEND_BLOCK;
        }
        else
        {
            KCRIT_LOG(p_user->uid, "type:%u is not allowed.", p_req->type);
            return -1;
        }
        const static uint8_t no_update = -1;
        p_body->friend_info.is_bestfriend = no_update;
    }

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
        return 0;
    }
    return 0;
}

int process_get_shop_item(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint32_t), 0);

    int idx = 0;
    uint32_t shop_id = 0;
    taomee::unpack(p_msg_body, shop_id, idx);
    KINFO_LOG(p_user->uid, "get shop:%u", shop_id);

    //统计商店访问数
    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_store_base + shop_id - 10000, &stat_data, sizeof(stat_data));

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_get_shop_item, p_user->uid, GET_SVR_SN(p_user), sizeof(shop_key_t));
    shop_key_t *p_body = (shop_key_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->id = shop_id;
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "get role info failed.");
        return -1;
    }
    p_body->type = p_role->role.monster_level;


    //发给switch
    if (g_p_switch_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_SWITCH;
        KCRIT_LOG(p_user->uid, "send switch server failed.");
        return 0;
    }

    return 0;
}

int process_get_stranger(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);

    //获得memcached里的活跃用户id
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_get_active_user, p_user->uid, GET_SVR_SN(p_user), 0);

    //发给switch
    if (g_p_switch_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_SWITCH;
        KCRIT_LOG(p_user->uid, "send switch server failed.");
        return 0;
    }

    return 0;
}

int process_add_friend_to_cache(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint32_t), 0);

    int idx = 0;
    uint32_t peer_id = 0;
    taomee::unpack(p_msg_body, peer_id, idx);
    *(uint32_t *)p_user->buffer = peer_id;

    KINFO_LOG(p_user->uid, "req:%u info for friend", peer_id);
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, peer_id, GET_SVR_SN(p_user), 0);
    if (g_p_db_proxy_svr->send_data(NULL, g_send_buffer, p_header->len, NULL) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(peer_id, "send to db proxy failed, user:%u", p_user->uid);
    }

    return 0;
}

int process_trade_stuff(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_trade_stuff_req_t), 0);

    int idx = 0;
    as_msg_trade_stuff_req_t req = {0};
    taomee::unpack(p_msg_body, req.stuff_id, idx);
    taomee::unpack(p_msg_body, req.type, idx);

    map<uint32_t, item_t>::iterator iter = g_item_map.find(req.stuff_id);
    if (iter == g_item_map.end())
    {
        KCRIT_LOG(p_user->uid, "stuff id:%u is not exists.", req.stuff_id);
        g_errno = ERR_STUFF_NOT_EXIST;
        return 0;
    }
    //判断背包里有没有这个物品
    int stuff_num = p_user->user_cache.get_stuff_num(req.stuff_id);
    if (stuff_num < 0)
    {
        KCRIT_LOG(p_user->uid, "get stuff:%u num failed", req.stuff_id);
        return -1;
    }
    if (0 == stuff_num)
    {
        KCRIT_LOG(p_user->uid, "not enough stuff:%u", req.stuff_id);
        g_errno = ERR_NOT_ENOUGH_STUFF;
        return 0;
    }

    KINFO_LOG(p_user->uid, "trade stuff:%u", req.stuff_id);

    stuff_trade_t *p_trade = (stuff_trade_t *)p_user->buffer;
    p_trade->item_id = req.stuff_id;
    if (PERCENT_FORTY == req.type)
    {
        p_trade->percent = 4;
    }
    else
    {
        p_trade->percent = uniform_rand(1, 10);
    }

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_eat_food, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_eat_food_req_t));

    db_msg_eat_food_req_t *p_body = (db_msg_eat_food_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->item_id = req.stuff_id;
    p_body->reward_health = 0;
    p_body->reward_happy = 0;
    float reward_coins = iter->second.price * ((float)p_trade->percent / 10);
    p_trade->coins = ceil(reward_coins);
    p_body->reward_coins = p_trade->coins;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        ERROR_LOG("send to db proxy server failed.");
        return 0;
    }

    return 0;
}

int process_get_pet(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint32_t), 0);

    int idx = 0;
    uint32_t *p_user_id = (uint32_t *)p_user->buffer;
    taomee::unpack(p_msg_body, *p_user_id, idx);
    KINFO_LOG(p_user->uid, "get user:%u zoom", *p_user_id);


    if (*p_user_id == p_user->uid)    //自己的从缓存里获得
    {
    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_enter_petall, &stat_data, sizeof(stat_data));
        char buffer[MAX_SEND_PKG_SIZE] = {0};
        all_pet_t *p_all_pet = (all_pet_t *)buffer;
        if (p_user->user_cache.get_all_pet(p_all_pet) != 0)
        {
            KCRIT_LOG(p_user->uid, "get pet failed");
            return -1;
        }

        send_cache_pet_info(p_user, p_all_pet);

        return 0;
    }

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_all_pet, *p_user_id, GET_SVR_SN(p_user), 0);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        ERROR_LOG("send to db proxy server failed.");
        return 0;
    }

    return 0;
}

/**
 * @brief process_enter_two_puzzle 进入二人益智游戏比赛
 */
int process_enter_two_puzzle(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)//40134
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        g_errno = ERR_GET_ROLE_CACHE;
        KCRIT_LOG(p_user->uid, "get role info failed.");
        return 0;
    }

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    two_gamer_info_t *p_body = (two_gamer_info_t*)(g_send_buffer + sizeof(svr_msg_header_t));
    memcpy(p_body->uname, p_role->role.name, 16);
    p_body->is_vip = p_role->role.user_type;
    p_body->monster_id = p_role->role.monster_id;
    p_body->monster_main_color = p_role->role.monster_main_color;
    p_body->monster_exp_color = p_role->role.monster_ex_color;
    p_body->monster_eye_color = p_role->role.monster_eye_color;

    pack_svr_pkg_head(p_header, svr_msg_online_multi_enter, p_user->uid, GET_SVR_SN(p_user), sizeof(two_gamer_info_t));

    if (g_p_multi_server_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_MULTI_SERVER;
        ERROR_LOG("send to multi server failed.");
        return 0;
    }

    //统计参与多人挑战的人数
    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(multi_online_particate, &stat_data, sizeof(stat_data));

    return 0;
}

/**
 * @brief process_start_two_puzzle 点击start按钮
 */
int process_start_two_puzzle(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)//40135
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);

    svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_online_multi_start, p_user->uid, GET_SVR_SN(p_user), 0);

    if (g_p_multi_server_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_MULTI_SERVER;
        ERROR_LOG("send to multi server failed.");
        return 0;
    }

    //统计进入多人挑战的人数
    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(multi_online_start, &stat_data, sizeof(stat_data));

    return 0;
}

/**
 * @brief process_answer_two_puzzle 答题
 */
int process_answer_two_puzzle(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)//40136
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(answer_question_request_t), 0);

    svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
    answer_question_request_t *p_request = (answer_question_request_t*)(g_send_buffer + sizeof(svr_msg_header_t));
    int idx = 0;
    taomee::unpack(p_msg_body, p_request->is_corrent, idx);
    taomee::unpack(p_msg_body, p_request->is_end, idx);
    taomee::unpack(p_msg_body, p_request->remain_time, idx);

   KINFO_LOG(p_user->uid, "is_correct:%u, is_end:%u, remain_time:%u", p_request->is_corrent, p_request->is_end, p_request->remain_time);
    pack_svr_pkg_head(p_header, svr_msg_online_multi_answer, p_user->uid, GET_SVR_SN(p_user), msg_body_len);

    if (g_p_multi_server_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_MULTI_SERVER;
        KERROR_LOG(p_user->uid, "send to multi server failed.");
        return 0;
    }

    return 0;
}

/**
 * @brief process_interactive_element 40137处理函数
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_interactive_element(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);

    int idx = 0;
    uint32_t interactive_id = 0;
    taomee::unpack(p_msg_body, interactive_id, idx);
    KINFO_LOG(p_user->uid, "interactive element %u", interactive_id);

	/*dragon boat activity*/
	if(interactive_id == DRAGON_BOAT_INTERACTIVE_ID)
	{
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
			*req = 1;
		    pack_svr_pkg_head(p_head, svr_msg_db_update_dragon_boat, p_user->uid, GET_SVR_SN(p_user), sizeof(uint8_t));
		    if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
		    {
		        g_errno = ERR_MSG_DB_PROXY;
		        KCRIT_LOG(p_user->uid, "send to db proxy failed");
		    }
		}
	}
	else
	{
		/*pick money from interactive element*/
	    map<uint32_t, interactive_rewards_t>::iterator iter = g_interactive_reward_map.find(interactive_id);
	    if(iter == g_interactive_reward_map.end())
	    {
	        KERROR_LOG(p_user->uid, "Interactive id:%u not exists", interactive_id);
	        g_errno = ERR_NOT_EXIST_INTERACTIVE;
	        return 0;
	    }
	    else
	    {
	        uint16_t reward_coins = uniform_rand(iter->second.min_rewards, iter->second.max_rewards);
	        reward_coins = get_current_gains(reward_coins, p_user);
	        svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
	        pack_svr_pkg_head(p_header, svr_msg_db_interactive, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_interactive_req_t));
	        db_msg_interactive_req_t *p_body = (db_msg_interactive_req_t*)(g_send_buffer + sizeof(svr_msg_header_t));
	        p_body->day_time = timestamp_begin_day();
	        p_body->interactive_id  = interactive_id;
	        p_body->reward_coins = reward_coins;
	        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
	        {
	            g_errno = ERR_MSG_DB_PROXY;
	            ERROR_LOG("send to db proxy server failed.");
	            return 0;
	        }
	    }
	}
    return 0;
}

int process_ucount_add_visit(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);

    int idx = 0;
    uint32_t peer_id = 0;
    taomee::unpack(p_msg_body, peer_id, idx);
    KINFO_LOG(p_user->uid, "visit user:%u room", peer_id);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_ucount_visit, peer_id, GET_SVR_SN(p_user), sizeof(uint32_t));
    uint32_t *p_body = (uint32_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    *p_body = p_user->uid;

    if (g_p_ucount_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_UCOUNT;
        ERROR_LOG("send to ucount server failed.");
        return 0;
    }

    return 0;
}

int process_stat_data(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    int idx = 0;
    uint8_t type = 0;
    taomee::unpack(p_msg_body, type, idx);
    KINFO_LOG(p_user->uid, "as stat type:%u", type);

    time_t now = time(NULL);
    switch (type)
    {
        case  AS_STAT_LEAVE_SHOP:
            {
                //去除离开商店的统计2012-06-07
    //            uint32_t shop_id = 0;
    //            taomee::unpack(p_msg_body, shop_id, idx);
    //            if (p_user->stat_time != 0 && p_user->stat_time <= now)
    //            {
    //                stat_three_param_t stat_data = {p_user->uid, 1, now - p_user->stat_time};
    //                msg_log(stat_leave_store_base + shop_id - 10000, &stat_data, sizeof(stat_data));

    //                p_user->stat_time = 0;
    //            }
            }
            break;
        case AS_STAT_START_GAME:
            {
                uint32_t game_id = 0;
                taomee::unpack(p_msg_body, game_id, idx);

                p_user->stat_time = now;
                stat_two_param_t stat_data = {p_user->uid, 1};
                msg_log(stat_picture_matching + game_id - 1, &stat_data, sizeof(stat_data));
            }
            break;
        case AS_MAGGIC_VISIT:
            {
                stat_two_param_t stat_data = {p_user->uid, 1};
                msg_log(stat_maggic_visit, &stat_data, sizeof(stat_data));
            }
            break;
        case AS_START_TASK:
            {
                uint32_t task_id = 0;
                taomee::unpack(p_msg_body, task_id, idx);

                stat_two_param_t stat_data = {p_user->uid, 1};
                msg_log(stat_task_start_base + task_id - 100000, &stat_data, sizeof(stat_data));
                break;

            }
        default:
            KCRIT_LOG(p_user->uid, "stat type:%u is unknown", type);
            return -1;
    }

    p_user->waitcmd = 0;
    p_user->inc_counter();
    return 0;
}

int process_multi_puzzle_add_coin(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    two_puzzle_req_t *p_result = (two_puzzle_req_t*)p_user->buffer;

    int idx = 0;
    taomee::unpack(p_msg_body, p_result->result.self_score, idx);
    taomee::unpack(p_msg_body, p_result->result.opp_score, idx);
    taomee::unpack(p_msg_body, p_result->result.result, idx);
    taomee::unpack(p_msg_body, p_result->result.rewards_coins, idx);
    taomee::unpack(p_msg_body, p_result->result.is_restrict, idx);
    taomee::unpack(p_msg_body, p_result->msg_type, idx);

    KINFO_LOG(p_user->uid, "[process_multi_puzzle_add_coin]:自己得分:%u 对手得分:%u, 结果:%u 奖励:%u 限制:%u 消息:%u", p_result->result.self_score, p_result->result.opp_score, p_result->result.result, p_result->result.rewards_coins, p_result->result.is_restrict, p_result->msg_type);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    db_msg_update_role_req_t *p_body = (db_msg_update_role_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, svr_msg_db_add_role_value, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_update_role_req_t) + sizeof(field_t));

    memset(p_body, 0, sizeof(db_msg_update_role_req_t));
    p_body->type = DB_ADD_ROLE;
    p_body->count = 1;
    p_body->field[0].type = FIELD_COIN;
    p_body->field[0].value = get_current_gains(p_result->result.rewards_coins, p_user);

    p_result->result.rewards_coins = p_body->field[0].value;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        g_errno = ERR_MSG_DB_PROXY;
        return 0;
    }

    return 0;
}

int process_get_latest_visit(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);

    //向ucount取得最近访问过的用户id
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;

    pack_svr_pkg_head(p_header, svr_msg_ucount_latest_visit, p_user->uid, GET_SVR_SN(p_user), 0);

    if (g_p_ucount_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_UCOUNT;
        KCRIT_LOG(p_user->uid, "send to ucount server failed.");
        return 0;
    }

    return 0;
}


int process_get_real_message_count(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint32_t), 0);

    uint32_t peer_id = 0;
    int idx = 0;
    taomee::unpack(p_msg_body, peer_id, idx);

    svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_real_message_count, peer_id, GET_SVR_SN(p_user), sizeof(uint32_t));
    uint32_t *p_peer_id = (uint32_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    *p_peer_id = p_user->uid;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        g_errno = ERR_MSG_DB_PROXY;
        return 0;
    }

    return 0;
}


int process_get_all_badge(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint32_t), 0);

    uint32_t peer_id = 0;
    int idx = 0;
    taomee::unpack(p_msg_body, peer_id, idx);

    if(p_user->uid == peer_id)
    {//请求自己的成就，从缓存中取
        char buffer[MAX_SEND_PKG_SIZE] = {0};
        all_badge_info_t *p_all_badge = (all_badge_info_t*)buffer;
        if(p_user->user_cache.get_all_badge(p_all_badge) == 0)
        {//缓存里面有

            //统计成就面板的打开
            stat_two_param_t stat_data2 = {p_user->uid, 1};
            msg_log(stat_badge_num, &stat_data2, sizeof(stat_data2));

            //通知db将未读的成就数置0,db返回后将缓存中未读的成就数置0
            svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
            uint16_t *p_unread_badge = (uint16_t*)(g_send_buffer + sizeof(svr_msg_header_t));

            *p_unread_badge = (uint16_t)0;//将未读数设置为0

            pack_svr_pkg_head(p_header, svr_msg_db_set_unread_badge, p_user->uid, GET_SVR_SN(p_user), sizeof(uint16_t));
            if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
            {
                g_errno = ERR_MSG_DB_PROXY;
                ERROR_LOG("send to db proxy server failed.");
                return 0;
            }
        }
        else
        {//缓存里面没有，不应该的
            g_errno = ERR_SYSTEM_ERR;
            KCRIT_LOG(p_user->uid, "cli_proto:It not possible to come here, we have get badge list at login time.");
        }
    }
    else
    {//请求他人的成就，从db取
              svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
              pack_svr_pkg_head(p_header, svr_msg_db_get_all_badge, peer_id, GET_SVR_SN(p_user), 0);

              if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
              {
                  g_errno = ERR_MSG_DB_PROXY;
                  ERROR_LOG("send to db proxy server failed.");
                  return 0;
              }
    }
    return 0;
}

int process_update_activity_status(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);


    char buffer[2048] = {0};
    single_activity_t *p_sa = (single_activity_t*)buffer;

    int idx = 0;
    taomee::unpack(p_msg_body, p_sa->activity_id, idx);
    taomee::unpack(p_msg_body, p_sa->kind_num, idx);

    CHECK_VAL(p_user->uid, msg_body_len,(int)(sizeof(single_activity_t) + p_sa->kind_num * sizeof(single_reward_t)) , 0);

    for(uint16_t i = 0; i < p_sa->kind_num; i++)
    {
        taomee::unpack(p_msg_body, p_sa->reward[i].reward_id, idx);
        taomee::unpack(p_msg_body, p_sa->reward[i].reward_num, idx);
        taomee::unpack(p_msg_body, p_sa->reward[i].reward_status, idx);
    }

    svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
    char *p_body = g_send_buffer + sizeof(svr_msg_header_t);

    memcpy(p_body, (char*)p_sa, msg_body_len);

    pack_svr_pkg_head(p_header, svr_msg_update_activity_status, p_user->uid, GET_SVR_SN(p_user), msg_body_len);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        ERROR_LOG("send to db proxy server failed in send_badge_to_db.");
        return 0;
    }

    return 0;
}

int process_update_badge_status(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    badge_info_t badge;

    int idx = 0;
    taomee::unpack(p_msg_body, badge.badge_id, idx);
    taomee::unpack(p_msg_body, badge.badge_status, idx);
    taomee::unpack(p_msg_body, badge.badge_progress, idx);

    KINFO_LOG(p_user->uid, "update_badge_status badge_id:%u, badge_status:%u badge_progress:%u", badge.badge_id, badge.badge_status, badge.badge_progress);

    svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
    char *p_body = g_send_buffer + sizeof(svr_msg_header_t);

    memcpy(p_body, (char*)&badge, sizeof(badge_info_t));

    pack_svr_pkg_head(p_header, svr_msg_db_update_badge, p_user->uid, GET_SVR_SN(p_user), sizeof(badge_info_t));

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
                  g_errno = ERR_MSG_DB_PROXY;
        ERROR_LOG("send to db proxy server failed in send_badge_to_db.");
        return 0;
    }

    return 0;
}


int process_get_unread_visit(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_get_unread_count, p_user->uid, GET_SVR_SN(p_user), 0);
    if (g_p_ucount_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_UCOUNT;
        KCRIT_LOG(p_user->uid, "send to ucount server failed.");
        return 0;
    }

    return 0;
}

int process_get_factory(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "get role cache");
        return -1;
    }

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    if (0 == p_role->role.compose_time)     //没有物品在合成
    {
        g_send_msg.pack((uint8_t)FACTORY_IDLE);

    }
    else if (time(NULL) < p_role->role.compose_time) //物品没有合成完
    {
        g_send_msg.pack((uint8_t)FACTORY_CREATE);
        g_send_msg.pack((uint32_t)(p_role->role.compose_time - time(NULL)));
        g_send_msg.pack(p_role->role.map_id);
    }
    else
    {
        //物品合成成功
        g_send_msg.pack((uint8_t)FACTORY_FINISH);
        g_send_msg.pack(p_role->role.compose_id);
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    return 0;
}

/**
 * @brief process_create_stuff 提交合成信息
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_create_stuff(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint32_t), 0);

    int idx = 0;
    uint32_t map_id = 0;
    taomee::unpack(p_msg_body, map_id, idx);
    KINFO_LOG(p_user->uid, "client commit compose info, map:%u", map_id);


    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "get role cache");
        return -1;
    }

    if (0 != p_role->role.compose_time)     //有物品在合成, 不可以提交新的合成信息
    {
        KCRIT_LOG(p_user->uid, "There have map %u at factory, can not commit now", p_role->role.compose_id);
        g_errno = ERR_HAVE_MAP_IN_FACTORY;
        return 0;
    }

    //判断图纸是否解锁
    int num = p_user->user_cache.get_stuff_num(map_id);
    if (num < 0)
    {
        KCRIT_LOG(p_user->uid, "get stuff:%u num failed", map_id);
        return -1;
    }
    if (0 == num)
    {
        KCRIT_LOG(p_user->uid, "map:%u is not unlock", map_id);
        return -1;
    }

    db_msg_compose_stuff_req_t *p_req = (db_msg_compose_stuff_req_t *)p_user->buffer;
    p_req->num = 0;

    map<uint32_t, vector<factory_cost_t> >::iterator cost_iter = g_factory_cost_map.find(map_id);
    if (cost_iter == g_factory_cost_map.end())
    {
        KCRIT_LOG(p_user->uid, "factory map:%u is not exist", map_id);
        return -1;
    }
    const vector<factory_cost_t> &cost_vec = cost_iter->second;
    for (vector<factory_cost_t>::const_iterator iter = cost_vec.begin(); iter != cost_vec.end(); ++iter)
    {
        uint32_t item_id = iter->item_id;
        uint16_t item_num = iter->item_num;
        int stuff_num = p_user->user_cache.get_stuff_num(item_id);
        if (stuff_num < 0)
        {
            KCRIT_LOG(p_user->uid, "get stuff:%u num failed", item_id);
            return -1;
        }
        if (item_num > stuff_num)
        {
            KCRIT_LOG(p_user->uid, "stuff:%u need_num:%u owned_num:%d is small", item_id, item_num, stuff_num);
            g_errno = ERR_NOT_ENOUGH_material;
            return 0;
        }
        p_req->item_cost[p_req->num].item_id = item_id;
        p_req->item_cost[p_req->num].item_num = item_num;
        ++p_req->num;
    }

    map<uint32_t, vector<uint32_t> >::iterator create_iter = g_factory_create_map.find(map_id);
    if (create_iter == g_factory_create_map.end())
    {
        KCRIT_LOG(p_user->uid, "factory map:%u is not exist", map_id);
        return -1;
    }
    const vector<uint32_t> &create_vec = create_iter->second;
    //随机生成一个物品
    int index = uniform_rand(0, create_vec.size() - 1);
    p_req->stuff_id = create_vec[index];

    map<uint32_t, uint32_t>::iterator time_iter = g_factory_cost_time_map.find(map_id);
    if (time_iter == g_factory_cost_time_map.end())
    {
        KCRIT_LOG(p_user->uid, "factory map:%u is not exist", map_id);
        return -1;
    }
    uint32_t create_time = time_iter->second;
    p_req->finish_time = create_time + time(NULL);

    p_req->map_id = map_id;

    svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
    char *p_body = g_send_buffer + sizeof(svr_msg_header_t);

    pack_svr_pkg_head(p_header, svr_msg_db_compose_stuff, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_compose_stuff_req_t) + p_req->num * sizeof(item_cost_t));
    memcpy(p_body, p_user->buffer, sizeof(db_msg_compose_stuff_req_t) + p_req->num * sizeof(item_cost_t));

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
                  g_errno = ERR_MSG_DB_PROXY;
        ERROR_LOG("send to db proxy server failed in send_badge_to_db.");
        return 0;
    }

    return 0;
}

/**
 * @brief process_factory_op 合成工厂的操作:领取合成的物品以及取消合成
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_factory_op(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint8_t), 0);

    int idx = 0;
    uint8_t type = 0;
    taomee::unpack(p_msg_body, type, idx);
    KINFO_LOG(p_user->uid, "compose op:%u", type);

    factory_op_t *p_op = (factory_op_t *)p_user->buffer;
    p_op->type = type;
    if (type == GET_COMPOSE)
    {
        role_cache_t *p_role = p_user->user_cache.get_role();
        if (NULL == p_role)
        {
            g_errno = ERR_GET_ROLE_CACHE;
            KCRIT_LOG(p_user->uid, "get role info failed.");
            return 0;
        }
        if (p_role->role.compose_time > time(NULL))
        {
            g_errno = ERR_NOT_TIME_GET_COMPOSE;
            KCRIT_LOG(p_user->uid, "not time to get compose stuff.");
            return 0;
        }
        //增加用户合成的物品
        p_op->item_id = p_role->role.compose_id;

        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
        factory_op_get_stuff_t *p_body = (factory_op_get_stuff_t*)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->count = 1;
        p_body->stuff[0].stuff_id = p_op->item_id;
        p_body->stuff[0].stuff_num = 1;

        //发送请求
        pack_svr_pkg_head(p_header, svr_msg_db_factory_op, p_user->uid, GET_SVR_SN(p_user), sizeof(factory_op_get_stuff_t) + p_body->count * sizeof(as_msg_buy_stuff_req_t));
        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
            return 0;
        }

        memcpy(p_user->buffer, p_body, sizeof(factory_op_get_stuff_t) + p_body->count * sizeof(as_msg_buy_stuff_req_t));
        return 0;
    }

    //更新用户的role信息
    svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_update_role, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_update_login_t) + 2 * sizeof(field_t));

    db_msg_update_login_t *p_body = (db_msg_update_login_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->count = 2;
    p_body->field[0].type = FIELD_COMPOSE_TIME;
    p_body->field[0].value = 0;
    p_body->field[1].type = FIELD_COMPOSE_ID;
    p_body->field[1].value = 0;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }
    memcpy(p_user->buffer, p_body, sizeof(db_msg_update_login_t) + 2 * sizeof(field_t));

    return 0;
}

int process_add_new_unlock_map(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    KINFO_LOG(p_user->uid, "unlock new map");

    int idx = 0;
    uint16_t count = 0;
    taomee::unpack(p_msg_body, count, idx);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    db_msg_add_stuff_req_t *p_body = (db_msg_add_stuff_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    p_body->count = count;
    for (int i = 0; i != count; ++i)
    {
        uint32_t stuff_id = 0;
        taomee::unpack(p_msg_body, stuff_id, idx);
        p_body->stuff[i].stuff_id = stuff_id;
        p_body->stuff[i].stuff_num = 1;
        p_body->coins = 0;
        p_body->happy = 0;
        p_body->reward_exp = 0;
        p_body->level_up = 0;
    }

    //发送请求
    pack_svr_pkg_head(p_header, svr_msg_db_add_bag_stuff, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_add_stuff_req_t) + p_body->count * sizeof(as_msg_buy_stuff_req_t));
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
        return 0;
    }

    *(uint8_t *)p_user->buffer = 0; //这里物品类型没有使用
    memcpy(p_user->buffer + sizeof(uint8_t), p_body, sizeof(db_msg_add_stuff_req_t) + p_body->count * sizeof(as_msg_buy_stuff_req_t));


    return 0;
}

int process_get_game_level(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint32_t), 0);

    int idx = 0;
    uint32_t game_id = 0;
    taomee::unpack(p_msg_body, game_id, idx);

    map<uint32_t, uint32_t>::iterator iter = g_game_map.find(game_id);
        if(iter == g_game_map.end())
        {
            KCRIT_LOG(p_user->uid, "1:request a unknown game:%u", game_id);
            return -1;
        }
    char buffer[4096] = {0};
    game_level_info_t *p_lvl = (game_level_info_t*)buffer;

    int ret = p_user->user_cache.get_game_level(game_id, iter->second, p_lvl);
    if(ret == 0)
    {//有缓存
        KINFO_LOG(p_user->uid, "Get game_lvl from cache:unlock_next:%u, level_num:%u", p_lvl->unlock_next_level, p_lvl->level_num);
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
    else if(ret == 1)
    {//没有缓存,从数据库取
        game_level_t *p_game = (game_level_t*)p_user->buffer;
        p_game->game_id = game_id;
        p_game->total_level = iter->second;//一共有多少关
        svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
        pack_svr_pkg_head(p_head, svr_msg_db_get_game_level, p_user->uid, GET_SVR_SN(p_user), sizeof(uint32_t));

        uint32_t *p_game_id = (uint32_t *)(g_send_buffer + sizeof(svr_msg_header_t));
        *p_game_id = game_id;

        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy failed.");
            return 0;
        }
    }
    else
    {//未知错误
            KCRIT_LOG(p_user->uid, "2:request a unknown game:%u", game_id);
            return -1;
    }

    return 0;
}

int process_check_dirty_word(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL_GE(p_user->uid, msg_body_len, 1, 0);
    CHECK_VAL_LE(p_user->uid, msg_body_len, 1024, 0);

    char check_name[1025] = {0};
    memcpy(check_name, p_msg_body, msg_body_len);
    int ret = tm_dirty_check(0, check_name);
    if (ret < 0)
    {
        g_errno = ERR_DIRTY_WORDS;
        KCRIT_LOG(p_user->uid, "dirty name");
        return 0;
    }

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);

    if (ret > 0)
    {
        g_send_msg.pack((uint8_t)DIRTY_YES);
    }
    else
    {
        g_send_msg.pack((uint8_t)DIRTY_NO);
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_modify_monster_name(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 16, 0);

    char *p_monster_name = (char *)p_user->buffer;
    memset(p_monster_name, 0, 17);
    memcpy(p_monster_name, p_msg_body, 16);
    int ret = tm_dirty_check(0, p_monster_name);
    if (ret != 0)
    {
        g_errno = ERR_DIRTY_WORDS;
        KCRIT_LOG(p_user->uid, "dirty name");
        return 0;
    }

    svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_modify_monster_name, p_user->uid, GET_SVR_SN(p_user), 16);
    char *p_name = (char *)(g_send_buffer + sizeof(svr_msg_header_t));
    memcpy(p_name, p_msg_body, 16);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_get_bobo_reward(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        g_errno = ERR_GET_ROLE_CACHE;
        KCRIT_LOG(p_user->uid, "get role info failed.");
        return 0;
    }

    stat_one_param_t stat_data = {1};
    msg_log(stat_kick_bobo_reward, &stat_data, sizeof(stat_data));

    //没有阅读过，不能领教
    if (p_role->role.last_paper_read != g_current_paper_term)
    {
        g_errno = ERR_HAS_GET_BOBO_REWARD;
        KCRIT_LOG(p_user->uid, "cann't get reward.");
        return 0;
    }

    if (p_role->role.last_paper_reward >= g_current_paper_term)
    {
        g_errno = ERR_HAS_GET_BOBO_REWARD;
        KCRIT_LOG(p_user->uid, "get reward repeatly.");
        return 0;
    }

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    db_msg_update_role_req_t *p_body = (db_msg_update_role_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, svr_msg_db_add_role_value, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_update_role_req_t) + 2 * sizeof(field_t));

    memset(p_body, 0, sizeof(db_msg_update_role_req_t));
    p_body->type = DB_ADD_ROLE;
    p_body->count = 2;
    p_body->field[0].type = FIELD_COIN;
    p_body->field[0].value = uniform_rand(g_bobo_reward_min, g_bobo_reward_max);

    //防成谜
    p_body->field[0].value = get_current_gains(p_body->field[0].value, p_user);

    p_body->field[1].type = FIELD_BOBO_REWARD;
    p_body->field[1].value = g_current_paper_term - p_role->role.last_paper_reward;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        g_errno = ERR_MSG_DB_PROXY;
        return 0;
    }

    bobo_cache_t *p_cache = (bobo_cache_t *)p_user->buffer;
    p_cache->coins = p_body->field[0].value;
    p_cache->paper_term = p_body->field[1].value;

    return 0;
}


int process_get_random_name(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{//随机获取名字
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint8_t), 0);

    int idx = 0;
    uint8_t gender = 0;
    taomee::unpack(p_msg_body, gender, idx);

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    if(gender == 0)
    {//女生
        idx = uniform_rand(0, g_female_name_num - 1);
        g_send_msg.pack((char*)g_female_name_vec[idx].name, 16);
    }
    else
    {
        idx = uniform_rand(0, g_male_name_num - 1);
        g_send_msg.pack((char*)g_male_name_vec[idx].name, 16);
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_read_bobo_newspaper(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);
    KINFO_LOG(p_user->uid, "read bobo newspaper");

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        g_errno = ERR_GET_ROLE_CACHE;
        KCRIT_LOG(p_user->uid, "get role info failed.");
        return 0;
    }

    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_kick_bobo, &stat_data, sizeof(stat_data));

    if (p_role->role.last_paper_read == g_current_paper_term)
    {
        send_header_to_user(p_user, SYSTEM_ERR_NUM, g_errno);
        return 0;
    }

    svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_update_role, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_update_login_t) + sizeof(field_t));

    db_msg_update_login_t *p_body = (db_msg_update_login_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->count = 1;
    p_body->field[0].type = FIELD_BOBO_READ;
    p_body->field[0].value = g_current_paper_term;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    memcpy(p_user->buffer, p_body, sizeof(db_msg_update_login_t) + sizeof(field_t));

    return 0;
}

int process_get_sun_reward(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(as_msg_get_sun_reward_t), 0);
    as_msg_get_sun_reward_t req;

    int idx = 0;
    taomee::unpack(p_msg_body, req.hole_id, idx);
    taomee::unpack(p_msg_body, req.reward_id, idx);

    KINFO_LOG(p_user->uid, "get_sun_reward hole:%u reward_id:%u", req.hole_id, req.reward_id);
    if(req.hole_id <= 0 || req.hole_id > PLANTATION_NUM)
    {
        g_errno = ERR_HOLE_ID;
        KCRIT_LOG(p_user->uid, "Not valid hole id (%u)", req.hole_id);
        return 0;
    }

    hole_sun_reward_t sun_reward;
    if(p_user->user_cache.get_hole_reward_i(req.hole_id, req.reward_id, &sun_reward) != 0)
    {
        g_errno = ERR_REWARD_ID;
        KCRIT_LOG(p_user->uid, "Not correct reward_id:%u in hole %u", req.reward_id, req.hole_id);
        return 0;
    }

    idx = uniform_rand(0, 1);
    if(idx == 0)
    {//只奖励愉悦值
        sun_reward_happy_cache_t *p_cache = (sun_reward_happy_cache_t*)p_user->buffer;

        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
        db_msg_sun_reward_happy_t *p_body = (db_msg_sun_reward_happy_t *)(g_send_buffer + sizeof(svr_msg_header_t));

        KINFO_LOG(p_user->uid, "cached reward_happy:%u", sun_reward.reward_happy);
        pack_svr_pkg_head(p_header, svr_msg_sun_reward_happy, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_sun_reward_happy_t));
        p_body->sun_reward = req;
        p_body->reward_happy = sun_reward.reward_happy;

        role_cache_t *p_role = p_user->user_cache.get_role();
        if (NULL == p_role)
        {
            KCRIT_LOG(p_user->uid, "get role cache");
            return -1;
        }

        time_t now = time(NULL);
        uint32_t now_happy = get_new_happy(now, p_role->role.last_login_time, p_role->role.monster_happy);
        if (now_happy + p_body->reward_happy > MAX_HAPPY_VALUE)
        {
            p_body->reward_happy = MAX_HAPPY_VALUE > now_happy ? MAX_HAPPY_VALUE - now_happy : 0;
        }

        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            ERROR_LOG("send to db proxy server failed.");
            return -1;
        }
        memcpy(p_cache, p_body, sizeof(db_msg_sun_reward_happy_t));
    }
    else if(idx == 1)
    {//只奖励经验值

        sun_reward_exp_cache_t *p_cache = (sun_reward_exp_cache_t*)p_user->buffer;

        p_cache->sun_reward = req;
        p_cache->reward_exp = get_current_gains(sun_reward.reward_exp, p_user);
        KINFO_LOG(p_user->uid, "cached reward_exp:%u", sun_reward.reward_exp);

        uint32_t day_time = timestamp_begin_day();
        restrict_key_t exp_key = {day_time, STRICT_SUN_EXP};
        uint32_t day_exp = 0;
        //获得今天已经获得的经验值
        if(p_user->user_cache.get_day_restrict(exp_key, &day_exp) == 0)
        {//缓存中有
            return process_sun_reward_exp(p_user, p_cache, day_exp);
        }
        else
        {//缓存中没有
             svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
             pack_svr_pkg_head(p_header, svr_msg_db_get_day_restrict, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_get_day_restrict_req_t));

             db_msg_get_day_restrict_req_t  *p_req = (db_msg_get_day_restrict_req_t*)(g_send_buffer + sizeof(svr_msg_header_t));
             p_req->type = STRICT_SUN_EXP;
            p_req->time = day_time;
            if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
            {
                g_errno = ERR_MSG_DB_PROXY;
                KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
                return 0;
            }
        }
    }
    else
    {//只奖励咕咚果
        sun_reward_coins_cache_t *p_cache = (sun_reward_coins_cache_t*)p_user->buffer;
        p_cache->sun_reward = req;
        p_cache->reward_coins = get_current_gains(sun_reward.reward_coins, p_user);
        KINFO_LOG(p_user->uid, "cached reward_coins:%u", sun_reward.reward_coins);

        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
        pack_svr_pkg_head(p_header, svr_msg_db_sun_reward_coins, p_user->uid, GET_SVR_SN(p_user), sizeof(sun_reward_coins_cache_t));

        sun_reward_coins_cache_t  *p_req = (sun_reward_coins_cache_t*)(g_send_buffer + sizeof(svr_msg_header_t));
        p_req->sun_reward = req;
        p_req->reward_coins = sun_reward.reward_coins;
        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
            return 0;
        }
    }
    return 0;
}

/**
 * @brief process_enter_npc_score 进入大众点评，或者换一批
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_enter_npc_score(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        g_errno = ERR_GET_ROLE_CACHE;
        KCRIT_LOG(p_user->uid, "get role info failed.");
        return 0;
    }

    uint32_t day_time = timestamp_begin_day();
    if(p_role->role.npc_score_daytime == day_time)
    {//今天已经评过分数了
        npc_score_info_t npc_score;
        p_user->user_cache.get_npc_score(&npc_score);
        if(npc_score.status == NPC_SCORE_INIT)
        {//未初始化，从数据库取
            KINFO_LOG(p_user->uid, "enter npc_score, today have scored, get from db");
            svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
            pack_svr_pkg_head(p_head, svr_msg_db_get_npc_score, p_user->uid, GET_SVR_SN(p_user), 0);

            if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
            {
                g_errno = ERR_MSG_DB_PROXY;
                KCRIT_LOG(p_user->uid, "send to db proxy failed.");
                return 0;
            }
            return 0;
        }
        else if(npc_score.status == NPC_SCORED)
        {
            KINFO_LOG(p_user->uid, "enter npc_score, today have scored, get from cache, return to as");
            send_npc_score_to_as(p_user, npc_score);
             return 0;
        }
        else
        {
            g_errno = ERR_SYSTEM_ERR;
            KCRIT_LOG(p_user->uid, "Not possible, day_time:%u, cache_day_time:%u status:%u", day_time, p_role->role.npc_score_daytime, npc_score.status);
            return 0;
        }
    }
    else
    {//今天未点评过
        KINFO_LOG(p_user->uid, "enter npc_score, today not scored, gen npc and cached, return to as");
        npc_score_info_t npc_score;
        npc_score.status = NPC_SCORE_NOT;
        npc_score.total_score = 0;
        npc_score.npc_num = NPC_SCORE_NUM;
        int idx = 0;
        uint32_t tmp_npc_id = 0;
        for(int i = 0; i < npc_score.npc_num; i++)
        {
             idx = uniform_rand(0, g_npc_num - i - 1);
             npc_score.npc[i].npc_id = g_npc_vec[idx];

             tmp_npc_id = g_npc_vec[idx];
             g_npc_vec[idx] = g_npc_vec[g_npc_num - i - 1];
             g_npc_vec[g_npc_num - i - 1] = tmp_npc_id;

             npc_score.npc[i].npc_score = 0;
        }

        send_npc_score_to_as(p_user, npc_score);
        p_user->user_cache.set_npc_score(&npc_score);

        return 0;
    }

    return 0;
}

/**
 * @brief process_npc_score 评分
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_npc_score(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        g_errno = ERR_GET_ROLE_CACHE;
        KCRIT_LOG(p_user->uid, "get role info failed.");
        return 0;
    }

    stat_one_param_t stat_data = {1};
    msg_log(stat_npc_score, &stat_data, sizeof(stat_data));

    uint32_t day_time = timestamp_begin_day();

    npc_score_info_t npc_score;
    p_user->user_cache.get_npc_score(&npc_score);
    if(npc_score.status != NPC_SCORE_NOT || p_role->role.npc_score_daytime == day_time)
    {
        g_errno = ERR_SCORED_TODAY;
        KCRIT_LOG(p_user->uid, "npc_score_status= %u not NPC_SCORE_NOT, or daytime:%u, cache_day_time:%u",npc_score.status, day_time, p_role->role.npc_score_daytime);
        return 0;
    }


    room_value_t *p_room = p_user->user_cache.get_default_room();

    get_stuff_npc_num(p_room, &npc_score);//返回后npc_score中的total_socre暂时为总的物品个数，npc.npc_score为相应NPC的物品个数
    KINFO_LOG(p_user->uid, "npc_score.total_score=%u", npc_score.total_score);

    int score_a[NPC_SCORE_NUM] = {0, 0, 0};
  //  int16_t stuff_num = ((user_room_buf_t *)p_room->buf)->stuff_num;
    if(npc_score.total_score != 0)
    {
        for(int i = 0; i < npc_score.npc_num; i++)
        {
            KINFO_LOG(p_user->uid, "npc:%u npc_score.npc_score=%u", npc_score.npc[i].npc_id,  npc_score.npc[i].npc_score);

            score_a[i] = ceil(1 + (float)(npc_score.npc[i].npc_score * 3) / (float)npc_score.total_score + (float)npc_score.total_score / 25);
            //score_a[i] = ceil((float)npc_score.npc[i].npc_score * 9 / (float)stuff_num + (float)stuff_num / 200);
	    if(score_a[i] > 12)
            {
                score_a[i] = 12;
            }
        }
    }

    uint32_t score_b = 0;
    score_b = ceil((float)p_role->role.thumb / ((float)p_role->role.visits) * npc_score.total_score);
    if(score_b > 10)
    {
	    score_b = 10;
    }

    uint32_t score_c = ceil((float)(p_role->role.monster_health  + p_role->role.monster_happy) / 999 + p_role->role.monster_level / 5);
    if(score_c > 8)
    {
        score_c = 8;
    }


    npc_score.total_score  = 0;
    for(int i = 0; i < npc_score.npc_num; i++)
    {
        KINFO_LOG(p_user->uid, "score_a[%d]:%u score_b:%u score_c:%u", i, score_a[i], score_b, score_c);
        npc_score.npc[i].npc_score = score_a[i] + score_b + score_c;
        npc_score.total_score += npc_score.npc[i].npc_score;
    }
    // npc_score.total_score  /= 3;

    if(npc_score.total_score > MAX_NPC_SCORE)
    {
        npc_score.total_score = npc_score.total_score % MAX_NPC_SCORE;
    }

    //为了记录6.6-6.28期间第一个达到27分的用户
//    if(npc_score.total_score >= 27)
//    {
//        KWARN_LOG(p_user->uid, "total_score:%u time:%u", npc_score.total_score, (uint32_t)time(NULL));
//    }

    p_user->user_cache.set_npc_score(&npc_score);

    svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_set_npc_score, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_set_npc_score_t));

    db_msg_set_npc_score_t *p_body = (db_msg_set_npc_score_t*)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->day_time = day_time;
    p_body->total_score = npc_score.total_score;
    p_body->npc_num = npc_score.npc_num;
    memcpy(p_body->npc, npc_score.npc, sizeof(npc_score.npc));

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;

    }

    return 0;
}


/**
 * @brief process_get_game_change 拉取小游戏的奖励兑换信息
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_get_game_change(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint32_t), 0);

    uint32_t game_id = 0;
    int idx = 0;
    taomee::unpack(p_msg_body, game_id, idx);


    if(g_game_change_period_map.find(game_id) == g_game_change_period_map.end())
    {
        g_errno = ERR_NO_THIS_GAME;
        KCRIT_LOG(p_user->uid, "game %u change stuff not exist", game_id);
        return -1;
    }

  //  stat_two_param_t stat_data = {p_user->uid, 1};
   // msg_log(stat_picture_matching + game_id - 1, &stat_data, sizeof(stat_data));

    char buffer[1024] = {0};
    db_msg_changed_stuff_t *p_changed_stuff = (db_msg_changed_stuff_t*)buffer;
    p_changed_stuff->game_id = game_id;
    if(p_user->user_cache.get_game_changed(p_changed_stuff) == 0)
    {//缓存中有
        KINFO_LOG(p_user->uid, "get game %u changed stuff info from cache, changed_num:%u", p_changed_stuff->game_id, p_changed_stuff->changed_stuff_num);
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_changed_stuff->game_id);
        g_send_msg.pack(p_changed_stuff->changed_stuff_num);
        for(uint16_t i = 0; i < p_changed_stuff->changed_stuff_num; i++)
        {
            g_send_msg.pack(p_changed_stuff->changed_stuff[i]);
        }
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }
    else
    {//缓存中没有，从db获取

        uint32_t period_id = g_game_change_period_map[game_id];
        KINFO_LOG(p_user->uid, "get game %u changed stuff info from db, period:%u", game_id, period_id);

        svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
        pack_svr_pkg_head(p_head, svr_msg_db_get_game_changed_stuff, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_get_changed_stuff_t));

        db_msg_get_changed_stuff_t *p_body = (db_msg_get_changed_stuff_t*)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->game_id = game_id;
        p_body->period_id = period_id;

        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy failed.");
            return 0;
        }
    }

    return 0;
}

/**
 * @brief process_game_change 小游戏兑换物品
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_game_change(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 3 * (int)sizeof(uint32_t) + (int)sizeof(uint16_t), 0);

    uint32_t game_id = 0;
    uint32_t stuff_id = 0;
    uint32_t game_score = 0;
    uint16_t level_num = 0;
    int idx = 0;
    taomee::unpack(p_msg_body, game_id, idx);
    taomee::unpack(p_msg_body, stuff_id, idx);
    taomee::unpack(p_msg_body, game_score, idx);
    taomee::unpack(p_msg_body, level_num, idx);


    map<uint32_t, map<uint32_t, uint8_t> >::iterator game_iter = g_game_change_stuff_map.find(game_id);
    if(game_iter == g_game_change_stuff_map.end())
    {
        g_errno = ERR_NO_THIS_GAME;
        KCRIT_LOG(p_user->uid, "set game %u change not exist", game_id);
        return -1;
    }

    map<uint32_t, uint8_t> reward_st_map = game_iter->second;
    map<uint32_t, uint8_t>::iterator rst_iter = reward_st_map.find(stuff_id);
    if(rst_iter == reward_st_map.end())
    {
        KCRIT_LOG(p_user->uid, "set game %u stuff %u not exist", game_id, stuff_id);
        return -1;
    }

    uint8_t stuff_repeat = rst_iter->second;

    int ret = p_user->user_cache.is_stuff_changed(game_id, stuff_id);
    if(ret == -1)
    {
        g_errno = ERR_SYSTEM_ERR;
        KCRIT_LOG(p_user->uid, "set_game_change: game:%u stuff:%u not cached.", game_id, stuff_id);
        return 0;
    }
    else if(ret == 1)
    {
        g_errno = ERR_STUFF_CHANGED;
        KCRIT_LOG(p_user->uid, "set_game_change: game:%u stuff:%u have changed.", game_id, stuff_id);
        return 0;
    }

    badge_step(p_user, badge_cake_game_const, game_score);

    //统计小游戏次数
    time_t now = time(NULL);
    if (p_user->stat_time > 0 && now >= p_user->stat_time)
    {
        stat_four_param_t stat_data = {p_user->uid, 1, now - p_user->stat_time, game_score};
        msg_log(stat_leave_picture_matching + game_id - 1, &stat_data, sizeof(stat_data));
        p_user->stat_time = 0;
    }

    //通知 db兑换物品
    uint32_t period_id = g_game_change_period_map[game_id];
    KINFO_LOG(p_user->uid, "set game %u changed stuff %u  repeat %u info to db, period:%u", game_id, stuff_id, stuff_repeat, period_id);

    svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_set_game_changed_stuff, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_get_changed_stuff_t));

    db_msg_get_changed_stuff_t *p_body = (db_msg_get_changed_stuff_t*)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->game_id = game_id;
    p_body->period_id = period_id;
    p_body->stuff_id = stuff_id;
    p_body->repeat = stuff_repeat;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    return 0;
}


/**
 * @brief process_get_finished_task 拉取已完成任务列表
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_get_finished_task(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, 0, 0);

    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_task_base, &stat_data, sizeof(stat_data));

    char buffer[4096] = {0};
    as_msg_finished_task_rsp_t *p_finished_task = (as_msg_finished_task_rsp_t*)buffer;
    if(p_user->user_cache.get_finished_task_list(p_finished_task) == 0)
    {//已做缓存，从缓存中取出发给as
        KINFO_LOG(p_user->uid, "Get finished_task_list from cache. finished_num:%u", p_finished_task->finished_task_num);
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_finished_task->finished_task_num);
        for(uint16_t i = 0; i < p_finished_task->finished_task_num; i++)
        {
            g_send_msg.pack(p_finished_task->task_id[i]);
        }
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }
    else
    {//完成任务列表未做缓存，从db获取
        KINFO_LOG(p_user->uid, "Get finished_task_list from db");

        svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
        pack_svr_pkg_head(p_head, svr_msg_db_get_finished_task, p_user->uid, GET_SVR_SN(p_user), 0);

        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy failed.");
            return 0;
        }

    }

    return 0;
}

/**
 * @brief process_finish_task 完成一个任务
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_finish_task(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, (int)sizeof(uint32_t), 0);

    uint32_t task_id = 0;
    int idx = 0;
    taomee::unpack(p_msg_body, task_id, idx);

    //判断是否满足接取这个任务的条件
    map<uint32_t, task_xml_t>::iterator iter = g_task_map.find(task_id);
    if(iter == g_task_map.end())
    {
        KCRIT_LOG(p_user->uid, "Not a valid task %u", task_id);
        return -1;
    }

    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_task_base + task_id - 100000, &stat_data, sizeof(stat_data));

    int ret = p_user->user_cache.task_need_reward(task_id, iter->second.lvl, iter->second.pre_mission,iter->second.reward_cycle);

    if(ret == -1)
    {//已完成任务列表未做缓存, 应该先请求上个协议
        KCRIT_LOG(p_user->uid, "invliad user send finish task(%u) cmd", task_id);
        return -1;
    }
    else if(ret == -2)
    {
        KCRIT_LOG(p_user->uid, "user finish task %u, level %u not reached", task_id, iter->second.lvl);
        g_errno = ERR_TASK_LEVEL;
        return 0;
    }
    else if(ret == -3)
    {
        KCRIT_LOG(p_user->uid, "user finish task %u, pre_mission %u not finished", task_id, iter->second.pre_mission);
        g_errno = ERR_TASK_PRE_MISSION;
        return 0;
    }
    else if(ret == 0)
    {//已经奖励过了，不需要奖励,直接给as回复
        KINFO_LOG(p_user->uid, "finish task %u, have rewarded or not reach finish condition", task_id);
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(task_id);
        g_send_msg.pack((uint32_t)0);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }
    else
    {//需要奖励,生成奖励，发给db

        svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
        db_msg_task_reward_t *p_body = (db_msg_task_reward_t*)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->task_id = task_id;
        p_body->level_up = 0;
        p_body->reward_time = time(NULL);

        p_body->reward_num = (uint8_t)iter->second.task_reward_vec.size();
        int idx = 0;
        for(; idx < p_body->reward_num; idx++)
        {
            p_body->reward[idx].reward_id = iter->second.task_reward_vec[idx].reward_id;
            if(p_body->reward[idx].reward_id == TASK_REWARD_COIN)
            {
                p_body->reward[idx].reward_num = get_current_gains(iter->second.task_reward_vec[idx].reward_num, p_user);
            }
            else if(p_body->reward[idx].reward_id == TASK_REWARD_EXP)
            {
                p_body->reward[idx].reward_num = get_current_gains(iter->second.task_reward_vec[idx].reward_num, p_user);
                role_cache_t *p_role = p_user->user_cache.get_role();
                if(p_role == NULL)
                {
                        KCRIT_LOG(p_user->uid, "It should not come here, p_role is NULL");
                }
                else
                {
                        int now_level = get_level_from_exp(p_role->role.monster_exp + p_body->reward[idx].reward_num);
                        if(now_level > p_role->role.monster_level && p_role->role.monster_happy > LEVEL_HAPPY_VALUE
                                && p_role->role.monster_health > LEVEL_HEALTH_VALUE)
                        {//可以升级
                            p_body->level_up = now_level - p_role->role.monster_level;
                        }
                        else
                        {//不可以升级,将经验值加满
                            uint16_t max_left_exp = get_level_left_exp(p_role->role.monster_exp, p_role->role.monster_level);
                            if(max_left_exp < p_body->reward[idx].reward_num)
                            {
                                p_body->reward[idx].reward_num = max_left_exp;
                            }
                            p_body->level_up = 0;
                        }

                        KINFO_LOG(p_user->uid, "finish task: reward_exp:%u", p_body->reward[idx].reward_num);
                }
            }
            else
            {
                p_body->reward[idx].reward_num = iter->second.task_reward_vec[idx].reward_num;
            }

        }

        KINFO_LOG(p_user->uid, "finish task %u, send reward to db, level_up:%u, reward_num:%u", task_id, p_body->level_up, idx);

        pack_svr_pkg_head(p_head, svr_msg_db_finish_task, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_task_reward_t) + idx * sizeof(task_reward_t));

        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy failed.");
            return 0;
        }

    }

    return 0;
}


int process_enter_museum(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, sizeof(uint32_t), 0);

	int idx = 0;
	uint32_t museum_id = 0;
	taomee::unpack(p_msg_body, museum_id, idx);


	if(g_museum_map.find(museum_id) == g_museum_map.end())
	{
		KINFO_LOG(p_user->uid, "museum id %u  not exist!", museum_id);
		return -1;
	}

    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_museum_base + museum_id, &stat_data, sizeof(stat_data));


	museum_info_t *p_museum = p_user->user_cache.get_museum_item(museum_id);
	if(p_museum == NULL)
	{
		KINFO_LOG(p_user->uid, "enter_museum, get museum  %u info from db", museum_id);
		svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
   	 	pack_svr_pkg_head(p_head, svr_msg_db_enter_museum, p_user->uid, GET_SVR_SN(p_user), sizeof(uint32_t));
		uint32_t *p_body = (uint32_t *)(g_send_buffer + sizeof(svr_msg_header_t));
		*p_body = museum_id;

    		if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
   		{
        		g_errno = ERR_MSG_DB_PROXY;
			KCRIT_LOG(p_user->uid, "send to db proxy failed.");
    		}
	}
	else
	{
		KINFO_LOG(p_user->uid, "enter_museum, get museum  %u info from cache", museum_id);

		uint32_t cur_time = timestamp2day(time(NULL));
		if(cur_time > p_museum->timestamp)
		{
			p_museum->timestamp = cur_time;
			p_museum->level_id = 0;
			p_museum->reward_flag = HAVE_GET_REWARD;
			p_museum->next_level_id= 1;
		}

		pack_as_pkg_header(p_user->uid,p_user->waitcmd,0,ERR_NO_ERR);
		g_send_msg.pack(museum_id);
		g_send_msg.pack(p_museum->reward_flag);
		uint8_t level_id = p_museum->reward_flag == HAVENOT_GET_REWARD?p_museum->level_id:p_museum->next_level_id;
		g_send_msg.pack(level_id);
		g_send_msg.end();
		send_data_to_user(p_user,g_send_msg.get_msg(),g_send_msg.get_msg_len());


	}
    return 0;
}


int process_commit_museum_game(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, sizeof(as_msg_get_museum_req_t), 0);

    int idx = 0;
    as_msg_get_museum_req_t museum_req;
	taomee::unpack(p_msg_body, museum_req.museum_id, idx);
	taomee::unpack(p_msg_body, museum_req.level_id, idx);

	map<uint32_t, uint8_t>::iterator iter = g_museum_map.find(museum_req.museum_id);
	if(iter == g_museum_map.end())
	{
		KCRIT_LOG(p_user->uid, "museum ID %u is not exist.", museum_req.museum_id);
		return -1;
	}
	museum_info_t *museum_item = p_user->user_cache.get_museum_item(museum_req.museum_id);
	if(museum_item == NULL)
	{
		 KCRIT_LOG(p_user->uid, "museum cache is NULL");
            return -1;
	}
	if(museum_req.level_id != museum_item->next_level_id)
	{
		KCRIT_LOG(p_user->uid, "museum_id:%u request level_id:%u != cache_next_level:%u", museum_req.museum_id, museum_req.level_id, museum_item->next_level_id);
       	return -1;
	}

	if(museum_item->level_id < museum_item->next_level_id)
	{
		svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
   		pack_svr_pkg_head(p_head,svr_msg_db_commit_museum_game,p_user->uid,GET_SVR_SN(p_user),sizeof(db_request_commit_museum_t));

		db_request_commit_museum_t *p_body = (db_request_commit_museum_t *)(g_send_buffer + sizeof(svr_msg_header_t));
        	p_body->museum_id = museum_req.museum_id;
      	p_body->level_id = museum_req.level_id;
		p_body->timestamp = museum_item->timestamp;

		if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    		{
        		g_errno = ERR_MSG_DB_PROXY;
        		KCRIT_LOG(p_user->uid, "send to db proxy failed.");
    		}
	}
	else
	{
		KINFO_LOG(p_user->uid, "repeat commit museum %u ", museum_req.museum_id);
		pack_as_pkg_header(p_user->uid,p_user->waitcmd,0,ERR_NO_ERR);
		g_send_msg.pack((uint8_t)HAVE_GET_REWARD);
		g_send_msg.pack(museum_item->next_level_id);
		g_send_msg.end();

		send_data_to_user(p_user,g_send_msg.get_msg(),g_send_msg.get_msg_len());
	}

    return 0;
}

int process_get_museum_reward(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, sizeof(as_msg_get_museum_req_t), 0);

    int idx = 0;
    as_msg_get_museum_req_t museum_req;
	taomee::unpack(p_msg_body, museum_req.museum_id, idx);
	taomee::unpack(p_msg_body, museum_req.level_id, idx);

    KINFO_LOG(p_user->uid, "museum level info, museum_id:%u, level_id:%u", museum_req.museum_id, museum_req.level_id);

	museum_info_t *museum_item = p_user->user_cache.get_museum_item(museum_req.museum_id);
	if(museum_item == NULL)
	{
		KCRIT_LOG(p_user->uid,"get museum_reward,museum cache is NULL!  not invalid museum_id:%u level_id:%u", museum_req.museum_id, museum_req.level_id);
        return -1;
	}


    if(museum_item->level_id != museum_req.level_id)
    {
		KCRIT_LOG(p_user->uid,"get museum_reward, level id not consisten, cache_level_id:%u req museum_id:%u level_id:%u", museum_item->level_id, museum_req.museum_id, museum_req.level_id);
        return -1;
    }

	if(museum_item->reward_flag == HAVE_GET_REWARD)
	{
		KINFO_LOG(p_user->uid,"get museum_reward, req museum_id:%u level_id:%u have get reward repeat request", museum_req.museum_id, museum_req.level_id);
		pack_as_pkg_header(p_user->uid,p_user->waitcmd,0,ERR_NO_ERR);
		g_send_msg.pack((uint32_t)0);
		g_send_msg.pack((uint16_t)0);
		g_send_msg.pack((uint8_t)(museum_item->next_level_id));
		g_send_msg.end();

		send_data_to_user(p_user,g_send_msg.get_msg(),g_send_msg.get_msg_len());
       	return 0;
	}

    uint32_t key = museum_req.museum_id * 100 + museum_req.level_id;
    map<uint32_t, level_item_t>::iterator iter = g_museum_level_map.find(key);
    if(iter == g_museum_level_map.end())
    {
		KCRIT_LOG(p_user->uid,"get museum_reward, req museum_id:%u level_id:%u reward_key not found.", museum_req.museum_id, museum_req.level_id);
       	return -1;
    }

    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_museum_reward_base + museum_req.museum_id, &stat_data, sizeof(stat_data));

	svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_get_museum_reward, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_add_museum_reward_t));

    db_msg_add_museum_reward_t *p_body = (db_msg_add_museum_reward_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->museum_id = museum_req.museum_id;
    p_body->level_id = museum_req.level_id;
	p_body->timestamp= museum_item->timestamp;
    p_body->reward_id = iter->second.reward_id;
    p_body->reward_num = iter->second.reward_num;

    KINFO_LOG(p_user->uid, "send museum reward to db:(id:%u level:%u reward_id:%u reward_num:%u)", p_body->museum_id, p_body->level_id, p_body->reward_id, p_body->reward_num);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    return 0;
}


int process_commit_game_change_score(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(p_user->uid, msg_body_len, sizeof(as_msg_cake_game_t), 0);

    int idx = 0;
	as_msg_cake_game_t req;
	taomee::unpack(p_msg_body, req.game_id, idx);
	taomee::unpack(p_msg_body, req.level_id, idx);
	taomee::unpack(p_msg_body, req.score, idx);

    badge_step(p_user, badge_cake_game_const, req.score);

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
		if(req.level_id >= 5)
		{
		    svr_msg_header_t *p_head = (svr_msg_header_t*)g_send_buffer;
			uint8_t *req = (uint8_t *)(g_send_buffer + sizeof(svr_msg_header_t));
			*req = 3;
		    pack_svr_pkg_head(p_head, svr_msg_db_update_dragon_boat, p_user->uid, GET_SVR_SN(p_user), sizeof(uint8_t));
		    if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
		    {
		        g_errno = ERR_MSG_DB_PROXY;
		        KCRIT_LOG(p_user->uid, "send to db proxy failed");
		    }
		}
		else
		{
			pack_as_pkg_header(p_user->uid,p_user->waitcmd,0,ERR_NO_ERR);
			g_send_msg.pack((uint8_t)1);
			g_send_msg.end();
			send_data_to_user(p_user,g_send_msg.get_msg(),g_send_msg.get_msg_len());
		}
		return 0;
	}

    p_user->waitcmd = 0;
    p_user->inc_counter();
    return 0;
}

int process_reload_conf(fdsession_t *fdsess, char *p_msg_body, uint16_t msg_body_len)
{
    assert(fdsess != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, (int)sizeof(uint32_t), 0);


    uint32_t remote_ip = fdsess->remote_ip;
    char ipchar[16] = {0};
    if(inet_ntop(AF_INET, &remote_ip, ipchar, 16) == NULL)
    {
        KCRIT_LOG(0, "inet_ntop %u failed(%s).", remote_ip, strerror(errno));
        return 0;
    }

    if(strcmp(g_allowed_ip, ipchar) != 0)
    {
        KCRIT_LOG(0, "Not allow ip %s send this msg, allowed_ip %s", ipchar, g_allowed_ip);
        return 0;
    }

    int idx = 0;
    uint32_t conf_id = 0;

	taomee::unpack(p_msg_body, conf_id, idx);

    if(conf_id >=0 && conf_id < MAX_CONF_FILE_NUM)
    {
    if(g_conf_parse_func[conf_id])
    {
        KINFO_LOG(0, "reload conf_id %u", conf_id);
        g_conf_parse_func[conf_id]();
    }
    else
    {
        KCRIT_LOG(0, "conf_id %u 's func is NULL", conf_id);
    }
    }
    else
    {
        KCRIT_LOG(0, "illegal conf_id %u", conf_id);
    }

    return 0;
}

int process_history_donate(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, 0, 0);

    KINFO_LOG(p_user->uid, "Get history donate info");
    svr_msg_header_t *p_head = (svr_msg_header_t*)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_request_history_donate, p_user->uid, GET_SVR_SN(p_user), 0);
    if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed");
        return 0;
    }

    return 0;
}

int process_cur_donate_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)//重写捐助计划的三个协议
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, 0, 0);

    KINFO_LOG(p_user->uid, "Get cur donate info");
    svr_msg_header_t *p_head = (svr_msg_header_t*)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_request_cur_donate, p_user->uid, GET_SVR_SN(p_user), 0);
    if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed");
        return 0;
    }

    return 0;
}

int process_donate_coins(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)//重写捐助计划的三个协议
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, 2*(int)sizeof(uint32_t), 0);

    uint32_t donate_id = 0;
    uint32_t donate_num = 0;
    int idx = 0;
    taomee::unpack(p_msg_body, donate_id, idx);
    taomee::unpack(p_msg_body, donate_num, idx);

    map<uint32_t, donate_info_t>::iterator iter = g_donate_map.find(donate_id);
    if(iter == g_donate_map.end())
    {
        KCRIT_LOG(p_user->uid, "donate_id:%u not exist", donate_id);
        return -1;
    }

    if(donate_num != 10 && donate_num != 50 && donate_num != 100)
    {
        KCRIT_LOG(p_user->uid, "donate_id %u donate_num %u not in (10, 50, 100)", donate_id, donate_num);
        return -1;
    }

    role_cache_t *p_role = p_user->user_cache.get_role();
    if(NULL == p_role)
    {
        g_errno = ERR_GET_ROLE_CACHE;
        KCRIT_LOG(p_user->uid, "donate_id %u donate_num %u get role info failed.", donate_id, donate_num);
        return 0;
    }

    if(p_role->role.coins < donate_num)
    {
        KINFO_LOG(p_user->uid, "coins not enough, owned_coins:%u donate_num:%u donate_id:%u", p_role->role.coins, donate_num, donate_id);
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(donate_id);
        g_send_msg.pack((uint8_t)DENOTE_COINS_NOT_ENOUGH);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }
    else
    {
        KINFO_LOG(p_user->uid, "send donate info to db, donate_id:%u, donate_num:%u", donate_id, donate_num);
        svr_msg_header_t *p_head = (svr_msg_header_t*)g_send_buffer;
        pack_svr_pkg_head(p_head, svr_msg_db_request_donate_coins, p_user->uid, GET_SVR_SN(p_user), sizeof(db_request_donate_coins_t));

        db_request_donate_coins_t *p_req = (db_request_donate_coins_t*)(g_send_buffer + sizeof(svr_msg_header_t));
        p_req->req_donate_id = donate_id;
        p_req->req_donate_coins = donate_num;
        p_req->req_donate_timestamp = time(NULL);
        p_req->donate_info.donate_total = iter->second.donate_total;
        p_req->donate_info.helped_pet = iter->second.helped_pet;
        p_req->donate_info.donate_limit = iter->second.donate_limit;
        p_req->donate_info.reward_id = iter->second.reward_id;

        if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy failed");
            return 0;
        }

    }

    return 0;
}



int process_get_a_activity_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, (int)sizeof(uint32_t), 0);

    uint32_t activity_id = 0;
    int idx = 0;
    taomee::unpack(p_msg_body, activity_id, idx);

    map<uint32_t, activity_info_t>::iterator iter = g_activity_map.find(activity_id);
    if(iter == g_activity_map.end())
    {//请求一个不存在的活动
        KCRIT_LOG(p_user->uid, "activity_id:%u not exist", activity_id);
        return -1;
    }

    uint32_t now_timestamp = (uint32_t)time(NULL);

    if(iter->second.start_timestamp <= now_timestamp && iter->second.end_timestamp >= now_timestamp)
    {
        KINFO_LOG(p_user->uid, "get activity %u info", activity_id);
        get_a_activity_info(p_user, activity_id);
    }
    else
    {//请求的不是当前时间段内的活动
        g_errno = ERR_NOT_CURRENT_ACTIVITY;
        KCRIT_LOG(p_user->uid, "request activity %u is not in current time", activity_id);
        return 0;
    }


    return 0;
}

int process_get_activity_reward(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, (int)sizeof(as_request_activity_reward_t), 0);

    as_request_activity_reward_t req;
    int idx = 0;
    taomee::unpack(p_msg_body, req.activity_id, idx);
    taomee::unpack(p_msg_body, req.reward_id, idx);
    map<uint32_t, activity_info_t>::iterator iter = g_activity_map.find(req.activity_id);
    if(iter == g_activity_map.end())
    {//请求一个不存在的活动
        KCRIT_LOG(p_user->uid, "get reward activity_id:%u not exist", req.activity_id);
        return -1;
    }

    uint32_t now_timestamp = (uint32_t)time(NULL);

    if(iter->second.start_timestamp <= now_timestamp && iter->second.end_timestamp >= now_timestamp)
    {//是当前活动
        get_act_reward(p_user, req);
    }
    else
    {//请求的不是当前时间段内的活动
        g_errno = ERR_NOT_CURRENT_ACTIVITY;
        KCRIT_LOG(p_user->uid, "request activity %u is not in current time", req.activity_id);
        return 0;
    }


    return 0;
}

int process_get_activity_reward_ex(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, (int)sizeof(db_request_get_activity_reward_ex_t), 0);

    db_request_get_activity_reward_ex_t req;
    int idx = 0;
    taomee::unpack(p_msg_body, req.activity_id, idx);
    taomee::unpack(p_msg_body, req.level_id, idx);
	taomee::unpack(p_msg_body, req.reward_id, idx);
	taomee::unpack(p_msg_body, req.reward_num, idx);

    map<uint32_t, activity_info_t>::iterator iter = g_activity_map.find(req.activity_id);
    if(iter == g_activity_map.end())
    {//请求一个不存在的活动
        KCRIT_LOG(p_user->uid, "get reward activity_id:%u not exist", req.activity_id);
        return -1;
    }

    uint32_t now_timestamp = (uint32_t)time(NULL);

    if(iter->second.start_timestamp <= now_timestamp && iter->second.end_timestamp >= now_timestamp)
    {//是当前活动
		KINFO_LOG(p_user->uid, "get active reward ex");

		switch(req.activity_id)
		{
			case INVITOR_ACTIVITY_ID:
			{
				if(req.level_id == 1)
				{
					stat_one_param_t stat_data = {p_user->uid};
					msg_log(stat_invitor_first, &stat_data, sizeof(stat_data));
				}
				if(req.level_id == 2)
				{
					stat_one_param_t stat_data = {p_user->uid};
					msg_log(stat_invitor_second, &stat_data, sizeof(stat_data));
				}
				if(req.level_id == 3)
				{
					stat_one_param_t stat_data = {p_user->uid};
					msg_log(stat_invitor_third, &stat_data, sizeof(stat_data));
				}
			break;
			}

		}
		svr_msg_header_t *p_head = (svr_msg_header_t*)g_send_buffer;
		pack_svr_pkg_head(p_head, svr_msg_db_get_activity_reward_ex, p_user->uid, GET_SVR_SN(p_user), sizeof(db_request_get_activity_reward_ex_t));

		db_request_get_activity_reward_ex_t *db_req = (db_request_get_activity_reward_ex_t *)(g_send_buffer + sizeof(svr_msg_header_t));
		*db_req = req;

		if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
		{
			g_errno = ERR_MSG_DB_PROXY;
			KCRIT_LOG(p_user->uid, "send to db proxy failed");
			return 0;
		}
    }
    else
    {//请求的不是当前时间段内的活动
        g_errno = ERR_NOT_CURRENT_ACTIVITY;
        KCRIT_LOG(p_user->uid, "request activity %u is not in current time", req.activity_id);
        return 0;
    }
    return 0;
}


/**
 * @brief process_get_activity_list 拉取当前的活动列表(目前只从配表中读取，没有对活动的状态进行判断)
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_get_activity_list(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, 0, 0);

    char act_buffer[1024] = {0};
    activity_list_t *p_act_list = (activity_list_t*)act_buffer;

    get_cur_activity_from_conf(p_act_list);

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_act_list->activity_num);
    for(uint16_t idx = 0; idx < p_act_list->activity_num; idx++)
    {
        g_send_msg.pack(p_act_list->activity[idx].activity_id);
        g_send_msg.pack(p_act_list->activity[idx].activity_status);
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());


    return 0;
}


int process_prize_lottery(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, 2*sizeof(uint32_t), 0);

    KINFO_LOG(p_user->uid, "Process prize lottery");

	int idx = 0;
	uint32_t stuff_a_id, stuff_b_id;

	taomee::unpack(p_msg_body, stuff_a_id, idx);
	taomee::unpack(p_msg_body, stuff_b_id, idx);
	uint16_t stuff_a_num = p_user->user_cache.get_stuff_num(stuff_a_id);

	map<uint32_t, item_t>::iterator iter = g_item_map.find(stuff_b_id);
	if(iter == g_item_map.end())
	{
		g_errno = ERR_STUFF_NOT_EXIST;
		KCRIT_LOG(p_user->uid, "this stuff in prize shop isn't exist!");
		return 0;
	}

	if(stuff_a_num < iter->second.price)
	{
	    g_errno = ERR_BUY_STUFF;
        KCRIT_LOG(p_user->uid, "can't buy this stuff:%u.", stuff_b_id);
	}
	else
	{
		 svr_msg_header_t *p_head = (svr_msg_header_t*)g_send_buffer;
		 pack_svr_pkg_head(p_head, svr_msg_db_request_prize_lottery, p_user->uid, GET_SVR_SN(p_user), sizeof(db_request_prize_lottery_t));
		 db_request_prize_lottery_t *req = (db_request_prize_lottery_t *)(g_send_buffer + sizeof(svr_msg_header_t));
		 req->stuff_a_id = stuff_a_id;
 		 req->cost = iter->second.price;
 		 req->stuff_b_id = stuff_b_id;
		 req->category = iter->second.category;

		 if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
		 {
		     g_errno = ERR_MSG_DB_PROXY;
		     KCRIT_LOG(p_user->uid, "send to db proxy failed");
		 }
	}
    return 0;
}


int process_enter_show(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, 0, 0);

    role_cache_t *p_role = p_user->user_cache.get_role();
    if(p_role == NULL)
    {
        g_errno = ERR_GET_ROLE_CACHE;
        KCRIT_LOG(p_user->uid, "get role info failed.");
        return 0;
    }

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_enter_show, p_user->uid, GET_SVR_SN(p_user), sizeof(uint32_t));
    uint32_t *p_body = (uint32_t*)(g_send_buffer + sizeof(svr_msg_header_t));
    *p_body = p_role->role.last_show_id;

    KINFO_LOG(p_user->uid, "enter show,  last_show_id:%u", *p_body);
    if(g_p_db_cache_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db cache  server failed");
        return -1;
    }

    return 0;
}

int process_join_show(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, sizeof(uint32_t), 0);

    int idx = 0;
    uint32_t show_id = 0;
    taomee::unpack(p_msg_body, show_id, idx);

    KINFO_LOG(p_user->uid, "Process join show %u", show_id);
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        g_errno = ERR_GET_ROLE_CACHE;
        KCRIT_LOG(p_user->uid, "get role info failed in join show.");
        return 0;
    }
    stat_one_param_t stat_data = {p_user->uid};
    msg_log(stat_join_num, &stat_data, sizeof(stat_data));

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_join_show, p_user->uid, GET_SVR_SN(p_user), sizeof(online_msg_join_show_t));
    online_msg_join_show_t *p_omjs = (online_msg_join_show_t*)(g_send_buffer + sizeof(svr_msg_header_t));
    p_omjs->show_id = show_id;
    p_omjs->user_info.user_id = p_user->uid;
    memcpy(p_omjs->user_info.user_name, p_role->role.name, 16);
    p_omjs->user_info.mon_id = p_role->role.monster_id;
    p_omjs->user_info.mon_main_color = p_role->role.monster_main_color;
    p_omjs->user_info.mon_exp_color = p_role->role.monster_ex_color;
    p_omjs->user_info.mon_eye_color = p_role->role.monster_eye_color;
    p_omjs->user_info.npc_score = p_role->role.npc_score;
    p_omjs->user_info.npc_timestamp = p_role->role.npc_score_daytime;
    p_omjs->user_info.votes = 0;
    p_omjs->user_info.is_top = 0;
    p_omjs->user_info.is_notice = 0;//未通知
    if(g_p_db_cache_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db cache failed in join show");
        return -1;
    }

    return 0;
}

int process_guess_show(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, sizeof(msg_vote_show_t), 0);


    stat_one_param_t stat_data = {p_user->uid};
    msg_log(stat_vote_num, &stat_data, sizeof(stat_data));

    int idx = 0;
    msg_vote_show_t *req = (msg_vote_show_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    taomee::unpack(p_msg_body, req->show_id, idx);
    taomee::unpack(p_msg_body, req->peer_id, idx);

    KINFO_LOG(p_user->uid, "Process vote show %u candidate %u", req->show_id, req->peer_id);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_vote_show, p_user->uid, GET_SVR_SN(p_user), sizeof(msg_vote_show_t));
    if(g_p_db_cache_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed");
        return -1;
    }

    return 0;
}


int process_history_show(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, sizeof(uint32_t), 0);

    int idx = 0;
    uint32_t show_id = 0;
    taomee::unpack(p_msg_body, show_id, idx);

    KINFO_LOG(p_user->uid, "Process history show %u", show_id);

    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_history_show, p_user->uid, GET_SVR_SN(p_user), sizeof(uint32_t));
    uint32_t *p_show_id = (uint32_t*)(g_send_buffer + sizeof(svr_msg_header_t));
    *p_show_id = show_id;
    if(g_p_db_cache_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed");
        return -1;
    }


    return 0;
}
