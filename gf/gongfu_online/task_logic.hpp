/*
 * =====================================================================================
 *
 *       Filename:  task_logic.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/01/2011 01:25:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#ifndef TASK_LOGIC_HPP
#define TASK_LOGIC_HPP
#include <map>
struct player_t;
struct task_t;
struct task_going_t;

//serve_buf[20]
//++++++++++++++++++++++++++++++
//|<--4-->|<------->|
// take_tm step_info
//+++++++++++++++++++++++++++++
//

void update_step_by_kill_monster(player_t *p, int mon_id);

void update_step_by_pass_stage(player_t *p, int stage_id);

void init_task_step(player_t * p, task_going_t * task_info);

bool task_step_over(player_t *p,   const task_t * tast);

void do_extra_tasks_finish_logic(player_t * p, int taskid);

int pack_task_monster_info(player_t * p, void * buf, int stage);

#endif


