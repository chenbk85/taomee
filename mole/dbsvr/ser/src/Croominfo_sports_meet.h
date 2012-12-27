/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_sports_meet.h
 *
 *    Description:  deal in table roominfo_0.t_sports_meet_0
 *
 *        Version:  1.0
 *        Created:  05/10/2010 07:08:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

       
#ifndef  CROOMINFO_SPORTS_MEET_INCL
#define  CROOMINFO_SPORTS_MEET_INCL
#include <algorithm>
#include <cstring>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "common.h"
#include "CtableRoute10x10.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"

class Croominfo_sports_meet : CtableRoute10x10 
{
	protected:
		
	public:
		Croominfo_sports_meet(mysql_interface * db);

		int teamid_set(uint32_t userid, uint32_t teamid);
		int teamid_get(uint32_t userid, uint32_t* teamid, uint32_t* p_medal);
		
		int medal_update(uint32_t userid, uint32_t medal_count);
		int top_medal_get(uint32_t userid, uint32_t* p_medal, uint32_t* p_time_limit);

		int reward_flag_select(uint32_t userid, roominfo_sports_reward_get_out *p_out);

		int reward_flag_update(uint32_t userid);

		int reward_flag_insert(uint32_t userid);
};


#endif

