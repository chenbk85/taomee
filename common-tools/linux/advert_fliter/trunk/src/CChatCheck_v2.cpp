/*
 * =====================================================================================
 *
 *       Filename:  Cchat_check.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/16/2011 05:15:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */

#include "CChatCheck_v2.h"
#include <time.h>
#include <math.h>
#include <libtaomee++/utils/strings.hpp>
#include <stdio.h>

bool CChatCheck_v2::check_msg(uint32_t userid, uint32_t timestamp, const char* msg, int len)
{
	if (len >= 4096) len = 4095;

	char buff[4096];
	memcpy(buff, msg, len);
	buff[len] = '\0';

	// 计算hash值，为0直接返回
	uint64_t msgid = BKDRHash(buff);
	if (msgid == 0) {
		KDEBUG_LOG(userid, "all ascii");
		return true;
	}

	// 加入用户列表
	user_msg_v2_t* umsg = this->get_or_alloc_user(userid);
	if (!this->user_push_msg_with_pop(umsg, msgid, timestamp))
		return false;

	// 加入全局消息列表
	meta_msg_v2_t* mmsg = this->get_or_alloc_meta(msgid);
	this->meta_push_msg_with_pop(mmsg, timestamp);

	DEBUG_LOG("MSG\t[%s mid=%lu uid=%u meta=%u user=%u]", 
			buff, msgid, userid, mmsg->msg_list.size(), umsg->msg_list.size());


	return ret;
}

// 获取消息ID的全局信息，不存在则分配一个
meta_msg_v2_t* CChatCheck_v2::get_or_alloc_meta(uint64_t msgid)
{
	// 如果找到直接返回
	meta_msg_v2_t* mmsg = NULL;
	std::map<uint64_t, meta_msg_v2_t*>::iterator it;
	it = this->meta_map.find(msgid);
	if (it != this->meta_map.end()) {
		mmsg = it->second;
		list_del(&mmsg->time_list);
	} else if (this->meta_map.size() >= 1000) {
	// 如果当前已经超过上限，则尝试删除最久没有更新的
		list_head_t* top = this->meta_list_head.next;
		mmsg = list_entry(top, meta_msg_v2_t, time_list);
		this->meta_map.erase(mmsg->msgid);
		list_del(&mmsg->time_list);
	} else {
		mmsg = new meta_msg_v2_t;
		mmsg->msgid = msgid;
		this->meta_map.insert(std::pair<uint64_t, meta_msg_v2_t*>(msgid, mmsg));
		INIT_LIST_HEAD(&mmsg->time_list);
	}

	// 更新消息更新时间列表
	list_add_tail(&mmsg->time_list, &this->meta_list_head);

	return mmsg;
}

// 获取用户信息，不存在则分配一个
user_msg_v2_t* CChatCheck_v2::get_or_alloc_user(uint32_t userid)
{
	// 如果找到直接返回
	user_msg_v2_t* umsg = NULL;
	std::map<uint32_t, user_msg_v2_t*>::iterator it;
	it = this->user_map.find(msgid);
	if (it != this->user_map.end()) {
		umsg = it->second;
		list_del(&umsg->time_list);
	} else if (this->user_map.size() >= 1000) {
	// 如果当前已经超过上限，则尝试删除最久没有更新的
		list_head_t* top = this->user_list_head.next;
		umsg = list_entry(top, user_msg_v2_t, time_list);
		this->user_map.erase(umsg->userid);
		list_del(&umsg->time_list);
	} else {
		umsg = new user_msg_v2_t;
		umsg->userid = userid;
		this->user_map.insert(std::pair<uint64_t, user_msg_v2_t*>(userid, umsg));
		INIT_LIST_HEAD(&umsg->time_list);
	}

	// 更新消息更新时间列表
	list_add_tail(&umsg->time_list, &this->user_list_head);

	return umsg;
}

// 在全局列表插入一条消息（同时删除已超时消息）
void CChatCheck_v2::meta_push_msg_with_pop(meta_msg_v2_t* mmsg, uint32_t timestamp)
{
	// 删除十分钟前的消息
	while (mmsg->msg_list.front() + 600 < timestamp) {
		mmsg->msg_list.pop_front();
	}

	mmsg->msg_list.push_back(timestamp);
}

// 在用户列表插入一条消息（同时删除已超时消息）
// 对于1s内连续多条消息的用户直接干掉
bool CChatCheck_v2::user_push_msg_with_pop(user_msg_v2_t* umsg, uint64_t msgid, uint32_t timestamp)
{
	if (timestamp <= umsg->msg_list.back().timestamp)
		return false;

	// 删除一分钟前的消息
	while (umsg->msg_list.front().timestamp + 600 < timestamp) {
		umsg->msg_list.pop_front();
	}

	message_v2_t msg;
	msg.msgid = msgid;
	msg.timestamp = timestamp;
	umsg->msg_list.push_back(msg);
	return true;
}
