#ifndef __DB_TASK_HPP_
#define __DB_TASK_HPP_


#include "fwd_decl.hpp"
#include "dbproxy.hpp"


//------------------------------------------------------------------
// obtain_task
//------------------------------------------------------------------
int db_obtain_task(player_t *p, uint32_t tskid);
int db_proto_obtain_task_callback(DEFAULT_ARG);


//------------------------------------------------------------------
//	cancel_task 
//------------------------------------------------------------------
int db_cancel_task(player_t *p, uint32_t tskid);
int db_proto_cancel_task_callback(DEFAULT_ARG);


//------------------------------------------------------------------
//	setbuff_task 
//------------------------------------------------------------------
int db_setbuff_task(player_t *p, uint32_t tskid, void *buff, int bufflen);
int db_proto_setbuff_task_callback(DEFAULT_ARG);


//------------------------------------------------------------------
//	finish_task 
//------------------------------------------------------------------
int db_finish_task(player_t *p, uint32_t tskid);
int db_proto_finish_task_callback(DEFAULT_ARG);


//------------------------------------------------------------------
//	get_task_flag_list
//------------------------------------------------------------------
int db_get_task_flag_list(player_t *p, uint32_t begin_tskid, uint32_t end_tskid);
int db_proto_get_task_flag_list_callback(DEFAULT_ARG);


//------------------------------------------------------------------
//	get_task_full_list
//------------------------------------------------------------------
int db_get_task_full_list(player_t *p, uint32_t begin_tskid, uint32_t end_tskid);
int db_proto_get_task_full_list_callback(DEFAULT_ARG);



#endif // __DB_TASK_HPP_
