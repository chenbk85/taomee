/*
 * =====================================================================================
 *
 *       Filename:  Cdwg_user_friend_pet.cpp
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

#include "Cdwg_user_friend.h"
//user_pet
Cdwg_user_friend::Cdwg_user_friend(mysql_interface * db ) 
	:CtableRoute100x100_ex ( db,"dawg_user" ,"user_friend","uid")
{ 

}

int  Cdwg_user_friend:: get_list(userid_t userid, 
		uint32_t *p_count, stru_id ** pp_list   )	
{
	sprintf( this->sqlstr, "select f_uid from %s where uid=%u ", 
		this->get_table_name(userid),userid );
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id);
	STD_QUERY_WHILE_END();
}
