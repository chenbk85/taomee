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

#ifndef CCHAT_CHECK_H_
#define CCHAT_CHECK_H_

#include "utils.h"
#include <set>
#include "CChatString.h"
#include "CChatForbid.h"

extern uint32_t max_msg_count;

enum chat_const_val {
	max_msg_list_len		= 1024 * 300,		// 最大缓存消息数量，如果超过，尝试释放
	max_msglv				= 4,				// 消息以及用户队列优先级数量
};

// 消息基本信息
typedef struct message {
	uint32_t					timestamp;		// 消息时间戳
	uint32_t					userid;			// 米米号
	uint64_t					msgid;			// 消息字符串的hash
	list_head_t					msg_time_list;
	list_head_t					msg_user_list;
	list_head_t					msg_meta_list;
} __attribute__ ((packed)) message_t;

// 消息元数据
typedef struct meta_msg {
public:
	uint32_t					all_count;		// 一段时间以来出现该消息的数量
	uint32_t					last_arise;		// 上次出现该消息的时间
	uint32_t					positive_cnt;	// 该消息被判定为正影响的次数
	uint32_t					negative_cnt;	// 该消息被判定为负影响的次数
	uint8_t						msglv;			// 消息等级，越高表示越有可能是不好的
	std::map<uint32_t, uint32_t>*	user_cnt_map;	// 每个用户出现该消息的数量
	list_head_t					count_time_list;
	list_head_t					meta_msg_head;
}__attribute__ ((packed)) meta_msg_t;


/* 
 * 消息的元数据管理器
 * */

typedef std::map<uint64_t, meta_msg_t*>::iterator meta_msg_ptr;

class CMsgMetaMap {
private:
	// 消息HASH与元数据的关系
	std::map<uint64_t, meta_msg_t*>		meta_map[max_msglv];
	// 消息HASH 按时间列表
	list_head_t						count_time_head[max_msglv];

public:
	CMsgMetaMap();

	meta_msg_ptr get_meta(message_t* msg);

	void update(meta_msg_ptr mmsg_it);
	void del_user_msg(uint64_t msgid, uint32_t userid);
	void addto_time_list(meta_msg_t* mmsg);
};

typedef struct user_msg {
	uint8_t						userlv;
	list_head_t					user_msg_head;
}__attribute__((packed)) user_msg_t;

typedef std::map<uint32_t, user_msg_t*>::iterator user_msg_ptr;
class CMsgUserMap {
	// 每个用户的消息队列，分优先级保存
	std::map<uint32_t, user_msg_t*>	user_msg_map[max_msglv];

public:
	// 得到某个用户的消息列表和用户等级
	user_msg_t* get_user_msg(uint32_t userid);
	// 将消息加入到用户的列表中
	void add_msg_to_user_list(uint32_t userid, int userlv, message_t* msg);
	void del_msg_from_list(message_t* msg);
	int check_user_msg(uint64_t msgid, uint32_t userid, user_msg_t* umsg, meta_msg_t* mmsg);
	bool try_change_level(uint32_t userid, user_msg_t* umsg, int change_level);
};


class CChatCheck {
public:
	// 消息检查的主要函数
	bool check_msg(uint32_t userid, uint32_t timestamp, const char* msg, int len);	

	void do_after_forbid(uint32_t userid);
	// 加消息加入队列中，如果超过设定的队列上线，则释放最老的消息
	message_t* add_msg_main(uint32_t userid, uint64_t msgid, uint32_t timestamp);

	void reload_valid_words();

	CChatCheck();
	~CChatCheck()
	{
		delete msg_count_map;
		delete msg_user_map;
	}

public:
	CChatForbid						chat_forbid;

private:
	// 主消息队列，按时间排序
	list_head_t						msg_list_head;
	uint32_t						msg_count;
	// 消息的数量管理器
	CMsgMetaMap*					msg_count_map;
	// 消息的用户管理器
	CMsgUserMap*					msg_user_map;
	CTrimAbnormal					trim_mgr;
	std::set<uint64_t>				valid_words;
};

#endif
