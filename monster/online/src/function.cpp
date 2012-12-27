/**
 * =====================================================================================
 *       @file  function.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/21/2011 01:47:48 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <assert.h>
#include <map>
#include <vector>
#include <set>
#include <stdio.h>
#include <math.h>

#include <libtaomee++/inet/pdumanip.hpp>
#include "message.h"
#include "stat.h"

#include "function.h"

using std::map;
using std::vector;
using std::pair;
using std::set;

void pack_as_pkg_header(uint32_t user_id, uint16_t msg_type, uint32_t result, uint32_t err_no)
{
    g_send_msg.begin(SEND_TO_AS);
    g_send_msg.pack(msg_type);
    g_send_msg.pack(user_id);
    g_send_msg.pack(result);
    g_send_msg.pack(err_no);
}



void send_header_to_user(usr_info_t *p_user, uint32_t result, uint32_t err_no, bool clear_waitcmd)
{
    if (NULL == p_user)
    {
        return;
    }
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, result, err_no);
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), clear_waitcmd);
}

void send_data_to_user(usr_info_t *p_user, const char *p_buf, uint32_t buf_len, bool clear_waitcmd)
{
    if (NULL == p_user)
    {
        return;
    }
    if (clear_waitcmd)
    {
        p_user->waitcmd = 0;
        p_user->inc_counter();
    }

    int ret = send_pkg_to_client(p_user->session, p_buf, buf_len);
    if (ret != 0)
    {
        close_client_conn(p_user->session->fd);
    }
}

void pack_svr_pkg_head(svr_msg_header_t *p_head, uint16_t msg_type, uint32_t user_id, uint32_t seq_id, uint32_t body_len)
{
    p_head->len = body_len + sizeof(svr_msg_header_t);
    p_head->seq_id = seq_id;
    p_head->user_id = user_id;
    p_head->msg_type = msg_type;
    p_head->result = 0;
}

int timeout_cb(void *p_owner, void *p_data)
{
    assert(p_owner != NULL);
    usr_info_t *p_user  = (usr_info_t *)p_owner;

    send_header_to_user(p_user, SYSTEM_ERR_NUM, ERR_MSG_TIMEOUT);

    return 0;
}

int register_online()
{
    DEBUG_LOG("pack register online data");
    g_send_msg.begin(SEND_TO_SVR);
    g_send_msg.pack((uint32_t)0);
    g_send_msg.pack((uint16_t)svr_msg_online_register);
    g_send_msg.pack((uint32_t)0);
    g_send_msg.pack((uint32_t)0);
    g_send_msg.pack((uint32_t)config_cache.bc_elem->online_id);   // online_id
    g_send_msg.pack(config_cache.bc_elem->bind_ip, sizeof(config_cache.bc_elem->bind_ip));   // online_ip
    g_send_msg.pack((uint16_t)config_cache.bc_elem->bind_port);   // online_port
    g_send_msg.pack((uint8_t)g_tel_or_net);
    g_send_msg.end();

    return g_p_switch_svr->send_data(NULL, g_send_msg.get_msg(), g_send_msg.get_msg_len(), NULL);
}

int exit_online()
{
    DEBUG_LOG("pack online exit data");
    g_send_msg.begin(SEND_TO_SVR);
    g_send_msg.pack((uint32_t)0);
    g_send_msg.pack((uint16_t)svr_msg_online_exit);
    g_send_msg.pack((uint32_t)0);
    g_send_msg.pack((uint32_t)0);
    g_send_msg.end();

    g_p_switch_svr->send_data(NULL, g_send_msg.get_msg(), g_send_msg.get_msg_len(), NULL);
    g_p_multi_server_svr->send_data(NULL, g_send_msg.get_msg(), g_send_msg.get_msg_len(), NULL);
return 0;
}

int register_to_switch(void *owner, void *p_data)
{
    if(g_p_switch_svr->get_server_fd() == -1)
    {
        DEBUG_LOG("pack register online data to switch");
        g_send_msg.begin(SEND_TO_SVR);
        g_send_msg.pack((uint32_t)0);
        g_send_msg.pack((uint16_t)svr_msg_online_register);
        g_send_msg.pack((uint32_t)0);
        g_send_msg.pack((uint32_t)0);
        g_send_msg.pack((uint32_t)config_cache.bc_elem->online_id);   // online_id
        g_send_msg.pack(config_cache.bc_elem->bind_ip, sizeof(config_cache.bc_elem->bind_ip));   // online_ip
        g_send_msg.pack((uint16_t)config_cache.bc_elem->bind_port);   // online_port
        g_send_msg.pack((uint8_t)g_tel_or_net);
        g_send_msg.end();
        g_p_switch_svr->send_data(NULL, g_send_msg.get_msg(), g_send_msg.get_msg_len(), NULL);
        DEBUG_LOG("register online to switch,id = %u,ip:%s,port:%u tel_or_net:%u", config_cache.bc_elem->online_id, config_cache.bc_elem->bind_ip, config_cache.bc_elem->bind_port, g_tel_or_net);
    }
    ADD_TIMER_EVENT(&g_register_to_multi_timer, register_to_switch, (void*)NULL, get_now_tv()->tv_sec + 10);
return 0;
}

int register_to_multi(void *owner, void *p_data)
{
    if(g_p_multi_server_svr->get_server_fd() == -1)
    {
        DEBUG_LOG("pack register online data to multi");
        g_send_msg.begin(SEND_TO_SVR);
        g_send_msg.pack((uint32_t)0);
        g_send_msg.pack((uint16_t)svr_msg_online_register);
        g_send_msg.pack((uint32_t)0);
        g_send_msg.pack((uint32_t)0);
        g_send_msg.pack((uint32_t)config_cache.bc_elem->online_id);   // online_id
        g_send_msg.pack(config_cache.bc_elem->bind_ip, sizeof(config_cache.bc_elem->bind_ip));   // online_ip
        g_send_msg.pack((uint16_t)config_cache.bc_elem->bind_port);   // online_port
        g_send_msg.end();
        g_p_multi_server_svr->send_data(NULL, g_send_msg.get_msg(), g_send_msg.get_msg_len(), NULL);
        DEBUG_LOG("register online to multi,id = %u,ip:%s,port:%u", config_cache.bc_elem->online_id, config_cache.bc_elem->bind_ip, config_cache.bc_elem->bind_port);
    }
    ADD_TIMER_EVENT(&g_register_to_multi_timer, register_to_multi, (void*)NULL, get_now_tv()->tv_sec + 10);
	return 0;
}

int keepalive_check(void *owner, void *p_data)
{
        DEBUG_LOG("pack keepalive msg");
        g_send_msg.begin(SEND_TO_SVR);
        g_send_msg.pack((uint32_t)0);
        g_send_msg.pack((uint16_t)svr_msg_online_keepalive);
        g_send_msg.pack((uint32_t)0);
        g_send_msg.pack((uint32_t)0);
        g_send_msg.end();
        g_p_switch_svr->send_data(NULL, g_send_msg.get_msg(), g_send_msg.get_msg_len(), NULL);
        g_p_multi_server_svr->send_data(NULL, g_send_msg.get_msg(), g_send_msg.get_msg_len(), NULL);
        ADD_TIMER_EVENT(&g_keepalive_timer, keepalive_check, (void*)NULL, get_now_tv()->tv_sec + 30);
	return 0;
}


uint32_t get_new_happy(uint32_t now_time, uint32_t last_time, uint32_t last_happy)
{
    //INFO_LOG("now:%u, last:%u, last happy:%u", now_time, last_time, last_happy);
    int past_time = now_time - last_time;
    if (past_time < 0)
    {
        CRIT_LOG("it cann't be here");
        past_time = 0;
    }
    uint32_t happy_desc = past_time / (g_happy_desc_time * 60);
    uint32_t now_happy = happy_desc >= last_happy ? 0 : last_happy - happy_desc;
    if (now_happy > MAX_HAPPY_VALUE)
    {
        now_happy = MAX_HAPPY_VALUE;
    }

    return now_happy;
}

int get_monster_health_on_login(uint32_t now, uint32_t last_logout_time, int health)
{
    if (last_logout_time != 0)
    {
        health = health - (now - last_logout_time) / (g_health_desc_offline * 60);
    }
#ifdef DEBUG
    INFO_LOG("now:%u, last logout time:%u, last health:%u, now:%u", now, last_logout_time, health, health < HEALTH_MIN ? HEALTH_MIN : health);
#endif

    if (health < HEALTH_MIN)
    {
        health = HEALTH_MIN;
    }
    if (health > MAX_HEALTH_VALUE)
    {
        health = MAX_HEALTH_VALUE;
    }

    return health;
}

/**
 * @brief 计算现在实际的健康值
 *
 * @return 无
 */
int get_now_health(int health, uint32_t last_update_health_time, uint32_t now)
{
    uint32_t interval = now - last_update_health_time;
    if (health <= HEALTH_LOW)
    {
        health = health - interval / (g_health_desc_slow * 60);
    }
    else if (health <= HEALTH_MIDDLE)
    {
        int temp_health = health -  interval / (g_health_desc_middle * 60);
        if (temp_health >= HEALTH_LOW)
        {
            health = temp_health;
        }
        else
        {
            uint32_t left_time = interval - (health - HEALTH_LOW) / (g_health_desc_middle * 60);
            health = HEALTH_LOW - left_time / 60;
        }
    }
    else
    {
        int temp_health = health - interval / (g_health_desc_high * 60);
        if (temp_health >= HEALTH_MIDDLE)
        {
            health = temp_health;
        }
        else
        {
            uint32_t left_time = interval - (health - HEALTH_MIDDLE) / (g_health_desc_high * 60);
            health = HEALTH_MIDDLE - left_time / (60 * g_health_desc_middle);
            if (health < HEALTH_LOW)
            {
                left_time = left_time - (HEALTH_MIDDLE - HEALTH_LOW) / (60 * g_health_desc_middle);
                health = health - left_time / (60 * g_health_desc_slow);
            }
        }
    }
    if (health < HEALTH_MIN)
    {
        health = HEALTH_MIN;
    }
    if (health > MAX_HEALTH_VALUE)
    {
        health = MAX_HEALTH_VALUE;
    }

    return health;
}

int get_other_monster_health(uint32_t now, uint32_t last_logout_time, uint32_t last_login_time, int last_health)
{
    if (last_logout_time > last_login_time)
    {
        last_health = last_health - (now - last_logout_time) / (g_health_desc_offline * 60);
    }
    else
    {
        if (last_logout_time != 0)
        {
            last_health = last_health - (last_login_time - last_logout_time) / (g_health_desc_offline * 60);
        }

        last_health = get_now_health(last_health, last_login_time, now);
    }

    if (last_health < HEALTH_MIN)
    {
        last_health = HEALTH_MIN;
    }
    if (last_health > MAX_HEALTH_VALUE)
    {
        last_health = MAX_HEALTH_VALUE;
    }

    return last_health;
}


uint32_t get_plant_growth_add(uint32_t now_happy, uint32_t last_growth_time)
{
    int past_time = time(NULL) - last_growth_time;
    if (past_time < 0)
    {
        CRIT_LOG("it cann't be here, now:%lu, last_growth_time:%u", time(NULL), last_growth_time);
    }
    if (now_happy >= HAPPY_VALUE)    //怪兽一直处于高兴状态
    {
        uint32_t growth_add = past_time / g_happy_growth_time;
        return growth_add <= MATURITY_GROWTH_VALUE ? growth_add : MATURITY_GROWTH_VALUE;
    }
    else if (now_happy + past_time / (g_happy_desc_time * 60) < HAPPY_VALUE) //怪兽一直处于不高兴状态
    {
        uint32_t growth_add = past_time / g_unhappy_growth_time;
        return growth_add <= MATURITY_GROWTH_VALUE ? growth_add : MATURITY_GROWTH_VALUE;
    }
    else            //怪兽从高兴到不高兴
    {
        int unhappy_time = (HAPPY_VALUE - now_happy) * (g_happy_desc_time * 60);
        uint32_t growth_add = unhappy_time / g_unhappy_growth_time + (past_time - unhappy_time) / g_happy_growth_time;
        return growth_add <= MATURITY_GROWTH_VALUE ? growth_add : MATURITY_GROWTH_VALUE;
    }
}

uint16_t get_level_from_exp(int exp)
{
    int min = 0;
    int max = g_level_num - 1;
    int middle = 0;

    while (min != max)
    {
        middle = min + (max - min) / 2;
        if (g_level[middle] == exp)
        {
            return middle + 1;
        }
        else if (exp < g_level[middle])
        {
            max = middle;
        }
        else
        {
            min = middle + 1;
        }
    }

    return min + 1;
}

uint16_t get_level_left_exp(int exp, int level)
{
    int max_exp = g_level[level - 1];
    return max_exp - exp;
}

int get_stuff_npc_num(room_value_t *p_room, npc_score_info_t *p_npc)
{
    if(p_room == NULL)
    {
        return -1;
    }

    int buf_len = p_room->buf_len;
    user_room_buf_t *p_r = (user_room_buf_t*)p_room->buf;
    if (taomee::bswap((uint16_t)(p_r->buf_len)) != buf_len || p_r->buf_len < (int)sizeof(user_room_buf_t))
    {
        CRIT_LOG("room_buf_len=%u, buf_len=%u", buf_len, taomee::bswap((uint16_t)(p_r->buf_len)));
        return -1;
    }

    uint32_t npc_id =  0;
    npc_id = g_item_npc_map[taomee::bswap(p_r->wallpaper_id)];

    for(int i = 0; i < p_npc->npc_num; i++)
    {
        p_npc->npc[i].npc_score = 0;

        if(p_npc->npc[i].npc_id == npc_id)
        {
            p_npc->npc[i].npc_score += 1;
        }
    }


    npc_id = g_item_npc_map[taomee::bswap(p_r->door_id)];
    for(int i = 0; i < p_npc->npc_num; i++)
    {
        if(p_npc->npc[i].npc_id == npc_id)
        {
            p_npc->npc[i].npc_score += 1;
        }
    }
    npc_id = g_item_npc_map[taomee::bswap(p_r->floor_id)];
    for(int i = 0; i < p_npc->npc_num; i++)
    {
        if(p_npc->npc[i].npc_id == npc_id)
        {
            p_npc->npc[i].npc_score += 1;
        }
    }

    p_npc->total_score= 4;

    npc_id = g_item_npc_map[taomee::bswap(p_r->windows_id)];
    for(int i = 0; i < p_npc->npc_num; i++)
    {
        if(p_npc->npc[i].npc_id == npc_id)
        {
            p_npc->npc[i].npc_score += 1;
        }
    }


    int parsed_len = sizeof(user_room_buf_t);
    stuff_child_t *p_child = (stuff_child_t *)(p_room->buf + sizeof(user_room_buf_t));
    while (parsed_len < buf_len)
    {
        npc_id = g_item_npc_map[taomee::bswap(p_child->stuff_id)];
        for(int i = 0; i < p_npc->npc_num; i++)
        {
            if(p_npc->npc[i].npc_id == npc_id)
            {
                p_npc->npc[i].npc_score += 1;
                p_npc->total_score += 1;
            }
        }
        ++p_child;
        parsed_len += sizeof(stuff_child_t);
    }


    return 0;
}

int parse_room_buf(const char *buf, int buf_len, room_stuff_t *p_room_value)
{
    user_room_buf_t *p_room = (user_room_buf_t *)buf;
    if (taomee::bswap((uint16_t)(p_room->buf_len)) != buf_len || buf_len < (int)sizeof(user_room_buf_t))
    {
        CRIT_LOG("buf_len=%d, room_buf_len=%u", buf_len, taomee::bswap((uint16_t)(p_room->buf_len)));
        return -1;
    }

    map<uint32_t, uint16_t> room_stuff_map;

    room_stuff_map[taomee::bswap(p_room->wallpaper_id)] += 1;

    room_stuff_map[taomee::bswap(p_room->door_id)] += 1;

    room_stuff_map[taomee::bswap(p_room->floor_id)] += 1;

    room_stuff_map[taomee::bswap(p_room->windows_id)] += 1;

    int parsed_len = sizeof(user_room_buf_t);
    stuff_child_t *p_child = (stuff_child_t *)(buf + sizeof(user_room_buf_t));
    while (parsed_len < buf_len)
    {
        room_stuff_map[taomee::bswap(p_child->stuff_id)] += 1;
        ++p_child;
        parsed_len += sizeof(stuff_child_t);
    }

    p_room_value->stuff_num = room_stuff_map.size();
    int index = 0;
    for(map<uint32_t, uint16_t>::iterator iter = room_stuff_map.begin(); iter != room_stuff_map.end(); iter++)
    {
        p_room_value->stuff[index].stuff_id = iter->first;
        p_room_value->stuff[index].stuff_num = iter->second;
        index++;
    }

    return 0;
}

int cmp_room_buf(usr_info_t *p_user, const room_stuff_t *p_old_room, const room_stuff_t *p_new_room, stuff_change_t *p_stuff_change)
{
    map<uint32_t, uint16_t> old_stuff_map;
    map<uint32_t, uint16_t> new_stuff_map;

    //将以前房间的物品放入一个map
    for (int i = 0; i != (int)p_old_room->stuff_num; ++i)
    {
        old_stuff_map.insert(pair<uint32_t, uint16_t>(p_old_room->stuff[i].stuff_id, p_old_room->stuff[i].stuff_num));
    }

    int new_room_stuff_num = 0;
    //将现在房间的物品放入一个map
    for (int i = 0; i != (int)p_new_room->stuff_num; ++i)
    {
        new_stuff_map.insert(pair<uint32_t, uint16_t>(p_new_room->stuff[i].stuff_id, p_new_room->stuff[i].stuff_num));
        new_room_stuff_num += p_new_room->stuff[i].stuff_num;
    }
#ifdef DEBUG
    KINFO_LOG(p_user->uid, "%u stuff in room", new_room_stuff_num);
#endif


    //比较
    int change_num = 0;
    for (map<uint32_t, uint16_t>::iterator new_iter = new_stuff_map.begin(); new_iter != new_stuff_map.end(); ++new_iter)
    {
        if (change_num > MAX_ROOM_STUFF)
        {
            KCRIT_LOG(p_user->uid, "1:stuff change num:%u is too large.", change_num);
            return -1;
        }
        uint32_t stuff_id = new_iter->first;
        uint16_t stuff_num = new_iter->second;
        map<uint32_t, uint16_t>::iterator old_stuff_iter = old_stuff_map.find(stuff_id);
        int bag_stuff_num = p_user->user_cache.get_stuff_num(stuff_id);
        if (-1 == bag_stuff_num)
        {
            KCRIT_LOG(p_user->uid, "stuff:%u is not in bag.", stuff_id);
            return -1;
        }

        if (change_num >= 2 * MAX_ROOM_STUFF)
        {
            KCRIT_LOG(p_user->uid, "3:stuff change num:%u is too large.", change_num);
            return -1;
        }

        //在以前的buf里面找不到,则是从背包放入房间的物品
        if (old_stuff_iter == old_stuff_map.end())
        {
            //验证背包里是否有足够的物品放入
            if (stuff_num > bag_stuff_num)
            {
                KCRIT_LOG(p_user->uid, "not enough stuff:%u in bag", stuff_id);
                return -1;
            }
            else
            {
                p_stuff_change->stuff_flow[change_num].stuff_id = stuff_id;
                p_stuff_change->stuff_flow[change_num].stuff_num = stuff_num;
                p_stuff_change->stuff_flow[change_num].stuff_flow = flow_bag_to_room;
                ++change_num;
            }
        }
        else        //在以前的buf里面找到，则判断房间前后的数量大小
        {
            //新的数量大于旧的数量，从背包拿出
            if (stuff_num > old_stuff_iter->second)
            {
                int num = stuff_num - old_stuff_iter->second;
                if (bag_stuff_num < num)
                {
                    KCRIT_LOG(p_user->uid, "2 not enough stuff:%u in bag", stuff_id);
                    return -1;
                }
                else
                {
                    p_stuff_change->stuff_flow[change_num].stuff_id = stuff_id;
                    p_stuff_change->stuff_flow[change_num].stuff_num = num;
                    p_stuff_change->stuff_flow[change_num].stuff_flow = flow_bag_to_room;
                    ++change_num;
                }
            }
            else if (stuff_num < old_stuff_iter->second) //新的数量小于旧的数量，放入背包
            {
                int num = old_stuff_iter->second - stuff_num;
                p_stuff_change->stuff_flow[change_num].stuff_id = stuff_id;
                p_stuff_change->stuff_flow[change_num].stuff_num = num;
                p_stuff_change->stuff_flow[change_num].stuff_flow = flow_room_to_bag;
                ++change_num;
            }
            else
            {
                //do nothing
            }

            old_stuff_map.erase(old_stuff_iter);    //将处理过的物品删掉
        }
    }
    //老的房间还剩下的物品是从房间到背包的
    for (map<uint32_t, uint16_t>::iterator left_iter = old_stuff_map.begin(); left_iter != old_stuff_map.end(); ++left_iter)
    {
#ifdef DEBUG
	KINFO_LOG(p_user->uid, "stuff:%u is from room to bag", left_iter->first);
#endif
        if (change_num >= 2 * MAX_ROOM_STUFF)
        {
            KCRIT_LOG(p_user->uid, "2:stuff change num:%u is too large.", change_num);
            return -1;
        }

        if (!p_user->user_cache.is_stuff_exist(left_iter->first))
        {
            KCRIT_LOG(p_user->uid, "stuff:%u is not in bag.", left_iter->first);
            return -1;
        }

        p_stuff_change->stuff_flow[change_num].stuff_id = left_iter->first;
        p_stuff_change->stuff_flow[change_num].stuff_num = left_iter->second;
        p_stuff_change->stuff_flow[change_num].stuff_flow = flow_room_to_bag;
        ++change_num;
    }
    p_stuff_change->num = change_num;

    badge_step(p_user, badge_stuffinhome_30_const, new_room_stuff_num - 4);//扣除4个默认的家具(门 窗 墙纸 地板)

    return 0;
}

int plantation_get_pet(usr_info_t *p_user, all_plant_t *p_all_plant, uint32_t pet_id)
{
    //获得所有植物信息
    if (!plant_all_maturity(p_all_plant) || p_all_plant->count != PLANTATION_NUM)
    {
        KCRIT_LOG(p_user->uid, "attract pets, but plants num is not maturity");
        return -1;
    }

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_all_plant->count);

    for (int i = 0; i != p_all_plant->count; ++i)
    {
        g_send_msg.pack(p_all_plant->plant[i].plant_info.plant_id);
        g_send_msg.pack(p_all_plant->plant[i].hole_id);
        g_send_msg.pack(p_all_plant->plant[i].plant_info.color);
        g_send_msg.pack(p_all_plant->plant[i].plant_info.last_growth_value);
        g_send_msg.pack(p_all_plant->plant[i].plant_info.growth);
        KINFO_LOG(p_user->uid, "plant:%u, hole_id:%u, color:%u, growth:%u",
                p_all_plant->plant[i].plant_info.plant_id, p_all_plant->plant[i].hole_id, p_all_plant->plant[i].plant_info.color, p_all_plant->plant[i].plant_info.growth);
    }
    KINFO_LOG(p_user->uid, "get pet:%u", pet_id);

    g_send_msg.pack((uint8_t)PLANTATION_ATTRACT_PET);
    g_send_msg.pack(pet_id);
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    return 0;
}

int plantation_no_plant(usr_info_t *p_user)
{
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);

    g_send_msg.pack((uint8_t)0);
    g_send_msg.pack((uint8_t)PLANTATION_UNATTRACT_PET);

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    KINFO_LOG(p_user->uid, "no plant in planation");
    return 0;
}

bool plant_all_maturity(all_plant_t *p_all_plant)
{
    for (int i = 0; i != (int)p_all_plant->count; ++i)
    {
        if (p_all_plant->plant[i].plant_info.growth < MATURITY_GROWTH_VALUE)
        {
            return false;
        }
    }

    return true;
}

void get_attract_pet(all_plant_t *p_all_plant, uint32_t *p_pet_id, int *p_seed_match)
{
    INFO_LOG("in match pet.");
    int max_match_num = 0;

    //查看常见小怪有没有匹配的
    for (map<uint32_t, map<seed_match_t, int, c_seed_cmp> >::iterator iter = g_common_pet.begin(); iter != g_common_pet.end(); ++iter)
    {
        uint32_t pet_id = iter->first;
        map<seed_match_t, int, c_seed_cmp> seed_map = iter->second;
        int match_num = 0;
        int seed_match[3] = {0};

        for (int i = 0; i != p_all_plant->count; ++i)
        {
            int color = p_all_plant->plant[i].plant_info.color;
            int plant_id = p_all_plant->plant[i].plant_info.plant_id;

            seed_match_t seed = {plant_id, color};
            map<seed_match_t, int, c_seed_cmp>::iterator seed_iter = seed_map.find(seed);
            if (seed_iter != seed_map.end() && seed_iter->second > 0)   //有对应的匹配
            {
                seed_match[i] = plant_id;
                --seed_iter->second;
                ++match_num;
            }
            else
            {
                seed_match_t any_seed = {plant_id, 0};
                seed_iter = seed_map.find(any_seed);
                if (seed_iter != seed_map.end() && seed_iter->second > 0)   //有对应的匹配
                {
                    seed_match[i] = plant_id;
                    --seed_iter->second;
                    ++match_num;
                }
            }
        }
        if (3 == match_num)
        {
            *p_pet_id = pet_id;
            return;
        }
        else if (match_num > max_match_num)
        {
            max_match_num = match_num;
            memcpy(p_seed_match, seed_match, sizeof(seed_match));
        }
    }
    //优秀小怪兽有没有匹配的
    for (map<uint32_t, map<seed_match_t, int, c_seed_cmp> >::iterator iter = g_good_pet.begin(); iter != g_good_pet.end(); ++iter)
    {
        uint32_t pet_id = iter->first;
        map<seed_match_t, int, c_seed_cmp> seed_map = iter->second;
        int match_num = 0;
        int seed_match[3] = {0};

        for (int i = 0; i != p_all_plant->count; ++i)
        {
            int color = p_all_plant->plant[i].plant_info.color;
            int plant_id = p_all_plant->plant[i].plant_info.plant_id;

            seed_match_t seed = {plant_id, color};
            map<seed_match_t, int, c_seed_cmp>::iterator seed_iter = seed_map.find(seed);
            if (seed_iter != seed_map.end() && seed_iter->second > 0)   //有对应的匹配
            {
                seed_match[i] = plant_id;
                --seed_iter->second;
                ++match_num;
            }
            else
            {
                seed_match_t any_seed = {plant_id, 0};
                seed_iter = seed_map.find(any_seed);
                if (seed_iter != seed_map.end() && seed_iter->second > 0)   //有对应的匹配
                {
                    seed_match[i] = plant_id;
                    --seed_iter->second;
                    ++match_num;
                }
            }
        }

        if (3 == match_num)
        {
            *p_pet_id = pet_id;
            return;
        }
        else if (match_num > max_match_num)
        {
            max_match_num = match_num;
            memcpy(p_seed_match, seed_match, sizeof(seed_match));
        }
    }
    //珍稀小怪兽有没有匹配的
    for (map<uint32_t, map<seed_match_t, int, c_seed_cmp> >::iterator iter = g_rare_pet.begin(); iter != g_rare_pet.end(); ++iter)
    {
        uint32_t pet_id = iter->first;
        map<seed_match_t, int, c_seed_cmp> seed_map = iter->second;
        int match_num = 0;
        int seed_match[3] = {0};

        for (int i = 0; i != p_all_plant->count; ++i)
        {
            int color = p_all_plant->plant[i].plant_info.color;
            int plant_id = p_all_plant->plant[i].plant_info.plant_id;

            seed_match_t seed = {plant_id, color};
            map<seed_match_t, int, c_seed_cmp>::iterator seed_iter = seed_map.find(seed);
            if (seed_iter != seed_map.end() && seed_iter->second > 0)   //有对应的匹配
            {
                seed_match[i] = plant_id;
                --seed_iter->second;
                ++match_num;
            }
            else
            {
                seed_match_t any_seed = {plant_id, 0};
                seed_iter = seed_map.find(any_seed);
                if (seed_iter != seed_map.end() && seed_iter->second > 0)   //有对应的匹配
                {
                    seed_match[i] = plant_id;
                    --seed_iter->second;
                    ++match_num;
                }
            }
        }

        if (3 == match_num)
        {
            *p_pet_id = pet_id;
            return;
        }
        else if (match_num > max_match_num)
        {
            max_match_num = match_num;
            memcpy(p_seed_match, seed_match, sizeof(seed_match));
        }
    }
    //濒危小怪兽有没有匹配的
    for (map<uint32_t, map<seed_match_t, int, c_seed_cmp> >::iterator iter = g_vip_pet.begin(); iter != g_vip_pet.end(); ++iter)
    {
        uint32_t pet_id = iter->first;
        map<seed_match_t, int, c_seed_cmp> seed_map = iter->second;
        int match_num = 0;
        int seed_match[3] = {0};

        for (int i = 0; i != p_all_plant->count; ++i)
        {
            int color = p_all_plant->plant[i].plant_info.color;
            int plant_id = p_all_plant->plant[i].plant_info.plant_id;

            seed_match_t seed = {plant_id, color};
            map<seed_match_t, int, c_seed_cmp>::iterator seed_iter = seed_map.find(seed);
            if (seed_iter != seed_map.end() && seed_iter->second > 0)   //有对应的匹配
            {
                seed_match[i] = plant_id;
                --seed_iter->second;
                ++match_num;
            }
            else
            {
                seed_match_t any_seed = {plant_id, 0};
                seed_iter = seed_map.find(any_seed);
                if (seed_iter != seed_map.end() && seed_iter->second > 0)   //有对应的匹配
                {
                    seed_match[i] = plant_id;
                    --seed_iter->second;
                    ++match_num;
                }
            }
        }

        if (3 == match_num)
        {
            *p_pet_id = pet_id;
            return;
        }
        else if (match_num > max_match_num)
        {
            max_match_num = match_num;
            memcpy(p_seed_match, seed_match, sizeof(seed_match));
        }
    }

    INFO_LOG("no pet is matched.");
}

void get_extra_growth(usr_info_t *p_user, all_plant_t *p_all_plant)
{
    time_t now = time(NULL);
    db_msg_update_plant_req_t *p_change = (db_msg_update_plant_req_t *)p_user->buffer;
    for (int i = 0; i != (int)p_all_plant->count; ++i)
    {
        if (now - p_all_plant->plant[i].plant_info.last_add_extra_growth_time >= g_plant_time)
        {
            p_change->plant_growth[i].growth += g_extra_growth;
            if (p_all_plant->plant[i].plant_info.growth + p_change->plant_growth[i].growth > MATURITY_GROWTH_VALUE)
            {
                p_change->plant_growth[i].growth = MATURITY_GROWTH_VALUE - p_all_plant->plant[i].plant_info.growth;
            }
            p_change->plant_growth[i].growth_time = now;
            p_change->plant_growth[i].last_add_extra_growth_time = now;
            p_change->plant_growth[i].last_growth_value = p_all_plant->plant[i].plant_info.growth + p_change->plant_growth[i].growth;
#ifdef DEBUG
            INFO_LOG("now:%zu, last add extra growth time:%u, last_growth_value:%u", now, p_all_plant->plant[i].plant_info.last_add_extra_growth_time, p_change->plant_growth[i].last_growth_value);
#endif
        }
    }
}

void set_friend_list(friend_cache_t *p_friend_cache, db_msg_friend_list_rsp_t *p_friend_list)
{
    memcpy(&p_friend_cache->friend_list, p_friend_list, sizeof(db_msg_friend_list_rsp_t) + p_friend_list->count * sizeof(friend_info_t));
}

void update_on_logout(usr_info_t *p_user)
{
    //通知switch有在线用户离开
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_logout, p_user->uid, 0, 0);
    g_p_switch_svr->send_data(NULL, g_send_buffer, p_header->len, NULL);
    //通知multi-serve有用户离线
    g_p_multi_server_svr->send_data(NULL, g_send_buffer, p_header->len, NULL);

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        return;
    }

    uint32_t now = time(NULL);
    uint32_t last_update_health_time = p_role->last_update_health_time;
    if (0 == last_update_health_time)
    {
        last_update_health_time = p_role->role.last_login_time;
    }
    uint32_t health = get_now_health(p_role->role.monster_health, last_update_health_time, now);

    pack_svr_pkg_head(p_header, svr_msg_db_update_role, p_user->uid, 0, sizeof(db_msg_update_login_t) + 3 * sizeof(field_t));

    db_msg_update_login_t *p_body = (db_msg_update_login_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->count = 3;
    p_body->field[0].type = FIELD_HEALTH;
    p_body->field[0].value = health;
    p_body->field[1].type = FIELD_LOGOUT_TIME;
    p_body->field[1].value = now;
    p_body->field[2].type = FIELD_ONLINE_TIME;
    p_body->field[2].value = p_role->role.online_time + now - p_role->role.last_login_time;

    KINFO_LOG(p_user->uid, "update health:%u, logout time:%u, online time:%u", health, now, p_body->field[2].value);

    g_p_db_proxy_svr->send_data(NULL, g_send_buffer, p_header->len, NULL);
}

void notify_as_current_user_info(usr_info_t *p_user)
{
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "cann't find user info in cache.");
        close_client_conn(p_user->session->fd);
        return;
    }
    // 统计当前的健康值愉悦值和咕咚果
    stat_eight_param_t stat_data = {p_user->uid, p_role->role.monster_health, p_user->uid, p_role->role.monster_happy, p_user->uid, p_role->role.coins, p_user->uid, p_role->role.monster_level};
    msg_log(stat_health_happy, &stat_data, sizeof(stat_data));

    pack_as_pkg_header(p_user->uid, as_msg_level_up, 0, ERR_NO_ERR);

    time_t now = time(NULL);
    uint32_t now_happy = get_new_happy(now, p_role->role.last_login_time, p_role->role.monster_happy);
    g_send_msg.pack(now_happy);
    uint32_t monster_health = get_now_health(p_role->role.monster_health, p_role->last_update_health_time, now);
    g_send_msg.pack(monster_health);
    g_send_msg.pack(p_role->role.monster_exp);
    g_send_msg.pack(p_role->role.monster_level);
    g_send_msg.pack(p_role->role.coins);
    g_send_msg.pack(p_role->role.visits);
    g_send_msg.pack(p_role->role.thumb);
#ifdef DEBUG
    KINFO_LOG(p_user->uid, "notify as, happy:%u, health:%u, exp:%u, level:%u, coins:%u", now_happy, monster_health, p_role->role.monster_exp, p_role->role.monster_level, p_role->role.coins);
#endif

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), false);
}

int get_stranger_id(usr_info_t *p_user, stranger_t *p_stranger_info, switch_msg_active_user_t *p_active_usr)
{
    int vip_num = 0;
    int normal_num = 0;
    set<uint32_t> stranger_set;
    //随机交换活跃用户的位置
    for (int i = p_active_usr->count - 1; i > 0; --i)
    {
        int index = uniform_rand(0, i);
        user_vip_t temp = {0};
        memcpy(&temp, &p_active_usr->user[i], sizeof(user_vip_t));
        memcpy(&p_active_usr->user[i], &p_active_usr->user[index], sizeof(user_vip_t));
        memcpy(&p_active_usr->user[index], &temp, sizeof(user_vip_t));
    }

    for (int i = 0; i != (int)p_active_usr->count; ++i)
    {
        if (p_active_usr->user[i].user_id == p_user->uid)   //不显示自己
        {
            continue;
        }

        int ret = p_user->friend_tree.is_friend(p_active_usr->user[i].user_id);
        if (ret < 0)
        {
            return -1;
        }
        else if (ret > 0) //是自己好友
        {
            continue;
        }

        if (stranger_set.count(p_active_usr->user[i].user_id) > 0)  //用户已经出现过
        {
            continue;
        }

        if (p_active_usr->user[i].is_vip)
        {
            if (vip_num < g_street_vip_num)
            {
                stranger_set.insert(p_active_usr->user[i].user_id);
                ++vip_num;
            }
            else if (normal_num + vip_num == g_street_vip_num)
            {
                break;
            }
        }
        else
        {
            if (normal_num < g_street_vip_num + g_street_normal_num - vip_num)
            {
                stranger_set.insert(p_active_usr->user[i].user_id);
                ++normal_num;
            }
            else if (vip_num == g_street_vip_num)
            {
                break;
            }
        }
    }

    int i = 0;
    for (set<uint32_t>::iterator iter = stranger_set.begin(); iter != stranger_set.end(); ++iter)
    {
        p_stranger_info->stranger[i].user_id = *iter;
       // KINFO_LOG(p_user->uid, "get stranger:%u", *iter);
        ++i;
    }
    p_stranger_info->count = i;

    return 0;
}

int broadcast_timeout_cb(void *p_owner, void *p_data)
{
    assert(p_owner != NULL);
    UserManager *p_user_mng  = (UserManager *)p_owner;

    p_user_mng->process_all_user(broadcast_to_user);
    ADD_TIMER_EVENT(&g_user_mng, broadcast_timeout_cb, NULL, time(NULL) + BROADCAST_TIME);

    return 0;
}

void broadcast_to_user(usr_info_t *p_user)
{
    pack_as_pkg_header(p_user->uid, as_msg_close_conn, 0, ERR_NO_ERR);
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), false);
}

void init_log_file(const char *stat_path)
{
    snprintf(g_log_file, sizeof(g_log_file) - 1, "%s_%u", stat_path, getpid());
}

void msg_log(uint32_t type, const void *stat_data, int len)
{
    if (len > 0)
    {
        msglog(g_log_file, type, time(NULL), stat_data, len);
    }
}

time_t get_timestamp(uint32_t birthday)
{
    struct tm timestamp = {0};
    timestamp.tm_year = (birthday / 10000) - 1900;
    timestamp.tm_mon = ((birthday / 100) % 100)  -  1;
    timestamp.tm_mday = birthday  % 100;
    return mktime(&timestamp);
}

int get_game_star(game_t *p_game, uint32_t one_star_score, uint32_t two_star_score, uint32_t three_star_score, uint32_t finish_score)
{
   if(p_game->score < one_star_score)
   {
       p_game->star_num = 0;
   }
   else if(p_game->score < two_star_score)
   {
       p_game->star_num = 1;
   }
   else if(p_game->score < three_star_score)
   {
       p_game->star_num = 2;
   }
   else
   {
       p_game->star_num = 3;
   }

   if(p_game->score > finish_score)
   {
       p_game->is_passed = 1;//通关;
   }
   else
   {
       p_game->is_passed = 0;
   }
   return 0;
}

int get_game_reward_item(game_t *p_game)
{
    map<uint32_t, map<int, game_item_t> >::iterator iter = g_game_reward_map.find(p_game->game_key);
    if (iter == g_game_reward_map.end())
    {
        CRIT_LOG("game:%u level:%u  key %uis invalid", p_game->game_id, p_game->level_id, p_game->game_key);
        return -1;
    }

    map<int, game_item_t> &item_map = iter->second;
    map<int, game_item_t>::reverse_iterator item_iter = item_map.rbegin();
    for (; item_iter != item_map.rend(); ++item_iter)
    {
        if ((int)p_game->score >= item_iter->first)
        {
            break;
        }
    }

    if (item_iter == item_map.rend())
    {
        CRIT_LOG("score:%u is invalid", p_game->score);
        return -1;
    }

    p_game->reward_items = 1;
    game_item_t &game_item = item_iter->second;
    p_game->value[0].value = game_item.num;
    int id = uniform_rand(0, game_item.item_num - 1);
    p_game->value[0].value_id = game_item.item_id[id];

    return 0;
}

void send_pet_info(usr_info_t *p_user)
{
    uint16_t *p_pet_num = (uint16_t *)p_user->buffer;
    map<uint32_t, uint16_t> following_pet_map;
    map<uint32_t, uint16_t> zoom_pet_map;

    for (int i = 0; i < g_max_pet_id; ++i)
    {
        if (p_pet_num[i] != 0)
        {
            following_pet_map.insert(pair<uint32_t, uint16_t>(i + 1, p_pet_num[i]));
            zoom_pet_map.insert(pair<uint32_t, uint16_t>(i + 1, p_pet_num[i]));
        }
    }
    for (int i = g_max_pet_id; i < 2 * g_max_pet_id; ++i)
    {
        if (p_pet_num[i] != 0)
        {
            zoom_pet_map.insert(pair<uint32_t, uint16_t>(i - g_max_pet_id + 1, p_pet_num[i]));
        }
    }

    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack((uint16_t)zoom_pet_map.size());
    for (map<uint32_t, uint16_t>::iterator iter = zoom_pet_map.begin(); iter != zoom_pet_map.end(); ++iter)
    {
        g_send_msg.pack(iter->first);
        g_send_msg.pack(iter->second);
        map<uint32_t, uint16_t>::iterator following_iter = following_pet_map.find(iter->first);
        if (following_iter != following_pet_map.end())
        {
            g_send_msg.pack(following_iter->second);
        }
        else
        {
            g_send_msg.pack((uint16_t)0);
        }
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
}


void send_npc_score_to_as(usr_info_t *p_user, npc_score_info_t npc_score)
{
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(npc_score.status);
    g_send_msg.pack(npc_score.total_score);
    g_send_msg.pack(npc_score.npc_num);
    KINFO_LOG(p_user->uid, "status:%u total_score:%u, npc_num:%u", npc_score.status, npc_score.total_score, npc_score.npc_num);
    for(int i = 0; i < npc_score.npc_num; i++)
    {
        g_send_msg.pack(npc_score.npc[i].npc_id);
        g_send_msg.pack(npc_score.npc[i].npc_score);
        KINFO_LOG(p_user->uid, "id:%u score:%u", npc_score.npc[i].npc_id, npc_score.npc[i].npc_score);
    }
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
}


void send_cache_pet_info(usr_info_t *p_user, all_pet_t *p_all_pet)
{
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_all_pet->count);
    KINFO_LOG(p_user->uid, "%u pet type in zoom", p_all_pet->count);
    for (int i = 0; i < (int)p_all_pet->count; ++i)
    {
        g_send_msg.pack(p_all_pet->pet[i].pet_id);
        g_send_msg.pack(p_all_pet->pet[i].total_num);
        g_send_msg.pack(p_all_pet->pet[i].follow_num);
    	KINFO_LOG(p_user->uid, "pet:%u, total:%u, follow:%u", p_all_pet->pet[i].pet_id, p_all_pet->pet[i].total_num, p_all_pet->pet[i].follow_num);
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    //成就
    badge_step(p_user, badge_get_one_monster_const, p_all_pet->count);
    if(p_all_pet->count != 0)
    {
        badge_step(p_user, badge_get_two_same_monster_const, 1);
    }

    for(int i = 0; i < (int)p_all_pet->count; i++)
    {
        if(p_all_pet->pet[i].total_num >= 2)
        {
            badge_step(p_user, badge_get_two_same_monster_const, 2);
            break;
        }
    }






}

int send_game_lvl_to_db(usr_info_t *p_user, game_t *p_game)
{
    if(p_game->need_update_db == 0)
    {//不需要更新db.直接返回as
        //判断下一关是否解锁
        uint8_t unlock_next_lvl = p_user->user_cache.unlock_next_level(p_game->game_id, p_game->level_id);
        if(unlock_next_lvl == -1)
        {//错误的情况
            KCRIT_LOG(p_user->uid, "It not possible ,not found gameid levelid");
            unlock_next_lvl = NOT_UNLOCK;//这种情况将其置为下一关不可解锁
        }
        KINFO_LOG(p_user->uid, "return commit_game:%u level:%u reward_coin:%u star_num:%u unlock_next:%u, reason:%u", p_game->game_id, p_game->level_id, p_game->reward_coins, p_game->star_num, unlock_next_lvl, p_game->reason);
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
            KINFO_LOG(p_user->uid, "game reward stuff:%u num:%u", (uint8_t)(p_game->value[0].value), p_game->value[0].value_id);
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
    else
    {
        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
        db_msg_update_game_level_req_t *p_body = (db_msg_update_game_level_req_t*)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->game_id = p_game->game_id;
        p_body->level_id = p_game->level_id;
        p_body->score = p_game->score;
        p_body->star_num = p_game->star_num;
        p_body->is_passed = p_game->is_passed;

        pack_svr_pkg_head(p_header, svr_msg_db_add_game_level, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_update_game_level_req_t));
        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
            return 0;
        }
    }
    return 0;
}

int do_game_restrict_and_star(usr_info_t *p_user, game_t *p_game, uint32_t restrict_coins, uint32_t restrict_item, uint32_t day_time)
{
        p_game->coin_restrict = 0;
        if(restrict_coins >= p_game->max_coins)
        {//当天的金币限制已经达到最大值，不能在奖励金币了
            p_game->coin_restrict = 1;
            p_game->reward_coins = 0;

        }
        else
        {
            if (restrict_coins + p_game->reward_coins >= p_game->max_coins)
            {
                p_game->reward_coins = p_game->max_coins - restrict_coins;
            }
        }

        if(p_game->score < p_game->reward_item_score)
        {//没有达到奖励道具的最低分数要求
            p_game->reason = GAME_NO_STUFF;
            p_game->reward_items = 0;
        }
        else if((int)restrict_item >= g_max_game_item)
        {//当天获得的道具已达到上限
            p_game->reason = GAME_STUFF_FULL;
            p_game->reward_items = 0;
        }
        else
        {
             if (get_game_reward_item(p_game) < 0)
             {
                  return -1;
             }

             if (p_game->reward_items + restrict_item > (uint32_t)g_max_game_item)
             {
                 p_game->reward_items = g_max_game_item - restrict_item;
             }
             p_game->reason = GAME_GET_STUFF;
        }

        if(p_game->reward_coins == 0 && p_game->reward_items == 0)
        {//没有奖励金币以及道具，故不需要通知db更新天限制
            return send_game_lvl_to_db(p_user, p_game);
        }
        else
        {//通知db更新天限制
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;

                db_msg_add_game_day_restrict_req_t *p_body = (db_msg_add_game_day_restrict_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
                p_body->time = day_time;
                p_body->reward_coins = p_game->reward_coins;
                p_body->type = STRICT_ITEM + p_game->game_id;
                if(p_game->reward_items != 0)
                {
                    p_body->count = 1;
                    p_body->restrict_stuff[0].value_id = p_game->value[0].value_id;
                    p_body->restrict_stuff[0].value = p_game->value[0].value;
                }
                else
                {
                    p_body->count = 0;
                }

                pack_svr_pkg_head(p_header, svr_msg_db_add_game_day_restrict, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_add_game_day_restrict_req_t) + p_body->count * sizeof(restrict_value_t));
                if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
                {
                    g_errno = ERR_MSG_DB_PROXY;
                    KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
                    return 0;
                }
        }
            return 0;
}

uint32_t get_current_gains(uint32_t now_gains, usr_info_t *p_user)
{

    if(p_user->uid == 359521811 || p_user->uid == 359664142 || p_user->uid == 356800664)
    {//这三个号不做防成谜
        return now_gains;
    }

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        return now_gains;
    }

    uint32_t online_time = p_role->role.online_time + time(NULL) - p_role->role.last_login_time;
    if (online_time < CHENGMI_HALF)
    {
        return now_gains;
    }
    else if (online_time < CHENGMI_NO)
    {
        return now_gains / 2;
    }

    return 0;
}


//益智游戏经验值限制处理
int compute_reward_commit_to_db(usr_info_t *p_user, as_msg_puzzle_req_t *p_req, uint32_t day_restrict_exp)
{
    if(p_req == NULL)
    {
        KCRIT_LOG(p_user->uid, "p_req is NULL");
        return -1;
    }

    svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_commit_puzzle, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_puzzle_req_t));
    db_msg_puzzle_req_t *p_body = (db_msg_puzzle_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    memset(p_body, 0, sizeof(db_msg_puzzle_req_t));

    uint16_t extra_coin = 0;
    p_body->puzzle.type = p_req->type;
    p_body->puzzle.score = p_req->right_num;
    p_body->puzzle.num = 1;

    puzzle_info_t *p_puzzle_cache = p_user->user_cache.get_puzzle(p_req->type);
    puzzle_info_t puzzle_buffer = {0};
    if (NULL == p_puzzle_cache)
    {
        p_puzzle_cache = (puzzle_info_t *)&puzzle_buffer;
    }

    int avg_num = 0;
    if (p_puzzle_cache->total_num != 0)
    {
        avg_num = p_puzzle_cache->total_score / p_puzzle_cache->total_num;
    }

    //当前得分大于最大得分，更新最大得分
    if (p_body->puzzle.score > p_puzzle_cache->max_score)
    {
        if (DAILY_PUZZLE == p_req->type)           //每日挑战,有额外奖励
        {
            extra_coin = EXTRA_COIN_REWARD;
        }
        p_body->puzzle.max_score = p_body->puzzle.score;
    }

    //玩的时间与上次不是同一天，更新玩游戏的时间
    int is_same_day = 0;
    if (p_puzzle_cache->last_playtime != 0)
    {
        is_same_day = timestamp_equal_day(time(NULL), p_puzzle_cache->last_playtime);
        if (-1 == is_same_day)
        {
            KCRIT_LOG(p_user->uid, "get day from timestamp failed.");
            return -1;
        }
    }

    if (0 == is_same_day)
    {
        p_body->puzzle.last_playtime = time(NULL);
    }


    if (DAILY_PUZZLE == p_req->type)           //每日挑战
    {
        //当天不能再挑战
        if (is_same_day)
        {
            KCRIT_LOG(p_user->uid, "daily puzzle is played today.");
            g_errno = ERR_REPEATLY_DAILY_PUZZLE;
            return 0;
        }

        p_body->reward.exp = p_req->right_num * DAILY_EXP_REWARD;
        p_body->reward.coins = p_req->right_num * DAILY_COIN_REWARD * (1 + (double)p_req->right_num / 20);
        p_body->reward.happy = p_req->right_num * PUZZLE_DAILY_HAPPY_REWARD;
        if (p_req->right_num > avg_num)
        {
            p_body->reward.happy += (p_req->right_num - avg_num) * PUZZLE_DAILY_HAPPY_ASC;
        }
        else
        {
            uint16_t desc_happy = (avg_num- p_req->right_num)* PUZZLE_DAILY_HAPPY_DESC;
            p_body->reward.happy = p_body->reward.happy > desc_happy ? (p_body->reward.happy - desc_happy) : 0;
        }
    }
    else            //其他类型的挑战
    {
        //只有第一次玩才有奖励
		/*
        if (0 == is_same_day)
        {
            p_body->reward.coins = SINGLE_COIN_REWARD;
            p_body->reward.exp = PUZZLE_EXP_REWARD;
            p_body->reward.happy = PUZZLE_SINGLE_HAPPY_REWARD * p_req->right_num;
            if (p_req->right_num > avg_num)
            {
                p_body->reward.happy += (p_req->right_num - avg_num) * PUZZLE_SINGLE_HAPPY_ASC;
            }
            else
            {
                uint16_t desc_happy = (avg_num- p_req->right_num)* PUZZLE_SINGLE_HAPPY_DESC;
                p_body->reward.happy = p_body->reward.happy > desc_happy ? (p_body->reward.happy - desc_happy) : 0;
            }
        }
		*/
		uint8_t play_times = is_same_day?p_puzzle_cache->play_times+1:1;
		p_body->reward.coins = round((float)p_req->right_num / 2 * pow(0.7f, play_times- 1));
		p_body->reward.exp = p_body->reward.coins;
		p_body->reward.happy = PUZZLE_SINGLE_HAPPY_REWARD * p_req->right_num;
		if (p_req->right_num > avg_num)
		{
			p_body->reward.happy += (p_req->right_num - avg_num) * PUZZLE_SINGLE_HAPPY_ASC;
		}
		else
		{
			uint16_t desc_happy = (avg_num- p_req->right_num)* PUZZLE_SINGLE_HAPPY_DESC;
			p_body->reward.happy = p_body->reward.happy > desc_happy ? (p_body->reward.happy - desc_happy) : 0;
		}
    }

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "get cached role info failed.");
        return -1;
    }

    if (p_role->role.monster_happy + p_body->reward.happy > MAX_HAPPY_VALUE)
    {
        p_body->reward.happy = MAX_HAPPY_VALUE > p_role->role.monster_happy ? MAX_HAPPY_VALUE - p_role->role.monster_happy : 0;
    }

    if (p_body->reward.coins > MAX_PUZZLE_COIN)
    {
        p_body->reward.coins = MAX_PUZZLE_COIN;
    }
    p_body->reward.coins += extra_coin;

    //防成谜
    p_body->reward.coins = get_current_gains(p_body->reward.coins, p_user);
    p_body->reward.exp = get_current_gains(p_body->reward.exp, p_user);


    //经验值不能超过每天的上限
    if((int)day_restrict_exp >= g_max_day_puzzle_exp)
    {
        p_body->reward.exp = 0;
    }
    else if((int)(day_restrict_exp + p_body->reward.exp) > g_max_day_puzzle_exp)
    {
        p_body->reward.exp = g_max_day_puzzle_exp - day_restrict_exp;
    }

    int now_level = get_level_from_exp(p_role->role.monster_exp + p_body->reward.exp);
    if (now_level > p_role->role.monster_level && p_role->role.monster_happy > LEVEL_HAPPY_VALUE && p_role->role.monster_health > LEVEL_HEALTH_VALUE)
    {
        //可以升级
        p_body->reward.level = now_level - p_role->role.monster_level;
    }
    else    //不能升级
    {
        uint16_t max_left_exp = get_level_left_exp(p_role->role.monster_exp, p_role->role.monster_level);
        if (max_left_exp < p_body->reward.exp)  //不能升级，则经验加满就不能再加了
        {
            p_body->reward.exp = max_left_exp;
        }
    }

    //pack请求包
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    //缓存提交的益智游戏信息，请求成功后更新缓存
    memcpy(p_user->buffer, p_body, sizeof(db_msg_puzzle_req_t));
    memcpy(p_user->buffer + sizeof(db_msg_puzzle_req_t), &extra_coin, sizeof(extra_coin));
    //统计益智游戏
    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_exercise_base + p_req->type, &stat_data, sizeof(stat_data));

    //统计益智游戏答题正确率
    //去除统计项 2012-06-07
//    stat_two_param_t stat_data2 = {p_req->total_num, p_req->right_num};
//    msg_log(stat_puzzle_num, &stat_data2, sizeof(stat_data2));


    return 0;
}

int encourage_guide_to_db(usr_info_t *p_user, uint8_t type, uint32_t score)
{
	db_request_encourage_t *p_body = (db_request_encourage_t *)(g_send_buffer + sizeof(svr_msg_header_t));
	p_body->type = type;
	p_body->score = score;
	p_body->timestamp = timestamp_begin_day();

	svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
	pack_svr_pkg_head(p_header, svr_msg_online_encourage, p_user->uid, GET_SVR_SN(p_user), sizeof(db_request_encourage_t));

   if (g_p_switch_svr->send_data(NULL, g_send_buffer, p_header->len, NULL) != 0)
   {
        KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
	     g_errno = ERR_MSG_DB_PROXY;
	}

	 return 0;
}


int update_wealth_to_db(usr_info_t *p_user, uint32_t money, uint32_t stuff_id, uint16_t stuff_num, const char *buf, int buf_len)
{
    db_request_update_wealth_t *p_body = (db_request_update_wealth_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->money = money;
    p_body->stuff_id = stuff_id;
    p_body->stuff_num = stuff_num;
    memcpy(p_body->buf, buf, buf_len);

    svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_request_update_wealth, p_user->uid, GET_SVR_SN(p_user), sizeof(db_request_update_wealth_t));

    if (g_p_db_proxy_svr->send_data(NULL, g_send_buffer, p_header->len, NULL) != 0)
    {
        KCRIT_LOG(p_user->uid, "update wealth to db send data to db proxy failed.");
        g_errno = ERR_MSG_DB_PROXY;
    }


    return 0;
}


int update_level_relative_info(usr_info_t *p_user, uint32_t activity_id)
{
	map<uint32_t, activity_info_t>::iterator iter = g_activity_map.find(activity_id);
	if(iter == g_activity_map.end())
	{//请求一个不存在的活动
		 KCRIT_LOG(p_user->uid, "get reward ex activity_id:%u not exist", activity_id);
		 return -1;
	}
	uint32_t cur_time = time(NULL);

	if(cur_time < iter->second.start_timestamp || cur_time > iter->second.end_timestamp)
	{
		KINFO_LOG(p_user->uid, "this activity is overtime or not start.");
		return 0;
	}

	switch(activity_id)
	{
		case INVITOR_ACTIVITY_ID:
		{
		    role_cache_t *p_role = p_user->user_cache.get_role();
		    if (NULL == p_role)
		    {
		        KCRIT_LOG(p_user->uid, "can't get usr role");
		        return -1;
		    }
			if(p_role->role.invitor_id)
			{
				if(p_role->role.monster_level == 5)
				{
					svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
					pack_svr_pkg_head(p_header, svr_msg_db_update_level_relative, p_role->role.invitor_id, 0, 0);
					if (g_p_db_proxy_svr->send_data(NULL, g_send_buffer, p_header->len, NULL) != 0)
					{
						KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
						g_errno = ERR_MSG_DB_PROXY;
					}
				}
			}

			break;
		}
	}

	 return 0;
}
