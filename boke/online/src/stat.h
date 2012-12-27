/*
 * =====================================================================================
 *
 *       Filename:  stat.h
 *
 *    Description:  pop stat
 *
 *        Version:  1.0
 *        Created:  09/25/2011 01:42:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */

#include "Csprite.h"
#include "util.h"

struct msglog_item2{
	uint32_t v1;
	uint32_t v2;
}__attribute__((packed));
struct  msglog_item1{
	uint32_t v1;
}__attribute__((packed));
struct msglog_item3{
	uint32_t v1;
	uint32_t v2;
	uint32_t v3;
}__attribute__((packed));

struct msglog_item4{
	uint32_t v1;
	uint32_t v2;
	uint32_t v3;
	uint32_t v4;
}__attribute__((packed));

inline void stat_log_login_ip(Csprite* p)
{
	/*
	   登录IP分布
	   0x11000006
	   （米米号、IP）
	   */
	struct msglog_item2 item;
	item.v1=p->id;
	item.v2=p->fdsess->remote_ip;
	msglog(statistic_file, 0x11000006, time(NULL), &item, sizeof(item)); 
}

inline void stat_log_new_user(Csprite* p)
{
	/*
		新增用户数
		0x11000003
		（米米号、1、米米号、注册时间）
	*/

	struct msglog_item4 item;
	item.v1=p->id;
	item.v2=1;
	item.v3=p->id;
	item.v4=time(NULL);

	msglog(statistic_file, 0x11000003, time(NULL), &item, sizeof(item)); 
}

inline void stat_log_login_on(Csprite* p)
{
	/*
	   登入/登出online
	   0x11000001
	   登录时发：(1、0、米米号、0)；登出时发：(0、1、米米号、登录时长)
   */
	struct msglog_item4 item;
	item.v1=1;
	item.v2=0;
	item.v3=p->id;
	item.v4=0;
	msglog(statistic_file ,0x11000001,  time(NULL), &item, sizeof(item)); 
}

inline void stat_log_login_off(Csprite* p, uint32_t online_time_once)
{
	/*
		 登入/登出online
 		0x11000001
 		登录时发：(1、0、米米号、0)；登出时发：(0、1、米米号、登录时长)
	*/
	struct msglog_item4 item;
	item.v1=0;
	item.v2=1;
	item.v3=p->id;
	item.v4=online_time_once;
	msglog(statistic_file ,0x11000001,  time(NULL), &item, sizeof(item)); 
}


inline void stat_log_online_time(Csprite* p, uint32_t online_time_once)
{
	/*
	   米米号-登录时长
	   0x11000002
	   登出时发：（米米号、登录时长）
   */
	struct msglog_item2 item;
	item.v1=p->id;
	item.v2=online_time_once;
	msglog(statistic_file ,0x11000002,  time(NULL), &item, sizeof(item)); 
}

inline void stat_log_task_node(Csprite* p, uint32_t islandid, uint32_t task_nodeid)
{
	/*XX岛任务
	 *  0x11101001+(islandID << 12)+taskID
	 *   （米米号）
	 *    需要计算总量，最后一步id这里作为0处理
	 */ 

	struct msglog_item1 item;
	item.v1=p->id;
	if (task_nodeid==END_TASK_NODEID){
		task_nodeid=0;
	}

	msglog(statistic_file , 0x11101001+(islandid <<12 )+task_nodeid, time(NULL), &item, sizeof(item)); 
}

inline void stat_log_enter_map(Csprite* p, uint32_t islandid, uint32_t mapid)
{
	/* 统计XX岛地图
 		0x11001000+(islandID << 12)+mapID
 		（米米号、1）
		 需计算每个地图的总量
 	*/

	struct msglog_item2 item;
	item.v1=p->id;
	item.v2=1;
	msglog(statistic_file, 0x11001000+(islandid<< 12)+mapid, time(NULL), &item, sizeof(item)); 
}

inline void stat_log_talk_npc(Csprite* p, uint32_t npc_id)
{
	/*	
		NPC说话
		0x11200000+npcID-10000
		（米米号、1）
		需计算每个NPC的总量
 	*/
 
	struct msglog_item2 item;
	item.v1=p->id;
	item.v2=1;
	msglog(statistic_file, 0x11200000+npc_id-10000, time(NULL), &item, sizeof(item)); 
}

inline void stat_log_game(uint32_t uid, uint32_t gameid)
{
	/*
	 * 小游戏参与统计 0x11300000 + gameid
	 */
	struct msglog_item2 item;
	item.v1=uid;
	item.v2=1;
	msglog(statistic_file, 0x11300000+gameid, time(NULL), &item, sizeof(item)); 
}

inline void stat_log_item(uint32_t itemid)
{
	struct msglog_item1 item;
	item.v1 = 1;
	switch (itemid / 100000) {
	case 2:
		msglog(statistic_file, 0x11314000+itemid - 200000, time(NULL), &item, sizeof(item)); 
		break;
	case 3:
		msglog(statistic_file, 0x11311000+itemid - 300000, time(NULL), &item, sizeof(item)); 
		break;
	case 4:
		msglog(statistic_file, 0x11312000+itemid - 400000, time(NULL), &item, sizeof(item)); 
		break;
	case 6:
		msglog(statistic_file, 0x11313000+itemid - 600000, time(NULL), &item, sizeof(item)); 
		break;
	}
}

inline void stat_log_task_finish(uint32_t id, uint32_t count)
{
	struct msglog_item2 item;
	item.v1 = id;
	item.v2 = count;
	msglog(statistic_file, 0x11300000, time(NULL), &item, sizeof(item)); 
}

inline void stat_log_work(uint32_t id)
{
	// 统计打工人数
	struct msglog_item1 item1;
	item1.v1 = id;
	msglog(statistic_file, 0x11315001, time(NULL), &item1, sizeof(item1)); 
}

inline void stat_log_draw(uint32_t id)
{
	struct msglog_item1 item1;
	item1.v1 = id;
	msglog(statistic_file, 0x11315003, time(NULL), &item1, sizeof(item1)); 
}

inline void stat_log_gift(uint32_t id)
{
	// 统计当日礼券获得数量
	struct msglog_item1 item1;
	item1.v1 = 1;
	msglog(statistic_file, 0x1131501d, time(NULL), &item1, sizeof(item1)); 

	// 统计获得礼券人数
	item1.v1 = id;
	msglog(statistic_file, 0x1131501e, time(NULL), &item1, sizeof(item1)); 
}

inline void stat_log_click_chris_tree(uint32_t id)
{
	struct msglog_item1 item1;
	item1.v1 = id;
	msglog(statistic_file, 0x1131501f, time(NULL), &item1, sizeof(item1)); 
}

inline void stat_log_chris_game(uint32_t id)
{
	struct msglog_item2 item;
	item.v1 = id;
	item.v2 = 1;
	msglog(statistic_file, 0x11315020, time(NULL), &item, sizeof(item)); 
}

inline void stat_log_chris_reward(uint32_t id)
{
	struct msglog_item1 item1;
	item1.v1 = id;
	msglog(statistic_file, 0x11315021, time(NULL), &item1, sizeof(item1)); 
}

inline void stat_log_bell(int bell)
{
	struct msglog_item1 item1;
	item1.v1 = 1;
	uint32_t msgid = 0;
	switch (bell) {
	case 0:
		msgid = 0x11315024;
		break;
	case 1:
		msgid = 0x11315023;
		break;
	case 2:
		msgid = 0x11315022;
		break;
	default:
		return;
	}
	msglog(statistic_file, msgid, time(NULL), &item1, sizeof(item1)); 
}

inline void stat_log_click_guide(uint32_t id)
{
	struct msglog_item2 item2;
	item2.v1 = id;
	item2.v2 = 1;
	msglog(statistic_file, 0x11314041, time(NULL), &item2, sizeof(item2)); 
}

inline void stat_log_click_doctor(uint32_t id)
{
	struct msglog_item2 item2;
	item2.v1 = id;
	item2.v2 = 1;
	msglog(statistic_file, 0x11314042, time(NULL), &item2, sizeof(item2)); 
}

inline void stat_log_click_game_notice(uint32_t id)
{
	struct msglog_item1 item1;
	item1.v1 = id;
	//msglog(statistic_file, 0x11314043, time(NULL), &item1, sizeof(item1)); 
	msglog(statistic_file, 0x11317001, time(NULL), &item1, sizeof(item1)); 
}

inline void stat_log_user_level(uint32_t id, uint32_t level)
{
	struct msglog_item2 item2;
	item2.v1 = id;
	item2.v2 = level;
	msglog(statistic_file, 0x1100000a, time(NULL), &item2, sizeof(item2)); 
}

inline void stat_log_click(uint32_t id, uint32_t type)
{
	struct msglog_item1 item;
	item.v1 = id;
	switch(type) {
	case 1:
		msglog(statistic_file, 0x11316001, time(NULL), &item, sizeof(item)); 
		break;
	case 2:
		msglog(statistic_file, 0x11316002, time(NULL), &item, sizeof(item)); 
		break;
	case 3:
		msglog(statistic_file, 0x11319001, time(NULL), &item, sizeof(item)); 
		break;
	case 4:
		msglog(statistic_file, 0x11319002, time(NULL), &item, sizeof(item)); 
		break;
	}
}

inline void stat_log_click_egg(uint32_t id, uint32_t count)
{
	struct msglog_item1 item;
	item.v1 = id;
	switch (count) {
	case 1:
		msglog(statistic_file, 0x11316003, time(NULL), &item, sizeof(item)); 
		break;
	case 2:
		msglog(statistic_file, 0x11316004, time(NULL), &item, sizeof(item)); 
		break;
	case 3:
		msglog(statistic_file, 0x11316005, time(NULL), &item, sizeof(item)); 
		break;
	case 4:
		msglog(statistic_file, 0x11316006, time(NULL), &item, sizeof(item)); 
		break;
	}
}

inline void stat_log_login_chest(uint32_t id)
{
	struct msglog_item1 item;
	item.v1 = id;
	msglog(statistic_file, 0x11318001, time(NULL), &item, sizeof(item)); 
}

inline void stat_log_login_reward(uint32_t id, int index)
{
	if (index > 6)
		return;

	struct msglog_item1 item;
	item.v1 = id;
	msglog(statistic_file, 0x11318002+index, time(NULL), &item, sizeof(item)); 
}
