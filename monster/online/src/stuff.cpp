/*
 * =====================================================================================
 *
 *       Filename:  stuff.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年03月08日 16时46分01秒
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
#include "stuff.h"
#include "cli_proto.h"
#include "data.h"
#include "utility.h"
#include "badge.h"
using namespace std;


int process_buy_stuff_after_get_restrict(usr_info_t *p_user, commit_stuff_cache_t *p_cm_stuff,  uint32_t day_restrict)
{
    if(p_user == NULL)
    {
        return -1;
    }

    if(p_cm_stuff == NULL)
    {
        KCRIT_LOG(p_user->uid, "para p_cm_stuff is NULL");
        return -1;
    }

    role_cache_t *p_role = p_user->user_cache.get_role();
    if (NULL == p_role)
    {
        KCRIT_LOG(p_user->uid, "get cached role info failed.");
        return -1;
    }

    uint32_t reward_exp = p_cm_stuff->add_stuff.coins / 5; //没消耗5个咕咚果奖励1点经验

    reward_exp = get_current_gains(reward_exp, p_user); //防成谜

    //经验值不能超过每天的上限
    if((int)day_restrict >= g_max_day_stuff_exp)
    {
        p_cm_stuff->add_stuff.reward_exp = 0;
    }
    else if((int)(day_restrict + reward_exp) >= g_max_day_stuff_exp)
    {
        p_cm_stuff->add_stuff.reward_exp = g_max_day_stuff_exp - day_restrict;
    }
    else
    {
        p_cm_stuff->add_stuff.reward_exp = (uint16_t)reward_exp;
    }

    int now_level = get_level_from_exp(p_role->role.monster_exp + p_cm_stuff->add_stuff.reward_exp);
    if (now_level > p_role->role.monster_level && p_role->role.monster_happy > LEVEL_HAPPY_VALUE && p_role->role.monster_health > LEVEL_HEALTH_VALUE)
    {
        p_cm_stuff->add_stuff.level_up = now_level - p_role->role.monster_level;
         //可以升级
    }
    else    //不能升级
    {
         uint16_t max_left_exp = get_level_left_exp(p_role->role.monster_exp, p_role->role.monster_level);
         if (max_left_exp < p_cm_stuff->add_stuff.reward_exp)  //不能升级，则经验加满就不能再加了
         {
             p_cm_stuff->add_stuff.reward_exp = max_left_exp;
         }
         p_cm_stuff->add_stuff.level_up = 0;
   }


      svr_msg_header_t *p_header = (svr_msg_header_t*)g_send_buffer;

      int body_len = sizeof(db_msg_add_stuff_req2_t);

    KINFO_LOG(p_user->uid, "send buy_stuff to db: count:%u coins:%u, happy:%u, reward-exp:%u, levl_up:%u, stuff_id:%u stuff_num:%u, body_len:%u",p_cm_stuff->add_stuff.count,  p_cm_stuff->add_stuff.coins, p_cm_stuff->add_stuff.happy, p_cm_stuff->add_stuff.reward_exp, p_cm_stuff->add_stuff.level_up, p_cm_stuff->add_stuff.stuff[0].stuff_id, p_cm_stuff->add_stuff.stuff[0].stuff_num, body_len);
        //发送请求
      pack_svr_pkg_head(p_header, svr_msg_db_add_bag_stuff, p_user->uid, GET_SVR_SN(p_user), body_len);

      memcpy((char*)p_header + sizeof(svr_msg_header_t), (char*)&(p_cm_stuff->add_stuff), body_len);

      if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
     {
         g_errno = ERR_MSG_DB_PROXY;
         KCRIT_LOG(p_user->uid, "send data to db proxy failed.");
         return 0;
      }

    return 0;
}


int update_plant_info_after_buy_stuff(usr_info_t *p_user, commit_stuff_cache_t *p_sf_cache)
{
    if(p_user == NULL)
    {
        return -1;
    }

    if(p_sf_cache == NULL)
    {
        KCRIT_LOG(p_user->uid, "para p_puzzle_cache is NULL");
        return -1;
    }

    char buffer[1024] = {0};
    all_hole_info_t *p_all_hole = (all_hole_info_t*)buffer;
    if(p_user->user_cache.get_all_hole(p_all_hole) != 0)
    {
        KCRIT_LOG(p_user->uid, "get plant in cache failed.");
        return -1;
    }

    if(0 == p_all_hole->plant_count || all_plant_maturity(p_all_hole) || p_sf_cache->add_stuff.happy == 0)
    {//没有植物或者植物豆成熟 或者 没有增加愉悦值， 直接给用户返回
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_sf_cache->add_stuff.stuff[0].stuff_id);
        g_send_msg.pack(p_sf_cache->category);
        g_send_msg.pack(p_sf_cache->add_stuff.reward_exp);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        notify_as_current_user_info(p_user);
        return 0;

    }


    int change_num = update_plant_growth(p_user, p_sf_cache->old_happy, p_all_hole, &(p_sf_cache->plant_change), 0);
    if(0 == change_num)
    {//没有植物的成长值发生变化
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_sf_cache->add_stuff.stuff[0].stuff_id);
        g_send_msg.pack(p_sf_cache->category);
        g_send_msg.pack(p_sf_cache->add_stuff.reward_exp);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        notify_as_current_user_info(p_user);
        return 0;

    }

    return 0;
}

int unlock_map_item(usr_info_t *p_user, uint16_t mon_level)
{
    if(p_user == NULL)
    {
		KINFO_LOG(0, "unlock p_user is NUL");
        return -1;
    }

        vector<uint32_t> unlock_vec;
        //遍历所有图纸，看有没有新的图纸可以解锁
        for (map<uint32_t, factory_unlock_t>::iterator iter = g_factory_unlock_map.begin(); iter != g_factory_unlock_map.end(); ++iter)
        {
            if (iter->second.unlock_type == CONDITION_LEVEL && iter->second.value <= mon_level)
            {
                if(p_user->user_cache.get_stuff_num(iter->first) == 0)
                {
                    unlock_vec.push_back(iter->first);
                }
            }
        }

        if (unlock_vec.size() > 0)
        {
            //把新解锁的物品插入item表
            pack_as_pkg_header(p_user->uid, add_new_unlock_map, 0, ERR_NO_ERR);
            g_send_msg.pack((uint16_t)unlock_vec.size());
            for (vector<uint32_t>::iterator iter = unlock_vec.begin(); iter != unlock_vec.end(); ++iter)
            {
                g_send_msg.pack(*iter);
            }
            g_send_msg.end();
            cache_a_pkg(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        }

    return 0;
}
