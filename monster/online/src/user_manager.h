/**
 *============================================================
 *  @file      user_manager.hpp
 *  @brief     用户管理器，管理所有连接到登录服务器的用户
 *
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef LIBTAOMEEPP_USER_MANAGER_HPP_
#define LIBTAOMEEPP_USER_MANAGER_HPP_

#include <queue>
#include <map>

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/types.h>

#include <async_serv/net_if.h>
}

#include "constant.h"
#include "c_user_cache.h"
#include "c_friend.h"

/**
 * @brief 保存as的每个请求
 */
struct cached_pkg_t {
    uint16_t len;
    uint8_t pkg[0];
};

/**
 * @brief 用户结构体，保存用户信息
 */
struct usr_info_t {
public:
	void inc_counter()
	{
//        INFO_LOG("inc counter");
		++counter;
		if (counter == 0) {
			++counter;
		}
	}

public:
	/*! 米米号 */
	uint32_t		           uid;
	/*! 当前正在处理的协议命令 */
	uint32_t		           waitcmd;
	/*! 计数器，用于服务器返回时验证该用户是否还是原来的用户 */
	uint16_t		           counter;
	/*! session，session->fd */
	fdsession_t*	           session;
	/*! 用于统计每个协议的处理时间 */
    time_t                     recv_time;
	/*! 用于统计项的时间,比如计算在商店的时间和小游戏玩的时间 */
    time_t                     stat_time;
    /*! 用户是否在等待链表里面 */
    uint8_t                    waiting_flag;
	/*! 定时器链表头 */
	list_head_t		           timer_list;
	/*! 保存每个请求的链表 */
    std::queue<cached_pkg_t *> cached_pkg_queue;
	/*! 用户在线信息的缓存 */
    c_user_cache               user_cache;
	/*! 用户好友树的缓存 */
    c_friend                   friend_tree;
	/*! 用户是否注册过角色 */
    uint8_t                    is_register;
	/*! 校验as过来的序列号 */
	uint32_t		           seq;
	/*! 可用于保存临时信息，8KB */
	uint8_t			           buffer[MAX_RECV_PKG_SIZE];

public:
	friend class UserManager;
};

/**
 * @brief 用户管理器，管理所有用户
 */
class UserManager {
public:
    UserManager() { INIT_LIST_HEAD(&timer_list); }
	/*
	 * @brief 分配一个新用户
	 * @param fd 用户的fd
	 * @param uid 用户的米米号
	 * @param fdsess fdsess
	 * @return 分配成功则返回合法指针，失败则返回0
 	 */
	usr_info_t* alloc_user(int fd, uint32_t uid, fdsession_t* fdsess);

	/*
	 * @brief 释放用户
	 * @param fd 用户的fd
 	 */
	void        dealloc_user(int fd);

	/**
	 * @brief 查找已存在的用户
	 * @param fd 用户的fd
	 * @return 查找成功则返回合法指针，失败则返回0
 	 */
	usr_info_t* get_user(int fd);

    /**
	 * @brief 获得用户的fd
	 * @param user_id 用户id
	 * @return -1:用户不在线，>= 0用户的连接fd
 	 */
	fdsession_t *get_fd(uint32_t user_id);

    /**
	 * @brief 对所有在线用户执行func
 	 */
	void process_all_user(void (*func)(usr_info_t *p_user));

public:
	list_head_t		           timer_list;

private:
	enum constant_t {
		//max_player_num	= 300
		max_player_num	= 60000
	};

private:
	/* 保存所有玩家 */
	usr_info_t	all_users[max_player_num];
    std::map<uint32_t, fdsession_t *> user_fd_map;
};

inline usr_info_t*
UserManager::alloc_user(int fd, uint32_t uid, fdsession_t* fdsess)
{
	usr_info_t* user = 0;

	if (fd >= 0 && fd < max_player_num) {
		user = &(all_users[fd]);
        if (user->uid == 0) {
            user->uid     = uid;
            user->waitcmd = 0;
            user->is_register = 0;
            user->session = fdsess;

            user->waiting_flag = 0;
            user->inc_counter();
            user->user_cache.init();
            user->friend_tree.init(uid);

            INIT_LIST_HEAD(&(user->timer_list));

            DEBUG_LOG("alloc user: uid=%u fd=%d", uid, fd);
            user_fd_map.insert(std::pair<uint32_t, fdsession_t *>(uid, fdsess));
            return user;
        }
	}

	ERROR_LOG("failed to alloc user: fd=%d uid=%u %u cmd=%d", fd, uid, user ? user->uid : 0, user ? user->waitcmd : 0);
	return 0;
}

inline void
UserManager::dealloc_user(int fd)
{
	usr_info_t* user = get_user(fd);
	if (user) {
		DEBUG_LOG("dealloc user: uid=%u fd=%d", user->uid, fd);

        while (!(user->cached_pkg_queue.empty()))
        {
            cached_pkg_t *p_cached_cmd = user->cached_pkg_queue.front();
            if (p_cached_cmd != NULL)
            {
                g_slice_free1(p_cached_cmd->len, p_cached_cmd);
            }
            user->cached_pkg_queue.pop();
        }

        user_fd_map.erase(user->uid);
		user->uid = 0;
		user->is_register = 0;
        user->waiting_flag = 0;
        user->user_cache.uninit();
        user->friend_tree.uninit();
		REMOVE_TIMERS(user);
	}
}

inline usr_info_t*
UserManager::get_user(int fd)
{
	if (fd >= 0 && fd < max_player_num) {
		usr_info_t* user = &(all_users[fd]);
		if (user->uid) {
			return user;
		}
	}

	return 0;
}

inline fdsession_t *
UserManager::get_fd(uint32_t user_id)
{
    std::map<uint32_t, fdsession_t *>::iterator iter = user_fd_map.find(user_id);
    if (iter == user_fd_map.end())
    {
        return NULL;
    }

    return iter->second;
}

inline void
UserManager::process_all_user(void (*func)(usr_info_t *p_user))
{
    for (int i = 0; i != max_player_num; ++i)
    {
        if (all_users[i].uid == 0)
        {
            continue;
        }
        func(&all_users[i]);
    }
}

#endif // LIBTAOMEEPP_USER_MANAGER_HPP_
