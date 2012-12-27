/*
 * =====================================================================================
 *
 *       Filename:  Cwriting_day_count.cpp
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
#include "Cwriting_day_count.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
Cwriting_day_count::Cwriting_day_count(mysql_interface * db ) :
	CtableRoute100( db,  "USERMSG_DB","t_writing_day_count","id" )
{ 
	this->writing_max_a_day=config_get_intval("WRITING_MAX_A_DAY",10 );
}

int Cwriting_day_count::update(uint32_t gameid, userid_t userid,uint32_t day,uint32_t count) 
{
	sprintf(this->sqlstr, "update %s set day=%u, count=%u  where userid = %u ",
			this->get_table_name(gameid),
			day,count,
			userid
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}
int Cwriting_day_count::insert(uint32_t gameid, userid_t userid,uint32_t day,uint32_t count) 
{
	sprintf(this->sqlstr, "insert into %s values (%u,%u,%u )", 
			this->get_table_name(gameid),userid, day , count );
	STD_INSERT_RETURN(this->sqlstr, NO_DEFINE_ERR);	
}

int Cwriting_day_count::add_count(uint32_t gameid, userid_t userid) 
{
	uint32_t day=0,count=0;	 		
	int ret;
	bool insert_flag=false;
	ret=this->get(gameid,userid,&day,&count );
	if (ret!=SUCC){
		insert_flag=true;
	}
	uint32_t now=get_date(time(NULL));
	if (now!=day ){
		count=0;	
		day=now;
	}
	count++;

	if(count>this->writing_max_a_day){
		return  VALUE_OUT_OF_RANGE_ERR;
	}
	if 	 (insert_flag){
		return this->insert(gameid,userid,day,count );
	}else{
		return this->update(gameid,userid,day,count );
	}


}

int Cwriting_day_count::get(uint32_t gameid, userid_t userid, uint32_t *p_day, uint32_t * p_count) 
{
	sprintf (this->sqlstr, "select day,count from %s where userid = %u",
			this->get_table_name(gameid),
			userid);
	 STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_day);
		INT_CPY_NEXT_FIELD(*p_count);
	 STD_QUERY_ONE_END();
}

