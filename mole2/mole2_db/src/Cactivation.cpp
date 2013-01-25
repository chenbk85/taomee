/*
 * =====================================================================================
 *
 *       Filename:  Cactivation.cpp
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

#include "Cactivation.h"


#define STR_STRCODE	"strcode"
#define STR_UID		"userid"

#define STR_COUNT	"count(*)"



Cactivation::Cactivation(mysql_interface * db)
	:Ctable(db , "RAND_ITEM" , "t_activation")
{

}

int Cactivation::update(char* strcode, userid_t userid)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u where %s='%s'",
			this->get_table_name(),
			STR_UID,
			userid, 
			STR_STRCODE,
			strcode
	);
	return this->exec_update_sql(this->sqlstr, MOLE2_ACTIVATION_ID_NOFIND_ERR);
}

int Cactivation::strcode_get(char* sz_strcode, uint32_t* p_userid)
{
	sprintf(this->sqlstr, "select %s from %s where %s = '%s'",
			STR_UID,
			this->get_table_name(),
			STR_STRCODE,	sz_strcode);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_ACTIVATION_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_userid);
	STD_QUERY_ONE_END();
}

int Cactivation::uid_count_get(uint32_t userid, uint32_t* p_count)
{
	sprintf(this->sqlstr, "select %s from %s where %s = %u",
			STR_COUNT,
			this->get_table_name(),
			STR_UID,	userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_ACTIVATION_UID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}
