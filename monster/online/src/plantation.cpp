/*
 * =====================================================================================
 *
 *       Filename:  plantation.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年03月08日 10时44分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <assert.h>
#include <map>
#include <vector>
#include <set>
#include <stdio.h>

#include <libtaomee++/inet/pdumanip.hpp>
#include "message.h"
#include "stat.h"
#include "function.h"
#include "plantation.h"
#include "data.h"
#include "utility.h"
#include "badge.h"

using namespace std;



/**
 * @brief update_plant_growth 更新植物的成长值
 *
 * @param p_user
 * @param p_all_plant
 * @param enter_plantation 是否时进种植园时的调用(0:不是 1：是)
 *
 * @return 返回发生成长值变化的个数
 */
int update_plant_growth(usr_info_t *p_user, uint32_t mon_happy, all_hole_info_t *p_all_hole, db_msg_update_plant_req_t *p_change, uint8_t enter_plantation)
{
    if(p_user == NULL)
    {
        return -1;
    }

    if(p_all_hole == NULL)
    {
        KCRIT_LOG(p_user->uid, "para p_all_hole is NULL");
        return -1;
    }

    if(p_change == NULL)
    {
        KCRIT_LOG(p_user->uid, "para p_change is NULL");
        return -1;
    }

    if(p_all_hole->plant_count == 0)
    {//没有植物在种植园里面
        KINFO_LOG(p_user->uid, "No plant in plantation");
        return 0;
    }




    time_t now = time(NULL);
    int ret = 0;


    p_change->count = p_all_hole->hole_count;

    for(int i = 0; i < p_all_hole->hole_count; i++)
    {
        KINFO_LOG(p_user->uid, "parameter plant[hole_id:%u plant_id:%u growth:%u, last growth_time:%u, last_growth_value:%u, last_extra_time:%u, maintain:%u,next_maintain_time:%u, last_reward_id:%u, maintain_cout:%u]", p_all_hole->hole[i].hole_id, p_all_hole->hole[i].plant_id, p_all_hole->hole[i].growth, p_all_hole->hole[i].last_growth_time, p_all_hole->hole[i].last_growth_value,  p_all_hole->hole[i].last_add_extra_growth_time, p_all_hole->hole[i].maintain, p_all_hole->hole[i].next_maintain_time, p_all_hole->hole[i].last_reward_id, p_all_hole->hole[i].maintain_count);

        p_change->plant_growth[i].hole_id = p_all_hole->hole[i].hole_id;
        if(p_all_hole->hole[i].plant_id == 0)
        {
            p_change->plant_growth[i].growth = 0;
            p_change->plant_growth[i].growth_time = 0;
            p_change->plant_growth[i].last_add_extra_growth_time = 0;
            p_change->plant_growth[i].new_maintain_type = 0;
            p_change->plant_growth[i].next_maintain_time = 0;
            p_change->plant_growth[i].last_growth_value = 0;
            p_change->plant_growth[i].last_reward_id = 0;
            continue;
        }

        p_change->plant_growth[i].growth = get_plant_growth_add(mon_happy, p_all_hole->hole[i].last_growth_time);
        p_change->plant_growth[i].growth_time = p_change->plant_growth[i].growth ? now : 0;
        p_change->plant_growth[i].last_add_extra_growth_time = 0;
        p_change->plant_growth[i].new_maintain_type = p_all_hole->hole[i].maintain;
        p_change->plant_growth[i].next_maintain_time = p_all_hole->hole[i].next_maintain_time;
        p_change->plant_growth[i].last_reward_id = 0;
        p_change->plant_growth[i].maintain_count = p_all_hole->hole[i].maintain_count;

        if(enter_plantation)
        {//进种植园，有额外的成长值
            if(now - p_all_hole->hole[i].last_add_extra_growth_time >= g_plant_time)
            {//两次进种植园的时间超过预设的时间，获得额外的成长值
                p_change->plant_growth[i].growth += g_extra_growth;
                p_change->plant_growth[i].last_add_extra_growth_time = now;
            }
            else
            {
                p_change->plant_growth[i].last_add_extra_growth_time = 0;
            }


            //判断维护选项
            if(time(NULL) > p_all_hole->hole[i].next_maintain_time)
            {//当前时间大于新一轮维护开始时间，进行新的维护选线逻辑判断
                p_change->plant_growth[i].new_maintain_type = get_new_maintain();
                if(p_change->plant_growth[i].new_maintain_type != NOT_MAINTAIN)
                {
                    p_change->plant_growth[i].next_maintain_time = time(NULL) + g_maintain_plant_time * 60;
                }
                else
                {
                    p_change->plant_growth[i].next_maintain_time = p_all_hole->hole[i].next_maintain_time;
                }
                p_change->plant_growth[i].maintain_count = 0;
            }
        }

        if(p_all_hole->hole[i].growth >= MATURITY_GROWTH_VALUE)
        {
            p_change->plant_growth[i].growth = 0;
            p_change->plant_growth[i].new_maintain_type = MAINTAIN_GET;
        }
        else if (p_all_hole->hole[i].growth + p_change->plant_growth[i].growth >= MATURITY_GROWTH_VALUE)
        {//已有的成长值加上本次增加的成长值
            p_change->plant_growth[i].growth = MATURITY_GROWTH_VALUE - p_all_hole->hole[i].growth;
            p_change->plant_growth[i].new_maintain_type = MAINTAIN_GET;
        }

        p_change->plant_growth[i].hole_id = p_all_hole->hole[i].hole_id;
        if(enter_plantation)
        {// 如果是进入种植园，更新用户上次看到的成长值
            p_change->plant_growth[i].last_growth_value = p_all_hole->hole[i].growth + p_change->plant_growth[i].growth;
        }
        else
        {
            p_change->plant_growth[i].last_growth_value = 0;
        }

        KINFO_LOG(p_user->uid, "enter_plantation:%u hole:%u plant:%u growth(本次增加):%u last_growth_time:%u last_extra_growth_time:%u now_value(增加后的成长值):%u, new_maintain_type:%u", enter_plantation, p_change->plant_growth[i].hole_id, p_all_hole->hole[i].plant_id, p_change->plant_growth[i].growth, p_change->plant_growth[i].growth_time, p_change->plant_growth[i].last_add_extra_growth_time, p_change->plant_growth[i].last_growth_value, p_change->plant_growth[i].new_maintain_type);

        if(p_change->plant_growth[i].growth != 0)
        {
            ret++;
        }

    }

    if(!enter_plantation && ret == 0)
    {//不是进入种植园，并且没有成长值变化
        return 0;
    }

    svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;
    db_msg_update_plant_req_t *p_body = (db_msg_update_plant_req_t*)(g_send_buffer + sizeof(svr_msg_header_t));
    int update_len = sizeof(db_msg_update_plant_req_t) + p_change->count * sizeof(plant_growth_t);
    memcpy(p_body, p_change, update_len);
    pack_svr_pkg_head(p_header, svr_msg_db_update_plant, p_user->uid, GET_SVR_SN(p_user), update_len);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
         g_errno = ERR_MSG_DB_PROXY;
         KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
         return 0;
    }

    return ret;
}

int get_loop_maintain(uint8_t maintain_count)
{
    if(maintain_count >= MAX_MAINTAIN_COUNT_PER_QUARTER)
    {
        return NOT_MAINTAIN;
    }

    int rand = uniform_rand(0, 100);
    if(rand < 80)
    {
        return NOT_MAINTAIN;
    }
    else
    {
        return get_new_maintain();
    }
    return 0;
}


/**
 * @brief get_new_maintain
 *
 * @return 返回新的维护类型
 */
int get_new_maintain()
{
    int random = uniform_rand(0, 100);
    for(map<uint8_t, maintain_type_t>::iterator iter = g_maintain_map.begin(); iter != g_maintain_map.end(); iter++)
    {
        if(random <= iter->second.rate)
        {
            return iter->second.type;
        }
    }
    return 0;
}

int update_plant_info_in_cache(usr_info_t *p_user, db_msg_update_plant_req_t *p_change)
{
    if(p_user == NULL)
    {
        return -1;
    }

    if(p_change == NULL)
    {
        KCRIT_LOG(p_user->uid, "para p_all_plant is NULL");
        return -1;
    }

    for(int i = 0; i < (int)p_change->count; i++)
    {
        p_user->user_cache.update_plant_info(p_change->plant_growth[i]);
    }

    return 0;
}

int update_plant_info_after_eat_food(usr_info_t *p_user, uint32_t old_happy, db_msg_update_plant_req_t *p_change)
{
    if(p_user == NULL)
    {
        return -1;
    }

    if(p_change == NULL)
    {
        KCRIT_LOG(p_user->uid, "p_change is NULL");
        return -1;
    }

    char buffer[1024] = {0};
    all_hole_info_t *p_all_hole = (all_hole_info_t*)buffer;
    if(p_user->user_cache.get_all_hole(p_all_hole) != 0)
    {
        KCRIT_LOG(p_user->uid, "get all plant in cache failed");
        return -1;
    }

    if(p_all_hole->plant_count == 0 || all_plant_maturity(p_all_hole))
    {//么有植物，或者植物都已经成熟
        send_header_to_user(p_user, 0, ERR_NO_ERR);
        notify_as_current_user_info(p_user);
        return 0;
    }
    int change_num = update_plant_growth(p_user, old_happy, p_all_hole, p_change, 0);
    if(change_num == 0)
    {
        send_header_to_user(p_user, 0, ERR_NO_ERR);
        notify_as_current_user_info(p_user);
        return 0;
    }

    return 0;
}

int update_plant_info_after_commit_puzzle_return(usr_info_t *p_user, puzzle_cache_t *p_puzzle_cache)
{
    if(p_user == NULL)
    {
        return -1;
    }

    char buffer[1024] = {0};
    all_hole_info_t *p_all_hole = (all_hole_info_t *)buffer;
    if (p_user->user_cache.get_all_hole(p_all_hole) != 0)
    {
        KCRIT_LOG(p_user->uid, "get plant in cache");
        return -1;
    }

    db_msg_puzzle_req_t *p_puzzle = &(p_puzzle_cache->puzzle_reward);
    int extra_coin = p_puzzle_cache->extra_coins;
	KINFO_LOG(p_user->uid, "waitcmd:%u extra_coin:%d, reward_coin:%u, old_happy:%u", p_user->waitcmd, extra_coin, p_puzzle->reward.coins, p_puzzle_cache->old_happy);

    if (p_all_hole->plant_count == 0 || all_plant_maturity(p_all_hole))    //植物都成熟，不用更新db里植物的成长值
    {//没有植物或植物都已经成熟
        KINFO_LOG(p_user->uid, "plant_count:%u or all plant have been matured", p_all_hole->plant_count);
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_puzzle->puzzle.type);
        g_send_msg.pack((uint16_t)(p_puzzle->reward.coins - extra_coin));
        g_send_msg.pack(p_puzzle->reward.exp);
        g_send_msg.pack(p_puzzle->reward.happy);
        g_send_msg.pack((uint16_t)extra_coin);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        notify_as_current_user_info(p_user);

        if(p_puzzle->puzzle.type == DAILY_PUZZLE)
        {
			        encourage_guide_to_db(p_user, DAILY_TEST, p_puzzle->puzzle.score);
        }

        return 0;
    }

    int change_num = update_plant_growth(p_user, p_puzzle_cache->old_happy, p_all_hole, &(p_puzzle_cache->plant_change), 0);
    if (0 == change_num)    //植物都成熟，不用更新db里植物的成长值
    {
        KINFO_LOG(p_user->uid, "no plant changed growth");
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_puzzle->puzzle.type);
        g_send_msg.pack((uint16_t)(p_puzzle->reward.coins - extra_coin));
        g_send_msg.pack(p_puzzle->reward.exp);
        g_send_msg.pack(p_puzzle->reward.happy);
        g_send_msg.pack((uint16_t)extra_coin);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        notify_as_current_user_info(p_user);
        if(p_puzzle->puzzle.type == DAILY_PUZZLE)
        {
			        encourage_guide_to_db(p_user, DAILY_TEST, p_puzzle->puzzle.score);
        }
        return 0;
    }

    return 0;
}

/**
 * @brief all_plant_maturity 判断所有植物是否成熟
 *
 * @param p_all_hole
 *
 * @return 0:未全部成熟 1：全部成熟
 */
int all_plant_maturity(all_hole_info_t *p_all_hole)
{
    if(p_all_hole->plant_count != p_all_hole->hole_count)
    {
        return 0;
    }
    for(int i = 0; i < p_all_hole->hole_count; i++)
    {
        if(p_all_hole->hole[i].growth < MATURITY_GROWTH_VALUE)
        {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief pet_match 进行精灵的匹配
 *
 * @param p_all_hole
 * @param pet_id
 * @param p_seed_match
 *
 * @return
 */
int pet_match(all_hole_info_t *p_all_hole, uint32_t *p_pet_id, int *p_seed_match)
{
    int max_match_num = 0;
    //查看常见小怪有没有匹配的
    for (map<uint32_t, map<seed_match_t, int, c_seed_cmp> >::iterator iter = g_common_pet.begin(); iter != g_common_pet.end(); ++iter)
    {
        uint32_t pet_id = iter->first;
        map<seed_match_t, int, c_seed_cmp> seed_map = iter->second;
        int match_num = 0;
        int seed_match[PLANTATION_NUM] = {0};

        for (int i = 0; i < p_all_hole->hole_count; ++i)
        {
            int color = p_all_hole->hole[i].color;
            int plant_id = p_all_hole->hole[i].plant_id;

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
            return 0;
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
        int seed_match[PLANTATION_NUM] = {0};

        for (int i = 0; i < p_all_hole->hole_count; ++i)
        {
            int color = p_all_hole->hole[i].color;
            int plant_id = p_all_hole->hole[i].plant_id;

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
            return 0;
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

        for (int i = 0; i < p_all_hole->hole_count; ++i)
        {
            int color = p_all_hole->hole[i].color;
            int plant_id = p_all_hole->hole[i].plant_id;

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
            return 0;
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
        std::map<seed_match_t, int, c_seed_cmp> seed_map = iter->second;
        int match_num = 0;
        int seed_match[3] = {0};

        for (int i = 0; i < p_all_hole->hole_count; ++i)
        {
            int color = p_all_hole->hole[i].color;
            int plant_id = p_all_hole->hole[i].plant_id;

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
            return 0;
        }
        else if (match_num > max_match_num)
        {
            max_match_num = match_num;
            memcpy(p_seed_match, seed_match, sizeof(seed_match));
        }
    }

    return 0;
}

int process_plantation_after_all_hole(usr_info_t *p_user, all_hole_info_t *p_all_hole)
{
    if(p_user == NULL || p_all_hole == NULL)
    {
        return -1;
    }

    if(p_all_hole->plant_count == 0)
    {//没有植物
        //先打包各个坑的信息
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_all_hole->hole_count);
        for(int i = 0; i < (int)p_all_hole->hole_count; i++)
        {
            g_send_msg.pack(p_all_hole->hole[i].plant_id);
            g_send_msg.pack(p_all_hole->hole[i].hole_id);
            g_send_msg.pack(p_all_hole->hole[i].color);
            g_send_msg.pack(p_all_hole->hole[i].last_growth_value);
            g_send_msg.pack(p_all_hole->hole[i].growth);
            g_send_msg.pack(p_all_hole->hole[i].maintain);
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

        g_send_msg.pack((uint8_t)PLANTATION_UNATTRACT_PET);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        KINFO_LOG(p_user->uid, "No plant in plantation");
        return 0;
    }

    //判断植物是否全部成熟
    if(all_plant_maturity(p_all_hole) == 1)
    {//全部成熟,判断精灵匹配情况

        //判断精灵匹配情况
        int seed_match[PLANTATION_NUM] = {0};
        uint32_t pet_id = 0;
        pet_match(p_all_hole, &pet_id, seed_match);

        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        //先打包各个坑的信息
        g_send_msg.pack(p_all_hole->hole_count);
        for(int i = 0; i < (int)p_all_hole->hole_count; i++)
        {
            g_send_msg.pack(p_all_hole->hole[i].plant_id);
            g_send_msg.pack(p_all_hole->hole[i].hole_id);
            g_send_msg.pack(p_all_hole->hole[i].color);
            g_send_msg.pack(p_all_hole->hole[i].last_growth_value);
            g_send_msg.pack(p_all_hole->hole[i].growth);
            if(pet_id == 0)
            {//没有匹配成功，点击收获
                g_send_msg.pack((uint8_t)MAINTAIN_GET);
            }
            else
            {//匹配成功，不需要维护
                g_send_msg.pack((uint8_t)NOT_MAINTAIN);
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

        //统计有没有匹配到精灵
        stat_one_param_t stat_data = {1};

        if(pet_id == 0)
        {//没有匹配到精灵
              g_send_msg.pack((uint8_t)PLANTATION_NOT_MATCH);
              for (int i = 0; i < PLANTATION_NUM; ++i)
              {
                  g_send_msg.pack((uint8_t)(seed_match[i] == 0 ? 0 : 1));
              }
              KINFO_LOG(p_user->uid, "not match pet");
              msg_log(stat_not_attract_pet, &stat_data, sizeof(stat_data));
        }
        else
        {//匹配到了精灵
             g_send_msg.pack((uint8_t)PLANTATION_ATTRACT_PET);
             g_send_msg.pack(pet_id);
             KINFO_LOG(p_user->uid, "matched pet %u", pet_id);
             p_user->user_cache.add_attract_pet(pet_id);
             msg_log(stat_attract_pet, &stat_data, sizeof(stat_data));
        }
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

        return 0;
    }

    //更新植物的成长值
        //待写
     uint8_t enter_plantation = 1;
     role_cache_t *p_role = p_user->user_cache.get_role();
     if(p_role == NULL)
     {
         KCRIT_LOG(p_user->uid, "get user role cache failed in process_plantation_after_all_hole.");
         return -1;
     }


     uint32_t mon_cur_happy = get_new_happy(time(NULL), p_role->role.last_login_time, p_role->role.monster_happy);
     db_msg_update_plant_req_t *p_change = (db_msg_update_plant_req_t*)p_user->buffer;
      int ret =  update_plant_growth(p_user, mon_cur_happy, p_all_hole, p_change, enter_plantation);
      if(ret == -1)
      {
          return -1;
      }
      return 0;
}

int process_sun_reward_exp(usr_info_t *p_user, sun_reward_exp_cache_t *p_cache, uint32_t day_restrict)
{
    if(p_user == NULL || p_cache == NULL)
    {
        return -1;
    }
    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "get cached role info failed.");
        return -1;
    }

    //经验值不能超过每天的上限
    if((int)day_restrict >= g_max_exp_in_plant)
    {
        p_cache->reward_exp = 0;
    }
    else if((int)(day_restrict + p_cache->reward_exp) > g_max_exp_in_plant)
    {
        p_cache->reward_exp = g_max_exp_in_plant - day_restrict;
    }

    int now_level = get_level_from_exp(p_role->role.monster_exp + p_cache->reward_exp);
    if (now_level > p_role->role.monster_level && p_role->role.monster_happy > LEVEL_HAPPY_VALUE && p_role->role.monster_health > LEVEL_HEALTH_VALUE)
    {
        p_cache->level_up = now_level - p_role->role.monster_level;
         //可以升级
    }
    else    //不能升级
    {
         uint16_t max_left_exp = get_level_left_exp(p_role->role.monster_exp, p_role->role.monster_level);
         if (max_left_exp < p_cache->reward_exp)  //不能升级，则经验加满就不能再加了
         {
             p_cache->reward_exp = max_left_exp;
         }
         p_cache->level_up = 0;
   }

      svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;

      int body_len = sizeof(sun_reward_exp_cache_t);
      KINFO_LOG(p_user->uid, "sun-reward-exp:hole_id:%u, reward_id:%u,reward_exp:%u level_up:%u", p_cache->sun_reward.hole_id, p_cache->sun_reward.reward_id, p_cache->reward_exp, p_cache->level_up);

        //发送请求
      pack_svr_pkg_head(p_header, svr_msg_db_sun_reward_exp, p_user->uid, GET_SVR_SN(p_user), body_len);

      memcpy((char*)p_header + sizeof(svr_msg_header_t), (char*)p_cache, body_len);

      if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
     {
         g_errno = ERR_MSG_DB_PROXY;
         KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
         return 0;
      }

    return 0;
}
