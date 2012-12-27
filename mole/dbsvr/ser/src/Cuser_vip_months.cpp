/*
 * =====================================================================================
 *
 *       Filename:  Cuser_vip_months.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/19/2011 05:09:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_vip_months.h"
#include "common.h"
#include "proto.h"

Cuser_vip_months::Cuser_vip_months(mysql_interface *db):
    CtableRoute100x10(db, "USER", "t_user_vip_months_award","userid")
{

}

    
int Cuser_vip_months::insert(userid_t userid, uint32_t type, uint32_t count, uint32_t flag)
{       
    sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u)", 
            this->get_table_name(userid),
            userid,
            type,
            count,
            flag
            );

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_vip_months::update(userid_t userid, uint32_t type, const char* col, uint32_t value)
{
    sprintf(this->sqlstr, "update %s set %s = %u where userid = %u and type = %u",
            this->get_table_name(userid),
            col,
            value,
            userid,
            type
            );
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_vip_months::update_inc(userid_t userid, uint32_t type, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u where userid = %u and type = %u",
			this->get_table_name(userid),
			col,
			col,
			value,
			userid,
			type
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_vip_months::update_all(userid_t userid, uint32_t type, uint32_t count, uint32_t receive)
{
    sprintf(this->sqlstr, "update %s set months = %u, receive = %u where userid = %u and type = %u",
            this->get_table_name(userid),
            count,
            receive,
            userid,
            type
            );

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_vip_months::query(userid_t userid, uint32_t type, const char* col, uint32_t *value)
{
    sprintf(this->sqlstr, "select %s from %s where userid = %u and type = %u",
            col,
            this->get_table_name(userid),
            userid,
            type
            );

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*value);
    STD_QUERY_ONE_END();
}

int Cuser_vip_months::add(userid_t userid, uint32_t type, uint32_t months)
{
    int ret = update_inc(userid, type, "months", months);
    if(ret != SUCC){
        ret = insert(userid, type, months, 0);
    }
    return ret;
}

int Cuser_vip_months::select(userid_t userid, uint32_t type, uint32_t *months, uint32_t *receive)
{
    sprintf(this->sqlstr, "select months, receive from %s where userid = %u and type = %u",
            this->get_table_name(userid),
            userid,
            type
            );

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*months);
        INT_CPY_NEXT_FIELD(*receive);
    STD_QUERY_ONE_END();
}
