/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_db_sports_gamescore.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/24/2010 10:04:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef  CSYSARG_DB_SPORTS_GAMESCORE_INCL
#define  CSYSARG_DB_SPORTS_GAMESCORE_INCL
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

class Csysarg_db_sports_gamescore : Ctable
{
protected:
	
public:
	Csysarg_db_sports_gamescore(mysql_interface * db);

	int min_score_select(uint32_t gameid, uint32_t* score);	

	int min_score_delete(uint32_t gameid, uint32_t min_score);

	int score_update(uint32_t userid, uint32_t gameid, uint32_t score);

	int score_insert(uint32_t userid, uint32_t gameid, uint32_t score, char* p_user_nick);

	int max_score_select(uint32_t gameid, uint32_t* p_count, sysarg_db_sports_gamescore_get_out_item** pp_list);	
	int top9_score_select(uint32_t userid,	uint32_t *p_score);
	
};


#endif

