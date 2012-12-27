#ifndef  __GF_CGF_TEAM_H__
#define  __GF_CGF_TEAM_H__
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_team: Ctable
{
	public:
		Cgf_team(mysql_interface * db ); 

        int create_team(userid_t userid, uint32_t usertm, const char* team_name, const char* cp_name, uint32_t* p_team_id);

		int del_team(userid_t userid, uint32_t usertm, uint32_t teamid);
		
		int increase_team_member_count(uint32_t teamid);

		int reduce_team_member_count(uint32_t teamid);

		int get_team_info(uint32_t teamid, gf_get_team_info_out_header* p_out);

		int increase_score(uint32_t teamid, uint32_t score);

		int reduce_score(uint32_t teamid, uint32_t score);

        int increase_active_score(uint32_t teamid, uint32_t flag, uint32_t score);
        int reduce_active_score(uint32_t teamid, uint32_t flag, uint32_t score);

		int get_team_top10(uint32_t* p_count, gf_get_team_top10_out_item** pp_list);

		int get_team_top100(uint32_t* p_count, gf_get_team_top100_out_item** pp_list);
		
        int get_team_active_top100(uint32_t flag, uint32_t* p_count, gf_get_team_active_top100_out_item** pp_list);

        int get_team_int_value(uint32_t teamid, const char* column_type, uint32_t* p_count);

        int set_team_int_value(uint32_t teamid, const char* column_type, uint32_t value);

        int increase_team_int_value(uint32_t teamid, const char* column_type, uint32_t value, uint32_t* p_left);

        int reduce_team_int_value(uint32_t teamid, uint32_t errid, const char* column_type, uint32_t value,uint32_t* p_left);

		int change_team_name(gf_change_team_name_in * p_in);

		int change_team_mcast(gf_change_team_mcast_in * p_in);

		int search_team_info(uint32_t team_id, uint32_t captain_id, uint32_t * p_count, gf_search_team_info_out_item ** pp_list);

		int get_team_last_tax_info(uint32_t team_id, uint32_t * last_time, uint32_t * team_coin);

		int reduce_team_tax_coin(uint32_t team_id, uint32_t left_coin, uint32_t last_tax_time);

	private:

};
#endif   /* ----- #ifndef __GF_CGF_SUMMON_H__  ----- */
