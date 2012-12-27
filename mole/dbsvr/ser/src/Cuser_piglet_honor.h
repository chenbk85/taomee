/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_honor.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2012 01:18:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PIGLET_HONOR_INCL
#define CUSER_PIGLET_HONOR_INCL


#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_piglet_honor:public CtableRoute100x10
{
	private:

	public:
		Cuser_piglet_honor(mysql_interface *db);
		int insert(userid_t userid, uint32_t type, uint32_t honorid, uint32_t light, 
				uint32_t timestamp, uint32_t cur_times, uint32_t max_times, uint32_t mul_value);
		int update_two_cols(uint32_t userid, uint32_t type, uint32_t honorid, const char* col_1,
			   	const char* col_2, uint32_t value_1, uint32_t value_2);
		int update(userid_t userid, uint32_t type, uint32_t honorid, const char* col, uint32_t value);
		int get_all(userid_t userid, user_piglet_honor_t** pp_tmp, uint32_t *p_count);
		
		int set_honor(userid_t userid, user_set_piglet_honor_in *p_in, uint32_t *flag);
		int add_honor(userid_t userid, uint32_t breed, uint32_t *res);
		int get_honor_by_id(userid_t userid, uint32_t type, uint32_t honorid, uint32_t* light,
			   	uint32_t *cur_times, uint32_t *max_times, uint32_t *mul_values);
};
#endif
