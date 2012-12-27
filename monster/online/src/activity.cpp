/*
 * =====================================================================================
 *
 *       Filename:  activity.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年05月30日 11时11分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <map>
#include "c_user_cache.h"
#include "online_structure.h"
#include "message.h"
#include "constant.h"
#include "function.h"
#include "cli_proto.h"
#include "data.h"
#include "stat.h"

#include "activity.h"

using namespace std;

int get_cur_activity_from_conf(activity_list_t *p_list)
{
    uint16_t cur_act_num = 0;
    for(map<uint32_t, activity_info_t>::iterator iter = g_activity_map.begin(); iter != g_activity_map.end(); iter++)
    {
        if(iter->second.activity_id == LIMIT_ACTIVITY_ID)
        {//跳过封测活动
            continue;
        }
        if(iter->second.start_timestamp <= time(NULL) && iter->second.end_timestamp >= time(NULL))
        {//当前时间处于活动时间之内
            p_list->activity[cur_act_num].activity_id = iter->second.activity_id;
            p_list->activity[cur_act_num].activity_status = ACTIVITY_STATUS_RUNNING;//目前全部取值未进行中
            cur_act_num++;
        }
    }

    p_list->activity_num = cur_act_num;
    return 0;
}

int get_a_activity_info(usr_info_t *p_user, uint32_t act_id)
{
    switch(act_id)
    {
        case OPEN_ACTIVITY_ID:
            process_open_activity(p_user, act_id);
            break;
        case INVITOR_ACTIVITY_ID:
            process_invitor_activity(p_user, act_id);
            break;
        case DRAGON_BOAT_ACTIVITY_ID:
            process_dragon_boat_activity(p_user, act_id);
            break;
        default:
            KCRIT_LOG(p_user->uid, "Should not come here activity_id:%u", act_id);
            g_errno = ERR_SYSTEM_ERR;
            return -1;
    }

    return 0;

}

int get_actinfo_from_conf(single_activity_t *p_act)
{
    map<uint32_t, activity_info_t>::iterator iter = g_activity_map.find(p_act->activity_id);
    if(iter != g_activity_map.end())
    {
        vector<task_reward_t> act_reward_vec = iter->second.reward_vec;
        uint16_t idx = 0;
        for(vector<task_reward_t>::iterator reward_iter = act_reward_vec.begin(); reward_iter != act_reward_vec.end(); reward_iter++)
        {
            p_act->reward[idx].reward_id = reward_iter->reward_id;
            p_act->reward[idx].reward_num = reward_iter->reward_num;
            p_act->reward[idx].reward_status = CANNOT_GET;
            idx++;
        }
        p_act->kind_num = idx;
        return 0;
    }
    else
    {
        return -1;
    }
}


int get_act_reward(usr_info_t *p_user, as_request_activity_reward_t req)
{
    uint32_t act_id = req.activity_id;

    switch(act_id)
    {
        case OPEN_ACTIVITY_ID:
            process_open_act_reward(p_user, req.activity_id, req.reward_id);
            break;
        default:
            KCRIT_LOG(p_user->uid, "Should not come here get_act_reward activity_id:%u", act_id);
            g_errno = ERR_SYSTEM_ERR;
            return -1;

    }

    return 0;

}


int process_open_activity(usr_info_t *p_user, uint32_t act_id)
{
    char buffer[2048] ={0};
    single_activity_t *p_act = (single_activity_t*)buffer;
    p_act->activity_id = act_id;

    if(p_user->user_cache.get_cached_open_act(p_act) == 0)
    {//缓存中没有，从db取
        KINFO_LOG(p_user->uid, "Not cached activity reward, get from db");
        svr_msg_header_t *p_head = (svr_msg_header_t*)g_send_buffer;

        pack_svr_pkg_head(p_head, svr_msg_db_get_activity_reward, p_user->uid, GET_SVR_SN(p_user), sizeof(uint32_t));
        uint32_t *p_body = (uint32_t*)(g_send_buffer + sizeof(svr_msg_header_t));
        *p_body = act_id;
        if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy failed");
            return 0;
        }
    }
    else
    {
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
    }
    return 0;
}

int process_open_act_reward(usr_info_t *p_user, uint32_t act_id,  uint32_t reward_id)
{
    uint16_t reward_num = (uint16_t)p_user->user_cache.can_get_open_reward(reward_id);
    if(reward_num != 0)
    {//满足领奖条件,返回奖品个数
        KINFO_LOG(p_user->uid, "get act %u reward %u reward_num %u", act_id, reward_id, reward_num);

        svr_msg_header_t *p_head = (svr_msg_header_t*)g_send_buffer;

        pack_svr_pkg_head(p_head, svr_msg_db_get_open_reward, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_get_open_reward_t));
        db_msg_get_open_reward_t *p_body = (db_msg_get_open_reward_t*)(g_send_buffer + sizeof(svr_msg_header_t));
        p_body->act_id = act_id;
        p_body->reward_id = reward_id;
        p_body->reward_num = reward_num;
        if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
        {
            g_errno = ERR_MSG_DB_PROXY;
            KCRIT_LOG(p_user->uid, "send to db proxy failed");
            return 0;
        }
    }
    else
    {//不满足领奖条件
        g_errno = ERR_NOT_REACH_REWARD_CONDITION;
        KCRIT_LOG(p_user->uid, "act_id:%u reward_id:%u not reached condition", act_id, reward_id);
        return -1;
    }
    return 0;
}

int process_invitor_activity(usr_info_t *p_user, uint32_t act_id)
{
    svr_msg_header_t *p_head = (svr_msg_header_t*)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_get_invite_info, p_user->uid, GET_SVR_SN(p_user), sizeof(uint32_t));

	uint32_t *db_req = (uint32_t *)(g_send_buffer + sizeof(svr_msg_header_t));
	*db_req = act_id;
	 
    if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed");
        return 0;
    }
    return 0;
}

int process_dragon_boat_activity(usr_info_t *p_user, uint32_t act_id)
{
	svr_msg_header_t *p_head = (svr_msg_header_t*)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_get_dragon_boat_info, p_user->uid, GET_SVR_SN(p_user), 0);
	if(g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
	{
		g_errno = ERR_MSG_DB_PROXY;
		KCRIT_LOG(p_user->uid, "send to db proxy failed");
	}

    return 0;
}

int process_get_dragon_boat_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
	assert(p_user != NULL && p_db_pkg != NULL);
	if (p_db_pkg->result != 0)
	{
	  g_errno = p_db_pkg->result;
	  KCRIT_LOG(p_user->uid, "db get dragon boat failed:%u", p_db_pkg->result);
	  return 0;
	}

	db_return_get_dragon_boat_t *res = (db_return_get_dragon_boat_t *)p_db_pkg->body;
	pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
	g_send_msg.pack(res->is_reward);
	for(uint8_t i = 0; i < 3; i++)
	{
		g_send_msg.pack(res->status[i]);
	}
	KINFO_LOG(p_user->uid, "send dragon boat info to AS: is_reward:%u", res->is_reward);
	g_send_msg.end();
	send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    
	return 0;
}

int process_update_dragon_boat_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
	assert(p_user != NULL && p_db_pkg != NULL);
	if (p_db_pkg->result != 0)
	{
	  g_errno = p_db_pkg->result;
	  KCRIT_LOG(p_user->uid, "db get invite info failed:%u", p_db_pkg->result);
	  return 0;
	}

	pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
	db_return_update_dragon_boat_t *res = (db_return_update_dragon_boat_t *)(p_db_pkg->body);
	g_send_msg.pack(res->status);
	KINFO_LOG(p_user->uid, "send dragon boat relative info to AS: is_reward:%u", res->status);
	g_send_msg.end();
	send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

	if(!res->status)
	{
		stat_one_param_t stat_data = {p_user->uid};
		switch(res->level_id)
		{
		case 1:
			msg_log(stat_get_leaf_a, &stat_data, sizeof(stat_data));
		case 2:
			msg_log(stat_get_leaf_b, &stat_data, sizeof(stat_data));
		case 3:
			msg_log(stat_get_leaf_c, &stat_data, sizeof(stat_data));
		default:
			KCRIT_LOG(p_user->uid, "dragon boat level id error");
		}
	}
	
	return 0;
}

int process_get_invite_info_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
	assert(p_user != NULL && p_db_pkg != NULL);
	if (p_db_pkg->result != 0)
	{
	  g_errno = p_db_pkg->result;
	  KCRIT_LOG(p_user->uid, "db get invite info failed:%u", p_db_pkg->result);
	  return 0;
	}

	pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
	db_return_get_invite_info_t *p_body = (db_return_get_invite_info_t *)(p_db_pkg->body);
	g_send_msg.pack(p_body->invite_num);
	g_send_msg.pack(p_body->qualified_num);
	for(uint8_t i=0; i<3; i++)
	{
		g_send_msg.pack(p_body->level_status[i]);
	}

	KINFO_LOG(p_user->uid, "send invite info to AS: invite_num:%u qualified_num:%u", p_body->invite_num, p_body->qualified_num);
	g_send_msg.end();
	send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    
	return 0;
}

int process_get_activity_reward_ex_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg)
{
	assert(p_user != NULL && p_db_pkg != NULL);
	if (p_db_pkg->result != 0)
	{
	  g_errno = p_db_pkg->result;
	  KCRIT_LOG(p_user->uid, "db get invite reward failed:%u", p_db_pkg->result);
	  return 0;
	}
	
	db_return_get_activity_reward_ex_t *p_body = (db_return_get_activity_reward_ex_t *)(p_db_pkg->body);

	if(p_body->reward_id == TASK_REWARD_COIN)
	{
		(p_user->user_cache.get_role())->role.coins += p_body->reward_num;
	}
	else if(p_body->reward_id == TASK_REWARD_EXP)
	{	
	}
	else if(p_body->reward_id >= ITEM_ID_BEGIN && p_body->reward_id <= ITEM_ID_END)
	{
		p_user->user_cache.add_stuff(p_body->reward_id, p_body->reward_num);
	}
	else if(p_body->reward_id >= PET_ID_BEGIN && p_body->reward_id <= PET_ID_END)
	{
		pet_info_t pet = {p_body->reward_id, p_body->reward_num, 1};
		 // 缓存里增加跟随的小怪兽
		if (p_user->user_cache.add_pet(&pet) != 0)
		{
		  KCRIT_LOG(p_user->uid, "add following pet");
		  return -1;
		}
		KINFO_LOG(p_user->uid, "add pet:%u to cache, number:%u.", p_body->reward_id, p_body->reward_num);
	
		//增加小怪兽的数量
		role_cache_t *p_role = p_user->user_cache.get_role();
		if (p_role == NULL)
		{
		  KCRIT_LOG(p_user->uid, "get role cache");
		  return -1;
		}
		
		p_role->role.pet_num += 1;
		//增加成就项
		map<uint32_t, uint32_t>::iterator iter = g_pet_series_map.find(pet.pet_id);
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
						KCRIT_LOG(p_user->uid, "unknown pet_id %u not in g_pet_series", p_body->reward_id);
						break;
			 }
		}
	}
	else if(p_body->reward_id == TASK_REWARD_NONE)
	{
	}
	else
	{
		KCRIT_LOG(p_user->uid, "reward id error.");
		return -1;
	}
	
	pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);

	KINFO_LOG(p_user->uid, "send get activity reward to AS: reward_id:%u reward_num:%u", p_body->reward_id, p_body->reward_num);
	g_send_msg.end();
	send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

	if(p_body->reward_id == TASK_REWARD_COIN)
	{
		notify_as_current_user_info(p_user);
	}
	return 0;
}



int activity_open(usr_info_t *p_user, uint32_t act_id, single_activity_t *p_sa)
{
    //先判断当前时间是否处于活动的时间段内
    uint32_t time_now = (uint32_t)time(NULL);
    map<uint32_t, activity_info_t>::iterator iter = g_activity_map.find(act_id);
    if(iter == g_activity_map.end())
    {//不存在的活动id
        return -1;
    }

    if(iter->second.start_timestamp <= time_now && iter->second.end_timestamp >= time_now)
    {//处于活动时间内的
        if(p_sa == NULL)
        {
            char buffer[2048] = {0};
            p_sa = (single_activity_t*)buffer;
            p_sa->activity_id = act_id;
            if(p_user->user_cache.get_cached_open_act(p_sa) == 0)
            {//活动项未做缓存
//                    KCRIT_LOG(p_user->uid, "error act_id:%u not in cache", p_sa->activity_id);
                    return -1;
            }
        }

        uint16_t have_get_reward_num = 0;
        for(uint16_t idx = 0; idx < p_sa->kind_num; idx++)
        {
            if(p_sa->reward[idx].reward_status == HAVE_GET)
            {//活动奖品已经领取
                have_get_reward_num++;
            }
            else
            {
                role_cache_t *p_role = p_user->user_cache.get_role();
                if(p_role == NULL)
                {
                        p_sa->reward[idx].reward_status =  CANNOT_GET;
                        continue;
                }

                p_sa->reward[idx].reward_status = CANNOT_GET;

                switch(p_sa->reward[idx].reward_id)
                {
                    case OPEN_ACT_REWARD3:
                    case OPEN_ACT_REWARD1:
                        {
                            if(iter->second.start_timestamp <= p_role->role.last_login_time && iter->second.end_timestamp >= p_role->role.last_login_time)
                            {//可以领取
                                p_sa->reward[idx].reward_status = CAN_GET;
                            }
                            else
                            {
                                have_get_reward_num++;
                            }
                            break;
                        }
                    case OPEN_ACT_REWARD2:
                        {
                            if(iter->second.start_timestamp <= p_role->role.last_logout_time
                                    && iter->second.end_timestamp >= p_role->role.last_logout_time
                                    && iter->second.start_timestamp <= p_role->role.last_login_time
                                    && iter->second.end_timestamp >= p_role->role.last_login_time
                                    && timestamp_equal_day(p_role->role.last_logout_time , p_role->role.last_login_time) == 0)
                            {//不是同一天登录
                                p_sa->reward[idx].reward_status = CAN_GET;
                            }
                            else
                            {
                                have_get_reward_num++;
                            }
                            break;
                        }
                    case OPEN_ACT_REWARD4:
                        {
                            if(p_role->role.monster_level >= 8)
                            {//等级达到8级
                                p_sa->reward[idx].reward_status = CAN_GET;
                            }
                            else
                            {
                                have_get_reward_num++;
                            }
                            break;
                        }
                    default:
                        p_sa->reward[idx].reward_status = CANNOT_GET;
                        break;
                }
            }
        }

        if(have_get_reward_num == p_sa->kind_num)
        {
            //不需要更新数据库，所有奖品都已经领取或者没有更新发生
            KINFO_LOG(p_user->uid, "NOt update activity %u status, have_get_reward_num:%u kind_num:%u", act_id, have_get_reward_num, p_sa->kind_num);
            return 0;
        }
        else
        {
            KINFO_LOG(p_user->uid, "cache activity_update_status as AS pkg %u ", act_id);
                pack_as_pkg_header(p_user->uid, activity_update_status, 0, ERR_NO_ERR);
                g_send_msg.pack(p_sa->activity_id);
                g_send_msg.pack(p_sa->kind_num);
                for(uint16_t idx = 0; idx < p_sa->kind_num; idx++)
                {
                    g_send_msg.pack(p_sa->reward[idx].reward_id);
                    g_send_msg.pack(p_sa->reward[idx].reward_num);
                    g_send_msg.pack(p_sa->reward[idx].reward_status);
                }
                g_send_msg.end();
                cache_a_pkg(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                return 0;
        }
    }
    else
    {//活动已经过期了
        KCRIT_LOG(p_user->uid, "activity_id %u have expired", act_id);
           return 0;
    }

    return 0;
}

