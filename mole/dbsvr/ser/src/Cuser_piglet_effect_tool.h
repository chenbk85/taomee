/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_effect_tool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/17/2011 11:33:55 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PIGLET_EFFECT_TOOL_INC
#define CUSER_PIGLET_EFFECT_TOOL_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_piglet_effect_tool:public CtableRoute100x10
{
	private:
	
		int get_lasttime(uint32_t itemid);
	public:
		Cuser_piglet_effect_tool(mysql_interface *db);
		int insert(userid_t userid, uint32_t itemid, uint32_t otherid, uint32_t dur_time = 0, uint32_t flag = 0);
		int drop(userid_t userid, uint32_t itemid);
		int update(userid_t userid, uint32_t itemid, uint32_t datetime, 
				uint32_t lasttime, uint32_t otherid);
		int update_state(userid_t userid, uint32_t itemid, uint32_t state);
		int select(userid_t userid, uint32_t itemid, uint32_t *affect_time ,uint32_t *lasttime);
		int get_all(userid_t userid, user_piglet_get_effect_tools_out_item** pp_list, uint32_t *p_count);
		int add(userid_t userid, uint32_t itemid, uint32_t otherid);
		int  get_buff_tools(userid_t userid, uint32_t *p_count, buff_tool_t **pp_list);
		int update_two_time(userid_t userid, uint32_t effect_time, uint32_t lasttime, uint32_t itemid);
		int get_beauty_effect_tools(userid_t userid, beauty_piglet_effect_t **pp_list, uint32_t *p_count);
		int n_insert(userid_t userid, uint32_t itemid, uint32_t otherid, uint32_t lasttime);
};

#endif
