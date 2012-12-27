/*
 * =====================================================================================
 *
 *       Filename:  Cuser_cow_pasture.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/2011 03:47:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <ctime> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "msglog.h"
#include "common.h"
#include <map>
#include "Cuser_cow_pasture.h"


Cuser_cow_pasture:: Cuser_cow_pasture(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_milk_cow_pasture", "userid")
{

}

int Cuser_cow_pasture::insert(userid_t userid)
{
	uint32_t default_bg = 1351065;
	stru_grass_info grass_info;
	memset(&grass_info, 0, sizeof(stru_grass_info));
	grass_info.count = 2;
	for(uint32_t k = 0; k < 2; ++k){
		grass_info.grass[k].grassid = 1351085;
		grass_info.grass[k].index = k + 1;
	}
	char grass_mysql[mysql_str_len(sizeof(stru_grass_info))];
	memset(grass_mysql, 0, sizeof(grass_mysql));
	set_mysql_string(grass_mysql, (char*)(&grass_info), sizeof(stru_grass_info));
	sprintf(this->sqlstr, "insert into %s values(%u, %u, 0, 0, '%s', 0)",
			this->get_table_name(userid),
			userid,
			default_bg,
			grass_mysql
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_cow_pasture::update_col(userid_t userid, const char *col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u",
			this->get_table_name(userid),
			col,
			value,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_cow_pasture::update_two_col(userid_t userid, const char *col_1, const char * col_2, 
		uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u where userid = %u",
			this->get_table_name(userid),
			col_1,
			value_1,
			col_2,
			value_2,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_cow_pasture::get_col(userid_t userid, const char *col, uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u",
			col,
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value);
	STD_QUERY_ONE_END();
}
int Cuser_cow_pasture::get_two_col(userid_t userid, const char *col_1, const char *col_2,
		uint32_t *value_1, uint32_t *value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value_1);
		INT_CPY_NEXT_FIELD (*value_2);
	STD_QUERY_ONE_END();
}

int Cuser_cow_pasture::get_all(userid_t userid, uint32_t *mk_storage, uint32_t *background,
	   	uint32_t *day_count, stru_grass_info *grass_info)
{
	sprintf(this->sqlstr, "select background, milk_storage, day_count, grass_list \
			from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*background);
		INT_CPY_NEXT_FIELD (*mk_storage);
		INT_CPY_NEXT_FIELD (*day_count);
        BIN_CPY_NEXT_FIELD ((char*)grass_info, sizeof(*grass_info) );
	STD_QUERY_ONE_END();
}

int Cuser_cow_pasture::update_grass_info(userid_t userid, stru_grass_info *grass_info)
{
	char grass_mysql[mysql_str_len(sizeof(stru_grass_info))];
	memset(grass_mysql, 0, sizeof(grass_mysql));
	set_mysql_string(grass_mysql, (char*)grass_info, sizeof(stru_grass_info));
	sprintf(this->sqlstr, "update %s set grass_list = '%s' where userid = %u",
			this->get_table_name(userid),
			grass_mysql,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_cow_pasture::get_grass_info(userid_t userid, stru_grass_info *grass_info)
{
	sprintf(this->sqlstr, "select grass_list from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD ((char*)grass_info, sizeof(*grass_info) );
	STD_QUERY_ONE_END();
}

