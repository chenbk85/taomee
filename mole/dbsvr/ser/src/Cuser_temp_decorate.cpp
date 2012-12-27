/*
 * =====================================================================================
 *
 *       Filename:  Cuser_temp_decorate.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/13/2012 05:29:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "Cuser_temp_decorate.h"
Cuser_temp_decorate::Cuser_temp_decorate(mysql_interface*db):
	CtableRoute100x10(db,"USER","t_user_temp_decorate","userid")
{

}
int Cuser_temp_decorate::insert(userid_t userid, char* value)
{
	char mysql_value[mysql_str_len(500)];
	memset(mysql_value, 0, sizeof(mysql_value)); 
	set_mysql_string(mysql_value, value, 500);

	sprintf(this->sqlstr,"insert into %s values(%u, '%s')",
			this->get_table_name(userid),
			userid,
			mysql_value);
    STD_SET_RETURN_EX(this->sqlstr,USER_ID_EXISTED_ERR);
}

int  Cuser_temp_decorate::update(userid_t userid, char*  value)
{

	char mysql_value[mysql_str_len(500)];
	memset(mysql_value, 0, sizeof(mysql_value)); 
	set_mysql_string(mysql_value, value, 500);

	sprintf(this->sqlstr,"update %s set value = '%s' where userid = %u",
			this->get_table_name(userid),
			mysql_value,
			userid
			);
	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);
}

int Cuser_temp_decorate::add(userid_t userid, char* value)
{ 
	int ret=update(userid,value);
	if(ret == USER_ID_NOFIND_ERR){
   	    ret = insert(userid, value);					
	}
	return ret;
}

int Cuser_temp_decorate::get_one(userid_t userid, char *value)
{
	sprintf(this->sqlstr, "select value from %s where userid = %u",
			this->get_table_name(userid),
			userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		BIN_CPY_NEXT_FIELD(value,MAX_TEXT_LEN);
	STD_QUERY_ONE_END();
}
