/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_db_sports_tmpuser.h
 *
 *    Description:  sports meet 20100520
 *
 *        Version:  1.0
 *        Created:  05/17/2010 09:54:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef  CSYSARG_DB_SPORTS_TMPUSER_INCL
#define  CSYSARG_DB_SPORTS_TMPUSER_INCL
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

class Csysarg_db_sports_tmpuser : Ctable
{
protected:
	
public:
	Csysarg_db_sports_tmpuser(mysql_interface * db);

		
int tmpuser_create();	
	
int userid_select(uint32_t userid, uint32_t *flag);
	
int score_insert(uint32_t userid, uint32_t score);

int score_select(uint32_t userid, uint32_t *score);

int score_update(uint32_t userid, uint32_t score);

int userid_insert(uint32_t userid, uint32_t flag);

int userid_update(uint32_t userid, uint32_t flag);

int tmpuser_del();
};


#endif

