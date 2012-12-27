#ifndef  GF_CGF_KILLBOSS_H
#define    GF_CGF_KILLBOSS_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_killboss:public CtableRoute{
	public:
		Cgf_killboss(mysql_interface * db ); 

		int get_killed_boss(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
			gf_get_killed_boss_out_item** pp_list);

		int get_killed_boss_kf(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
			gf_get_killed_boss_kf_out_item** pp_list);

		int add_killed_boss(userid_t userid, uint32_t role_regtime, gf_add_killed_boss_in* p_in );

		int clear_role_killboss(userid_t userid,uint32_t role_regtime);

		int set_stage_info(userid_t userid, uint32_t role_regtime, gf_set_role_stage_info_in* p_in);

	
	int get_stage_info(userid_t userid, uint32_t role_regtime, gf_set_role_stage_info_in* p_in,
		uint32_t* grade_lv, uint32_t* grade_first_tm, uint32_t* grade_s_tm, uint32_t* grade_damage);

	int insert_stage_info(userid_t userid, uint32_t role_regtime, gf_set_role_stage_info_in* p_in);

	int clear_role_info(userid_t userid,uint32_t role_regtime);
	private:

};
#endif   /* ----- #ifndef CDD_ATTIRE_INCL  ----- */




