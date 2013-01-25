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

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/types.h>

#include <async_serv/net_if.h>
}

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
	userid_t		uid;
	/*! 当前正在处理的协议命令 */
	uint32_t		waitcmd;
	/*! 为了和老版本的登录服务器兼容，可随意使用 */
	uint32_t		channel;	
	/*! 计数器，用于服务器返回时验证该用户是否还是原来的用户 */
	uint16_t		counter;
	/*! session，session->fd */
	fdsession_t*	session;
	/* 定时器链表头 */
	list_head_t		timer_list;
	/* 临时保存用户是否已经创建了某角色 */
	uint32_t		create_role_flag;
	/*! 可用于保存临时信息，4KB */
	uint8_t			tmpinfo[4096];

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
	usr_info_t* alloc_user(int fd, userid_t uid, fdsession_t* fdsess);
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
		max_player_num	= 60000
	};

private:
	/* 保存所有玩家 */
	usr_info_t	all_users[max_player_num];
};

inline usr_info_t*
UserManager::alloc_user(int fd, userid_t uid, fdsession_t* fdsess)
{
	usr_info_t* user = 0;

	if (fd >= 0 && fd < max_player_num) {
		user = &(all_users[fd]);
		if (user->uid == 0) {
			user->uid     = uid;
			user->waitcmd = 0;
			user->channel = 0;
			user->session = fdsess;
			user->create_role_flag = 0;

			user->inc_counter();

			INIT_LIST_HEAD(&(user->timer_list));

			TRACE_LOG("alloc user: uid=%u fd=%d", uid, fd);
			return user;
		}
	}

	WARN_LOG("failed to alloc user: fd=%d uid=%u %u cmd=%d", fd, uid, user ? user->uid : 0, user ? user->waitcmd : 0);
	return 0;
}

inline void
UserManager::dealloc_user(int fd)
{
	usr_info_t* user = get_user(fd);
	if (user) {
		TRACE_LOG("dealloc user: uid=%u fd=%d", user->uid, fd);

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

/*! 用户管理器的实例，可以通过它添加、查找或者删除用户 */
extern UserManager user_mng;

#endif // LIBTAOMEEPP_USER_MANAGER_HPP_

