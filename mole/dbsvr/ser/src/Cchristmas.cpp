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
#include "Cchristmas.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//#define INIT_SHM_FLAG  -100 
//create
Cchristmas::Cchristmas (mysql_interface * db ) :CtableRoute10( db, "CHRISTMAS_DB","t_christmas","userid" )
{ 

}

int Cchristmas::add(userid_t userid, christmas_msg* p_in  )
{

	char msg_mysql[mysql_str_len(sizeof(p_in->msg) )];
	set_mysql_string(msg_mysql,(char*)(p_in->msg), sizeof(p_in->msg)); 

	sprintf(this->sqlstr, "insert into %s values (%u,'%s', 0,0)", 
			this->get_table_name(userid), userid, msg_mysql   );
	STD_INSERT_RETURN(this->sqlstr, CHRISTMAS_IS_ADD_ERR );	
}

int Cchristmas::is_get_msg(userid_t userid, stru_is_seted *p_is_existed ) 
{
	sprintf( this->sqlstr, "select  flag1 from %s where userid=%d", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,CHRISTMAS_ISNOT_ADD_ERR );
		INT_CPY_NEXT_FIELD(p_is_existed->is_seted);
	STD_QUERY_ONE_END();
}

int Cchristmas::get_msg(userid_t userid,christmas_get_msg_out  * p_out )
{
	int ret;
	stru_is_seted is_existed;
	ret=this->is_get_msg(userid,& is_existed  );
	if (ret!=SUCC) return ret;
	if ( is_existed.is_seted== 1 ) 
		return  CHRISTMAS_IS_GETED_ERR;

	ret=this->get_msg_db(userid , p_out );
	if (ret!=SUCC) return ret;

	this->set_isget_msg(userid );
	this->set_msg_lost(p_out->send_id);
	return SUCC;
}

int Cchristmas::set_isget_msg(userid_t userid) 
{

	sprintf( this->sqlstr, " update %s set \
			flag1=1 \
			where userid=%u " ,
			this->get_table_name(userid),  userid); 
	STD_SET_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}

int Cchristmas::set_msg_lost(userid_t userid) 
{
	sprintf( this->sqlstr, " update %s set \
			flag2=1 \
			where userid=%u " ,
			this->get_table_name(userid),  userid); 
	STD_SET_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}


int Cchristmas::get_msg_db(userid_t userid, christmas_get_msg_out * p_out  )
{
	sprintf( this->sqlstr, "select  userid, msg from %s where flag2=0  and userid<>%u limit 0,1", 
			this->get_table_name(userid),userid );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->send_id);
		BIN_CPY_NEXT_FIELD(p_out->msg, sizeof(msg));
	STD_QUERY_ONE_END();
}
