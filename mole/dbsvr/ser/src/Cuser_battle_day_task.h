/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_day_task.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/19/2011 10:29:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_DAY_TASK_INC
#define CUSER_BATTLE_DAY_TASK_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_battle_day_task:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_battle_day_task(mysql_interface *db);
		int insert(userid_t userid, uint32_t taskid, uint32_t type,
				uint32_t maxcount, uint32_t object_id);
		int update(userid_t userid, uint32_t taskid, const char* col, uint32_t value);
		int clear_old_task(userid_t userid);
		int get_day_task(userid_t userid, battle_day_tsk_t **pp_list, uint32_t *p_count);
		int get_one_column(userid_t userid, uint32_t taskid, const char* col,uint32_t *value);
		int get_taskid(userid_t userid, battle_task_id_t **pp_list, uint32_t *p_count);
		int del_task_by_id(userid_t userid, uint32_t taskid);
		int update_one_task(userid_t userid, uint32_t taskid, uint32_t type, 
				uint32_t maxcount, uint32_t object_id, uint32_t old_id);
		int update_day_task(userid_t userid, uint32_t taskid);
		int get_taskid_by_object_type(userid_t userid, uint32_t type, uint32_t object_id,
				battle_task_id_t **pp_list, uint32_t *p_count);
		int get_two_columns(userid_t userid, uint32_t taskid, const char* col_1,
					const char* col_2, uint32_t *value_1, uint32_t *value_2);
};
#endif
