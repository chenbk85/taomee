/*
 * =====================================================================================
 *
 *       Filename:  Csys_arg.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Csys_arg.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include <algorithm>

//create
Csys_arg::Csys_arg(mysql_interface * db ) :Ctable( db, "SYSARG_DB","t_sys_arg" )
{ 

}

int Csys_arg::insert( uint32_t sys_arg_type,int value )
{
	sprintf( this->sqlstr, "insert into %s values(%u,%d )",
	this->get_table_name(),  sys_arg_type, value );
	STD_SET_RETURN_EX(this->sqlstr, SUCC);	
}

int Csys_arg::update_add( uint32_t sys_arg_type,int value )
{
	sprintf( this->sqlstr, "update %s set value=value+(%d) \
			where type=%u ",
	this->get_table_name(), value , sys_arg_type );
	STD_SET_RETURN_EX(this->sqlstr, KEY_NOFIND_ERR );	
}

int Csys_arg::add_value( uint32_t sys_arg_type,int value )
{
	int ret= this->update_add(sys_arg_type, value);
	if (ret!=SUCC){
		ret= this->insert(sys_arg_type, value ); 
	}
	return ret;
}

int Csys_arg::get_value_list( uint32_t type_start ,uint32_t type_end ,
			uint32_t * p_count,	sysarg_get_count_list_out_item **pp_item  )
{
	sprintf( this->sqlstr, "select type, value from %s where type>=%u and type<=%u order by type ", 
			this->get_table_name(),type_start,type_end);
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_item, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_item+i)->type);
		INT_CPY_NEXT_FIELD((*pp_item+i)->value);
	STD_QUERY_WHILE_END();
}

int Csys_arg::get_value( uint32_t sys_arg_type,int * p_value )
{
	sprintf( this->sqlstr, "select value from %s where type=%u ", 
			this->get_table_name(),sys_arg_type);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, KEY_NOFIND_ERR );
			INT_CPY_NEXT_FIELD(*p_value);
	STD_QUERY_ONE_END();
}

int Csys_arg::update_value(uint32_t sys_arg_type, int value)
{
	sprintf( this->sqlstr, "update %s set value = %u where type = %u",
			this->get_table_name(), value, sys_arg_type);
	STD_SET_RETURN_EX(this->sqlstr, KEY_NOFIND_ERR );	
}

int Csys_arg::set_value(uint32_t sys_arg_type, int value)
{
	int ret = this->update_value(sys_arg_type, value);
	if (ret != SUCC) {
		ret = this->insert(sys_arg_type, value);
	}
	return ret;
}

int Csys_arg::get_team(uint32_t *p_out)
{
//	uint32_t count = 0;
//	sysarg_get_count_list_out_item *p_item = NULL;
//	int ret = this->get_value_list(100, 104, &count, &p_item);
//	if (ret != SUCC) {
//		return ret;
//	}
//	if (count == 0) {
//		return VALUE_OUT_OF_RANGE_ERR;
//	}
//	uint32_t index = p_item->type;
//	int32_t min = p_item->value;
//	for (uint32_t i = 0; i < count; i++) {
//		if (min > (p_item + i)->value) {
//			min = (p_item + i)->value;
//			index = (p_item + i)->type;
//		}
//	}
	int index = 0;	
	*p_out = rand() % 5 + 1;
	if (*p_out > 5) {
		*p_out = 5;
	}
	index = *p_out + 99;
	this->add_value(index, 1);
	//*p_out = index - 99;
	//free(p_item);
	return SUCC;
}

int Csys_arg :: get_npc_and_task(sysarg_get_fire_cup_out *p_item, uint32_t team)
{
	sysarg_get_count_list_out_item *p_npc = NULL;
	uint32_t count = 0;
	int ret  = this->get_value_list(110, 130, &count, &p_npc);
	if (ret != SUCC) {
		return ret;
	}
	p_item->npc_pos = (p_npc + team - 1)->value;
	p_item->npc_state = (p_npc + team + 4)->value;
	p_item->task_id = (p_npc + 9 + team)->value;
	p_item->high_air = (p_npc + 20)->value;
	free(p_npc);
	DEBUG_LOG("===task id %u", p_item->task_id);
	return SUCC;
}


