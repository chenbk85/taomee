/*
 * =====================================================================================
 *
 *       Filename:  Cpresent.cpp
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
 *  common.h
 * =====================================================================================
 */

#include "Cargs.h"

Cargs::Cargs(mysql_interface * db)
	:Ctable(db , "RAND_ITEM" , "t_args")
{

}

int Cargs::select(userid_t userid,uint32_t min,uint32_t max,get_args_out_item **pp_key,uint32_t *p_count)
{
	sprintf(this->sqlstr, "select type,value from %s where id = %u and type >= %u and type <= %u",
			this->get_table_name(),userid,min,max);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_key,p_count);
		INT_CPY_NEXT_FIELD((*pp_key+i)->key);
		INT_CPY_NEXT_FIELD((*pp_key+i)->value);
	STD_QUERY_WHILE_END();
}

int Cargs::update(userid_t userid,uint32_t key,uint32_t value)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,%u,%u) ON DUPLICATE KEY UPDATE value=%u",
		this->get_table_name(),userid,key,value,value);
	return this->exec_insert_sql(this->sqlstr, SUCC);
}

