#ifndef  __GF_CGF_SUMMON_SKILL_H__
#define  __GF_CGF_SUMMON_SKILL_H__
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_summon_skill:public CtableRoute{
	public:
		Cgf_summon_skill(mysql_interface * db ); 
        int get_summon_skill_list(userid_t userid, uint32_t usertm, 
            uint32_t mon_tm, uint32_t* p_count, sum_skill_t** pp_list);

		int add_summon_skill(userid_t userid,uint32_t usertm, uint32_t mon_tm, uint32_t id, uint32_t order);

		int del_summon_skill(userid_t userid,uint32_t usertm, uint32_t mon_tm, uint32_t id);

		int update_summon_skill(userid_t userid,uint32_t usertm, uint32_t mon_tm, uint32_t sid,uint32_t id, uint32_t lv);

		int get_summon_skill_cnt(userid_t userid, uint32_t usertm, uint32_t mon_tm, uint32_t* db_num);
		
        int clear_summon_skill(userid_t userid, uint32_t usertm, uint32_t mon_tm);
		int clear_role_info(userid_t userid,uint32_t role_regtime);
    private:

};
#endif   /* ----- #ifndef __GF_CGF_SUMMON_SKILL_H__  ----- */
