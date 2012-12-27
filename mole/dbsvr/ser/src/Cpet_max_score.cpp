/*
 * =====================================================================================
 *
 *       Filename:  Cpet_max_score.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年02月23日 13时06分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "Cpet_max_score.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//create
Cpet_max_score::Cpet_max_score(mysql_interface * db ) :Ctable( db, "PET_SPORTS_DB","t_pet_max_score" )
{ 

}


int Cpet_max_score::set_score_db( uint32_t type  , uint32_t score , 
		 uint32_t groupid, char * user_nick,  char * pet_nick )
{
	char usernick_mysql[mysql_str_len(NICK_LEN)];
	char petnick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(usernick_mysql,user_nick ,NICK_LEN );
	set_mysql_string(petnick_mysql,pet_nick ,NICK_LEN );
	sprintf( this->sqlstr, " update %s set \
					score=%u, \
					groupid=%d, \
					usernick='%s', \
					petnick='%s' \
		   			where type=%u " ,
				this->get_table_name(), score , groupid ,
			   	usernick_mysql,petnick_mysql,type );
	STD_SET_RETURN_EX(this->sqlstr,KEY_NOFIND_ERR );	
}


int Cpet_max_score::get_score( uint32_t type, temp_pet_sports_get_max_score_out * p_out )
{
	sprintf( this->sqlstr, "select score, groupid,usernick, petnick from %s where type=%u ", 
			this->get_table_name(),type);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, KEY_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->score );
		INT_CPY_NEXT_FIELD(p_out->pet_groupid);
		BIN_CPY_NEXT_FIELD(p_out->usernick,NICK_LEN );
		BIN_CPY_NEXT_FIELD(p_out->petnick,NICK_LEN );
	STD_QUERY_ONE_END();

}

int Cpet_max_score::set_score( uint32_t type  , uint32_t score , 
		uint32_t groupid, char * user_nick,  char * pet_nick )
{
	uint32_t db_score;
	int ret;
	ret=this->get_score(type, &db_score);
	if(ret!=SUCC ) return ret;
	//更新分数
	bool update_flag=false;
	if ( db_score==0 ){
		//没有值
		update_flag=true;
	}

	if ( score>db_score && type ==3 ){
		update_flag=true;
	}
	if ((type==1 || type==2) && score<db_score ){
		update_flag=true;
	}
	if(update_flag ){
		return  this->set_score_db(type,score,groupid,user_nick,pet_nick  ); 
		if (ret!=SUCC) return ret;
	}
	return  SUCC;
}

int Cpet_max_score::get_score( uint32_t type, uint32_t * p_score)
{
	sprintf( this->sqlstr, "select score from %s where type=%u ", 
			this->get_table_name(),type);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, KEY_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *p_score );
	STD_QUERY_ONE_END();
}
