/*
 * =====================================================================================
 *
 *       Filename:  Cpet_sports.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年02月16日 13时40分52秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Cpet_sports.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"


Cpet_sports::Cpet_sports(mysql_interface * db ) :
	CtableRoute100( db, "PET_SPORTS_DB","t_pet_sports","userid" )
{ 

}


int Cpet_sports::init(userid_t userid,uint32_t petid, uint32_t groupid)
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u, 0,0,0,0,0)",
	this->get_table_name(userid),userid, petid,groupid );
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}


int  Cpet_sports:: get_petlist(userid_t userid,
		uint32_t *count, temp_pet_get_pet_list_out_item** list   )	
{
	sprintf( this->sqlstr, 
		"select  petid from %s \
			where userid=%u ",
		this->get_table_name(userid),userid ); 

	STD_QUERY_WHILE_BEGIN( this->sqlstr,list, count ) ;
		(*list+i)->petid=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

int  Cpet_sports::get_score(userid_t userid, uint32_t petid,  uint32_t type,
		uint32_t *p_groupid, uint32_t* p_score )	
{
	sprintf( this->sqlstr, "select  groupid ,score%d from %s where userid=%u  and petid=%u", 
			type,this->get_table_name(userid),userid,petid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_PET_ID_NOFIND_ERR );
		 INT_CPY_NEXT_FIELD(*p_groupid);
		 INT_CPY_NEXT_FIELD(*p_score);
	STD_QUERY_ONE_END();
}
int  Cpet_sports::set_score(userid_t userid, uint32_t petid,  uint32_t type,
	uint32_t	score )	
{
	sprintf( this->sqlstr, "update %s set score%d=%d where userid=%u  and petid=%u", 
			this->get_table_name(userid),type,score, userid,petid);
	
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}



int  Cpet_sports::get_scorelist(userid_t userid,
		uint32_t *count, temp_pet_get_score_list_out_item  **pp_list   )	
{
	sprintf( this->sqlstr, 
		"select  petid,score1,score2,score3,score4,score5 from %s \
			where userid=%u ",
		this->get_table_name(userid),userid ); 

	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, count ) ;
		INT_CPY_NEXT_FIELD((*pp_list+i)->petid); 
		INT_CPY_NEXT_FIELD((*pp_list+i)->pet_score_1); 
		INT_CPY_NEXT_FIELD((*pp_list+i)->pet_score_2); 
		INT_CPY_NEXT_FIELD((*pp_list+i)->pet_score_3); 
		INT_CPY_NEXT_FIELD((*pp_list+i)->pet_score_4); 
		INT_CPY_NEXT_FIELD((*pp_list+i)->pet_score_5); 
	STD_QUERY_WHILE_END();
}

int  Cpet_sports::get_petcount(userid_t userid, uint32_t* p_count )
{
	sprintf( this->sqlstr, "select count(1) from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, SUCC);
		 INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();

}


