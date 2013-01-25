/*
 * =====================================================================================
 *
 *       Filename:  Cmonster_handbook.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010/11/08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef  CTASK_INC
#define  CTASK_INC

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"

#include <algorithm>

class Ctask : public CtableRoute
{
public:
	Ctask(mysql_interface * db);
  
	int insert(uint32_t userid, stru_mole2_task_info* p_in);

	int task_set(uint32_t userid, stru_mole2_task_info* p_in);

	int task_get(uint32_t userid, uint32_t taskid, stru_mole2_task_info* p_out);

	int on_list_get(userid_t userid, stru_mole2_task_info** pp_out_item, uint32_t* p_count);

	int other_list_get(userid_t userid, stru_mole2_task_info_simple** pp_out_item, uint32_t* p_count);

	int svr_buf_set(userid_t userid, uint32_t taskid, char* p_buf);

	int task_del(userid_t userid, uint32_t taskid);

	int task_add(userid_t userid, uint32_t taskid);
	int fin_time_get(uint32_t userid, uint32_t taskid, uint32_t* p_time);

	int set_field_value(uint32_t userid, su_mole2_set_field_value_in* p_in);
	int set_state(uint32_t userid, uint32_t taskid, uint32_t state);
	int get_tasks_done(userid_t userid, std::vector<stru_task_done> & tasks);
	int get_tasks_doing(userid_t userid, std::vector<stru_task_doing> & tasks);

};

#endif

