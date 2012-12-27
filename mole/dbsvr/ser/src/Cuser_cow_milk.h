/*
 * =====================================================================================
 *
 *       Filename:  Cuser_cow_milk.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/2011 11:14:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_COW_MILK_INC
#define CUSER_COW_MILK_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_milk_cow:public CtableRoute100x10{
	private:

	public:
		Cuser_milk_cow(mysql_interface *db);
		int insert(userid_t userid,  uint32_t cowid);
		int cal_cow_attribute(userid_t userid, stru_milk_cow_info *one, stru_grass_info *grass_info,
			   			uint32_t today);
		int update_attribute(userid_t userid, uint32_t cowid, const char *col, uint32_t value);
		int get_all(userid_t userid, stru_milk_cow_info **pplist, uint32_t *p_count);
		int get_milk_cow_by_id(userid_t userid, uint32_t id, stru_milk_cow_info *ret_cow);
		int set_attributes(userid_t userid, stru_milk_cow_info *cow, uint32_t datetime);
		int get_count(userid_t userid, uint32_t *count);

};
#endif
