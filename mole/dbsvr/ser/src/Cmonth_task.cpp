/*
 * ===============
 *
 *       Filename:  Cmonth_task.cpp
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
 * ==============
 */
#include "Cmonth_task.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"

//create
Cmonth_task::Cmonth_task(mysql_interface * db ) :CtableRoute100( db,"SYSARG_DB","t_month_task", "userid")
{ 

}

int Cmonth_task::insert(uint32_t type ,userid_t userid,uint32_t count )
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u)", 
			this->get_table_name(userid),
			type, 
			userid,
			count
		  );
	STD_INSERT_RETURN(this->sqlstr, EMAIL_EXISTED_ERR);	
}

int Cmonth_task::update(uint32_t type ,userid_t userid,uint32_t count )
{
	sprintf( this->sqlstr, "update %s set count=%u where  type=%u and  userid=%u", 
	this->get_table_name(userid), count ,type, userid);
	STD_SET_RETURN_EX(this->sqlstr, EMAIL_EXISTED_ERR);	
}

int Cmonth_task::get(uint32_t type ,userid_t userid,uint32_t * p_count  )
{
	sprintf( this->sqlstr, "select count from %s where type=%u and userid=%u ", 
	this->get_table_name(userid), type, userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Cmonth_task::get_ex(uint32_t type ,userid_t userid ,uint32_t * p_count )
{
	int ret=this->get(type,userid, p_count );
	if (ret==USER_ID_NOFIND_ERR){
		*p_count=0;
		return SUCC;
	}else{
		return ret;
	}
}

int Cmonth_task::add( uint32_t type ,userid_t userid, uint32_t maxvalue )
{
	uint32_t count;
	int ret=this->get_ex(type,userid, &count );
	if(ret!=SUCC) return ret;
	count++;
	if (count>maxvalue){
		return EMAIL_EXISTED_ERR;
	}
	if (count==1){
		//要insert;
		return this->insert(type,userid,count);
	}else{
		return this->update(type,userid,count);
	}
}

int Cmonth_task::remove( userid_t userid )
{
	sprintf( this->sqlstr, "delete from %s where  userid=%u ", 
	this->get_table_name(userid),  userid);
	STD_SET_LIST_RETURN( sqlstr ); 
}

int Cmonth_task::remove_by_type ( userid_t userid, uint32_t type )
{
	sprintf( this->sqlstr, "delete from %s where type=%u and userid=%u ", 
	this->get_table_name(userid), type, userid);
	STD_SET_LIST_RETURN( sqlstr ); 
}

int Cmonth_task::get_value_list(uint32_t userid, uint32_t * p_count,
		sysarg_day_get_list_by_userid_out_item **pp_item  )
{
	sprintf( this->sqlstr, "select type, count from %s where userid=%u  ", 
			this->get_table_name(userid),userid);
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_item, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_item+i)->type);
		INT_CPY_NEXT_FIELD((*pp_item+i)->count );
	STD_QUERY_WHILE_END();
}


