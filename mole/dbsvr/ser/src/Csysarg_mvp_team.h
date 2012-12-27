/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_mvp_team.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/21/2012 02:53:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_MVP_TEAM_INCL
#define CSYSARG_MVP_TEAM_INCL

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_mvp_team:Ctable
{
	public:
		Csysarg_mvp_team(mysql_interface *db);
		int insert(uint32_t mvp_team, char *nick, uint32_t logo, uint32_t teamid);
		int update(uint32_t mvp_team, uint32_t badge);
		int drop(uint32_t mvp_team);
		int get_count(uint32_t *count);
		int get_page(uint32_t index, sysarg_get_mvp_team_out_item **pp_list, uint32_t *p_count);
		int change_member_count(uint32_t mvp_team, int32_t cnt);
		int get_max_not_full(uint32_t sport_team, uint32_t *team_id);
		int get_one(uint32_t teamid,const char* col,uint32_t* data);
		int select_mvp_team(uint32_t mvp_team, user_get_sysarg_mvp_teaminfo_out* out);
		int change_member_counts(uint32_t mvp_team, int32_t cnt);
};
#endif

