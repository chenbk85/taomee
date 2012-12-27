/*
 * =====================================================================================
 *
 *       Filename:  Cuser_paradise_contract.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/2011 05:30:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PARADISE_CONTRACT_INC
#define CUSER_PARADISE_CONTRACT_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_paradise_contract:public CtableRoute100x10
{
	private:
		char *msglog_file;
	public:
		Cuser_paradise_contract(mysql_interface *db);
		int get_angelid_speci(userid_t userid, uint32_t angelid, uint32_t &count);
		int insert(userid_t userid, uint32_t angelid, uint32_t count, uint32_t type);
		int update(userid_t userid, uint32_t angelid, uint32_t value);
		int update_used_count(userid_t userid, uint32_t angelid, uint32_t value);
		int get_all(userid_t userid, user_get_angel_contract_all_out_item **pp_list, uint32_t *count);
		int get_by_angelid(userid_t userid, uint32_t angelid, uint32_t *count);
		int get_by_angelid_terminate(userid_t userid, uint32_t angelid, uint32_t *count, uint32_t *total);
		int sign_contract(userid_t userid, uint32_t angelid, uint32_t value, uint32_t type);
		int del_contract_angel(userid_t userid, uint32_t angelid, uint32_t count);
		int mapping_to_dragon(uint32_t angelid, uint32_t *dragonid);
		int mapping_to_angel(uint32_t dragonid, uint32_t *angelid);
		int get_type(userid_t userid, uint32_t angelid, uint32_t *type);
		int get_type_used_count(userid_t userid, uint32_t angelid, uint32_t *type, uint32_t *used_count);
		int clear_used_count_no_dragon(userid_t userid);
		int get_count_type2(userid_t userid,  uint32_t &count);
		int get_counts(userid_t userid,  uint32_t **pp_list, uint32_t *p_count);
		int update_used_count_dragon(userid_t userid, uint32_t value);
		int get_contract_angel_count(userid_t userid, uint32_t *count);
		int get_battle_angel_info(uint32_t userid,  user_get_battle_angel_info_out_item **pp_list, 
		   uint32_t *count);
		int update_hurt_count_inc(userid_t userid, uint32_t angelid, uint32_t value);
		int update_hurt_count(userid_t userid, uint32_t angelid, uint32_t value);
		int get_hurt_count(userid_t userid, uint32_t angelid, uint32_t *count);
		int get_angel_in_favorite(uint32_t userid, user_get_angel_favorite_out_item **pp_list,
					uint32_t *count);
		int get_angelid_count(userid_t userid, uint32_t angelid, uint32_t &count);
		int update_count_by_angelid(userid_t userid, uint32_t exhibit_angelid, int32_t value);
		int get_contract_angel_all_ex(userid_t userid, user_get_angel_contract_all_ex_out_item **pp_list,
					uint32_t *count);
		int set_angel_type(userid_t userid, uint32_t angelid, uint32_t type);
		int get_vip_angel(userid_t userid, uint32_t *vip_angel, uint32_t count, 
				vip_angel_t **pp_list, uint32_t *p_count);
		int set_angelid(userid_t userid, uint32_t old_angelid, uint32_t new_angelid);
		int get_another_count(userid_t userid, uint32_t angelid, uint32_t &count);
		int del_contract_angel_another(userid_t userid, uint32_t angelid, uint32_t count);
		int update_count_by_angelid_another(userid_t userid, uint32_t exhibit_angelid, int32_t value);
};
#endif
