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

#include "CChatCheck.h"
#include <time.h>
#include <math.h>
#include <libtaomee++/utils/strings.hpp>
#include <stdio.h>

CMsgMetaMap::CMsgMetaMap()
{
	for (uint32_t loop = 0; loop < max_msglv; loop ++) {
		INIT_LIST_HEAD(&count_time_head[loop]);
	}
}

meta_msg_ptr CMsgMetaMap::get_meta(message_t* msg)
{
	meta_msg_ptr it;
	for (uint32_t loop = 0; loop < max_msglv; loop ++) {
		it = this->meta_map[loop].find(msg->msgid);
		if (it != this->meta_map[loop].end()) {
			it->second->all_count ++;
			(*it->second->user_cnt_map)[msg->userid] ++;
			list_add_tail(&msg->msg_meta_list, &it->second->meta_msg_head);
			return it;
		}
	}

	meta_msg_t *mmsg = new meta_msg_t;
	mmsg->all_count = 1;
	mmsg->last_arise = time(NULL);
	mmsg->msglv = 0;
	mmsg->positive_cnt = 0;
	mmsg->negative_cnt = 0;
	mmsg->user_cnt_map = new std::map<uint32_t, uint32_t>;
	INIT_LIST_HEAD(&mmsg->count_time_list);
	INIT_LIST_HEAD(&mmsg->meta_msg_head);
	list_add_tail(&msg->msg_meta_list, &mmsg->meta_msg_head);
	it = meta_map[0].insert(std::pair<uint64_t, meta_msg_t*>(msg->msgid, mmsg)).first;
	(*mmsg->user_cnt_map)[msg->userid] = 1;
	return it;
}

void CMsgMetaMap::del_user_msg(uint64_t msgid, uint32_t userid)
{
	for (uint32_t loop = 0; loop < max_msglv; loop ++) {
		meta_msg_ptr it = this->meta_map[loop].find(msgid);
		if (it != this->meta_map[loop].end()) {
			meta_msg_t* mmsg = it->second;
			DEBUG_LOG("meta del user find\t[%lu %u %u %lu]", msgid, userid, mmsg->msglv, mmsg->user_cnt_map->size());
			std::map<uint32_t, uint32_t>::iterator ituser = mmsg->user_cnt_map->find(userid);
			if (ituser != mmsg->user_cnt_map->end()) {
				if (ituser->second == 1) {
					mmsg->user_cnt_map->erase(ituser);
					DEBUG_LOG("meta del user erase user\t[%lu %u]", msgid, userid);
				} else {
					ituser->second --;
					DEBUG_LOG("meta del user normal\t[%lu %u]", msgid, userid);
				}
			}

			if (mmsg->user_cnt_map->size() == 0) {
				DEBUG_LOG("meta del user erase msg\t[%lu %u %p]", msgid, userid, mmsg);
				delete mmsg->user_cnt_map;
				delete mmsg;
				this->meta_map[loop].erase(it);
			}

			break;
		}
	}
}

void CMsgMetaMap::update(meta_msg_ptr it)
{
	meta_msg_t* mmsg = it->second;

	mmsg->last_arise = time(NULL);	
	double ratio = (double)mmsg->negative_cnt / mmsg->positive_cnt - 2;
	ratio += log((double)mmsg->all_count / 4.0);
	int new_level = ratio < 0 ? 0 : ratio >= max_msglv ? max_msglv - 1 : ratio;

	if (new_level != mmsg->msglv) {
		DEBUG_LOG("meta update level up\t[%lu %u %u %p]", it->first, mmsg->msglv, new_level, mmsg);
		meta_map[mmsg->msglv].erase(it);
		list_del(&mmsg->count_time_list);

		mmsg->msglv = new_level;

		it = meta_map[mmsg->msglv].insert(std::pair<uint64_t, meta_msg_t*>(it->first, mmsg)).first;
		addto_time_list(mmsg);
	}
}

void CMsgMetaMap::addto_time_list(meta_msg_t* mmsg)
{
	list_add_tail(&mmsg->count_time_list, &count_time_head[mmsg->msglv]);
}

message_t* CChatCheck::add_msg_main(uint32_t userid, uint64_t msgid, uint32_t timestamp)
{
	// 已经到达消息缓存的上限
	if (msg_count >= max_msg_count) {
		list_head_t* top = msg_list_head.next;
		message_t* msg = list_entry(top, message_t, msg_time_list);
		DEBUG_LOG("FREE\t[id=%lu u=%u t=%u c=%u]", msg->msgid, msg->userid, msg->timestamp, msg_count);
		this->msg_count_map->del_user_msg(msg->msgid, msg->userid);	
		list_del(&msg->msg_time_list);
		list_del(&msg->msg_user_list);
		list_del(&msg->msg_meta_list);
		this->msg_user_map->del_msg_from_list(msg);
		delete msg;
		msg_count --;
	}


	// 放入队列中去
	message_t* msginfo = new message_t;
	msginfo->userid = userid;
	msginfo->timestamp = timestamp;
	msginfo->msgid = msgid;
	INIT_LIST_HEAD(&msginfo->msg_time_list);
	INIT_LIST_HEAD(&msginfo->msg_user_list);
	INIT_LIST_HEAD(&msginfo->msg_meta_list);

	list_add_tail(&msginfo->msg_time_list, &msg_list_head);
	msg_count ++;

	return msginfo;
}

user_msg_t* CMsgUserMap::get_user_msg(uint32_t userid)
{
	for (int loop = 0; loop < max_msglv; loop ++) {
		user_msg_ptr it = user_msg_map[loop].find(userid);
		if (it != user_msg_map[loop].end()) {
			return it->second;
		}
	}
	return NULL;
}

void CMsgUserMap::del_msg_from_list(message_t* msg)
{
	for (uint32_t loop = 0; loop < max_msglv; loop ++) {
		user_msg_ptr it = user_msg_map[loop].find(msg->userid);
		if (it != user_msg_map[loop].end()) {
			list_head_t* msg_head = &it->second->user_msg_head;
			if (msg_head->prev == msg_head->next && msg_head == msg_head->next) {
				delete it->second;
				user_msg_map[loop].erase(it);
			}
			break;
		}
	}
}

void CMsgUserMap::add_msg_to_user_list(uint32_t userid, int userlv, message_t* msg)
{
	user_msg_ptr it = this->user_msg_map[userlv].find(userid);

	if (it == user_msg_map[userlv].end()) {
		user_msg_t* umsg = new user_msg_t;
		umsg->userlv = userlv;
		INIT_LIST_HEAD(&umsg->user_msg_head);
		it = user_msg_map[userlv].insert(std::pair<uint32_t, user_msg_t*>(userid, umsg)).first;
	}

	list_add_tail(&msg->msg_user_list, &it->second->user_msg_head);
}

int CMsgUserMap::check_user_msg(uint64_t msgid, uint32_t userid, user_msg_t* umsg, meta_msg_t* mmsg)
{	
	int max_continuous = 1, continuous = 0;
   	int	interval[50] = {0};
	uint32_t count = 0, last_time = 0, total_time = 0;
	bool is_last_con = true, is_near_same = true;

	list_head_t* p;
	__list_for_each_prev(p, &umsg->user_msg_head) {
		message_t* msg = list_entry(p, message_t, msg_user_list);
		if (msg->msgid == msgid)
			continuous ++;
		else {
			if (continuous > max_continuous) {
				max_continuous = continuous;
				if (!is_last_con) is_near_same = false;
			}
			is_last_con = false;
			continuous = 0;
		}
		
		if (count >= 30) break;

		if (last_time) {
			interval[count] = last_time > msg->timestamp ? (last_time - msg->timestamp) : 0;
			total_time += interval[count];
			count ++;
		}
		last_time = msg->timestamp;
	}

	if (max_continuous < continuous) max_continuous = continuous;
	if (!is_near_same) max_continuous = (max_continuous + 1) / 2;

	int addlv = 0;

	if (count >= 1) {
		int avg_interval = (total_time + count / 2) / count;
		total_time = 0;
		for (uint32_t loop = 0; loop < count; loop ++) {
			int diff = avg_interval - interval[loop];
			total_time += diff * diff;
		}
	
		int valiance = (total_time + count / 2) / count;
		DEBUG_LOG("AVG\t[%u %d %d]", count, avg_interval, valiance);

		if (avg_interval > 300) {
			addlv = -2;
		} if (avg_interval > 60) {
			addlv = -1;
		} else if (count >=4) {
			addlv = valiance < 4 ? 1 : 0;
			if (avg_interval < 5) addlv += 1;
		}

	}

	int max_same_cnt = 8 - 2 * mmsg->msglv, samelv;
	int same_msg_cnt = (*mmsg->user_cnt_map)[userid];

	KDEBUG_LOG(userid, "CNT\t[%d %d %d %d]", max_same_cnt, same_msg_cnt, max_continuous, addlv);
	if (!max_same_cnt) {
		samelv = 2;
	} if (same_msg_cnt >= max_same_cnt) {
		samelv = 2 * max_continuous / max_same_cnt + 1;
	} else {
		samelv = max_continuous < 3 ? -1 : 2 * max_continuous / max_same_cnt;
	}

	float msg_user_cnt = mmsg->user_cnt_map->size();
	if (msg_user_cnt > 50) {
		float avg_user_cnt = (float)mmsg->all_count	/ msg_user_cnt;
		if (avg_user_cnt < 1.5) {
			addlv -= 2;
		} else if (avg_user_cnt < 2.0) {
			addlv -= 1;
		} else if (avg_user_cnt > 3.5) {
			addlv += 1;
		}
	}

	return samelv + addlv;
}

bool CMsgUserMap::try_change_level(uint32_t userid, user_msg_t* umsg, int change_level)
{
	if (change_level == 0 || (umsg->userlv == 0 && change_level < 0))
		return true;

	if (umsg->userlv  == max_msglv - 1 && change_level >= 0)
		return false;

	int new_level = umsg->userlv + change_level;
	bool ret = true;
	if (new_level < 0) new_level = 0;
	if (new_level >= max_msglv) {
		new_level = max_msglv - 1;
		ret = false;
	}

	user_msg_ptr it = user_msg_map[umsg->userlv].find(userid);
	user_msg_map[umsg->userlv].erase(it);

	umsg->userlv = new_level;
	user_msg_map[new_level].insert(std::pair<uint32_t, user_msg_t*>(userid, umsg));

	return ret;
}

CChatCheck::CChatCheck() : msg_count(0)
{
	INIT_LIST_HEAD(&msg_list_head);
	msg_count_map = new CMsgMetaMap();
	msg_user_map = new CMsgUserMap();
	reload_valid_words();
}

bool CChatCheck::check_msg(uint32_t userid, uint32_t timestamp, const char* msg, int len)
{
	if (len >= 4096) len = 4095;

	char buff[4096];
	memcpy(buff, msg, len);
	buff[len] = '\0';

	// 不检测一两个字的话，防止类似“哦、嗯” 这些常用语最终被判定为广告语
	if (strlen(buff) <= 6) {
		KDEBUG_LOG(userid, "SHORT");
		return true;
	}

	trim_mgr.trim_abnormal_utf8(buff);

	// 计算hash值，为0直接返回
	uint64_t msgid = BKDRHash(buff);
	if (msgid == 0) {
		KDEBUG_LOG(userid, "all ascii");
		return true;
	}

	if (valid_words.find(msgid) != valid_words.end()) {
		KDEBUG_LOG(userid, "valid word\t[%lu]", msgid);
		return true;
	}

	// 添加到主列表中
	message_t* msginfo = add_msg_main(userid, msgid, timestamp);
	// 获取消息的元数据信息
	meta_msg_ptr mmsg_iter = msg_count_map->get_meta(msginfo);
	meta_msg_t* mmsg = mmsg_iter->second;
	user_msg_t* umsg = msg_user_map->get_user_msg(userid);

	DEBUG_LOG("MSG\t[id=%lu u=%u c=%u t=%u %s meta=%u %u %u %u user=%u]", 
			msgid, userid, msg_count, timestamp, buff, 
			mmsg->all_count, mmsg->positive_cnt, mmsg->negative_cnt, mmsg->msglv,
			umsg ? umsg->userlv : 0);

	msg_user_map->add_msg_to_user_list(userid, umsg ? umsg->userlv : (mmsg->msglv + 1) / 2, msginfo);

	bool ret = true;
	if (umsg == NULL) { 						// 用户的第一条消息 
		mmsg->positive_cnt ++;
		msg_count_map->addto_time_list(mmsg);
	} else if (umsg->userlv < max_msglv) {		// 已经有一些消息记录
		int change_level = msg_user_map->check_user_msg(msginfo->msgid, msginfo->userid, umsg, mmsg);
		if (!change_level) return true;
		if (change_level < 0)
			mmsg->positive_cnt ++;
		else
			mmsg->negative_cnt ++;

		msg_count_map->update(mmsg_iter);

		ret = msg_user_map->try_change_level(userid, umsg, change_level);
	} else if (mmsg->msglv > 0) {				// 该用户曾经被禁言，又说了一句可疑的话
		mmsg->negative_cnt ++;
		msg_count_map->update(mmsg_iter);
		ret = false;;
	}

	if (ret == false) {
		memcpy(buff, msg, len);
		buff[len] = '\0';
		KDEBUG_LOG(userid, "invalid msg\t[%u %s]", timestamp, buff);
	}

	return ret;
}

void CChatCheck::do_after_forbid(uint32_t userid)
{
	user_msg_t* umsg = this->msg_user_map->get_user_msg(userid);
	if (umsg) {
		this->msg_user_map->try_change_level(userid, umsg, -2);
	}
}

void CChatCheck::reload_valid_words()
{
	FILE* fp = fopen("../data/valid_words", "r");
	if (fp == NULL){
		ERROR_LOG("open valid words fail");
		return;
	}	

	valid_words.clear();

	char buf[4096];
	while (fgets(buf, 4096, fp) != NULL) {
		this->trim_mgr.trim_abnormal_utf8(buf);
		uint64_t msgid = BKDRHash(buf);
		if (msgid) {
			DEBUG_LOG("VALID WORDS\t[%lu %s]", msgid, buf);
			valid_words.insert(msgid);
		}
	}

	fclose(fp);
}
