/**
 * =====================================================================================
 *       @file  c_user_cache.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/20/2011 09:26:00 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <string.h>

extern "C"
{
#include <libtaomee/log.h>
}
#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "online_structure.h"
#include "c_user_cache.h"

using std::map;
using std::set;
using std::pair;
using std::vector;

c_user_cache::c_user_cache() : m_inited(0), m_p_default_room(NULL), m_cur_room_id(0), m_p_cur_room(NULL), m_attract_pet(0)
{
    memset(&m_role, 0, sizeof(role_cache_t));
}

c_user_cache::~c_user_cache()
{
    uninit();
}

int c_user_cache::init()
{
    if (m_inited)
    {
        ERROR_LOG("c_user_cache has been inited");
        return -1;
    }

    DEBUG_LOG("c_user_cache inited");
    m_badge_cached = 0;
    m_hole_cached = 0;
    m_hole_reward_cached = 0;
    m_attract_pet = 0;
    m_task_cached = 0;

    m_bag_map.clear();
    m_puzzle_map.clear();
    m_plant_map.clear();
    m_pinboard_map.clear();
    m_pinboard_id_map.clear();

    m_pet_map.clear();
    m_game_level_map.clear();
    m_day_restrict_map.clear();
    m_badge_map.clear();

    m_open_act_cached = 0;
    m_open_act_reward_map.clear();

    m_hole1_reward_map.clear();
    m_hole2_reward_map.clear();
    m_hole3_reward_map.clear();

    m_peer_msg.peer_id = 0;
    m_peer_msg.real_num = 0;

    m_game_change_stuff_map.clear();
    m_finished_task_map.clear();

    m_museum_info_map.clear();

    m_npc_score.status = 0;
    m_inited = 1;
    return 0;
}

int c_user_cache::uninit()
{
    if (!m_inited)
    {
        return 0;
    }

    memset(&m_role, 0, sizeof(role_cache_t));
    if (m_p_default_room != NULL)
    {
        g_slice_free1(sizeof(room_value_t), m_p_default_room);
        m_p_default_room = NULL;
    }
    m_cur_room_id = 0;
    if (m_p_cur_room != NULL)
    {
        g_slice_free1(sizeof(room_value_t), m_p_cur_room);
        m_p_cur_room = NULL;
    }
    m_room_set.clear();
    m_bag_map.clear();
    m_puzzle_map.clear();
    m_plant_map.clear();
    m_pinboard_map.clear();
    m_pinboard_id_map.clear();

    m_pet_map.clear();
    m_game_level_map.clear();
    m_day_restrict_map.clear();
    m_badge_map.clear();

    m_open_act_cached = 0;
    m_open_act_reward_map.clear();

    m_hole1_reward_map.clear();
    m_hole2_reward_map.clear();
    m_hole3_reward_map.clear();

    m_game_change_stuff_map.clear();
    m_finished_task_map.clear();
    m_badge_cached = 0;
    m_museum_info_map.clear();

    m_hole_cached = 0;
	m_hole_reward_cached = 0;
    m_task_cached = 0;
    m_npc_score.status = 0;

    m_peer_msg.peer_id = 0;
    m_peer_msg.real_num = 0;

    for (map<int, pinboard_t *>::iterator iter = m_pinboard_map.begin(); iter != m_pinboard_map.end(); ++iter)
    {
        int pinboard_len = sizeof(pinboard_t) + iter->second->count * sizeof(pinboard_cache_t);
        g_slice_free1(pinboard_len, iter->second);
    }
    m_pinboard_map.clear();
    m_pinboard_id_map.clear();

    DEBUG_LOG("c_user_cache uninited");
    m_inited = 0;
    return 0;
}

int c_user_cache::add_role(const role_cache_t *p_role)
{
    if (NULL == p_role)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }
    memcpy(&m_role, p_role, sizeof(role_cache_t));
    return 0;
}

role_cache_t *c_user_cache::get_role()
{
    return &m_role;
}

int c_user_cache::add_default_room(room_value_t *p_room_value)
{
    if (NULL == p_room_value)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }
    if (m_p_default_room != NULL)
    {
        ERROR_LOG("default room is already in cache");
        return -1;
    }

    m_p_default_room = reinterpret_cast<room_value_t *>(g_slice_alloc(sizeof(room_value_t)));
    if (NULL == m_p_default_room)
    {
        ERROR_LOG("alloc room mem");
        return -1;
    }
    memcpy(m_p_default_room, p_room_value, sizeof(uint16_t) + p_room_value->buf_len);

    return 0;
}

room_value_t *c_user_cache::get_default_room()
{
    return m_p_default_room;
}

int c_user_cache::add_cur_room(uint32_t room_id, room_value_t *p_room_value)
{
    if (NULL == p_room_value)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }
    if (m_p_cur_room != NULL)
    {
        g_slice_free1(sizeof(room_value_t), m_p_cur_room);
    }

    m_p_cur_room = reinterpret_cast<room_value_t *>(g_slice_alloc(sizeof(room_value_t)));
    if (NULL == m_p_cur_room)
    {
        ERROR_LOG("alloc room mem");
        return -1;
    }
    memcpy(m_p_cur_room, p_room_value, sizeof(uint16_t) + p_room_value->buf_len);

    m_cur_room_id = room_id;
    return 0;
}

room_value_t *c_user_cache::get_cur_room()
{
    return m_p_cur_room;
}

uint32_t c_user_cache::get_cur_room_id()
{
    return m_cur_room_id;
}

int c_user_cache::update_room(room_update_t *p_room)
{
    if (NULL == p_room)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }

    if (ROOM_DEFAULT_ID == p_room->room_id)
    {
        if (m_p_default_room != NULL)
        {
            m_p_default_room->buf_len = p_room->buf_len;
            memcpy(m_p_default_room->buf, p_room->buf, p_room->buf_len);
        }
    }
    else if (m_cur_room_id == (int)p_room->room_id)
    {
        if (m_p_cur_room != NULL)
        {
            m_p_cur_room->buf_len = p_room->buf_len;
            memcpy(m_p_cur_room->buf, p_room->buf, p_room->buf_len);
        }
    }
    return 0;
}

int c_user_cache::add_room_id(uint32_t *p_room_id, int p_room_num)
{
    if (NULL == p_room_id)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }
    set<uint32_t>::iterator iter = m_room_set.find(0);
    if (iter != m_room_set.end())
    {
        ERROR_LOG("all room id is already in cache");
        return -1;
    }
    m_room_set.insert(0);
    for (int i = 0; i != p_room_num; ++i)
    {
        m_room_set.insert(p_room_id[i]);
    }

    return 0;
}

int c_user_cache::get_all_room_id(uint32_t *p_room_id, int *p_room_num)
{
    if (NULL == p_room_id)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }
    set<uint32_t>::iterator iter = m_room_set.find(0);
    if (iter == m_room_set.end())
    {
        return -1;
    }

    int i = 0;
    for (set<uint32_t>::iterator iter = m_room_set.begin(); iter != m_room_set.end(); ++iter)
    {
        if (0 == *iter)
        {
            continue;
        }
        p_room_id[i++] = *iter;
    }
    *p_room_num = i;
    return 0;
}

int c_user_cache::get_bag(db_msg_get_bag_rsp_t *p_bag)
{
    if (NULL == p_bag)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }
    if (!is_stuff_exist(0))
    {
        return -1;
    }

    int i = 0;
    for (map<uint32_t, stuff_num_t>::const_iterator stuff_iter = m_bag_map.begin(); stuff_iter != m_bag_map.end(); ++stuff_iter)
    {
        if ( 0 == stuff_iter->first)
        {
            continue;
        }
        p_bag->stuff[i].stuff_id = stuff_iter->first;
        p_bag->stuff[i].stuff_num = stuff_iter->second.stuff_num;
        p_bag->stuff[i].used_num = stuff_iter->second.used_num;
        ++i;
    }
    p_bag->num = i;

    return 0;
}

int c_user_cache::add_bag(const db_msg_get_bag_rsp_t *p_bag)
{
    if (NULL == p_bag)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }
    if (is_stuff_exist(0))
    {
        ERROR_LOG("bag is already in cache");
        return -1;
    }

    stuff_num_t stuff = {0};
    m_bag_map.insert(pair<uint32_t, stuff_num_t>(0, stuff));

    for (int i = 0; i != p_bag->num; ++i)
    {
        stuff_num_t stuff = {p_bag->stuff[i].stuff_num, p_bag->stuff[i].used_num};
        m_bag_map.insert(pair<uint32_t, stuff_num_t>(p_bag->stuff[i].stuff_id, stuff));
    }

    return 0;
}

int c_user_cache::get_stuff_num(uint32_t stuff_id)
{
    if (!is_stuff_exist(0))
    {
        ERROR_LOG("bag is not in cache");
        return -1;
    }

    map<uint32_t, stuff_num_t>::const_iterator num_iter = m_bag_map.find(stuff_id);
    if (num_iter == m_bag_map.end())
    {
        return 0;
    }
    else
    {
        return num_iter->second.stuff_num - num_iter->second.used_num;
    }
}

void c_user_cache::add_stuff(uint32_t stuff_id, uint16_t stuff_num)
{
    if (!is_stuff_exist(0))
    {
        return;
    }

    map<uint32_t, stuff_num_t>::iterator iter = m_bag_map.find(stuff_id);
    if (iter == m_bag_map.end())
    {
        stuff_num_t stuff = {stuff_num , 0};
        m_bag_map.insert(pair<uint32_t, stuff_num_t>(stuff_id, stuff));
    }
    else
    {
        iter->second.stuff_num += stuff_num;
    }
    return;
}

int c_user_cache::desc_stuff(uint32_t stuff_id, uint16_t stuff_num)
{
    if (!is_stuff_exist(0))
    {
        return 0;
    }

    map<uint32_t, stuff_num_t>::iterator iter = m_bag_map.find(stuff_id);
    if (iter == m_bag_map.end())
    {
        return 0;
    }

    if (iter->second.stuff_num - stuff_num < iter->second.used_num)
    {
        ERROR_LOG("cann't desc stuff[%u:%u]", stuff_id, stuff_num);
        return -1;
    }
    iter->second.stuff_num -= stuff_num;
    return 0;
}

int c_user_cache::add_stuff_used(uint32_t stuff_id, uint16_t used_num)
{
    if (!is_stuff_exist(0))
    {
        return 0;
    }

    map<uint32_t, stuff_num_t>::iterator iter = m_bag_map.find(stuff_id);
    if (iter == m_bag_map.end())
    {
        return 0;
    }
    if (iter->second.used_num + used_num > iter->second.stuff_num)
    {
        ERROR_LOG("cann't use stuff[%u:%u]", stuff_id, used_num);
        return -1;
    }

    iter->second.used_num += used_num;
    return 0;
}

int c_user_cache::desc_stuff_used(uint32_t stuff_id, uint16_t used_num)
{
    if (!is_stuff_exist(0))
    {
        return 0;
    }

    map<uint32_t, stuff_num_t>::iterator iter = m_bag_map.find(stuff_id);
    if (iter == m_bag_map.end())
    {
        return 0;
    }
    if (iter->second.used_num < used_num)
    {
        ERROR_LOG("cann't desc used stuff[%u:%u]", stuff_id, used_num);
        return -1;
    }

    iter->second.used_num -= used_num;
    return 0;
}

bool c_user_cache::is_stuff_exist(uint32_t stuff_id)
{
    map<uint32_t, stuff_num_t>::iterator iter = m_bag_map.find(stuff_id);
    if (iter == m_bag_map.end())
    {
        return false;
    }
    return true;
}

int c_user_cache::get_all_puzzle(db_msg_puzzle_rsp_t *p_puzzle)
{
    if (NULL == p_puzzle)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }
    if (get_puzzle(-1) == NULL)
    {
        return -1;
    }

    int i = 0;
    map<int, puzzle_info_t>::iterator puzzle_iter = m_puzzle_map.begin();
    for (; puzzle_iter != m_puzzle_map.end(); ++puzzle_iter)
    {
        if (-1 == puzzle_iter->first)
        {
            continue;
        }
        p_puzzle->puzzle[i].type = puzzle_iter->first;
        p_puzzle->puzzle[i].last_playtime = puzzle_iter->second.last_playtime;
        p_puzzle->puzzle[i].max_score = puzzle_iter->second.max_score;
        p_puzzle->puzzle[i].score = puzzle_iter->second.total_score;
        p_puzzle->puzzle[i].num = puzzle_iter->second.total_num;
        ++i;
    }
    p_puzzle->num = i;

    return 0;
}

int c_user_cache::add_puzzle(const db_msg_puzzle_rsp_t *p_puzzle)
{
    if (NULL == p_puzzle)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }
    if (get_puzzle(-1) != NULL)
    {
        ERROR_LOG("puzzle is already in cache");
        return -1;
    }
    puzzle_info_t puzzle = {0};
    add_puzzle_type(-1, &puzzle);

    for (int i = 0; i != p_puzzle->num; ++i)
    {
        puzzle_info_t puzzle_info = {p_puzzle->puzzle[i].last_playtime, p_puzzle->puzzle[i].max_score, p_puzzle->puzzle[i].score, p_puzzle->puzzle[i].num};
        m_puzzle_map.insert(pair<int, puzzle_info_t>(p_puzzle->puzzle[i].type, puzzle_info));
    }

    return 0;
}

int c_user_cache::add_puzzle_type(int type, puzzle_info_t *p_puzzle)
{
    if (NULL == p_puzzle)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }
    if (get_puzzle(type) != NULL)
    {
        ERROR_LOG("puzzle:%u is already in cache", type);
        return -1;
    }

    m_puzzle_map.insert(pair<int, puzzle_info_t>(type, *p_puzzle));
    return 0;
}

puzzle_info_t *c_user_cache::get_puzzle(int type)
{
    map<int, puzzle_info_t>::iterator iter = m_puzzle_map.find(type);
    if (iter == m_puzzle_map.end())
    {
        return NULL;
    }

    return &iter->second;
}

int c_user_cache::cached_all_hole(all_hole_info_t *p_all_hole)
{
    for(int i = 0; i < PLANTATION_NUM; i++)
    {
        memcpy(&m_hole[i], &(p_all_hole->hole[i]), sizeof(hole_info_t));
    }

    m_hole_cached = 1;

    return 0;
}

int c_user_cache::get_all_hole(all_hole_info_t *p_all_hole)
{
    if(m_hole_cached == 0)
    {//坑还没有做缓存
        return 1;
    }

    p_all_hole->plant_count = 0;
    p_all_hole->hole_count = PLANTATION_NUM;
    for(int i = 0; i < PLANTATION_NUM; i++)
    {
        memcpy(&(p_all_hole->hole[i]), &(m_hole[i]), sizeof(hole_info_t));
        if(m_hole[i].plant_id != 0)
        {
            p_all_hole->plant_count += 1;
        }
    }
    return 0;
}

int c_user_cache::del_plant_at_hole(uint8_t hole_id)
{
    if(m_hole_cached == 0)
    {
        return 1;
    }

    m_hole[hole_id - 1].plant_id = 0;
    m_hole[hole_id - 1].color = 0;
    m_hole[hole_id - 1].growth = 0;
    m_hole[hole_id - 1].last_growth_time = 0;
    m_hole[hole_id - 1].last_add_extra_growth_time = 0;
    m_hole[hole_id - 1].last_growth_value = 0;
    m_hole[hole_id - 1].maintain = 0;
    m_hole[hole_id - 1].next_maintain_time = 0;

    return 0;
}

int c_user_cache::is_hole_reward_cached()
{
    return m_hole_reward_cached;
}

int c_user_cache::cache_all_hole_reward(db_all_hole_reward_rsp_t *p_all_reward)
{
    if(m_hole_reward_cached == 1)
    {
        m_hole1_reward_map.clear();
        m_hole2_reward_map.clear();
        m_hole3_reward_map.clear();
    }

    for(uint32_t idx = 0; idx < p_all_reward->reward_num; idx++)
    {
        if(p_all_reward->hole_reward[idx].hole_id == 1)
        {
            m_hole1_reward_map.insert(pair<uint32_t, hole_sun_reward_t>(p_all_reward->hole_reward[idx].reward_id, p_all_reward->hole_reward[idx].sun_reward));
        }
        else if(p_all_reward->hole_reward[idx].hole_id == 2)
        {
            m_hole2_reward_map.insert(pair<uint32_t, hole_sun_reward_t>(p_all_reward->hole_reward[idx].reward_id, p_all_reward->hole_reward[idx].sun_reward));
        }
        else if(p_all_reward->hole_reward[idx].hole_id == 3)
        {
            m_hole3_reward_map.insert(pair<uint32_t, hole_sun_reward_t>(p_all_reward->hole_reward[idx].reward_id, p_all_reward->hole_reward[idx].sun_reward));
        }
    }

    m_hole_reward_cached = 1;

    return 0;
}

int c_user_cache::del_hole_reward_i(uint8_t hole_id, uint32_t reward_id)
{
    if(hole_id == 1)
    {
            m_hole1_reward_map.erase(reward_id);
    }
    else if(hole_id == 2)
    {

            m_hole2_reward_map.erase(reward_id);
    }
    else if(hole_id == 3)
    {
            m_hole3_reward_map.erase(reward_id);
    }
    return 0;
}

int c_user_cache::get_hole_reward_i(uint8_t hole_id, uint32_t reward_id, hole_sun_reward_t *sun_reward)
{
    if(hole_id == 1)
    {
        map<uint32_t, hole_sun_reward_t>::iterator iter = m_hole1_reward_map.find(reward_id);
        if(iter != m_hole1_reward_map.end())
        {
            *sun_reward = iter->second;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if(hole_id == 2)
    {
        map<uint32_t, hole_sun_reward_t>::iterator iter = m_hole2_reward_map.find(reward_id);
        if(iter != m_hole1_reward_map.end())
        {
            *sun_reward = iter->second;
            return 0;
        }
        else
        {
            return -1;
        }

    }
    else if(hole_id == 3)
    {
        map<uint32_t, hole_sun_reward_t>::iterator iter = m_hole3_reward_map.find(reward_id);
        if(iter != m_hole1_reward_map.end())
        {
            *sun_reward = iter->second;
            return 0;
        }
        else
        {
            return -1;
        }

    }
    return -1;
}

int c_user_cache::get_hole_reward(uint8_t hole_id, single_hole_reward_t *p_hole_reward)
{
    if(hole_id == 1)
    {
        uint32_t i = 0;
       for(map<uint32_t, hole_sun_reward_t>::iterator iter = m_hole1_reward_map.begin(); iter != m_hole1_reward_map.end(); iter++)
       {
           p_hole_reward->reward_id[i] = iter->first;
           i++;
       }
       p_hole_reward->reward_num = i;
    }
    else if(hole_id == 2)
    {
        uint32_t i = 0;
       for(map<uint32_t, hole_sun_reward_t>::iterator iter = m_hole2_reward_map.begin(); iter != m_hole2_reward_map.end(); iter++)
       {
           p_hole_reward->reward_id[i] = iter->first;
           i++;
       }
       p_hole_reward->reward_num = i;

    }
    else if(hole_id == 3)
    {
        uint32_t i = 0;
       for(map<uint32_t, hole_sun_reward_t>::iterator iter = m_hole3_reward_map.begin(); iter != m_hole3_reward_map.end(); iter++)
       {
           p_hole_reward->reward_id[i] = iter->first;
           i++;
       }
       p_hole_reward->reward_num = i;

    }
    else
    {
        ERROR_LOG("unknown hole_id:%u", hole_id);
        return -1;
    }
    return 0;
}

int c_user_cache::get_all_plants(all_plant_t *p_all_plant)
{
    if (NULL == p_all_plant)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }

    map<uint8_t, plant_info_t>::const_iterator plant_iter = m_plant_map.begin();
    int i = 0;
    for (; plant_iter != m_plant_map.end(); ++plant_iter)
    {
        p_all_plant->plant[i].hole_id = plant_iter->first;
        p_all_plant->plant[i].plant_info.plant_id = plant_iter->second.plant_id;
        p_all_plant->plant[i].plant_info.color = plant_iter->second.color;
        p_all_plant->plant[i].plant_info.growth = plant_iter->second.growth;
        p_all_plant->plant[i].plant_info.last_growth_time = plant_iter->second.last_growth_time;
        p_all_plant->plant[i].plant_info.last_growth_value = plant_iter->second.last_growth_value;
        p_all_plant->plant[i].plant_info.last_add_extra_growth_time = plant_iter->second.last_add_extra_growth_time;
        ++i;
    }
    p_all_plant->count = i;
    return 0;
}

int c_user_cache::update_plant_in_hole(db_msg_add_plant_req_t *p_plant)
{
    if (NULL == p_plant)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }
	uint8_t hole_id = p_plant->hole_id;
    if (!(hole_id > 0 && hole_id <= PLANTATION_NUM))
    {
        ERROR_LOG("hole:%u is not exist", hole_id);
        return -1;
    }

    m_hole[hole_id - 1].plant_id = p_plant->plant_id;
    m_hole[hole_id - 1].color = p_plant->color;
    m_hole[hole_id - 1].last_add_extra_growth_time = p_plant->last_extra_growth_time;
    m_hole[hole_id - 1].last_growth_time = p_plant->last_grown_time;
    m_hole[hole_id - 1].maintain = p_plant->maintain;
    m_hole[hole_id - 1].next_maintain_time = p_plant->maintain_time;

    return 0;
}

void c_user_cache::del_plant(uint8_t hole_id)
{
    if (!(hole_id > 0 && hole_id < 4))
    {
        return;
    }
    m_plant_map.erase(hole_id);
}

void c_user_cache::del_all_plant()
{
    for(int i = 0; i< PLANTATION_NUM; i++)
    {
         del_plant_at_hole(m_hole[i].hole_id);
    }
    //m_plant_map.clear();
}

hole_info_t *c_user_cache::get_hole_plant(uint8_t hole_id)
{
    if(hole_id <= 0 || hole_id > PLANTATION_NUM)
    {
        return NULL;
    }

    return &m_hole[hole_id - 1];
}

int c_user_cache::update_hole_info_after_maintain(db_msg_maintain_req_t *p_req)
{
    hole_info_t *p_hole = NULL;
    hole_sun_reward_t reward;
    reward.reward_happy = p_req->reward_happy;
    reward.reward_exp = p_req->reward_exp;
    reward.reward_coins = p_req->reward_coins;

    if(p_req->hole_id == 1)
    {
        p_hole= &m_hole[0];
        m_hole1_reward_map.insert(pair<uint32_t, hole_sun_reward_t>(p_req->last_reward_id, reward));
    }
    else if(p_req->hole_id == 2)
    {
        p_hole= &m_hole[1];
        m_hole2_reward_map.insert(pair<uint32_t, hole_sun_reward_t>(p_req->last_reward_id, reward));
    }
    else if(p_req->hole_id == 3)
    {
        p_hole= &m_hole[2];
        m_hole3_reward_map.insert(pair<uint32_t, hole_sun_reward_t>(p_req->last_reward_id, reward));
    }

    if(p_hole)
    {
        p_hole->growth += p_req->add_growth;
        p_hole->last_growth_time = p_req->add_growth_time;
        p_hole->last_growth_value = p_hole->growth;
        p_hole->maintain = p_req->new_maintain_type;
        p_hole->maintain_count = p_req->maintain_count;
        p_hole->last_reward_id = p_req->last_reward_id;
    }

    return 0;
}

int c_user_cache::update_plant_info(plant_growth_t plant_growth)
{
    hole_info_t *p_hole = NULL;
    if(plant_growth.hole_id == 1)
    {
        p_hole= &m_hole[0];
    }
    else if(plant_growth.hole_id == 2)
    {
        p_hole= &m_hole[1];
    }
    else if(plant_growth.hole_id == 3)
    {
        p_hole= &m_hole[2];
    }

    if(p_hole)
    {//第一个坑
       p_hole->growth += plant_growth.growth;
       if(plant_growth.growth_time)
       {
           p_hole->last_growth_time = plant_growth.growth_time;
       }

       if(plant_growth.last_growth_value)
       {
           p_hole->last_growth_value = plant_growth.last_growth_value;
       }

       if (plant_growth.last_add_extra_growth_time)
       {
           p_hole->last_add_extra_growth_time = plant_growth.last_add_extra_growth_time;
       }

       p_hole->maintain = plant_growth.new_maintain_type;
       p_hole->next_maintain_time = plant_growth.next_maintain_time;
       p_hole->maintain_count = plant_growth.maintain_count;
       if(plant_growth.last_reward_id)
       {
           p_hole->last_reward_id = plant_growth.last_reward_id;
       }
    }

    return 0;

}

int c_user_cache::add_attract_pet(uint32_t pet_id)
{
    if (m_attract_pet != 0)
    {
        ERROR_LOG("pet:%u is already atrract", pet_id);
        return -1;
    }
    m_attract_pet = pet_id;
    return 0;
}

uint32_t c_user_cache::get_attract_pet()
{
    return m_attract_pet;
}

void c_user_cache::del_attract_pet()
{
    m_attract_pet = 0;
}


int c_user_cache::add_pet(pet_info_t *p_pet)
{
    if (NULL == p_pet)
    {
        ERROR_LOG("parameter is NULL");
        return -1;
    }


    map<uint32_t, pet_num_t>::iterator pet_iter = m_pet_map.find(p_pet->pet_id);
    if (pet_iter != m_pet_map.end())
    {
        pet_iter->second.total_num += p_pet->total_num;
        pet_iter->second.follow_num += p_pet->follow_num;
    }
    else
    {
        pet_num_t num = {p_pet->total_num, p_pet->follow_num};
        m_pet_map.insert(pair<uint32_t, pet_num_t>(p_pet->pet_id, num));
    }

    return 0;
}

int c_user_cache::del_a_pet(uint32_t pet_id)
{
    map<uint32_t, pet_num_t>::iterator pet_iter = m_pet_map.find(pet_id);
    if (pet_iter != m_pet_map.end())
    {
        if(pet_iter->second.total_num > 1)
        {
            pet_iter->second.total_num -= 1;
            pet_iter->second.follow_num -= 1;
        }
        else
        {
            m_pet_map.erase(pet_iter);
        }
    }

    return 0;
}

int c_user_cache::is_pet_exist(uint32_t pet_id)
{
    if(m_pet_map.find(pet_id) == m_pet_map.end())
    {
        return 1;
    }
    return 0;
}

void c_user_cache::get_following_pet(following_pet_t *p_pet)
{
    int count = 0;
    for (map<uint32_t, pet_num_t>::const_iterator pet_iter = m_pet_map.begin(); pet_iter != m_pet_map.end(); ++pet_iter)
    {
        if (pet_iter->second.follow_num != 0)
        {
            for (int i = 0; i != (int)pet_iter->second.follow_num; ++i)
            {
                p_pet->pet_id[count] = pet_iter->first;
                ++count;
            }
        }
    }
    p_pet->count = count;

    return;
}

int c_user_cache::get_all_pet(all_pet_t *p_pet)
{
    int count = 0;
    for (map<uint32_t, pet_num_t>::const_iterator pet_iter = m_pet_map.begin(); pet_iter != m_pet_map.end(); ++pet_iter)
    {
        p_pet->pet[count].pet_id = pet_iter->first;
        p_pet->pet[count].total_num = pet_iter->second.total_num;
        p_pet->pet[count].follow_num = pet_iter->second.follow_num;
        ++count;
    }

    p_pet->count = count;
    return 0;
}

void c_user_cache::update_day_restrict(restrict_key_t key, uint32_t value)
{
    map<restrict_key_t, uint32_t, c_restrict_cmp>::iterator iter = m_day_restrict_map.find(key);
    if (iter != m_day_restrict_map.end())
    {
        iter->second += value;
        return;
    }

    m_day_restrict_map.insert(pair<restrict_key_t, uint32_t>(key, value));
    return;

}

void c_user_cache::add_day_restrict(restrict_key_t key, uint32_t value)
{
    map<restrict_key_t, uint32_t, c_restrict_cmp>::iterator iter = m_day_restrict_map.find(key);
    if (iter != m_day_restrict_map.end())
    {
        m_day_restrict_map.erase(iter);
    }

    m_day_restrict_map.insert(pair<restrict_key_t, uint32_t>(key, value));
    return;
}

int c_user_cache::get_day_restrict(restrict_key_t key, uint32_t *p_value)
{
    map<restrict_key_t, uint32_t, c_restrict_cmp>::iterator iter = m_day_restrict_map.find(key);
    if (iter == m_day_restrict_map.end())
    {
        return -1;
    }

    *p_value = iter->second;
    return 0;
}

int c_user_cache::update_game_day_restrict(restrict_key_t key_coin, uint32_t restrict_coins, restrict_key_t key_item, uint32_t restrict_value)
{
    map<restrict_key_t, uint32_t, c_restrict_cmp>::iterator coin_iter = m_day_restrict_map.find(key_coin);
    if (coin_iter != m_day_restrict_map.end())
    {
        coin_iter->second += restrict_coins;
    }
    else
    {
        m_day_restrict_map.insert(pair<restrict_key_t, uint32_t>(key_coin, restrict_coins));
    }

    map<restrict_key_t, uint32_t, c_restrict_cmp>::iterator item_iter = m_day_restrict_map.find(key_item);
    if (item_iter != m_day_restrict_map.end())
    {
        item_iter->second += restrict_value;
    }
    else
    {
        m_day_restrict_map.insert(pair<restrict_key_t, uint32_t>(key_item, restrict_value));
    }

    return 0;
}

int c_user_cache::add_game_day_restrict(restrict_key_t key_coin, uint32_t restrict_coins, restrict_key_t key_item, uint32_t restrict_value)
{
    map<restrict_key_t, uint32_t, c_restrict_cmp>::iterator coin_iter = m_day_restrict_map.find(key_coin);
    if (coin_iter != m_day_restrict_map.end())
    {
        m_day_restrict_map.erase(coin_iter);
    }

    m_day_restrict_map.insert(pair<restrict_key_t, uint32_t>(key_coin, restrict_coins));

    map<restrict_key_t, uint32_t, c_restrict_cmp>::iterator item_iter = m_day_restrict_map.find(key_item);
    if (item_iter != m_day_restrict_map.end())
    {
        m_day_restrict_map.erase(item_iter);
    }

    m_day_restrict_map.insert(pair<restrict_key_t, uint32_t>(key_item, restrict_value));

    return 0;
}

int c_user_cache::get_game_day_restrict(restrict_key_t key_coin, restrict_key_t key_item, uint32_t *p_restrict_coins, uint32_t *p_restrict_value)
{
    map<restrict_key_t, uint32_t, c_restrict_cmp>::iterator coin_iter = m_day_restrict_map.find(key_coin);
    if (coin_iter == m_day_restrict_map.end())
    {
        return -1;
    }

    *p_restrict_coins = coin_iter->second;

    map<restrict_key_t, uint32_t, c_restrict_cmp>::iterator iter = m_day_restrict_map.find(key_item);
    if (iter == m_day_restrict_map.end())
    {
        return -1;
    }

    *p_restrict_value = iter->second;

    return 0;

}

int c_user_cache::add_pinboard(int page, pinboard_t *p_pinboard)
{
    map<int, pinboard_t *>::iterator iter = m_pinboard_map.find(page);
    if (iter != m_pinboard_map.end())
    {
        ERROR_LOG("pinboard page:%u is already in cache", page);
        return -1;
    }

    int pinboard_len = sizeof(pinboard_t) + p_pinboard->count * sizeof(pinboard_cache_t);
    pinboard_t *p_pinboard_cache = reinterpret_cast<pinboard_t *>(g_slice_alloc(pinboard_len));
    if (NULL == p_pinboard_cache)
    {
        ERROR_LOG("alloc pinboard failed");
        return -1;
    }
    memcpy(p_pinboard_cache, p_pinboard, pinboard_len);
    m_pinboard_map.insert(pair<int, pinboard_t *>(page, p_pinboard_cache));
    for (int i = 0; i != p_pinboard->count; ++i)
    {
        m_pinboard_id_map.insert(pair<int, pinboard_message_t *>(p_pinboard_cache->message[i].message.id, &p_pinboard_cache->message[i].message));
    }
    return 0;
}

pinboard_t *c_user_cache::get_pinboard(int page)
{
    map<int, pinboard_t *>::iterator iter = m_pinboard_map.find(page);
    if (iter == m_pinboard_map.end())
    {
        return NULL;
    }

    return iter->second;
}

pinboard_message_t *c_user_cache::get_pinboard_by_id(int id)
{
    map<int, pinboard_message_t *>::iterator iter = m_pinboard_id_map.find(id);
    if (iter == m_pinboard_id_map.end())
    {
        return NULL;
    }

    return iter->second;
}

void c_user_cache::del_pinboard()
{
    for (map<int, pinboard_t *>::iterator iter = m_pinboard_map.begin(); iter != m_pinboard_map.end(); ++iter)
    {
        pinboard_t *p_pinboard = iter->second;
        g_slice_free1(sizeof(pinboard_t) + p_pinboard->count * sizeof(pinboard_cache_t), p_pinboard);
    }
    m_pinboard_map.clear();
    m_pinboard_id_map.clear();
}

//获得所有的成就项(0:sucess  1:未做缓存 )
int c_user_cache::get_all_badge(all_badge_info_t *p_badge)
{
    //在做了缓存的情况下，需要将缓存中记录的未读数量置为0
    if(m_badge_cached == 0)
    {
        return 1;
    }

    int i = 0;
    for(map<uint32_t, badge_info_t>::iterator iter = m_badge_map.begin(); iter != m_badge_map.end(); iter++)
    {
        p_badge->badge[i].badge_id = iter->second.badge_id;
        p_badge->badge[i].badge_status = iter->second.badge_status;
        p_badge->badge[i].badge_progress = iter->second.badge_progress;
	i++;
    }
    p_badge->badge_num = (uint16_t)i;

    m_role.role.recent_unread_badge = 0;

    return 0;
}

int c_user_cache::set_all_badge_to_no_reward()
{

    for(map<uint32_t, badge_info_t>::iterator iter = m_badge_map.begin(); iter != m_badge_map.end(); iter++)
    {
        if(iter->second.badge_status == badge_acquired_no_screen)
        {
            iter->second.badge_status = badge_acquired_no_reward;
        }
    }

    return 0;
}

int c_user_cache::add_all_badge(all_badge_info_t *p_badge)
{
     for(uint16_t i = 0; i < p_badge->badge_num; i++)
    {
        badge_info_t badge;
        badge.badge_id = p_badge->badge[i].badge_id;
        badge.badge_status = p_badge->badge[i].badge_status;
        badge.badge_progress = p_badge->badge[i].badge_progress;

        m_badge_map.insert(pair<uint32_t, badge_info_t>(badge.badge_id, badge));
    }
    m_badge_cached = 1;//将成就置为已缓存状态
    return 0;
}

int c_user_cache::set_unread_badge(uint16_t unread_badge_num)
{
    m_role.role.recent_unread_badge = unread_badge_num;
    return 0;
}

//返回未读的成就数
int c_user_cache::update_badge_status(badge_info_t *p_badge)
{
    if(m_badge_cached == 0)
    {//未做缓存
        ERROR_LOG("update_badge_status, not cached");
        return -1;
    }

    map<uint32_t, badge_info_t>::iterator iter = m_badge_map.find(p_badge->badge_id);
    if(iter != m_badge_map.end())
    {
        iter->second.badge_status = p_badge->badge_status;
        iter->second.badge_progress = p_badge->badge_progress;
    }
    else
    {
        ERROR_LOG("update_badge_status, not found badge id %u in badge_vec", p_badge->badge_id);
        return -1;
    }

    return m_role.role.recent_unread_badge;
}

/**
 * @brief is_badge_acquired   返回成就项的状态
 *
 * @param p_badge
 * @param p_need_update 用于区分是否需要更新数据库以及是否是新加入的成就项
 * @param badge_type成就项的类型，(累加类型or直接设置类型)
 *
 * @return
 */
int c_user_cache::is_badge_acquired(badge_info_t *p_badge, uint8_t *p_need_update, uint8_t badge_type)
{
    if(m_badge_cached == 0)
    {//成就项没有做缓存
        return -1;
    }

    map<uint32_t, badge_info_t>::iterator iter = m_badge_map.find(p_badge->badge_id);
    if(iter != m_badge_map.end())
    {
        if(badge_type == BADGE_TYPE_SUM)
        {//累加类型，直接将缓存中的值返回
                p_badge->badge_progress = iter->second.badge_progress;
                *p_need_update = 0;
                return iter->second.badge_status;
        }

        if(p_badge->badge_progress == iter->second.badge_progress)
        {
            *p_need_update = 2;//不需要更新数据库
        }
        else
        {//记录最新值
            if(*p_need_update == BADGE_PROGRESS_TOP)
            {
                if(p_badge->badge_progress < iter->second.badge_progress)
                {
                    p_badge->badge_progress = iter->second.badge_progress;
                }
            }
            *p_need_update = 0;
        }
        return iter->second.badge_status;
    }
    else
    {
        m_badge_map.insert(pair<uint32_t, badge_info_t>(p_badge->badge_id, *p_badge));
        *p_need_update = 1;//是新加入的
        return p_badge->badge_status;
    }

}

int c_user_cache::update_game_level(game_t *p_game)
{
    map<uint32_t, level_info_t>::iterator iter = m_game_level_map.find(p_game->game_key);
    if(iter != m_game_level_map.end())
    {
        iter->second.max_score = p_game->score;
        iter->second.star_num = p_game->star_num;
        iter->second.is_passed = p_game->is_passed;
    }
    else
    {
        level_info_t level;
        level.game_id = p_game->game_id;
        level.level_id = p_game->level_id;
        level.max_score = p_game->score;
        level.star_num = p_game->star_num;
        level.is_passed = p_game->is_passed;
        m_game_level_map.insert(pair<uint32_t, level_info_t>(p_game->game_key, level));
    }
    return 0;
}

int c_user_cache::add_game_level(uint32_t game_id, uint32_t total_lvl_num, game_level_info_t *p_game_level)
{
    char buf[24] = {0};

    uint8_t is_passed = 0;//用于记录最后一关是否已经过关
    uint16_t total_star = 0;
    uint32_t last_lvl = 0;
    uint32_t game_key = 0;
    level_info_t level;

    int i = 0;

    for(i = 0; i < p_game_level->level_num; i++)
    {
        sprintf(buf, "%u%u", game_id, p_game_level->level[i].level_id);
        game_key = atoi(buf);

        level.game_id = game_id;
        level.level_id = p_game_level->level[i].level_id;
        level.max_score = p_game_level->level[i].max_score;
        level.star_num = p_game_level->level[i].star_num;
        level.is_passed = p_game_level->level[i].is_passed;

        total_star += level.star_num;
        if(level.level_id > last_lvl)
        {
             last_lvl = level.level_id;
             is_passed = level.is_passed;
        }
        m_game_level_map.insert(pair<uint32_t, level_info_t>(game_key, level));
    }

    if(i == 0)
    {//默认第一关时解锁的
        p_game_level->unlock_next_level = UNLOCK;
    }
    else if(i == (int)total_lvl_num)
    {
        p_game_level->unlock_next_level = NO_NEXT;
    }
   else if(is_passed == 0)
   {//最后一关没有过关,不能解锁下一关
       p_game_level->unlock_next_level = NOT_UNLOCK;
   }
   else
   {//判读下一关解锁需要的其他条件,即星星数
        uint32_t next_level = i + 1;
        char key_buf[32] = {0};
        sprintf(key_buf, "%u%u", game_id, next_level);
        uint32_t key = atoi(key_buf);
        map<uint32_t, game_level_item_t>::iterator item_iter = g_game_level_map.find(key);
        if(item_iter == g_game_level_map.end())
        {
            KERROR_LOG(0, "NOT possible, key_buf:%s, game_id:%u next_level:%u", key_buf, game_id, next_level);
            return -1;
        }
        else
        {
            if(total_star >= item_iter->second.unlock_cond)
            {
                p_game_level->unlock_next_level = UNLOCK;
            }
            else
            {
                p_game_level->unlock_next_level = NOT_UNLOCK;
            }
        }
   }

    return 0;
}

/**
 * @brief get_game_level
 *
 * @param game_id游戏id
 * @param total_level_num者个游戏总共的关卡数
 * @param p_game_level
 *
 * @return
 */
int c_user_cache::get_game_level(uint32_t game_id, uint32_t total_level_num,  game_level_info_t *p_game_level)
{
    int i = 0;
    uint8_t is_passed = 0;//用于记录最后一关是否已经过关
    uint16_t total_star = 0;
    uint32_t last_lvl = 0;
   for(map<uint32_t, level_info_t>::iterator iter = m_game_level_map.begin(); iter != m_game_level_map.end(); iter++)
   {
       if(iter->second.game_id == game_id)
       {
           p_game_level->level[i].level_id = iter->second.level_id;
           p_game_level->level[i].max_score = iter->second.max_score;
           p_game_level->level[i].star_num = iter->second.star_num;
           if(iter->second.level_id > last_lvl)
           {
             last_lvl = iter->second.level_id;
             is_passed = iter->second.is_passed;
           }
           total_star += iter->second.star_num;
           i++;
       }
   }

   if(i == 0)
   {//没有缓存
       return 1;
   }

   p_game_level->level_num = i;
   if(i == (int)total_level_num)
   {
       p_game_level->unlock_next_level = NO_NEXT;
   }
   else if(is_passed == 0)
   {//最后一关没有过关,不能解锁下一关
       p_game_level->unlock_next_level = NOT_UNLOCK;
   }
   else
   {//判读下一关解锁需要的其他条件,即星星数
        uint32_t next_level = i + 1;
        char key_buf[32] = {0};
        sprintf(key_buf, "%u%u", game_id, next_level);
        uint32_t key = atoi(key_buf);
        map<uint32_t, game_level_item_t>::iterator item_iter = g_game_level_map.find(key);
        if(item_iter == g_game_level_map.end())
        {
            KERROR_LOG(0, "NOT possible, key_buf:%s, game_id:%u next_level:%u", key_buf, game_id, next_level);
            return -1;
        }
        else
        {
            if(total_star > item_iter->second.unlock_cond)
            {
                p_game_level->unlock_next_level = UNLOCK;
            }
            else
            {
                p_game_level->unlock_next_level = NOT_UNLOCK;
            }
        }
   }

    return 0;
}


/**
 * @brief level_unlocked
 *
 * @param game_id
 * @param game_key
 * @param level_id
 * @param p_need_update是否需要更新数据库
 * @param p_badge_add对应成就需要增加的值
 *
 * @return 0:错误的 ，未解锁的关卡  1：解锁的关卡
 */
int c_user_cache::level_unlocked(uint32_t game_id, uint32_t game_key, uint32_t level_id, uint32_t score, uint32_t unlock_cond,  uint8_t *p_need_update, uint32_t *p_badge_add)
{
    int i = 0;
    uint8_t is_passed = 0;//用于记录最后一关是否已经过关
    uint16_t total_star = 0;
    uint32_t last_lvl = 0;

    *p_badge_add = score;

   for(map<uint32_t, level_info_t>::iterator iter = m_game_level_map.begin(); iter != m_game_level_map.end(); iter++)
   {
       if(iter->second.game_id == game_id)
       {
           if(iter->second.level_id > last_lvl)
           {
             last_lvl = iter->second.level_id;
             is_passed = iter->second.is_passed;
           }

           if(iter->second.level_id == level_id)
           {
                if(iter->second.max_score < score)
                {
                    *p_need_update = 1;//需要更新数据库
                    *p_badge_add = score - iter->second.max_score;
                }
                else
                {
                    *p_badge_add = 0;
                }
           }

           total_star += iter->second.star_num;
           i++;
       }
   }

   if(level_id == 1)
   {
       if(last_lvl == 0)
       {
           *p_need_update = 1;//需要更新数据库
       }
        return 1;
   }

    if(is_passed == 0 && level_id > last_lvl)
    {//上一关没有完成，
        return 0;
    }

    if(level_id > last_lvl + 1)
    {//跳跃其中的部分关卡
        return 0;
    }

    if(level_id == last_lvl + 1 && total_star < unlock_cond)
    {//星星数不够 未解锁
        return 0;
    }

    if(level_id == last_lvl + 1)
    {//新的一关，必然需要更新数据库
        *p_need_update = 1;
    }

    return 1;
}

/**
 * @brief unlock_next_level 判断下一关是否解锁
 *
 * @param game_id当前的游戏id
 * @param level_id当前的关卡id
 *
 * @return
 */
int c_user_cache::unlock_next_level(uint32_t game_id, uint32_t level_id)
{
    map<uint32_t, uint32_t>::iterator game_iter = g_game_map.find(game_id);
    if(game_iter == g_game_map.end())
    {
        return -1;
    }

    if(level_id == game_iter->second)
    {//最后一关了，没有下一关了
        return NO_NEXT;
    }

    char buf[32] = {0};
    sprintf(buf, "%u%u", game_id, level_id + 1);
    uint32_t buf_key = atoi(buf);

    if(m_game_level_map.find(buf_key) != m_game_level_map.end())
    {//下一关在列表中
        return UNLOCK;//解锁
    }

    uint8_t is_passed = 0;//用于记录最后一关是否已经过关
    uint16_t total_star = 0;
    uint32_t last_lvl = 0;
   for(map<uint32_t, level_info_t>::iterator iter = m_game_level_map.begin(); iter != m_game_level_map.end(); iter++)
   {
       if(iter->second.game_id == game_id)
       {
           if(iter->second.level_id > last_lvl)
           {
             last_lvl = iter->second.level_id;
             is_passed = iter->second.is_passed;
           }
           total_star += iter->second.star_num;
       }
   }

    if(is_passed == 0)
    {//上已关未过
        return NOT_UNLOCK;//不可解锁
    }

    map<uint32_t, game_level_item_t>::iterator lvl_iter = g_game_level_map.find(buf_key);
    if(lvl_iter == g_game_level_map.end())
    {
        return -1;
    }
    else
    {
        if(total_star >= lvl_iter->second.unlock_cond)
        {
            return UNLOCK;
        }
        else
        {//星星数不够，不能解锁
            return NOT_UNLOCK;
        }
    }

    return 0;
}

int c_user_cache::get_npc_score(npc_score_info_t *p_npc)
{
    memcpy(p_npc, &m_npc_score, sizeof(npc_score_info_t));
    return 0;
}

int c_user_cache::set_npc_score(npc_score_info_t *npc_score)
{
    memcpy(&m_npc_score, npc_score, sizeof(npc_score_info_t));
    return 0;
}


/**
 * @brief get_game_changed 获得小游戏已经兑换过的物品信息
 *
 * @param game_id
 *
 * @return
 */
int c_user_cache::get_game_changed(db_msg_changed_stuff_t *p_changed_stuff)
{
    uint32_t game_id = p_changed_stuff->game_id;
    map<uint32_t, std::vector<uint32_t> > ::iterator iter = m_game_change_stuff_map.find(game_id);
    if(iter == m_game_change_stuff_map.end())
    {//尚未做缓存
        return -1;
    }

    vector<uint32_t> stuff_vec = iter->second;
    uint16_t idx = 0;
    for(vector<uint32_t>::iterator stuff_iter = stuff_vec.begin(); stuff_iter != stuff_vec.end(); stuff_iter++)
    {
        p_changed_stuff->changed_stuff[idx] = *stuff_iter;
        idx++;
    }
    p_changed_stuff->changed_stuff_num = idx;

    return 0;
}

int c_user_cache::set_game_changed(db_msg_changed_stuff_t *p_changed_stuff)
{
    uint32_t game_id = p_changed_stuff->game_id;
    map<uint32_t, std::vector<uint32_t> > ::iterator iter = m_game_change_stuff_map.find(game_id);
    if(iter != m_game_change_stuff_map.end())
    {//
        m_game_change_stuff_map.erase(iter);
    }

    std::vector<uint32_t> stuff_vec;
    for(uint16_t idx = 0; idx < p_changed_stuff->changed_stuff_num; idx++)
    {
        stuff_vec.push_back(p_changed_stuff->changed_stuff[idx]);
    }

    m_game_change_stuff_map.insert(pair<uint32_t, std::vector<uint32_t> >(game_id, stuff_vec));

    return 0;
}

int c_user_cache::update_game_changed(db_msg_get_changed_stuff_t *p_changed)
{
    uint32_t game_id = p_changed->game_id;
    map<uint32_t, std::vector<uint32_t> > ::iterator iter = m_game_change_stuff_map.find(game_id);
    if(iter == m_game_change_stuff_map.end())
    {//在缓存中未找到该游戏的信息
        return -1;
    }

    std::vector<uint32_t> stuff_vec = iter->second;

    stuff_vec.push_back(p_changed->stuff_id);

    m_game_change_stuff_map.erase(iter);

    m_game_change_stuff_map.insert(pair<uint32_t, std::vector<uint32_t> >(game_id, stuff_vec));

    return 0;
}

/**
 * @brief is_stuff_changed 判断物品是否已经兑换过
 *
 * @return
 */
int c_user_cache::is_stuff_changed(uint32_t game_id, uint32_t stuff_id)
{
    map<uint32_t, std::vector<uint32_t> > ::iterator iter = m_game_change_stuff_map.find(game_id);
    if(iter == m_game_change_stuff_map.end())
    {//没有缓存
           return -1;
    }

    std::vector<uint32_t> stuff_vec = iter->second;
    for(vector<uint32_t>::iterator stuff_iter = stuff_vec.begin(); stuff_iter != stuff_vec.end(); stuff_iter++)
    {
        if(*stuff_iter == stuff_id)
        {//已经兑换过
            return 1;
        }
    }

    return 0;
}


int c_user_cache::get_finished_task_list(as_msg_finished_task_rsp_t *p_finished_task)
{
    if(m_task_cached == 0)
    {//任务列表未做缓存
        return 1;
    }

    uint16_t finished_num = 0;
    map<task_id_t, last_reward_time_t>::iterator iter = m_finished_task_map.begin();
    for(; iter != m_finished_task_map.end(); iter++)
    {
        p_finished_task->task_id[finished_num] = iter->first;
        finished_num++;
    }

    p_finished_task->finished_task_num = finished_num;
    return 0;
}


int c_user_cache::add_finished_task(db_msg_finished_task_rsp_t *p_finished_task)
{
    m_finished_task_map.clear();

    for(int idx = 0; idx < p_finished_task->finished_task_num; idx++)
    {
        m_finished_task_map.insert(pair<task_id_t, last_reward_time_t>(p_finished_task->finished_task[idx].task_id, p_finished_task->finished_task[idx].last_reward_time));
    }

    m_task_cached = 1;

    return 0;
}


/**
 * @brief task_need_reward 判断任务是否需要奖励，另外判断任务可否接取
 *
 * @param task_id 任务id
 * @param lvl     任务接取需要的等级
 * @param pre_mission 任务接取的前置任务
 *
 * @return -1已完成任务列表未做缓存 -2:等级条件没达到  -3：前置任务未完成 0：任务无需奖励 1:任务需要奖励
 */
int c_user_cache::task_need_reward(uint32_t task_id, uint16_t lvl, uint32_t pre_mission, uint16_t reward_cycle)
{
    if(m_task_cached == 0)
    {
        return -1;
    }

    if(m_role.role.monster_level < lvl)
    {//等级条件未达到
        return -2;
    }

    if(pre_mission != 0)
    {
	if(m_finished_task_map.find(pre_mission) == m_finished_task_map.end())
    	{//前置任务未完成
        	return -3;
    	}
   }

   map<task_id_t, last_reward_time_t>::iterator iter = m_finished_task_map.find(task_id);
   if(iter == m_finished_task_map.end())
   {//需要奖励
        return 1;
   }
   else
   {
        if((time(NULL) - iter->second)/(24*60*60) > reward_cycle)
        {//需要奖励
            return 1;
        }
        else
        {//不需要奖励
            return 0;
        }
   }

    return 0;
}

int c_user_cache::update_task_list(uint32_t task_id, uint32_t reward_time)
{

   map<task_id_t, last_reward_time_t>::iterator iter = m_finished_task_map.find(task_id);
   if(iter != m_finished_task_map.end())
   {
         iter->second = reward_time;
   }

   m_finished_task_map.insert(pair<task_id_t, last_reward_time_t>(task_id, reward_time));

   return 0;
}

museum_info_t*  c_user_cache::get_museum_item(uint32_t museum_id)
{
	map<uint32_t, museum_info_t>::iterator iter = m_museum_info_map.find(museum_id);
	if(iter != m_museum_info_map.end())
	{
		return &(iter->second);
	}
	else
	{
		return NULL;
	}
}

int c_user_cache::add_museum_item(museum_info_t *museum_item)
{
	museum_info_t museum_info;
	museum_info.museum_id = museum_item->museum_id;
	museum_info.level_id = museum_item->level_id;
	museum_info.reward_flag = museum_item->reward_flag;
	museum_info.next_level_id= museum_item->next_level_id;
	museum_info.timestamp = museum_item->timestamp;

	m_museum_info_map.insert(pair<uint32_t, museum_info_t>(museum_info.museum_id, museum_info));
	return 0;
}

int c_user_cache::get_cached_open_act(single_activity_t *p_nv)
{
    if(m_open_act_cached != 1)
    {//未缓存
        return 0;
    }

    int16_t idx = 0;
    for(map<uint32_t, single_reward_t>::iterator iter = m_open_act_reward_map.begin(); iter != m_open_act_reward_map.end(); iter++)
    {
        p_nv->reward[idx].reward_id = iter->second.reward_id;
        p_nv->reward[idx].reward_num = iter->second.reward_num;
        p_nv->reward[idx].reward_status = iter->second.reward_status;
        idx++;
    }
    p_nv->kind_num = idx;

    return 1;
}

int c_user_cache::set_cached_open_act(single_activity_t *p_nv)
{
    m_open_act_reward_map.clear();
    for(uint16_t idx = 0; idx < p_nv->kind_num; idx++)
    {
        m_open_act_reward_map.insert(pair<uint32_t, single_reward_t>(p_nv->reward[idx].reward_id, p_nv->reward[idx]));
    }

    m_open_act_cached = 1;
    return 0;
}

int c_user_cache::can_get_open_reward(uint32_t reward_id)
{
    if(m_open_act_cached == 0)
    {
        return 0;
    }

    map<uint32_t, single_reward_t>::iterator iter = m_open_act_reward_map.find(reward_id);
    if(iter == m_open_act_reward_map.end())
    {
        return 0;
    }
    else
    {
        if(iter->second.reward_status != CAN_GET)
        {
            return 0;
        }
        else
        {
            return iter->second.reward_num;
        }
    }

    return 0;
}

int c_user_cache::update_cached_open_act(uint32_t reward_id)
{
    if(m_open_act_cached == 0)
    {
        return 0;
    }

    map<uint32_t, single_reward_t>::iterator iter = m_open_act_reward_map.find(reward_id);
    if(iter == m_open_act_reward_map.end())
    {
        return 0;
    }
    else
    {
        iter->second.reward_status = HAVE_GET;
    }

    return 0;
}

int c_user_cache::get_peer_real(uint32_t peer_id)
{
    if(m_peer_msg.peer_id == peer_id)
    {
        return m_peer_msg.real_num;
    }
    else
    {
        return -1;
    }

}

int c_user_cache::set_peer_real(peer_real_msg_num_t *p_peer_msg)
{
    m_peer_msg.peer_id = p_peer_msg->peer_id;
    m_peer_msg.real_num = p_peer_msg->real_num;
	return 0;
}
