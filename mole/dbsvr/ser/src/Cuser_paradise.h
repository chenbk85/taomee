/*
 * =====================================================================================
 *
 *       Filename:  Cuser_paradise.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/16/2011 01:52:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PARADISE_INC
#define CUSER_PARADISE_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 


class Cuser_paradise:public CtableRoute100x10
{
	private:
		char *msglog_file;
		int get_dimensions(uint32_t level, bool is_vip,
				uint32_t &dimension_1, uint32_t &dimension_2);

	public:
		Cuser_paradise(mysql_interface *db);
		int insert(userid_t userid, bool is_vip);
		int get_all(uint32_t userid, uint32_t *nimbus, uint32_t *exp, uint32_t *limit_count,
				uint32_t *level, paradise_attirelist *p_out, uint32_t *count, uint32_t *exhibit, uint32_t layer);
		int update_nimbus(uint32_t userid, uint32_t &value, uint32_t level);
		int update_nimbus(uint32_t userid, uint32_t value );
		int update_nimbus_inc(uint32_t userid, uint32_t value, uint32_t level);
		int get_nimbus(uint32_t userid, uint32_t &value);
		int update_paradise_background(uint32_t userid, uint32_t new_id, uint32_t pos, uint32_t *background,
				uint32_t layer);
		int get_limit_count(userid_t userid,  uint32_t &ret_limit);
		int update_level_exp(uint32_t userid, uint32_t exp_inc, uint32_t *level, uint32_t *exp,
				uint32_t old_level);
		int get_level(userid_t userid,  uint32_t &level);
		int get_exp(userid_t userid,  uint32_t *exp);
		int exchange_attire_to_nimbus(uint32_t attireid, uint32_t &inc_nimbus);
		int update_limit_count_inc(userid_t userid, uint32_t level, 
				bool is_vip);
		int update_free_count(userid_t userid, uint32_t value);
		int get_bin_paradise_used_attirelist(uint32_t userid, paradise_attirelist *p_out,
				uint32_t layer);
		int get_accesslist(uint32_t userid, user_paradise_get_visitlist_out *accesslist, uint32_t layer);
		int update_paradise_accesslist(uint32_t userid, user_paradise_get_visitlist_out &accesslist,
				uint32_t layer);
		int update_plant_first(userid_t userid,  uint32_t value);
		int get_plant_first(userid_t userid,  uint32_t &first);
		int add_paradise_exp(userid_t userid,  uint32_t value);
		int update_unsuccess(userid_t userid, uint32_t count);
		int get_unsuccess(uint32_t userid, uint32_t &count);
		int update_level(uint32_t userid, uint32_t &out_level);
		int get_config_plant_limit(userid_t userid, uint32_t level,
				        bool is_vip, uint32_t &limit);
		int update_limit_count(userid_t userid, uint32_t count);
		int get_exhibit_angel(userid_t userid,  uint32_t *angelid);
		int update_exhibit_angel(uint32_t userid, uint32_t angel);
		int get_honor_first(uint32_t userid, uint32_t &flag);
		int update_honor_first(userid_t userid, uint32_t flag);
		int get_syn_exp(uint32_t userid, uint32_t &syn_exp);
		int update_syn_exp(userid_t userid, uint32_t syn_exp);
		int get_synthesis_level(userid_t userid, uint32_t &level, uint32_t &exp);
		int get_synthesis_lose_count(userid_t userid, uint32_t *lose_count);
		int update_synthesis_lose_count(userid_t userid, uint32_t lose_count);
};

#endif
