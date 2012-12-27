/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_effect_tool.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/17/2011 11:33:53 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_piglet_effect_tool.h"


static uint32_t buff_lasttime[][2] = {
	{1613128, 3},
	{1613129, 3},
	{1613130, 3},
};

int Cuser_piglet_effect_tool::get_lasttime(uint32_t itemid)
{
	for(uint32_t k = 0; k < sizeof(buff_lasttime)/8; ++k){
		if(buff_lasttime[k][0] == itemid){
			return buff_lasttime[k][1];
		}
	}
	return -1;
}

Cuser_piglet_effect_tool::Cuser_piglet_effect_tool(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_piglet_effect_tool", "userid")
{

}
int Cuser_piglet_effect_tool::insert(userid_t userid, uint32_t itemid, uint32_t otherid,
		uint32_t dur_time, uint32_t flag)
{
	int lasttime = 0;
	if(flag == 0){
		lasttime = get_lasttime(itemid);
		if(lasttime == -1){
			return USER_ID_EXISTED_ERR;
		}
	}	
	else{
		lasttime = dur_time;
	}
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, 0, %u)",
		this->get_table_name(userid),
		userid,
		itemid,
		(uint32_t)time(0),
		lasttime*24*3600,
		otherid
		);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_piglet_effect_tool::n_insert(userid_t userid, uint32_t itemid, uint32_t otherid,
		uint32_t lasttime)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, 0, %u)",
			this->get_table_name(userid),
			userid,
			itemid,
			(uint32_t)time(0),
			lasttime,
			otherid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_piglet_effect_tool::drop(userid_t userid, uint32_t itemid)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and itemid = %u",
			this->get_table_name(userid),
			userid,
			itemid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_effect_tool::select(userid_t userid, uint32_t itemid, uint32_t *affect_time ,uint32_t *lasttime)
{
	sprintf(this->sqlstr, "select datetime, lasttime from %s where userid = %u and itemid = %u",
			this->get_table_name(userid),
			userid,
			itemid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*affect_time);
		INT_CPY_NEXT_FIELD(*lasttime);
	STD_QUERY_ONE_END();	
}


int Cuser_piglet_effect_tool::get_all(userid_t userid, user_piglet_get_effect_tools_out_item** pp_list,
	   	uint32_t *p_count)
{
	uint32_t now = time(0);
	sprintf(this->sqlstr, "select itemid, datetime, lasttime, otherid from %s where userid = %u and \
			itemid >= 1613128 and itemid <= 1613131 and %u - datetime < lasttime",
			this->get_table_name(userid),
			userid,
			now
			);
	
	uint32_t datetime = 0, lasttime = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->itemid);
		INT_CPY_NEXT_FIELD(datetime);
		INT_CPY_NEXT_FIELD(lasttime);
		if(lasttime < (now - datetime)){
			(*pp_list + i)->lefttime = 0;
		}
		else{
			(*pp_list + i)->lefttime = lasttime - (now - datetime);

		}
		INT_CPY_NEXT_FIELD((*pp_list + i)->otherid);
	STD_QUERY_WHILE_END();
}

int  Cuser_piglet_effect_tool::get_buff_tools(userid_t userid, uint32_t *p_count, buff_tool_t **pp_list)
{
	sprintf(this->sqlstr, "select itemid, datetime, lasttime, state from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->itemid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->effect_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lasttime);
		INT_CPY_NEXT_FIELD((*pp_list + i)->state);
	STD_QUERY_WHILE_END();

}

int Cuser_piglet_effect_tool::update(userid_t userid, uint32_t itemid, uint32_t datetime,
	   	uint32_t lasttime, uint32_t otherid)
{
	sprintf(this->sqlstr, "update %s set datetime = %u, lasttime = %u, \
			state = 0, otherid = %u  where userid = %u and itemid = %u",
			this->get_table_name(userid),
			datetime,
			lasttime,
			otherid,
			userid,
			itemid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_effect_tool::update_state(userid_t userid, uint32_t itemid, uint32_t state)
{
	sprintf(this->sqlstr, "update %s set state = %u  where userid = %u and itemid = %u",
			this->get_table_name(userid),
			state,
			userid,
			itemid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_piglet_effect_tool::add(userid_t userid, uint32_t itemid, uint32_t otherid)
{
	uint32_t now = time(0);
	uint32_t lasttime = this->get_lasttime(itemid);
	DEBUG_LOG("lasttime: %u", lasttime);
	int ret = this->update(userid, itemid, now, lasttime*24*3600, otherid);
	if(ret == USER_ID_NOFIND_ERR){
		ret = this->insert(userid, itemid, otherid);
	}
	return ret;
}
int Cuser_piglet_effect_tool::update_two_time(userid_t userid, uint32_t effect_time, uint32_t lasttime,
		uint32_t itemid)
{
	sprintf(this->sqlstr, "update %s set datetime = %u, lasttime = %u where userid = %u and itemid = %u",
			this->get_table_name(userid),
			effect_time,
			lasttime,
			userid,
			itemid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_piglet_effect_tool::get_beauty_effect_tools(userid_t userid, beauty_piglet_effect_t **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select itemid, lasttime, datetime from %s where userid = %u and itemid >= 1613290",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->itemid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lasttime);
		INT_CPY_NEXT_FIELD((*pp_list + i)->start_time);
	STD_QUERY_WHILE_END();
}
