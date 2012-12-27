/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_vote_decorate.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/13/2012 06:11:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_VOTE_DECORATE_INCL
#define CSYSARG_VOTE_DECORATE_INCL

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_vote_decorate:Ctable
{
	public:
		Csysarg_vote_decorate(mysql_interface *db);
		int add(userid_t userid, char* nick, uint32_t vote);
		int get(userid_t userid, uint32_t index, sysarg_get_birthday_decorate_out_item** pp_list, uint32_t* p_count);
		int clear();
		int get_count(uint32_t *count);
	private:
		int insert(uint32_t userid, char* nick, uint32_t vote);
		int update(uint32_t userid, char* nick, uint32_t vote);
		int update_all(uint32_t userid, char* nick, uint32_t vote);
};

#endif

