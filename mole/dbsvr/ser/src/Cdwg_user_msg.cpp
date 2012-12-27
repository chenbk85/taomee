/*
 * =====================================================================================
 *
 *       Filename:  Cdwg_user_msg_pet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 13时46分59秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Cdwg_user_msg.h"
//user_pet
Cdwg_user_msg::Cdwg_user_msg(mysql_interface * db ) 
	:CtableRoute100x100_ex ( db,"dawg_user" ,"user_msg","uid")
{ 
}
int Cdwg_user_msg::insert(userid_t userid ,userid_t sendid, char *  msg,
				uint32_t sendtime  ,uint32_t flag    )
{

	int32_t count;
	db->exec_update_sql("set names utf8",&count);

	char msg_mysql[mysql_str_len(255)];
	set_mysql_string(msg_mysql,msg, strlen(msg) );

	
	sprintf( this->sqlstr, "insert into %s values (0,%u,%u,'%s',%u,%u,0)", 
		this->get_table_name(userid), userid, sendid ,msg_mysql,sendtime,flag );
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}


