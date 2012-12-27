#ifndef  __GF_CGF_TEAM_DETAIL_H__
#define  __GF_CGF_TEAM_DETAIL_H__
#include "CtableRoute100.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_team_detail: CtableRoute100
{
	public:
		Cgf_team_detail(mysql_interface * db ); 
		int add_team_member(uint32_t userid, uint32_t role_tm, uint32_t team_id);
		int del_team(uint32_t team_id);
		int del_team_member(uint32_t userid, uint32_t role_tm, uint32_t team_id);
		int get_team_member_list(uint32_t team_id, uint32_t* p_count, gf_get_team_info_out_item** pp_list);
		int get_team_member_list(uint32_t team_id, uint32_t* p_count, gf_add_fight_team_member_out_item** pp_list);
		int set_team_member_level(gf_set_team_member_level_in * p_in);
        int increase_team_member_int_value(uint32_t team_id, uint32_t uid, uint32_t rtm, uint32_t value, const char *char_name);
	private:

};
#endif   /* ----- #ifndef __GF_CGF_SUMMON_H__  ----- */
