/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_work.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/2012 05:01:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PIGLET_WORK_INCL
#define CUSER_PIGLET_WORK_INCL


#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

typedef struct ore_need_time{
	uint32_t ore_id;
	uint32_t need_time;
}ore_time_t;

class Cuser_piglet_work:public CtableRoute100x10
{
	public:
		Cuser_piglet_work(mysql_interface *db);
		int add(userid_t userid, uint32_t tool_type, uint32_t tool_index, uint32_t tool_itemid,
				uint32_t work_end_time, work_piglets_t* work_piglets, uint32_t raw_material, uint32_t cnt);
		int finish_work(userid_t userid, uint32_t tool_type, uint32_t tool_index);
		int upgrade(userid_t userid, uint32_t tool_type, uint32_t tool_index, uint32_t itemid);
		int get_work_info(userid_t userid, uint32_t *p_count, user_get_piglet_machine_work_out_item_2 **pp_list);
		int start_working(userid_t userid, uint32_t tool_type, uint32_t tool_index, uint32_t end_time,
			   	uint32_t need_time, work_piglets_t* work_piglets, uint32_t material, uint32_t cnt);
		int get_machine(userid_t userid, uint32_t tool_type, uint32_t *count);
		int get_need_count(userid_t userid, uint32_t stove_index, uint32_t batch, uint32_t *need_count);
		int find(uint32_t part_id, uint32_t type, part_material_t* pos);
		int get_one_col(userid_t userid, uint32_t tool_type, uint32_t tool_index, const char *col, uint32_t *value);
		int get_two_col(userid_t userid, uint32_t tool_type, uint32_t tool_index, const char *col_1,
				const char *col_2, uint32_t *value_1, uint32_t *value_2);
		int check_tool_satisfy_material(uint32_t type, uint32_t tool, uint32_t material_id,  uint32_t *flag);
		int get_piglets_datetime(userid_t userid, uint32_t tool_type, uint32_t tool_index,
			   	work_piglets_t* piglets, uint32_t *endtime, uint32_t* obj_part);
		int finish_working(userid_t userid, uint32_t tool_type, uint32_t tool_index, work_piglets_t *work_piglets);
		int get_tool_level(uint32_t tool_type, uint32_t tool_itemid, uint32_t *level);
		int get_one_tool_info(userid_t userid, uint32_t tool_type, uint32_t tool_index, 
				uint32_t *level, uint32_t *end_time, work_piglets_t *work_piglets);
		int update_one_col(userid_t userid, uint32_t tool_type, uint32_t tool_index, const char* col, uint32_t value);
		int accelerate_machine(userid_t userid, uint32_t type, uint32_t index, uint32_t time, uint32_t *state);
		int get_one_another_info(userid_t userid, uint32_t tool_type, uint32_t tool_index, 
				uint32_t *level, uint32_t *work_state, uint32_t *left_time);
		int get_stove_product(uint32_t ore_id, uint32_t *obj_product, uint32_t *count);
		int get_piglets_worktime(uint32_t userid, uint32_t tool_type, uint32_t tool_index,uint32_t* endtime);
		int slow_down(uint32_t userid, uint32_t tool_type, uint32_t tool_index, uint32_t tool_id, uint32_t *state);
		int recover(uint32_t userid, uint32_t tool_type, uint32_t tool_index, uint32_t tool_id, uint32_t *state);
		uint32_t get_ore_time(uint32_t ore_id, uint32_t batch);
};
#endif
