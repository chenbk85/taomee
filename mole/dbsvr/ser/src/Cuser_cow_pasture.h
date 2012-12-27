/*
 * =====================================================================================
 *
 *       Filename:  Cuser_cow_pasture.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/2011 03:47:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CUSER_COW_PASTURE_INC
#define CUSER_COW_PASTURE_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_cow_pasture:public CtableRoute100x10
{
	private:

	public:
		Cuser_cow_pasture(mysql_interface *db);
		int insert(userid_t userid);
		int update_col(userid_t userid, const char *col, uint32_t value);
		int get_col(userid_t userid, const char *col, uint32_t *value);
		int get_all(userid_t userid, uint32_t *mk_storage, uint32_t *background, uint32_t *day_count,
			   	stru_grass_info *grass_info);
		int update_grass_info(userid_t userid, stru_grass_info *grass_info);
		int get_grass_info(userid_t userid, stru_grass_info *grass_info);
		int update_two_col(userid_t userid, const char *col_1, const char * col_2, 
					uint32_t value_1, uint32_t value_2);
		int get_two_col(userid_t userid, const char *col_1, const char *col_2,
					uint32_t *value_1, uint32_t *value_2);

};
#endif

