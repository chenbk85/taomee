/*
 * =====================================================================================
 *
 *       Filename:  Cuser_farm_fairy.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/18/2011 09:31:24 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_FARM_FAIRY_INC
#define CUSER_FARM_FAIRY_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "algorithm"
#include <map>
class Cuser_farm_fairy:public CtableRoute100x10 {
	public:
		Cuser_farm_fairy(mysql_interface *db);
		int insert(userid_t userid, user_farm_fairy_in *p_in);
		int update_state_by_id(userid_t userid, uint32_t id, uint32_t state);
		int get_fairy_list(userid_t userid, user_farm_get_fairy_out_item **pp_list, uint32_t *count);
		int get_level_list(userid_t userid, std::map<uint32_t, uint32_t> &mp_level);
		int delete_fairy_by_id(userid_t userid, uint32_t id);
		int get_state_out_count(userid_t userid, uint32_t &count_in_fairy);
		int get_state_count(userid_t userid, uint32_t state, uint32_t &count_in_fairy);
		int get_fairy(userid_t userid, uint32_t id, uint32_t &animal_id, uint32_t &level);
		int update_state_outgo(userid_t userid);
		int get_fairyid(userid_t userid, uint32_t id, uint32_t &animal_id);
		int get_count(userid_t userid, uint32_t &count);
		int get_warehouse_count(userid_t userid, uint32_t &count_in_fairy);
		int convert_to_angel(uint32_t userid, uint32_t id, uint32_t *angelid, uint32_t *success, uint32_t *type);
		int get_fairyid_level(userid_t userid, uint32_t id, uint32_t &animal_id, uint32_t &level);
		int get_state(userid_t userid, uint32_t id, uint32_t *state);
	private:
};

#endif
