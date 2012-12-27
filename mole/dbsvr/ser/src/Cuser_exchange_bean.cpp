/*
 * =====================================================================================
 *
 *       Filename:  Cuser_exhange_bean.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/12/2011 09:24:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "common.h"
#include "Cuser_exchange_bean.h"

Cuser_exchange_bean::Cuser_exchange_bean(mysql_interface *db):
	CtableRoute100x10(db,"USER","t_user_exchange_bean","userid")
{

}
int Cuser_exchange_bean::insert(userid_t userid, uint32_t count)
{
	sprintf(this->sqlstr,"insert into %s values(%u, %u )",
			this->get_table_name(userid),
			userid,
			count
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_exchange_bean::update(userid_t userid, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = count + %u where userid = %u",
			this->get_table_name(userid),
			count,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_exchange_bean::select(userid_t userid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (count);
	STD_QUERY_ONE_END();
}
