/*
 * =====================================================================================
 *
 *       Filename:  Cuser_together_photo.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 05:34:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_together_photo.h"

Cuser_together_photo::Cuser_together_photo(mysql_interface* db):
	CtableRoute100x10(db, "USER", "t_user_together_photo", "userid")
{

}

int Cuser_together_photo::insert(userid_t userid, uint32_t type, user_photo_t * photo)
{
	char mysql_photo[mysql_str_len(sizeof(user_photo_t))];
	memset(mysql_photo, 0, sizeof(mysql_photo));
	set_mysql_string(mysql_photo, (char*)photo, sizeof(user_photo_t));
	sprintf(this->sqlstr, "insert into %s values(%u, %u, '%s')",
			this->get_table_name(userid),
			userid,
			type,
			mysql_photo
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_together_photo::get_info(userid_t userid, uint32_t type, user_photo_t* photo)
{
	sprintf(this->sqlstr, "select profile from %s where userid = %u and type = %u",
			this->get_table_name(userid),
			userid,
			type
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD ((char*)photo, sizeof(user_photo_t) );
	STD_QUERY_ONE_END();
}
