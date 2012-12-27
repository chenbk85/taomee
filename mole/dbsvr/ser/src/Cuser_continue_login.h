/*
 * =====================================================================================
 *
 *       Filename:  Cuser_continue_login.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/24/2011 04:20:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_CONTINUE_LOGIN_INC
#define CUSER_CONTINUE_LOGIN_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_continue_login:public CtableRoute100x10
{

	public:
		Cuser_continue_login(mysql_interface *db);
		int insert(userid_t userid, uint32_t type, uint32_t count = 1);
		int update_count(userid_t userid, uint32_t type, uint32_t count);
		int update_date(userid_t userid, uint32_t type, uint32_t date);
		int update_all(userid_t userid, uint32_t type, uint32_t count, uint32_t date);
		int select(userid_t userid, uint32_t type, uint32_t *count, uint32_t *date);
		int cal_continue(userid_t userid, uint32_t type, user_continue_login_shop_out *p_out);
		int cal_superlamu_value_continue(userid_t userid, uint32_t type, uint32_t* add_val);
		
		int cal_national_day_continue(userid_t userid, uint32_t type);
		int get_national_day_info(userid_t userid, uint32_t type, uint32_t* count);
		int set_national_day(userid_t userid, uint32_t type, uint32_t gift_cnt, uint32_t *state);
		int get_xhx_task_state(userid_t userid, uint32_t flag, uint32_t type, uint32_t *state);
		int update_inc(uint32_t userid, uint32_t type, uint32_t count);
		int get_two_cols(userid_t userid, uint32_t type, const char* col_1, const char* col_2, 
				uint32_t *value_1, uint32_t *value_2);
		int update_flags(userid_t userid, uint32_t type, uint32_t flags);
		int update_info(userid_t userid, uint32_t type);
		int get_flags(userid_t userid, uint32_t type, uint32_t *flags);
		int set_vip_login_flags(userid_t userid, uint32_t type, uint32_t index, uint32_t *state);
		int insert(userid_t userid, uint32_t type, uint32_t count, uint32_t datetime);
		int set_continue_login_weekend(userid_t userid, uint32_t type);
		int get_count(userid_t userid, uint32_t type, uint32_t *count);
		int	insert(userid_t userid, uint32_t type, uint32_t count, uint32_t datetime, uint32_t flags);
};

#endif
