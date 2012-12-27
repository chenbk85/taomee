/*
 * =====================================================================================
 *
 *       Filename:  Cuser_treasure_show.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/09/2011 11:05:04 AM
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
#include "Cuser_treasure_show.h"


Cuser_treasure_show::Cuser_treasure_show(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_treasure_show", "userid")
{

}

int Cuser_treasure_show::insert(userid_t userid, uint32_t id, uint32_t pos, uint32_t status)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			id,
			pos,
			status
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_treasure_show::del (userid_t userid, uint32_t pos)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and pos = %u",
			this->get_table_name(userid),
			userid,
			pos
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_treasure_show::update(userid_t userid, uint32_t id, uint32_t pos, uint32_t status)
{
	sprintf(this->sqlstr, "update %s set treasure_id = %u, status = %u where userid = %u and pos = %u",
			this->get_table_name(userid),
			id,
			status,
			userid,
			pos
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_treasure_show::get_all_show(userid_t userid, user_get_treasure_collection_out_item **pp_list, 
		uint32_t *p_count)
{
	sprintf(this->sqlstr, "select treasure_id, pos, status from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->id);
		INT_CPY_NEXT_FIELD((*pp_list + i )->pos);
		INT_CPY_NEXT_FIELD((*pp_list + i )->status);
	STD_QUERY_WHILE_END();	
}

int Cuser_treasure_show::get_pos_show(userid_t userid, uint32_t pos, uint32_t *itemid, uint32_t *status)
{
	sprintf(this->sqlstr, "select treasure_id, status from %s where userid = %u and pos = %u",
			this->get_table_name(userid),
			userid,
			pos
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*itemid);
		INT_CPY_NEXT_FIELD (*status);
	STD_QUERY_ONE_END();
}

int Cuser_treasure_show::update_show_status(userid_t userid, uint32_t pos, uint32_t status)
{
	sprintf(this->sqlstr, "update %s set status = %u where userid = %u and pos = %u",
			this->get_table_name(userid),
			status,
			userid,
			pos
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_treasure_show::get_show_count(userid_t userid, uint32_t itemid, uint32_t &show_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and treasure_id = %u",
			this->get_table_name(userid),
			userid,
			itemid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (show_count);
	STD_QUERY_ONE_END();
}
