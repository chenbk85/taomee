/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_task.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/14/2011 05:19:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PIGLET_TASK_INC
#define CUSER_PIGLET_TASK_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_piglet_task:public CtableRoute100x10{
	private:
	
	public:
		Cuser_piglet_task(mysql_interface *db);
		int insert(userid_t userid, uint32_t taskid, uint32_t type, uint32_t maxcount);
		int update(userid_t userid, uint32_t taskid, const char* col, uint32_t value);
		int get_one_col(userid_t userid, uint32_t taskid, const char *col, uint32_t *value);
		int get_two_col(userid_t userid, uint32_t taskid, const char *col_1, const char* col_2, 
				uint32_t *value_1, uint32_t *value_2);
		int get_all_task(userid_t userid, user_get_piglet_day_task_out_item **pp_list, uint32_t *p_count);
		int get_guide_task_count(userid_t userid, uint32_t *p_count);
		int clear_task(userid_t userid);
		int update_two_col(userid_t userid, uint32_t taskid, const char* col_1, const char* col_2,
				uint32_t value_1, uint32_t value_2);
		int change_task_state(userid_t userid, uint32_t taskid);
		int get_day_task_count(userid_t userid, uint32_t *count);
		int update_state_count(userid_t userid, uint32_t taskid, uint32_t state, uint32_t count, uint32_t maxcount);
		int get_finish_day_task_count(userid_t userid, cp_task_info_t** pp_list, uint32_t* p_count);
};
#endif
