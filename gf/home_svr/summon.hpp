/*
 * =====================================================================================
 *
 *       Filename:  summon.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/12/2011 02:30:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef SUMMON_HPP
#define SUMMON_HPP
#include "player.hpp"
int db_get_summon_list_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

enum {
    max_summon_learn_skills = 5,
};

#pragma pack(1)

struct sum_skill_t {
    uint32_t    skillid;
    uint32_t    skilllv;
};

struct summon_mon_info_t {
	uint32_t	mon_tm;
	uint32_t	mon_type;
	char 		nick[max_nick_size];
	uint32_t 	exp;
	uint32_t	lv;
	uint32_t	fight_value;
	uint32_t	fight_flag;
	int    attr_per;
    uint32_t    skill_cnt;
    sum_skill_t sum_skill[max_summon_learn_skills];
    //uint8_t     skills[50];
};

struct db_get_summon_list_rsp_t {
	uint32_t	mon_cnt;
	summon_mon_info_t	mons[];
};

#pragma pack()

#endif

	


