/**
 * =====================================================================================
 *       @file  c_friend.h
 *      @brief 操作好友树信息的类
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/20/2011 09:11:54 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_FREIND_H_20110913
#define H_FRIEND_H_20110913

#include <map>
#include <set>
#include <list>
#include <vector>

extern "C"
{
#include <libtaomee/log.h>
}

#include "online_structure.h"

class c_friend
{
    public:
        c_friend(): m_inited(0), m_user_id(0), m_other_user_id(0), is_add_friend_list(false) { }
        ~c_friend() { uninit(); }

        int init(uint32_t user_id);
        int uninit();

        //添加用户的好友列表
        int add_friend_list(const db_msg_friend_list_rsp_t *p_friend_list);
        int add_other_friend_list(uint32_t peer_id, const db_msg_friend_list_rsp_t *p_friend_list);
        //获得排序在min和max之间的用户的好友列表
        int get_friend_list(int min, int max, db_msg_friend_list_rsp_t *p_friend_list);
        int get_other_friend_list(int min, int max, db_msg_friend_list_rsp_t *p_friend_list);
        //判断用户是否是好友,-1:出错，0:不是好友，1:是好友
        int is_friend(uint32_t peer_id);
        //获得好友数量
        int get_friend_num();
        //用户的好友列表是否存在
        bool friend_list_exist() { return is_add_friend_list; }

        //获得用户排序在min和max之间的好友树信息
        int get_friend_tree(uint32_t user_id, int min, int max, friend_rsp_t *p_friend, uint16_t *p_all_friend_num);
        //将db获取的好友信息添加到用户的好友树,new_friend表示是否是拉取好友树后有增加的好友
        int add_friend(uint32_t user_id, const friend_list_t *p_friend_list, bool new_friend = false);
        //获得用户的好友树失败，删除用户已获取的所有好友信息
        void del_friend_tree(bool is_self);
        //设置好友是否为最爱
        int set_best_friend(uint32_t peer_id, int is_best_friend);
        //删除一个好友
        int del_friend(uint32_t user_id);

    protected:
        int get_self_friend_tree(int min, int max, friend_rsp_t *p_friend, uint16_t *p_all_friend_num);
        int get_other_friend_tree(uint32_t user_id, int min, int max, friend_rsp_t *p_friend, uint16_t *p_all_friend_num);
        int add_self_friend(const friend_list_t *p_friend_list, bool new_friend);
        int add_other_friend(const friend_list_t *p_friend_list);

    private:
        int m_inited;
        uint32_t m_user_id;                                    //用户自己的米米号
        uint32_t m_other_user_id;
        bool is_add_friend_list;
        bool is_add_other_friend_list;

        std::map<uint32_t, friend_info_t> m_friend_list_map;   //保存用户的好友列表
        std::list<uint32_t> m_best_friend_id_list;                //按顺序保存用户的最爱好友id
        std::list<uint32_t> m_normal_friend_id_list;              //按顺序保存用户的普通好友id

        //保存自己的好友树
        std::map<uint32_t, friend_list_t> m_best_friend_map;
        std::map<uint32_t, friend_list_t> m_normal_friend_map;

        std::map<uint32_t, friend_info_t> m_other_friend_list_map;   //保存用户的好友列表
        std::vector<uint32_t> m_other_best_friend_id_vec;                //按顺序保存用户的最爱好友id
        std::vector<uint32_t> m_other_normal_friend_id_vec;              //按顺序保存用户的普通好友id
        std::map<uint32_t, friend_list_t> m_other_best_friend_map;
        std::map<uint32_t, friend_list_t> m_other_normal_friend_map;
};

inline int c_friend::init(uint32_t user_id)
{
    if (m_inited)
    {
        return -1;
    }

    m_user_id = user_id;
    m_inited = 1;
    KDEBUG_LOG(user_id, "c_friend inited");
    return 0;
}

inline int c_friend::uninit()
{
    if (!m_inited)
    {
        return 0;
    }
    KDEBUG_LOG(m_user_id, "c_friend uninited");
    m_user_id = 0;
    KDEBUG_LOG(m_user_id, "c_friend uninited 1");
    m_other_user_id = 0;
    KDEBUG_LOG(m_user_id, "c_friend uninited 2");
    is_add_friend_list = false;
    KDEBUG_LOG(m_user_id, "c_friend uninited 3");
    is_add_other_friend_list = false;
    KDEBUG_LOG(m_user_id, "c_friend uninited 4");
    m_friend_list_map.clear();
    KDEBUG_LOG(m_user_id, "c_friend uninited 5");
    m_best_friend_map.clear();
    KDEBUG_LOG(m_user_id, "c_friend uninited 6");
    m_normal_friend_map.clear();
    KDEBUG_LOG(m_user_id, "c_friend uninited 7");
    m_best_friend_id_list.clear();
    KDEBUG_LOG(m_user_id, "c_friend uninited 8");
    m_normal_friend_id_list.clear();
    KDEBUG_LOG(m_user_id, "c_friend uninited 9");
    m_other_best_friend_map.clear();
    KDEBUG_LOG(m_user_id, "c_friend uninited 10");
    m_other_normal_friend_map.clear();
    KDEBUG_LOG(m_user_id, "c_friend uninited 11");
    m_other_friend_list_map.clear();
    KDEBUG_LOG(m_user_id, "c_friend uninited 12");
    m_other_best_friend_id_vec.clear();
    KDEBUG_LOG(m_user_id, "c_friend uninited 13");
    m_other_normal_friend_id_vec.clear();
    KDEBUG_LOG(m_user_id, "c_friend uninited 14");

    m_inited = 0;
    return 0;
}

inline void c_friend::del_friend_tree(bool is_self)
{
    if (is_self)
    {
        m_best_friend_map.clear();
        m_normal_friend_map.clear();
        m_best_friend_id_list.clear();
        m_normal_friend_id_list.clear();
    }
    else
    {
        m_other_best_friend_map.clear();
        m_other_normal_friend_map.clear();
        m_other_best_friend_id_vec.clear();
        m_other_normal_friend_id_vec.clear();
        m_other_user_id = 0;
        is_add_other_friend_list = false;
    }
}

#endif //H_FRIEND_H_20110913
