#ifndef  __GF_CGF_DECORATE_H__
#define  __GF_CGF_DECORATE_H__
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_decorate:public CtableRoute{
	public:
		Cgf_decorate(mysql_interface * db ); 

        int get_decorate_list(userid_t userid, uint32_t usertm, uint32_t* p_count, gf_get_decorate_list_out_item** pplist);

		int add_decorate_lv(userid_t userid, uint32_t role_regtime, uint32_t decorate_id, uint32_t add_lv);

		int del_decorate(userid_t userid,uint32_t usertm, uint32_t decorate_id);

        int set_decorate(userid_t userid , uint32_t role_regtime, uint32_t decorate_id, uint32_t tm);

        int set_decorate_status(userid_t userid , uint32_t role_regtime, uint32_t id, uint32_t status);
	private:

};
#endif   /* ----- #ifndef __GF_CGF_DECORATE_H__  ----- */
