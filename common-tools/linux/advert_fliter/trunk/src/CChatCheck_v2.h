/*
 * =====================================================================================
 *
 *       Filename:  CChatCheck.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/16/2011 05:15:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */

#ifndef CCHAT_CHECK_V2_H_
#define CCHAT_CHECK_V2_H_

#include <list>

// 消息元数据(全局消息列表项)
typedef struct meta_msg_v2 {
	uint64_t				msgid;
	list_head_t				time_list;
	std::list<uint32_t>		msg_list;
}__attribute__ ((packed)) meta_msg_v2_t;

typedef struct message_v2 {
	uint64_t				msgid;
	uint32_t				timestamp;
}__attribute__((packed)) message_v2_t;

// 消息元数据(用户消息列表项)
typedef struct user_msg_v2 {
	uint32_t				userid;
	list_head_t				timelist;
	std::list<message_v2_t>	msg_list;
}__attribute__((packed)) user_msg_v2_t;


class CChatCheck_v2 {
public:
	// 消息检查的主要函数
	bool check_msg(uint32_t userid, uint32_t timestamp, const char* msg, int len);	

	CChatCheck_v2()
	{
		INIT_LIST_HEAD(&meta_list_head);
		INIT_LIST_HEAD(&user_list_head);
	}

private:
	// 获取消息ID的全局信息，不存在则分配一个
	meta_msg_v2_t* get_or_alloc_meta(uint64_t msgid);
	// 获取用户信息，不存在则分配一个
	user_msg_v2_t* get_or_alloc_user(uint32_t userid);

	// 在全局列表插入一条消息（同时删除已超时消息）
	void meta_push_msg_with_pop(meta_msg_v2_t* mmsg, uint32_t timestamp);
	// 在用户列表插入一条消息（同时删除已超时消息）
	// 对于1s内连续多条消息的用户直接干掉
	bool user_push_msg_with_pop(user_msg_v2_t* umsg, uint64_t msgid, uint32_t timestamp);

private:
	// 消息队列，超过内存上限时释放
	list_head_t						meta_list_head;
	list_head_t						user_list_head;

	std::map<uint64_t, meta_msg_v2_t*>	meta_map;
	std::map<uint32_t, user_msg_v2_t*>	user_map;
};

#endif
