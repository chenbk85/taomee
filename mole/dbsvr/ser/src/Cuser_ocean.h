/*
 * =====================================================================================
 *
 *       Filename:  Cuser_ocean.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/22/2012 10:01:40 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_OCEAN_INCL
#define CUSER_OCEAN_INCL


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

#define FURNISH_CNT  100 

class Cuser_ocean:public CtableRoute100x10{
	private:
		int insert(userid_t userid, uint32_t oceanid, uint32_t siren);
	public:
		Cuser_ocean(mysql_interface *db);
		int add(userid_t userid, uint32_t siren);
		int diy(userid_t userid, uint32_t oceanid, diy_t& furnish, uint32_t &itemid);
		int get_current_ocean_count(userid_t userid, uint32_t &count);
		int get_all(userid_t userid, uint32_t oceanid, furnish_diy_t *diy, uint32_t *capacity);
		int get_diy(userid_t userid, uint32_t oceanid, furnish_diy_t* diy);
		int get_one_col(userid_t userid, uint32_t oceanid, const char* col, uint32_t *value);
		int update_one_col(userid_t userid, uint32_t oceanid, const char* col, uint32_t value);
		uint32_t get_level(uint32_t exp );
		int update(userid_t userid, uint32_t oceanid, furnish_diy_t* diy);
		uint32_t get_upgrade_exp(uint32_t level);
};
#endif
