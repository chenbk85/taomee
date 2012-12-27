/*
 * =====================================================================================
 *
 *       Filename:  Cuser_treasure_collection.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/09/2011 11:23:41 AM
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
#include "Cuser_treasure_collection.h"

uint32_t gid_count_list[] = {
	2, 2, 2, 4, 4, 4, 4, 8, 8, 8, 8, 12, 12, 12, 12, 18, 18, 18, 18, 24
};

Cuser_treasure_collection::Cuser_treasure_collection(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_treasure_collection", "userid")
{

}
int Cuser_treasure_collection::get_grid_count(uint32_t level, uint32_t &grid_limit)
{
	grid_limit =  gid_count_list[level - 1];	
	return SUCC;
}

int Cuser_treasure_collection::insert(userid_t userid, uint32_t login_first)
{
	uint32_t background = 1453357;
	sprintf(this->sqlstr, "insert into %s values(%u, %u, 0x0000000, %u, 0)",
			this->get_table_name(userid),
			userid,
			background,
			login_first
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_treasure_collection::update_background(userid_t userid, uint32_t background)
{
	sprintf(this->sqlstr, "update %s set background = %u where userid = %u",
			this->get_table_name(userid),
			background,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_treasure_collection::update_first_mod(userid_t userid, uint32_t first_mod)
{
	sprintf(this->sqlstr, "update %s set first_mod = %u where userid = %u",
			this->get_table_name(userid),
			first_mod,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_treasure_collection::get_background(userid_t userid, uint32_t *background)
{
	sprintf(this->sqlstr, "select background from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*background);
	STD_QUERY_ONE_END();
}

int Cuser_treasure_collection::get_first_mod(userid_t userid, uint32_t *first_mod)
{
	sprintf(this->sqlstr, "select first_mod from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*first_mod);
	STD_QUERY_ONE_END();
}

int Cuser_treasure_collection::get_visitlist(userid_t userid, user_treasure_collection_get_visitlist_out *visitlist)
{
	sprintf(this->sqlstr, "select visitlist from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD ((char*)(visitlist), sizeof(*visitlist) );
	STD_QUERY_ONE_END();
}

int Cuser_treasure_collection::update_visitlist(userid_t userid,user_treasure_collection_get_visitlist_out &visitlist )
{
	char mysql_visit[mysql_str_len(sizeof(user_treasure_collection_get_visitlist_out ))];
	memset(mysql_visit, 0, sizeof(mysql_visit));
	set_mysql_string(mysql_visit, (char*)(&visitlist), sizeof(visit_trace) * visitlist.count + 4);
	sprintf(this->sqlstr, "update %s set visitlist = '%s' where userid = %u",
			this->get_table_name(userid),
			mysql_visit,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_treasure_collection::get_first_login(userid_t userid, uint32_t &first)
{
	sprintf(this->sqlstr, "select first_login from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD (first);
	STD_QUERY_ONE_END();
}
int Cuser_treasure_collection::update_first_login(userid_t userid, uint32_t first)
{
	sprintf(this->sqlstr, "update %s set first_login = %u where userid = %u",
			this->get_table_name(userid),
			first,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
