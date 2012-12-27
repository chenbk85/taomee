/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_task_ex.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  03/23/2009 02:34:47 PM CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "Cuser.h"


#ifndef  CUSER_NPC_TASK_INC
#define  CUSER_NPC_TASK_INC 

class Cuser_npc_task : public CtableRoute100x10 {

public:
	Cuser_npc_task(mysql_interface *mysql);

	int update_buf(userid_t userid, uint32_t npc, uint32_t taskid, char *p_char);

	int finish_task(userid_t userid, uint32_t npc, uint32_t taskid, uint32_t *p_num);

	int recv_task(userid_t userid, uint32_t npc, uint32_t taskid, uint32_t maxnum, uint32_t week_day, uint32_t *p_num);

	int update(userid_t userid, uint32_t npc, uint32_t taskid, uint32_t state, uint32_t num, uint32_t now);

	int get_task_info(userid_t userid, uint32_t npc, uint32_t taskid, uint32_t *p_state,
		uint32_t *p_num, uint32_t *p_flag, uint32_t *p_tm);

	int get_buf(userid_t userid, uint32_t npc, uint32_t taskid, char *p_out);

	int insert(userid_t userid, uint32_t npc, uint32_t taskid, uint32_t week_day);

	int get_list(userid_t userid, uint32_t npc,  user_npc_task_get_npc_task_out_item **pp_list, uint32_t *p_count);

	int check_same_time(uint32_t date, uint32_t flag, uint32_t *p_flag, uint32_t *p_now);

	int delete_task(const userid_t uid, const uint32_t npc, const uint32_t taskid);
};
#endif   /* ----- #ifndef CUSER_DATA_INC  ----- */

