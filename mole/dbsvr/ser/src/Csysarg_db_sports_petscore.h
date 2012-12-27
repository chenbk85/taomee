/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_db_sports_petscore.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/24/2010 02:47:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

       
#ifndef  CSYSARG_DB_SPORTS_PETSCORE_INCL
#define  CSYSARG_DB_SPORTS_PETSCORE_INCL
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

class Csysarg_db_sports_petscore : Ctable 
{
	protected:
		
	public:
		Csysarg_db_sports_petscore(mysql_interface * db);

		int score_insert(uint32_t userid, uint32_t petid, uint32_t gameid, uint32_t score, char* p_pet_nick);
		
		int score_update(uint32_t userid, uint32_t petid, uint32_t gameid, uint32_t score);
		
		
		int score_get(uint32_t userid, uint32_t gameid, sysarg_db_sports_petscore_get_out_item **pp_list,
			uint32_t* p_count);
		
};


#endif

