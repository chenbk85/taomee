/**
 * =====================================================================================
 *       @file  c_friend.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  09/13/2011 09:26:00 AM
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
#include "c_friend.h"

using std::map;
using std::set;
using std::pair;
using std::vector;
using std::list;

int c_friend::add_friend_list(const db_msg_friend_list_rsp_t *p_friend_list)
{
    for (int i = 0; i != p_friend_list->count; ++i)
    {
        m_friend_list_map.insert(pair<uint32_t, friend_info_t>(p_friend_list->friend_info[i].friend_id, p_friend_list->friend_info[i]));
        if (p_friend_list->friend_info[i].is_bestfriend)
        {
            m_best_friend_id_list.push_back(p_friend_list->friend_info[i].friend_id);
        }
        else
        {
            m_normal_friend_id_list.push_back(p_friend_list->friend_info[i].friend_id);
        }
    }
    is_add_friend_list = true;

    return 0;
}

int c_friend::add_other_friend_list(uint32_t peer_id, const db_msg_friend_list_rsp_t *p_friend_list)
{
    if (m_other_user_id != peer_id)
    {
        KCRIT_LOG(m_user_id, "add friend:%u list is not %u", peer_id, m_other_user_id);
        return -1;
    }

    for (int i = 0; i != p_friend_list->count; ++i)
    {
        m_other_friend_list_map.insert(pair<uint32_t, friend_info_t>(p_friend_list->friend_info[i].friend_id, p_friend_list->friend_info[i]));
        if (p_friend_list->friend_info[i].is_bestfriend)
        {
            m_other_best_friend_id_vec.push_back(p_friend_list->friend_info[i].friend_id);
        }
        else
        {
            m_other_normal_friend_id_vec.push_back(p_friend_list->friend_info[i].friend_id);
        }
    }
    is_add_other_friend_list = true;
    KINFO_LOG(m_user_id, "user:%u other friend list true", peer_id);

    return 0;
}

int c_friend::get_friend_list(int min, int max, db_msg_friend_list_rsp_t *p_friend_list)
{
    if (!is_add_friend_list)
    {
        return -1;
    }

    int best_friend_num = m_best_friend_id_list.size();
    int normal_friend_num = m_normal_friend_id_list.size();

    if (0 == best_friend_num && 0 == normal_friend_num)
    {
        p_friend_list->count = 0;
        return 0;
    }

    if (max <= best_friend_num) //获取的全是最爱好友
    {
        int i = 1;
        for (list<uint32_t>::iterator iter = m_best_friend_id_list.begin(); iter != m_best_friend_id_list.end(); ++iter)
        {
            if (i < min)
            {
                ++i;
                continue;
            }

            p_friend_list->friend_info[i - min].friend_id = *iter;
            p_friend_list->friend_info[i - min].is_bestfriend = 1;
            if (++i > max)
            {
                break;
            }
        }

        p_friend_list->count = max - min + 1;
    }
    else if (min > best_friend_num) //获取的全是普通好友
    {
        int i = best_friend_num + 1;
        for (list<uint32_t>::iterator iter = m_normal_friend_id_list.begin(); iter != m_normal_friend_id_list.end(); ++iter)
        {
            if (i < min)
            {
                ++i;
                continue;
            }

            p_friend_list->friend_info[i - min].friend_id = *iter;
            p_friend_list->friend_info[i - min].is_bestfriend = 0;
            if (++i > max)
            {
                break;
            }
        }
        p_friend_list->count = i - min;
    }
    else                //既有最爱好友，又有普通好友
    {
        int i = 1;
        for (list<uint32_t>::iterator iter = m_best_friend_id_list.begin(); iter != m_best_friend_id_list.end(); ++iter)
        {
            if (i < min)
            {
                ++i;
                continue;
            }

            p_friend_list->friend_info[i - min].friend_id = *iter;
            p_friend_list->friend_info[i - min].is_bestfriend = 1;
            ++i;
        }

        for (list<uint32_t>::iterator iter = m_normal_friend_id_list.begin(); iter != m_normal_friend_id_list.end(); ++iter)
        {
            p_friend_list->friend_info[i - min].friend_id = *iter;
            p_friend_list->friend_info[i - min].is_bestfriend = 0;
            if (++i > max)
            {
                break;
            }
        }
        p_friend_list->count = i - min;
    }

    return 0;
}

int c_friend::is_friend(uint32_t peer_id)
{
    if (!is_add_friend_list)    //好没有好友列表
    {
        return -1;
    }

    map<uint32_t, friend_info_t>::iterator iter = m_friend_list_map.find(peer_id);
    if (iter == m_friend_list_map.end())
    {
        return 0;
    }

    return 1;
}

int c_friend::get_friend_num()
{
    if (!is_add_friend_list)    //没有好友列表
    {
        return -1;
    }

    return m_friend_list_map.size();
}

int c_friend::set_best_friend(uint32_t peer_id, int is_best_friend)
{
    map<uint32_t, friend_info_t>::iterator iter = m_friend_list_map.find(peer_id);
    if (iter == m_friend_list_map.end())
    {
        KCRIT_LOG(m_user_id, "cann't find friend:%u", peer_id);
        return -1;
    }
    iter->second.is_bestfriend = is_best_friend;

    int is_found = false;
    if (is_best_friend) //从普通好友成为最爱好友
    {
        for (list<uint32_t>::iterator iter = m_normal_friend_id_list.begin(); iter != m_normal_friend_id_list.end(); ++iter)
        {
            if (*iter == peer_id)
            {
                is_found = true;
                m_normal_friend_id_list.erase(iter);
                m_best_friend_id_list.push_front(peer_id);
                break;
            }
        }
        if (!is_found)
        {
            KCRIT_LOG(m_user_id, "user:%u is not best friend", peer_id);
            return -1;
        }
    }
    else                //从最爱好友成为普通好友
    {
        for (list<uint32_t>::iterator iter = m_best_friend_id_list.begin(); iter != m_best_friend_id_list.end(); ++iter)
        {
            if (*iter == peer_id)
            {
                is_found = true;
                m_best_friend_id_list.erase(iter);
                m_normal_friend_id_list.push_front(peer_id);
                break;
            }
        }
        if (!is_found)
        {
            KCRIT_LOG(m_user_id, "user:%u is not normal friend", peer_id);
            return -1;
        }
    }

    return 0;
}

int c_friend::del_friend(uint32_t peer_id)
{
    map<uint32_t, friend_info_t>::iterator iter = m_friend_list_map.find(peer_id);
    if (iter == m_friend_list_map.end())
    {
        KCRIT_LOG(m_user_id, "cann't find friend:%u", peer_id);
        return -1;
    }

    if (iter->second.is_bestfriend)
    {
        for (list<uint32_t>::iterator friend_iter = m_best_friend_id_list.begin(); friend_iter != m_best_friend_id_list.end(); ++friend_iter)
        {
            if (*friend_iter == peer_id)
            {
                m_best_friend_id_list.erase(friend_iter);
                break;
            }
        }
    }
    else
    {
        for (list<uint32_t>::iterator friend_iter = m_normal_friend_id_list.begin(); friend_iter != m_normal_friend_id_list.end(); ++friend_iter)
        {
            if (*friend_iter == peer_id)
            {
                m_normal_friend_id_list.erase(friend_iter);
                break;
            }
        }
    }

    m_friend_list_map.erase(iter);

    return 0;
}

int c_friend::add_friend(uint32_t user_id, const friend_list_t *p_friend_list, bool new_friend)
{
    if (user_id == m_user_id)
    {
        add_self_friend(p_friend_list, new_friend);
    }
    else if (user_id == m_other_user_id)
    {
        add_other_friend(p_friend_list);
    }
    else
    {
        return -1;
    }

    return 0;
}

int c_friend::add_other_friend(const friend_list_t *p_friend_list)
{
    uint32_t peer_id = p_friend_list->user_id;
    KINFO_LOG(m_user_id, "add other friend:%u", peer_id);

    if (p_friend_list->is_best_friend)
    {
        m_other_best_friend_map.insert(pair<uint32_t, friend_list_t>(peer_id, *p_friend_list));
    }
    else
    {
        m_other_normal_friend_map.insert(pair<uint32_t, friend_list_t>(peer_id, *p_friend_list));
    }

    return 0;
}

int c_friend::add_self_friend(const friend_list_t *p_friend_list, bool new_friend)
{
    uint32_t peer_id = p_friend_list->user_id;

    if (p_friend_list->is_best_friend)
    {
        m_best_friend_map.insert(pair<uint32_t, friend_list_t>(peer_id, *p_friend_list));
    }
    else
    {
        m_normal_friend_map.insert(pair<uint32_t, friend_list_t>(peer_id, *p_friend_list));
    }

    if (new_friend)
    {
        friend_info_t friend_info = {peer_id, p_friend_list->user_type, p_friend_list->is_best_friend};
        m_friend_list_map.insert(pair<uint32_t, friend_info_t>(peer_id, friend_info));   //保存用户的好友列表
        if (p_friend_list->is_best_friend)
        {
            m_best_friend_id_list.push_front(peer_id);
        }
        else
        {
            m_normal_friend_id_list.push_front(peer_id);
        }
    }

    return 0;
}

int c_friend::get_friend_tree(uint32_t user_id, int min, int max, friend_rsp_t *p_friend, uint16_t *p_all_friend_num)
{
    if (NULL == p_friend || NULL == p_all_friend_num)
    {
        KERROR_LOG(m_user_id, "get user:%u friend info, parameter is NULL.", user_id);
        return -1;
    }

    if (user_id == m_user_id)
    {
        return get_self_friend_tree(min, max, p_friend, p_all_friend_num);
    }
    else
    {
        return get_other_friend_tree(user_id, min, max, p_friend, p_all_friend_num);
    }
}

int c_friend::get_self_friend_tree(int min, int max, friend_rsp_t *p_friend, uint16_t *p_all_friend_num)
{
    char buffer[1024] = {0};
    db_msg_friend_list_rsp_t *p_friend_list = (db_msg_friend_list_rsp_t *)buffer;
    if (get_friend_list(min, max, p_friend_list) != 0)
    {
        KCRIT_LOG(m_user_id, "get friend list failed");
        return -1;
    }

    int best_friend_num = m_best_friend_id_list.size();
    int normal_friend_num = m_normal_friend_id_list.size();
    *p_all_friend_num = best_friend_num + normal_friend_num;
    KINFO_LOG(m_user_id, "get friend[%u:%u], all friend:%u", min, max, *p_all_friend_num);

    for (int i = 0; i != p_friend_list->count; ++i)
    {
        uint32_t peer_id = p_friend_list->friend_info[i].friend_id;
        if (p_friend_list->friend_info[i].is_bestfriend)
        {
            map<uint32_t, friend_list_t>::iterator iter = m_best_friend_map.find(peer_id);
            if (iter == m_best_friend_map.end())
            {
               // KWARN_LOG(m_user_id, "get peer_id:%u from m_best_friend_map failed.", peer_id);
                return -1;
            }
            memcpy(&p_friend->friend_list[i], &iter->second, sizeof(friend_list_t));
        }
        else
        {
            map<uint32_t, friend_list_t>::iterator iter = m_normal_friend_map.find(peer_id);
            if (iter == m_normal_friend_map.end())
            {
               // KWARN_LOG(m_user_id, "get peer_id:%u from m_normal_friend_map failed.", peer_id);
                return -1;
            }
            memcpy(&p_friend->friend_list[i], &iter->second, sizeof(friend_list_t));
        }

    }
    p_friend->count = p_friend_list->count;

    return 0;
}

int c_friend::get_other_friend_tree(uint32_t user_id, int min, int max, friend_rsp_t *p_friend, uint16_t *p_all_friend_num)
{
    KINFO_LOG(m_user_id, "get other friend tree:%u", user_id);
    //好友树还没有缓存过
    if (user_id != m_other_user_id)
    {
        if (!m_other_user_id)   //还保存有其他用户的好友树，先清空
        {
            del_friend_tree(false);
        }
        m_other_user_id = user_id;
        return -1;
    }

    char buffer[1024] = {0};
    db_msg_friend_list_rsp_t *p_friend_list = (db_msg_friend_list_rsp_t *)buffer;
    if (get_other_friend_list(min, max, p_friend_list) != 0)
    {
        KCRIT_LOG(m_user_id, "get friend list failed");
        return -1;
    }

    int best_friend_num = m_other_best_friend_id_vec.size();
    int normal_friend_num = m_other_normal_friend_id_vec.size();
    *p_all_friend_num = best_friend_num + normal_friend_num;

    for (int i = 0; i != p_friend_list->count; ++i)
    {
        uint32_t peer_id = p_friend_list->friend_info[i].friend_id;
        if (p_friend_list->friend_info[i].is_bestfriend)
        {
            map<uint32_t, friend_list_t>::iterator iter = m_other_best_friend_map.find(peer_id);
            if (iter == m_other_best_friend_map.end())
            {
                return -1;
            }
            memcpy(&p_friend->friend_list[i], &iter->second, sizeof(friend_list_t));
        }
        else
        {
            map<uint32_t, friend_list_t>::iterator iter = m_other_normal_friend_map.find(peer_id);
            if (iter == m_other_normal_friend_map.end())
            {
                return -1;
            }
            memcpy(&p_friend->friend_list[i], &iter->second, sizeof(friend_list_t));
        }

    }
    p_friend->count = p_friend_list->count;
    KINFO_LOG(m_user_id, "get other friends:%u", p_friend->count);

    return 0;
}

int c_friend::get_other_friend_list(int min, int max, db_msg_friend_list_rsp_t *p_friend_list)
{
    if (!is_add_other_friend_list)
    {
        return -1;
    }

    int best_friend_num = m_other_best_friend_id_vec.size();
    int normal_friend_num = m_other_normal_friend_id_vec.size();

    if (0 == best_friend_num && 0 == normal_friend_num)
    {
        p_friend_list->count = 0;
        return 0;
    }

    if (max <= best_friend_num) //获取的全是最爱好友
    {
        int i = 1;
        for (vector<uint32_t>::iterator iter = m_other_best_friend_id_vec.begin(); iter != m_other_best_friend_id_vec.end(); ++iter)
        {
            if (i < min)
            {
                ++i;
                continue;
            }

            p_friend_list->friend_info[i - min].friend_id = *iter;
            p_friend_list->friend_info[i - min].is_bestfriend = 1;
            if (++i > max)
            {
                break;
            }
        }

        p_friend_list->count = max - min + 1;
    }
    else if (min > best_friend_num) //获取的全是普通好友
    {
        int i = best_friend_num + 1;
        for (vector<uint32_t>::iterator iter = m_other_normal_friend_id_vec.begin(); iter != m_other_normal_friend_id_vec.end(); ++iter)
        {
            if (i < min)
            {
                ++i;
                continue;
            }

            p_friend_list->friend_info[i - min].friend_id = *iter;
            p_friend_list->friend_info[i - min].is_bestfriend = 0;
            if (++i > max)
            {
                break;
            }
        }
        p_friend_list->count = i - min;
    }
    else                //既有最爱好友，又有普通好友
    {
        int i = 1;
        for (vector<uint32_t>::iterator iter = m_other_best_friend_id_vec.begin(); iter != m_other_best_friend_id_vec.end(); ++iter)
        {
            if (i < min)
            {
                ++i;
                continue;
            }

            p_friend_list->friend_info[i - min].friend_id = *iter;
            p_friend_list->friend_info[i - min].is_bestfriend = 1;
            ++i;
        }

        for (vector<uint32_t>::iterator iter = m_other_normal_friend_id_vec.begin(); iter != m_other_normal_friend_id_vec.end(); ++iter)
        {
            p_friend_list->friend_info[i - min].friend_id = *iter;
            p_friend_list->friend_info[i - min].is_bestfriend = 0;
            if (++i > max)
            {
                break;
            }
        }
        p_friend_list->count = i - min;
    }

    return 0;
}
