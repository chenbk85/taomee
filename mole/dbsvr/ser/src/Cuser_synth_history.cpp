/*
 * =====================================================================================
 *
 *       Filename:  Cuser_synth_history.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/04/2011 09:42:28 AM
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
#include "msglog.h"
#include "common.h"
#include "Cuser_synth_history.h"

Cuser_synth_history::Cuser_synth_history(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_synth_history", "userid")
{

}

int Cuser_synth_history::insert(userid_t userid, uint32_t id, uint32_t type, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			id,
			type,
			count
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_synth_history::update(userid_t userid, uint32_t id, uint32_t type ,uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = count + %u where userid = %u and id = %u and type = %u",
			this->get_table_name(userid),
			count,
			userid,
			id,
			type
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
