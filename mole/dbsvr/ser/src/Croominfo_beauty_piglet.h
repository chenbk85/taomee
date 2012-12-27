/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_beauty_piglet.h
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  12/06/2011 02:08:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CROOMINFO_BEAUTY_PIGLET_INCL
#define CROOMINFO_BEAUTY_PIGLET_INCL



#include "CtableRoute10x10.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"


class Croominfo_beauty_piglet:public CtableRoute10x10
{
	public:
		Croominfo_beauty_piglet(mysql_interface *db);
		int insert(userid_t userid, uint32_t  award_flag, int32_t continuation);
		int update(userid_t userid, const char* col, int32_t val);
		int get(userid_t userid, uint32_t *award_flag, int32_t *val);
		int update_all(userid_t userid, uint32_t award_flag, int continuation);
		int set_attributes(userid_t userid, int count);
		int set_award(uint32_t userid, uint32_t type, uint32_t *state);
		int get_award_history(userid_t userid, uint32_t *history);
		int get_award_history_whole(uint32_t userid, uint32_t *history, int32_t* continuation);
};

#endif
