#ifndef _CGF_RING_TASK_H_
#define _CGF_RING_TASK_H_

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"


class Cgf_ring_task: public CtableRoute
{
public:
	Cgf_ring_task(mysql_interface * db);
	int get_ring_task_list(userid_t userid, uint32_t role_regtime, gf_get_ring_task_list_out_element** p_data, uint32_t* count);
	int replace_ring_task(userid_t userid, uint32_t role_regtime, uint32_t task_id, uint32_t type, uint32_t master_task_id, uint32_t task_group_id);
	int delete_ring_master_task(userid_t userid, uint32_t role_regtime, uint32_t task_id);
	int delete_ring_slave_task(userid_t userid, uint32_t role_regtime, uint32_t task_id);

	int clear_role_info(userid_t userid,uint32_t role_regtime);
};

class Cgf_ring_task_history: public CtableRoute
{
public:
	Cgf_ring_task_history(mysql_interface * db);
	int get_ring_task_history_list(userid_t userid, uint32_t role_regtime, gf_get_ring_task_history_list_out_element** p_data, uint32_t* count);
	int replace_ring_task_history(userid_t  userid, uint32_t role_regtime, uint32_t task_id, uint32_t day_count, uint32_t cur_time);

	int clear_role_info(userid_t userid,uint32_t role_regtime);
};


#endif
