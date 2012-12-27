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

#ifndef  CSYS_ARG_INCL
#define  CSYS_ARG_INCL
#include "Ctable.h"
#include "proto.h"

class Csys_arg : public Ctable{
	protected:
		int insert( uint32_t sys_arg_type,int value );
		int update_add( uint32_t sys_arg_type,int value );
	public:
		int add_value( uint32_t sys_arg_type,int value );
		int get_value( uint32_t sys_arg_type,int * p_value );
		int set_value( uint32_t sys_arg_type, int value );
		int update_value(uint32_t sys_arg_type, int value);
		int get_value_list( uint32_t type_start ,uint32_t type_end ,
			uint32_t * p_count,	sysarg_get_count_list_out_item **pp_item  );
		Csys_arg(mysql_interface * db );
		int get_team(uint32_t *p_out);
		int get_npc_and_task(sysarg_get_fire_cup_out *p_item, uint32_t team);
};

#endif   /* ----- #ifndef CSYS_ARG_INCL  ----- */

