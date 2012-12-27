/*
 * =====================================================================================
 *
 *       Filename:  Ccup.cpp
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
#include "Ccup.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//#define INIT_SHM_FLAG  -100 
//create
Ccup::Ccup(mysql_interface * db ) :Ctable( db, "CUP_DB","t_cup" )
{ 
	memset(&(this->group_history),0,sizeof(this->group_history) );
}

int Ccup::insert( stru_group_cup *p_item )
{
	sprintf( this->sqlstr, "insert into %s values( %u, %u,%u,%u,%u)  ",
	this->get_table_name(), get_date(time(NULL)),p_item->groupid,
		p_item->cup1,p_item->cup2,p_item->cup3 );
	STD_INSERT_RETURN(this->sqlstr, DB_ERR );	
}

int Ccup::add_cup_db(int groupid, int cup_type,int value )
{
	sprintf( this->sqlstr, "update %s set cup%u=cup%u+%u \
			where logdate=%u and  groupid=%u ",
	this->get_table_name(), cup_type, cup_type,value,get_date(time(NULL)),groupid );
	STD_SET_RETURN_EX(this->sqlstr, SUCC);	
}

int Ccup::add_cup(int groupid, int cup_type, int value )
{
	int ret ;
	ret=this->add_cup_db(groupid,cup_type,value);
	if (ret!=SUCC ){//没有记录
		stru_group_cup item;
		item.groupid=groupid;
		switch ( cup_type ){
			case 1 :
				item.cup1=1;
				break;
			case 2 :
				item.cup2=1;
				break;
			case 3 :
				item.cup3=1;
				break;
			default :
				break;
		}
		return this->insert(&item);
	}
}

/* 
int Ccup::get_history_list( GROUP_CUP_HISTORY * p_history )
{
	int ret;
	uint32_t now_date=get_date(time(NULL));
	if (now_date!=this->group_history.cachedate){
		ret=this->cache_history_list(now_date);
		if (ret!=SUCC) return ret;
	}
	memcpy(p_history,&(this->group_history),sizeof(*p_history));
	return SUCC;
} 
*/

/*
int Ccup::get_history_list_db( uint32_t cache_date ,GROUP_CUP_LIST**pp_list ,uint32_t *p_count )
{
	sprintf( this->sqlstr, "SELECT logdate,groupid,cup1 FROM %s \
			where logdate<%u  order by logdate asc,cup1 desc",
			this->get_table_name(),20080829);	

	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		(*pp_list+i)->date=atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->groupid=atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->cup1=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();

}
*/

int Ccup::get_group_list( uint32_t date , uint32_t *p_count, stru_group_cup **pp_list )
{
	sprintf( this->sqlstr, "SELECT groupid,cup1,cup2,cup3 FROM %s \
			where logdate=%u ",
			this->get_table_name(),date);	
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		(*pp_list+i)->groupid=atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->cup1=atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->cup2=atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->cup3=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}
/* 
int Ccup::cache_history_list(uint32_t cache_date )
{
	uint32_t db_count;
	GROUP_CUP_LIST * db_list;
	int ret=this->get_history_list_db(cache_date,&db_list,&db_count );
	if (ret!=SUCC){
		return ret;	
	}
	if (db_count>500) db_count=500;
	for (uint32_t i=0;i<(db_count/5); i++){
		this->group_history.group_item[i].date=db_list[i*5].date;
		this->group_history.group_item[i].groupid=db_list[i*5].groupid;
	}
	this->group_history.count=db_count/5;			
	this->group_history.cachedate=cache_date;
	return SUCC;	
} 
*/
