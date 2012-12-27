/*
 * =====================================================================================
 *
 *       Filename:  Cuser_profile_dress.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/01/2011 11:17:39 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_profile_dress.h"

Cuser_profile_dress::Cuser_profile_dress(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_profile_dress", "userid")
{

}

int Cuser_profile_dress::insert(userid_t userid, profile_dress_t *profile)
{
	char profile_mysql[mysql_str_len(sizeof(profile_dress_t))];
	memset(profile_mysql, 0, sizeof(profile_mysql));
	set_mysql_string(profile_mysql, (char*)profile, sizeof(profile_dress_t));

	sprintf(this->sqlstr, "insert into %s values( NULL, %u, '%s')",
			this->get_table_name(userid),
			userid,
			profile_mysql
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_profile_dress::get_profiles(userid_t userid, profiles_t **profiles, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select ind, profile from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN( this->sqlstr,profiles, p_count ) ;
		INT_CPY_NEXT_FIELD ((*profiles + i)->index);
        BIN_CPY_NEXT_FIELD ((char*)(&(*profiles+i)->dress), sizeof(profile_dress_t) );
	STD_QUERY_WHILE_END();
}
int Cuser_profile_dress::get_one_profile(userid_t userid, uint32_t index, profile_dress_t *profile)
{
	sprintf(this->sqlstr, "select profile from %s where userid = %u and ind = %u",
			this->get_table_name(userid),
			userid,
			index
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD ((char*)(profile), sizeof(*profile) );
	STD_QUERY_ONE_END();
}

int Cuser_profile_dress::get_count(userid_t userid, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
	STD_QUERY_ONE_END();
}
int Cuser_profile_dress::update_profile(userid_t userid, uint32_t index, profile_dress_t *profile)
{
	char profile_mysql[mysql_str_len(sizeof(profile_dress_t))];
	memset(profile_mysql, 0, sizeof(profile_mysql));
	set_mysql_string(profile_mysql, (char*)profile, sizeof(profile_dress_t));
	sprintf(this->sqlstr, "update %s set profile = '%s' where userid = %u and ind = %u",
			this->get_table_name(userid),
			profile_mysql,
			userid,
			index
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_profile_dress::remove(userid_t userid, uint32_t index)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and ind = %u",
			this->get_table_name(userid),
			userid,
			index
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
