/*
 * =====================================================================================
 *
 *       Filename:  Cuser_shake_dice.cpp
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

#include "Cuser_shake_dice.h"
#include "common.h"
#include "proto.h"

Cuser_shake_dice::Cuser_shake_dice(mysql_interface *db):
    CtableRoute100x10(db, "USER", "t_user_shake_dice","userid")
{

}

    
int Cuser_shake_dice::insert(userid_t userid, uint32_t last_time, uint32_t six_flag)
{       
	uint32_t today = get_date(time(0));
    sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u)", 
            this->get_table_name(userid),
            userid,
            last_time,
            six_flag,
			today
            );

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_shake_dice::update(userid_t userid, const char* col, uint32_t value)
{
    sprintf(this->sqlstr, "update %s set %s = %u where userid = %u ",
            this->get_table_name(userid),
            col,
            value,
            userid
            );
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_shake_dice::query_time_flag(userid_t userid, uint32_t *last_time, uint32_t *six_flag, uint32_t *last_date)
{
    sprintf(this->sqlstr, "select last_time, six_flag, last_date  from %s where userid = %u ",
            this->get_table_name(userid),
            userid
            );

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*last_time);
        INT_CPY_NEXT_FIELD(*six_flag);
        INT_CPY_NEXT_FIELD(*last_date);
    STD_QUERY_ONE_END();
}


