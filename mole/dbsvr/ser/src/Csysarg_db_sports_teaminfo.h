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

       
#ifndef  CSYSARG_DB_SPORTS_TEAMINFO_INCL
#define  CSYSARG_DB_SPORTS_TEAMINFO_INCL
#include <algorithm>
#include <cstring>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "common.h"
#include "CtableRoute.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"

class Csysarg_db_sports_teaminfo : Ctable
{
protected:
	
	uint32_t medal;
public:
	Csysarg_db_sports_teaminfo(mysql_interface * db);

	int teaminfo_set(sysarg_db_sports_teaminfo_set_in *in, sysarg_db_sports_teaminfo_set_out *out);
		
	int update1(uint32_t userid, uint32_t teamid, sysarg_db_sports_teaminfo_set_out *out);
	
	int update2(uint32_t userid, uint32_t teamid, sysarg_db_sports_teaminfo_set_out *out);
	
	int score_get(sysarg_db_sports_teaminfo_score_get_out *out);
	
	int medal_get(sysarg_db_sports_teaminfo_medal_get_out *out);
	
	int medal_update(uint32_t teamid, uint32_t medal);

	int win_flag_get(uint32_t* teamid);
};


#endif

