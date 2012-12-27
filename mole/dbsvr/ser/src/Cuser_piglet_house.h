/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_house.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/06/2011 04:54:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PIGLET_HOUSER_INC
#define CUSER_PIGLET_HOUSER_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_piglet_house: public CtableRoute100x10{
	private:

	public:
		Cuser_piglet_house(mysql_interface *db);
		int insert(userid_t userid, uint32_t logintime);
		int update_double_col(userid_t userid, const char* col_1,  const char* col_2, uint32_t value_1, uint32_t value_2);
		int get_double_col(userid_t userid, const char* col_1, const char* col_2, uint32_t *value_1, uint32_t *value_2);
		int get_piglet_house(userid_t userid, user_get_piglet_house_out_header* out, uint32_t* login_time);
		int get_level(uint32_t userid, uint32_t* level);
		int entry_piglet_house(userid_t userid, user_get_piglet_house_out_header* out, uint32_t* logintime, uint32_t* first_flag);
		int clear(userid_t userid, uint32_t now);
		int feed_piglets(userid_t userid, uint32_t feed_type, uint32_t exp);
		int update_inc_col(userid_t userid, const char* col, uint32_t value);
		int get_login_game(userid_t userid, uint32_t *exp, uint32_t *level);
		int change_formation(userid_t userid, user_change_piglet_formation_in *p_in);
		int update_inc_double(userid_t userid, const char* col_1, const char* col_2, uint32_t value_1, uint32_t value_2); 
		int cal_factory_level(uint32_t userid, uint32_t *factory_level);
		int set_poll(userid_t userid, uint32_t index);
		int get_levels(userid_t userid, uint32_t *exp, uint32_t *level, uint32_t *machine_level, uint32_t *warehouse_level );
};
#endif
