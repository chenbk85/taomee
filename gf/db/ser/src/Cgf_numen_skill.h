#ifndef  __GF_CGF_NUMEN_SKILL_H__
#define  __GF_CGF_NUMEN_SKILL_H__
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_numen_skill:public CtableRoute{
	public:
		Cgf_numen_skill(mysql_interface * db ); 
        int get_numen_skill_list(userid_t userid, uint32_t usertm, 
            uint32_t numen_id, uint32_t* p_count, numen_skill_t** pp_list);

		int add_numen_skill(userid_t userid,uint32_t usertm, uint32_t numen_id, uint32_t id, uint32_t lv);

		int del_numen_skill(userid_t userid,uint32_t usertm, uint32_t numen_id, uint32_t id, uint32_t lv);

		int update_numen_skill(userid_t userid,uint32_t usertm, uint32_t numen_id, uint32_t sid,uint32_t id, uint32_t lv);

    private:

};
#endif   /* ----- #ifndef __GF_CGF_NUMEN_SKILL_H__  ----- */
