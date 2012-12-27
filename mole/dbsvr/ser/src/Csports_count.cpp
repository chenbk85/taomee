/*
 * =====================================================================================
 *
 *       Filename:  Csports_count.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年02月16日 10时10分17秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Csports_count.h" 
#include "benchapi.h"

Csports_count::Csports_count(mysql_interface * db ) :Ctable( db, "PET_SPORTS_DB","t_sports_count" )
{ 
}

int Csports_count::init()
{
	uint32_t logdate=get_date(time(NULL));
		this->insert(logdate);
	return SUCC;

}


int Csports_count::insert(uint32_t logdate )
{
	sprintf( this->sqlstr, "insert into %s values (%u,0,0,0,0) ",
	this->get_table_name(), logdate );
	STD_SET_RETURN_EX(this->sqlstr, KEY_EXISTED_ERR );	
}

int Csports_count::add_count_db(  uint32_t  groupid)
{
	sprintf( this->sqlstr, "update %s set groupcount%u=groupcount%u+1 \
			where logdate=%u ",
	this->get_table_name(), groupid,groupid,get_date(time(NULL)));
	STD_SET_RETURN_EX(this->sqlstr, KEY_NOFIND_ERR );	
}

int Csports_count::add_count(  uint32_t  groupid)
{
	int ret;
	ret=this->add_count_db(  groupid);
	if (ret!=SUCC){
		this->init();
		ret=SUCC;
	}
	return ret;	
}


int Csports_count::get_count_list( uint32_t * p_count, pet_group_item **pp_item  )
{
	sprintf( this->sqlstr, "select logdate,groupcount1,groupcount2,groupcount3,\
			groupcount4  from %s ", 
			this->get_table_name() );
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_item, p_count ) ;
		INT_CPY_NEXT_FIELD ((*pp_item+i)->logdate); 
		INT_CPY_NEXT_FIELD ((*pp_item+i)->pet_group_count_1); 
		INT_CPY_NEXT_FIELD ((*pp_item+i)->pet_group_count_2); 
		INT_CPY_NEXT_FIELD ((*pp_item+i)->pet_group_count_3); 
		INT_CPY_NEXT_FIELD ((*pp_item+i)->pet_group_count_4); 
	STD_QUERY_WHILE_END();
}


int Csports_count::cache_allcount( uint32_t date )
{
	if (this->cache_list.date!=date){
				
		sprintf( this->sqlstr, "select \
				sum(groupcount1),\
				sum(groupcount2),\
				sum(groupcount3),\
				sum(groupcount4)\
				from %s where logdate<%u ", 
				this->get_table_name(),date );
		STD_QUERY_ONE_BEGIN(this-> sqlstr, SUCC);
			 this->cache_list.date=date;
			 INT_CPY_NEXT_FIELD(this->cache_list.group_allcount[0] );
			 INT_CPY_NEXT_FIELD(this->cache_list.group_allcount[1] );
			 INT_CPY_NEXT_FIELD(this->cache_list.group_allcount[2] );
			 INT_CPY_NEXT_FIELD(this->cache_list.group_allcount[3] );
		STD_QUERY_ONE_END();
	}
	return SUCC;
}

int Csports_count::get_count( uint32_t groupid,uint32_t  date, uint32_t * p_count )
{
	*p_count=0;
	sprintf( this->sqlstr, "select \
			groupcount%u \
			from %s where logdate=%u ", 
			groupid+1,this->get_table_name(),date );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Csports_count::get_all_count( uint32_t groupid, uint32_t * p_count )
{
	uint32_t	logdate=get_date(time(NULL));
	uint32_t  today_count;
	//得到昨天之前的数值
	this->cache_allcount(logdate);

	this->get_count(groupid ,logdate, &today_count );

	*p_count=this->cache_list.group_allcount[groupid]+today_count;
	return SUCC;
}

