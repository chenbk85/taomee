/*
 * =====================================================================================
 *
 *       Filename:  Cquestionnaire.cpp
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
#include "Cquestionnaire.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//create
Cquestionnaire::Cquestionnaire(mysql_interface * db ) :Ctable( db, "SYSARG_DB","t_questionnaire" )
{ 
}

int Cquestionnaire::insert(uint32_t type, uint32_t maintype,uint32_t subtype, int value )
{
	sprintf( this->sqlstr, "insert into %s values(%u,%u,%u,%d )",
	this->get_table_name(), type, maintype, subtype, value );
	STD_SET_RETURN_EX(this->sqlstr, SUCC);	
}

int Cquestionnaire::update_add(uint32_t type, uint32_t maintype,uint32_t subtype, int value )
{
	sprintf( this->sqlstr, "update %s set value=value+(%d) \
			where type=%u and  maintype=%u and subtype=%u  ",
	this->get_table_name(), value, type, maintype, subtype );
	STD_SET_RETURN_EX(this->sqlstr, KEY_NOFIND_ERR );	
}

int Cquestionnaire::add_value(uint32_t type, uint32_t maintype,uint32_t subtype, int value )
{
	int ret= this->update_add(type, maintype,subtype , value);
	if (ret!=SUCC){
		ret= this->insert(type, maintype, subtype, value ); 
	}
	return ret;
}

int Cquestionnaire::get_value_list(uint32_t type, uint32_t * p_count,	sysarg_get_questionnaire_list_out_item **pp_item  )
{
	sprintf( this->sqlstr, "select maintype,subtype, value from %s where type = %d  order by maintype,subtype  ", 
			this->get_table_name(), type);
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_item, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_item+i)->maintype);
		INT_CPY_NEXT_FIELD((*pp_item+i)->subtype);
		INT_CPY_NEXT_FIELD((*pp_item+i)->count );
	STD_QUERY_WHILE_END();
}

/*
int Cquestionnaire::get_value( uint32_t questionnaire_type,int * p_value )
{
	sprintf( this->sqlstr, "select value from %s where type=%u ", 
			this->get_table_name(),questionnaire_type);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, KEY_NOFIND_ERR );
			INT_CPY_NEXT_FIELD(*p_value);
	STD_QUERY_ONE_END();
}
*/

int Cquestionnaire::get_subtype_with_max_value(uint32_t type, uint32_t maintype, uint32_t* sub_type)
{
	sprintf(this->sqlstr, "select subtype, from %s where type = %d and maintype = %u order by value desc limit 1",
			this->get_table_name(), type, maintype);

	STD_QUERY_ONE_BEGIN(this->sqlstr, KEY_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*sub_type);
	STD_QUERY_ONE_END();
}
