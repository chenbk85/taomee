/*
 * =====================================================================================
 *
 *       Filename:  Cuser_farm_fairy_skill.h
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  01/20/2011 03:05:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_FARM_FAIRY_SKILL
#define CUSER_FARM_FAIRY_SKILL


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "algorithm"
#include <map>

class Cuser_farm_fairy_skill:public CtableRoute100x10
{
	public:
		Cuser_farm_fairy_skill(mysql_interface *);
		int update(uint32_t userid, uint32_t id, uint32_t animalid, uint32_t skillid, uint32_t cold_time,
				uint32_t used_count, uint32_t type);
		int get_user_animal_skill_list(userid_t userid, uint32_t id, user_farm_fairy_skill_used_count *pp_list, 
				uint32_t *skill_count);
		int delete_skill(userid_t userid, uint32_t id, uint32_t skillid);
		int insert(userid_t userid, uint32_t id, uint32_t animalid, uint32_t skillid, uint32_t cold_time, 
			uint32_t used_count, uint32_t type, uint32_t date);
		int get_animalid_all_skill_info(userid_t userid, uint32_t id, user_fairy_skill_get_info_out_item **pp_list, 
				uint32_t *count);

		int update_animal_one_skill(userid_t userid, user_fairy_skill_update_info_in *p_in, uint32_t used_count, 
				uint32_t cold_time, uint32_t date);
		int get_animal_one_skill_info(userid_t userid, uint32_t id, uint32_t skillid, user_fairy_skill_update_info_out *out, uint32_t *date);
	private:
};
#endif

