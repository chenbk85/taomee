/*
 * =====================================================================================
 *
 *       Filename:  Cuser_as_common.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/05/2011 03:56:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_as_common.h"


Cuser_as_common::Cuser_as_common(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_as_common", "userid")
{

}
int Cuser_as_common::insert(userid_t userid, uint32_t type, uint32_t data)
{
	sprintf(this->sqlstr, "insert into %s values(%u,%u,%u)",
			this->get_table_name(userid),
			userid,
			type,
			data
			);
	STD_SET_RETURN_EX(this->sqlstr,USER_ID_EXISTED_ERR);	
}

int Cuser_as_common::update(userid_t userid, uint32_t type, uint32_t data)
{
	sprintf(this->sqlstr, "update %s set data = %u where userid = %u and type = %u",
			this->get_table_name(userid),
			data,
			userid,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	
}
int Cuser_as_common::add(userid_t userid, uint32_t type, uint32_t data)
{
	int ret = update(userid, type, data);
	if(ret == USER_ID_NOFIND_ERR){
		ret = insert(userid, type, data);
	}
	return ret;
}

int Cuser_as_common::get_one(userid_t userid, uint32_t type, uint32_t *data)
{
	sprintf(this->sqlstr, "select data from %s where userid = %u and type = %u",
			this->get_table_name(userid),
			userid,
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*data=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}
