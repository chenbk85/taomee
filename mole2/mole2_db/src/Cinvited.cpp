/*
 * =====================================================================================
 *
 *       Filename:  Cpet.cpp
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
 *  common.h
 * =====================================================================================
 */

#include "Cinvited.h"

#define STR_USERID			"uid"
#define STR_INVITED_COUNT	"invite_succ_count"
#define STR_ALREADY_TIMES	"already_times"

Cinvited::Cinvited(mysql_interface * db ) 
	:Ctable(db , "event" , "hero_invite_user")	
{ 

}

int Cinvited::add_invited(userid_t userid)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %s + 1 where %s = %u",
			this->get_table_name(),
			STR_INVITED_COUNT,STR_INVITED_COUNT,
			STR_USERID,userid
	);

	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cinvited::get_invited(uint32_t userid, uint32_t *p_invited, uint32_t *p_used)
{
	sprintf(this->sqlstr, "select %s, %s from %s where %s = %u",
			STR_INVITED_COUNT,
			STR_ALREADY_TIMES,
			this->get_table_name(), 
			STR_USERID,	userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_invited);
		INT_CPY_NEXT_FIELD(*p_used);
	STD_QUERY_ONE_END();
}

