/*
 * ===============
 *
 *       Filename:  Cusermap_day.cpp
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
#include "Cusermap_day.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include <sstream>

//create
Cusermap_day::Cusermap_day(mysql_interface * db ) :Ctable( db,"SYSARG_DB","t_day")
{ 

}

int Cusermap_day::insert(uint32_t type ,userid_t userid,uint32_t count )
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u)", 
	this->get_table_name(), type, userid,count);
	STD_INSERT_RETURN(this->sqlstr, EMAIL_EXISTED_ERR);	
}

int Cusermap_day::update(uint32_t type ,userid_t userid,uint32_t count )
{
	sprintf( this->sqlstr, "update %s set count=%u where  type=%u and  userid=%u", 
	this->get_table_name(), count ,type, userid);
	STD_SET_RETURN_EX(this->sqlstr, EMAIL_EXISTED_ERR);	
}

int Cusermap_day::get(uint32_t type ,userid_t userid,uint32_t * p_count  )
{
	sprintf( this->sqlstr, "select count from %s where type=%u and userid=%u ", 
	this->get_table_name(), type, userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Cusermap_day::get_ex(uint32_t type ,userid_t userid ,uint32_t * p_count )
{
	int ret=this->get(type,userid, p_count );
	if (ret==USER_ID_NOFIND_ERR){
		*p_count=0;
		return SUCC;
	}else{
		return ret;
	}
}

int Cusermap_day::add( uint32_t type ,userid_t userid, uint32_t maxvalue )
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

int Cusermap_day::add_cnt( uint32_t type ,userid_t userid, uint32_t maxvalue,uint32_t cnt, uint32_t *total)
{
    uint32_t count;
	int ret=this->get_ex(type,userid, &count );
	if(ret!=SUCC) return ret;
	if(count >= maxvalue || cnt > maxvalue) {
		return EMAIL_EXISTED_ERR;
	}
	if ((type == 50000 || type == 50001 || type == 50017) && (cnt & count)) {//已经答过题了
		return EMAIL_EXISTED_ERR;
	}

	if(count == 0) {
		*total = cnt;
		return this->insert(type,userid,cnt);
	} else {
		*total = cnt + count;
		return this->update(type,userid,count + cnt);
	}
}

int Cusermap_day::remove( userid_t userid )
{
	sprintf( this->sqlstr, "delete from %s where  userid=%u ", 
	this->get_table_name(),  userid);
	STD_SET_LIST_RETURN( sqlstr ); 
}

int Cusermap_day::remove_by_type ( userid_t userid, uint32_t type )
{
	sprintf( this->sqlstr, "delete from %s where type=%u and userid=%u ", 
	this->get_table_name(), type, userid);
	STD_SET_LIST_RETURN( sqlstr ); 
}
int Cusermap_day::get_value_list(uint32_t userid, uint32_t * p_count,
		sysarg_day_get_list_by_userid_out_item **pp_item  )
{
	sprintf( this->sqlstr, "select type, count from %s where userid=%u  ", 
			this->get_table_name(),userid);
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_item, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_item+i)->type);
		INT_CPY_NEXT_FIELD((*pp_item+i)->count );
	STD_QUERY_WHILE_END();
}
/* @brief 根据米米号得到某个范围内的count值
 * @param userid 用户的米米号 
 * @param start_type
 * @param end_type
 * @param p_count
 * @param pp_item
 */
int Cusermap_day::get_value_list_by_type(uint32_t userid, uint32_t start_type, uint32_t end_type, uint32_t * p_count,
	 sysarg_day_get_list_out_item **pp_item  )
{
	sprintf( this->sqlstr, "select type, count from %s where userid=%u and type >= %u and type <= %u", 
			this->get_table_name(),
			userid,
			start_type,
			end_type
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_item, p_count) ;
		INT_CPY_NEXT_FIELD((*pp_item+i)->type);
		INT_CPY_NEXT_FIELD((*pp_item+i)->value );
	STD_QUERY_WHILE_END();
}

int Cusermap_day::get_value_list_by_some_type(userid_t userid, uint32_t in_count, 
		sysarg_day_get_value_in_item* p_in_item, sysarg_day_get_value_out_item **pp_out_item, uint32_t *out_count)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_item + i)->type;
		if(i < in_count - 1){
			in_str << ',';
		}
	}

	sprintf(this->sqlstr, "select type, count from %s where userid = %u and  type in (%s)",
			this->get_table_name(),
			 userid, 
			 in_str.str().c_str() 
			 );

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, out_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->type);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->count);
	STD_QUERY_WHILE_END();
}
