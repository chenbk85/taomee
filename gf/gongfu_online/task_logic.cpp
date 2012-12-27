/*
 * =====================================================================================
 *
 *       Filename:  task_logic.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/01/2011 01:26:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#include "task_logic.hpp"
#include "player.hpp"
#include "task.hpp"
#include "rank_top.hpp"



void update_step_by_pass_stage(player_t * p, int stage_id)
{
	std::map<uint32_t, task_going_t>::iterator it;
	for (it = p->going_tasks_map->begin() ; it != p->going_tasks_map->end(); ++it) {
		task_going_t * task_info = &(it->second);
		const task_t * task = get_task(task_info->id);
		if (task->type != slave_ring_task && task->type != master_ring_task) {
			uint32_t need_save = 0;
			for (int i = 0; i < task->step_cnt; i++) {
				if (task->step[i].step_type == 2 
						&& task->step[i].stage == stage_id
						&& task_info->serv_buf[4 + i] == 0) {
					task_info->serv_buf[4 + i] ++;
					need_save = 1;
				}
			}
			if (need_save) {
				db_set_task_buf(p, task_info->id, task_info->serv_buf, serv_buf_len, server_buf_type, false);
			}
		}
	}
}

void update_step_by_kill_monster(player_t * p, int mon_id)
{
	std::map<uint32_t, task_going_t>::iterator it;
	for (it = p->going_tasks_map->begin() ; it != p->going_tasks_map->end(); ++it) {
		task_going_t * task_info = &(it->second);
		const task_t * task = get_task(task_info->id);

		if (task->type != slave_ring_task && task->type != master_ring_task) {
			uint32_t need_save = 0;
			for (int i = 0; i < task->step_cnt; i++) {
				if (task->step[i].step_type == 1 
						&& task->step[i].get_id == mon_id
						&& task_info->serv_buf[4 + i] < task->step[i].completeness) {
					task_info->serv_buf[4 + i] ++;
					need_save = 1;
				}
			}
			if (need_save && task_info->id == 3004) {
				db_set_task_buf(p, task_info->id, task_info->serv_buf, serv_buf_len, server_buf_type, false);
			}
		}
	}
}

void init_task_step(player_t * p, task_going_t * task_info)
{
	if (task_info->id == 3004) {
		uint32_t now_tm = get_now_tv()->tv_sec;
		//	const task_t * task = get_task(task_info->id);
		int idx = 0;
		pack_h(task_info->serv_buf, now_tm, idx);
		db_set_task_buf(p, task_info->id, task_info->serv_buf, serv_buf_len, server_buf_type, false);
	}
}

bool task_step_over(player_t *p,   const task_t * tsk)
{
	if (tsk->id == 3004) return true;
	if (tsk->id != 3004) return true;
	std::map<uint32_t, task_going_t>::iterator it = p->going_tasks_map->find(tsk->id);
	if (it != p->going_tasks_map->end()) {
		task_going_t * task_info = &(it->second);
		for (int i = 0; i < tsk->step_cnt; i++) {
			if (tsk->step[i].step_type == 1) {
				if ((task_info->serv_buf[4 + i]) != tsk->step[i].completeness)   
					return false;
			} else if (task_info->serv_buf[4 + i] != 1) {
				return false;
			}
		}
	}
	return true;
}

void do_extra_tasks_finish_logic(player_t *p, int taskid)
{
	std::map<uint32_t, task_going_t>::iterator it = p->going_tasks_map->find(taskid);
	if (it != p->going_tasks_map->end()) {
		task_going_t * task_info = &(it->second);
		uint32_t now_tm = get_now_tv()->tv_sec;
		int idx = 0;
		uint32_t take_tm  = 0;
		unpack_h(task_info->serv_buf, take_tm, idx);
//		idx = 4;

	//	int pre_history_task = 1730;
		if (taskid == 3004) {
			uint32_t active_id = 2;
			uint32_t old_tm = get_player_other_info_value(p, active_id);
			 if (!old_tm || (old_tm && (old_tm > (now_tm - take_tm)))) {
				    rank_info_t rank;
					init_player_rank(p, &rank, now_tm -take_tm); 
					db_insert_rank_info(p, active_id, &rank);
					set_player_other_info(p, active_id, now_tm - take_tm);
			   }
		}
	}
}

int pack_task_monster_info(player_t * p, void * buf, int stage)
{
	std::map<uint32_t, task_going_t>::iterator it;
//   	= p->going_tasks_map->find(taskid);
	int idx = 4;
	uint32_t count = 0;
	for (it = p->going_tasks_map->begin(); it != p->going_tasks_map->end(); ++it) {
	   task_going_t * task_info = &(it->second);
	   const task_t * tsk = get_task(task_info->id);
	   for (int i = 0; i < tsk->step_cnt; i++) {
		   if (tsk->step[i].step_type == 1 && tsk->step[i].stage == stage) {
			   pack_h(buf, tsk->step[i].get_id, idx);
			   count ++;
		   }
	   }
	}	
	int ifx = 0;
	pack(buf, count, ifx);
	return idx;
}




