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


#ifndef  CUSER_DATA_INC
#define  CUSER_DATA_INC

class Cuser_task_ex : public CtableRoute100x10 {

public:
	Cuser_task_ex(mysql_interface *mysql);

	int insert(const uint32_t userid, user_task_set_in *p_in);

	int update(userid_t userid, user_task_set_in *p_in);

	int del(userid_t userid, uint32_t taskid);

	int get(userid_t userid, uint32_t taskid, user_task_get_out *p_out);

	int  get_list(userid_t userid, stru_user_task_ex **pp_list, uint32_t *p_count);

	int get_task_id_list(userid_t userid, get_user_task_ex_idlist_out_item **pp_list, uint32_t *p_count);
	int get_specify_list(userid_t userid, uint32_t in_count, user_get_task_list_in_item *p_in_item,
	  	user_get_task_list_out_item** p_out_item, uint32_t *out_count);
};
#endif   /* ----- #ifndef CUSER_DATA_INC  ----- */

