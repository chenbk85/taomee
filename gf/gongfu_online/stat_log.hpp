/*
 * =====================================================================================
 *
 *       Filename:  stat_log.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/29/2010 04:14:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */

#ifndef GF_STAT_LOG_HPP
#define GF_STAT_LOG_HPP
#include "player.hpp"
#include "task.hpp"
#include <libtaomee++/inet/pdumanip.hpp>

enum 
{
	log_pkg_size 	= 8192,
	NICK_SIZE       = 16,
};

enum stat_log_cmd_t {
stat_log_proto_hatch_summon_cmd  	  = 4001,
stat_log_proto_evolve_summon_cmd  	= 4002,
stat_log_proto_vip_supplement_cmd	= 4003,
stat_log_proto_finish_promotions_cmd 	= 4004,
stat_log_proto_add_achievement_cmd      = 4005
};

#pragma pack(1)

struct stat_proto_t {
	uint16_t pkglen;
	uint16_t cmd;
	uint32_t userid;
	uint8_t  version;
	uint8_t  body[];
};	

#pragma pack()

inline void init_stat_pkg_head(void *data, uint32_t uid, uint16_t cmd, uint32_t len, uint8_t version)
{
	stat_proto_t * head = reinterpret_cast<stat_proto_t *>(data);
	head->userid = uid;
	head->pkglen = len;
	head->cmd = cmd;
	head->version = version;

}

static uint8_t stat_logbuf[log_pkg_size];

inline void do_stat_log_hatch_summon_mon(player_t *p, uint32_t mon_type)
{
	int idx = sizeof(stat_proto_t); 
	uint32_t tm = time(0);
	taomee::pack_h(stat_logbuf, tm, idx);
	taomee::pack(stat_logbuf, p->nick, NICK_SIZE, idx);
	taomee::pack_h(stat_logbuf, mon_type, idx);
	init_stat_pkg_head(stat_logbuf, p->id, stat_log_proto_hatch_summon_cmd,idx, 1);
	msglog(statistic_logfile, 1, get_now_tv()->tv_sec, stat_logbuf, idx);
}

inline void do_stat_log_evolve_summon_mon(player_t *p, uint32_t mon_type)
{
	int idx = sizeof(stat_proto_t);
	uint32_t tm = time(0);
	taomee::pack_h(stat_logbuf, tm, idx);
	taomee::pack(stat_logbuf, p->nick, NICK_SIZE, idx);
	taomee::pack_h(stat_logbuf, mon_type, idx);
	init_stat_pkg_head(stat_logbuf, p->id, stat_log_proto_evolve_summon_cmd, idx, 1);
	msglog(statistic_logfile, 1, get_now_tv()->tv_sec, stat_logbuf, idx);
}

inline void do_stat_log_user_vip_supplement(player_t *p)
{
	int idx = sizeof(stat_proto_t);
	uint32_t tm = time(0);
	taomee::pack_h(stat_logbuf, tm, idx);
	taomee::pack(stat_logbuf, p->nick, NICK_SIZE, idx);	
	init_stat_pkg_head(stat_logbuf, p->id, stat_log_proto_vip_supplement_cmd, idx, 1);
	msglog(statistic_logfile, 1, get_now_tv()->tv_sec, stat_logbuf, idx);
}

inline void do_stat_log_finish_promotions_tast(player_t *p, uint32_t taskid, uint32_t rewardid = 0)
{
	/*
	int idx = sizeof(stat_proto_t);
	uint32_t tm = time(0);
	taomee::pack_h(stat_logbuf, tm, idx);
	taomee::pack(stat_logbuf, p->nick, NICK_SIZE, idx);
	taomee::pack_h(stat_logbuf, taskid, idx);
	taomee::pack_h(stat_logbuf, rewardid, idx);
	init_stat_pkg_head(stat_logbuf, p->id, stat_log_proto_finish_promotions_cmd, idx, 1);
	msglog(statistic_logfile, 1, get_now_tv()->tv_sec, stat_logbuf, idx);
	*/
}

inline void do_stat_log_add_achievement(player_t *p, uint32_t time, const char* achievement_name, const char* url)
{
	int idx = sizeof(stat_proto_t);
	taomee::pack_h(stat_logbuf, time, idx);
	taomee::pack(stat_logbuf, p->nick, NICK_SIZE, idx);
	taomee::pack(stat_logbuf, achievement_name, 128, idx);
	taomee::pack(stat_logbuf, url, 64, idx);
	init_stat_pkg_head(stat_logbuf, p->id, stat_log_proto_add_achievement_cmd, idx, 1);

	msglog(statistic_logfile, 1, get_now_tv()->tv_sec, stat_logbuf, idx);	
}


inline void do_stat_log_coin_add_reduce(player_t *p, uint32_t cur_coin, const char* channel_str = channel_string_other)
{
	if (p->coins > cur_coin) {
		//reduce
		uint32_t buf[2] = {0, p->coins - cur_coin};
		msglog(statistic_logfile, 0x09522067, get_now_tv()->tv_sec, buf, sizeof(buf));
		LONG_LOG("del_coins\t%u\tcnt:%u,total:%u,channel:%s", p->id, p->coins - cur_coin, p->coins, channel_str);
	} else {
		//add 
		uint32_t buf[2] = {cur_coin - p->coins, 0};
		msglog(statistic_logfile, 0x09522067, get_now_tv()->tv_sec, buf, sizeof(buf));
		LONG_LOG("add_coins\t%u\tcnt:%u,total:%u,channel:%s", p->id, cur_coin - p->coins, p->coins, channel_str);
	}
}


inline void do_stat_log_in_two_total_coin(player_t *p, uint32_t cur_coin, uint32_t new_total_coin, const char* channel_str = channel_string_other)
{
	if (cur_coin > new_total_coin) {
		//reduce
		uint32_t buf[2] = {0, cur_coin - new_total_coin};
		msglog(statistic_logfile, 0x09522067, get_now_tv()->tv_sec, buf, sizeof(buf));
		LONG_LOG("del_coins\t%u\tcnt:%u,total:%u,channel:%s", p->id, cur_coin - new_total_coin, p->coins, channel_str);
	} else {
		//add 
		uint32_t buf[2] = {new_total_coin - cur_coin, 0};
		msglog(statistic_logfile, 0x09522067, get_now_tv()->tv_sec, buf, sizeof(buf));
		LONG_LOG("add_coins\t%u\tcnt:%u,total:%u,channel:%s", p->id, new_total_coin - cur_coin, p->coins, channel_str);
	}
}

inline void do_stat_log_coin_add_reduce(player_t *p, uint32_t add_coin, uint32_t del_coin, const char* channel_str = channel_string_other)
{
	
	if (add_coin) {
		//reduce
		uint32_t buf[2] = {add_coin, 0};
		msglog(statistic_logfile, 0x09522067, get_now_tv()->tv_sec, buf, sizeof(buf));
		LONG_LOG("add_coins\t%u\tcnt:%u,total:%u,channel:%s", p->id, add_coin, p->coins, channel_str);
	}
	if (del_coin) {
		//add 
		uint32_t buf[2] = {0, del_coin};
		msglog(statistic_logfile, 0x09522067, get_now_tv()->tv_sec, buf, sizeof(buf));
		LONG_LOG("del_coins\t%u\tcnt:%u,total:%u,channel:%s", p->id, del_coin, p->coins, channel_str);
	}
}
inline void do_stat_log_exploit_add_reduce(player_t *p, uint32_t add_value, uint32_t del_value, const char* channel_str = channel_string_other)
{
	
	if (add_value) {
		//reduce
		LONG_LOG("add_exploit\t%u\tcnt:%u,total:%u,channel:%s", p->id, add_value, p->exploit, channel_str);
	}
	if (del_value) {
		//add 
		LONG_LOG("del_exploit\t%u\tcnt:%u,total:%u,channel:%s", p->id, del_value, p->exploit, channel_str);
	}
}

inline void do_stat_log_exploit_add_reduce(player_t *p, uint32_t new_value, const char* channel_str = channel_string_other)
{
	
	if (p->exploit >= new_value) {
		//reduce
		LONG_LOG("add_exploit\t%u\tcnt:%u,total:%u,channel:%s", p->id, p->exploit - new_value, p->exploit, channel_str);
	} else {
		//add 
		LONG_LOG("del_exploit\t%u\tcnt:%u,total:%u,channel:%s", p->id, new_value - p->exploit, p->exploit, channel_str);
	}
}


inline void do_stat_log_fumo_add_reduce(player_t *p, uint32_t cur_coin)
{
	if (p->fumo_points_total > cur_coin) {
		//reduce
		uint32_t buf[2] = {0, p->fumo_points_total - cur_coin};
		msglog(statistic_logfile, 0x09522067, get_now_tv()->tv_sec, buf, sizeof(buf));
	} else {
		//add 
		uint32_t buf[2] = {cur_coin - p->fumo_points_total, 0};
		msglog(statistic_logfile, 0x09522067, get_now_tv()->tv_sec, buf, sizeof(buf));
	}
}

inline void do_stat_log_fumo_add_reduce(uint32_t add_coin, uint32_t del_coin)
{
	
	if (add_coin) {
		//reduce
		uint32_t buf[2] = {add_coin, 0};
		msglog(statistic_logfile, 0x09522068, get_now_tv()->tv_sec, buf, sizeof(buf));
	}
	if (del_coin) {
		//add 
		uint32_t buf[2] = {0, del_coin};
		msglog(statistic_logfile, 0x09522068, get_now_tv()->tv_sec, buf, sizeof(buf));
	}
}

inline void do_stat_log_2num(uint32_t cmd, uint32_t num1, uint32_t num2)
{

	uint32_t buf[2] = {num1, num2};
	msglog(statistic_logfile, cmd, get_now_tv()->tv_sec, buf, sizeof(buf));
}


#endif
