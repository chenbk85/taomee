/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_props_info.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/20/2011 10:02:50 AM
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
#include "Cuser_battle_props_info.h"


Cuser_battle_props_info::Cuser_battle_props_info(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_battle_props_info","userid")
{

}

int Cuser_battle_props_info::insert(userid_t userid, uint32_t type, uint32_t flag, uint32_t round)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			type,
			flag,
			round
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_battle_props_info::update(userid_t userid, uint32_t type, uint32_t flag, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = %u where userid = %u and type = %u and flag = %u",
			this->get_table_name(userid),
			count,
			userid,
			type,
			flag
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_props_info::update_inc(userid_t userid, uint32_t type, uint32_t flag, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = count + %u where userid = %u and type = %u and flag = %u",
			this->get_table_name(userid),
			count,
			userid,
			type,
			flag
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_props_info::get_all(userid_t userid, props_info_t **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select flag, count from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->flag);
		INT_CPY_NEXT_FIELD((*pp_list + i )->count);
	STD_QUERY_WHILE_END();	
}

int Cuser_battle_props_info::get_flag(userid_t userid, props_gain_t **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select flag, count from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	props_gain_t* p_temp = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, &p_temp, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->flag);
	STD_QUERY_WHILE_END_WITHOUT_RETURN();	
	
	if(p_temp != 0){
		free(p_temp);
	}
	return SUCC;
}

int Cuser_battle_props_info::select(userid_t userid, uint32_t type, uint32_t flag, uint32_t *count)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u and type = %u and flag = %u",
			this->get_table_name(userid),
			userid,
			type,
			flag
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
	STD_QUERY_ONE_END();
}

int Cuser_battle_props_info::sub_count(userid_t userid, uint32_t flag, uint32_t count)
{
	//道具表里目前仅有增益道具，type默认值2
	if(count == 0){
		return SUCC;
	}
	uint32_t old_count = 0;
	int ret = select(userid, 2, flag, &old_count);
	if(ret != SUCC || old_count < count){
		 return ATTIRE_COUNT_NO_ENOUGH_ERR;
	}
	if(old_count > count){
		sprintf(this->sqlstr, "update %s set count = %u where userid = %u and type = 2 and flag = %u",
				this->get_table_name(userid),
				old_count - count,
				userid,
				flag
				);
	}
	else{
		sprintf(this->sqlstr, "delete from %s where userid = %u and type = 2 and flag = %u",
			  this->get_table_name(userid),
			  userid,
			  flag
			  );
	}

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	
}
