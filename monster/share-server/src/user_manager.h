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

#include "data_structure.h"

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
    /*! 用户是否在等待链表里面 */
    uint8_t                    waiting_flag;
	/*! 定时器链表头 */
	list_head_t		           timer_list;
	/*! 保存每个请求的链表 */
    std::queue<cached_pkg_t *> cached_pkg_queue;
    as_msg_get_room_req_t      buffer;

public:
	friend class UserManager;
};

/**
 * @brief 用户管理器，管理所有用户
 */
class UserManager {
public:
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

private:
	enum constant_t {
		max_player_num	= 100000
	};

private:
	/* 保存所有玩家 */
	usr_info_t	all_users[max_player_num];
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
			user->session = fdsess;

			user->inc_counter();

			INIT_LIST_HEAD(&(user->timer_list));

			DEBUG_LOG("alloc user: uid=%u fd=%d", uid, fd);
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

		user->uid = 0;
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

#endif // LIBTAOMEEPP_USER_MANAGER_HPP_
