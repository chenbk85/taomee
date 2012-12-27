/*
 * =====================================================================================
 * 
 *       Filename:  Csys_arg.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CSYS_MSG_INCL
#define  CSYS_MSG_INCL
#include "Ctable.h"
#include "proto.h"

class Csys_msg: public Ctable {

public:
	Csys_msg(mysql_interface * db);

	int insert(sysarg_db_msg_edit_in *p_in);

	int del(uint32_t date, uint32_t start); 

	int update(sysarg_db_msg_edit_in *p_in);

	int msg_edit(sysarg_db_msg_edit_in *p_in);

	int get_list(sysarg_db_msg_get_in *p_in, sysarg_db_msg_get_out_item **pp_list, uint32_t *p_count);

	int get_all(sysarg_db_msg_get_all_web_out_item **pp_list, uint32_t *p_count);

};

#endif   /* ----- #ifndef CSYS_ARG_INCL  ----- */

