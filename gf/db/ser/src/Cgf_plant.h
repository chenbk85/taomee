#ifndef  __GF_CGF_PLANT_H__
#define  __GF_CGF_PLANT_H__
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_plant:public CtableRoute{
	public:
		Cgf_plant(mysql_interface * db ); 

        int get_plant_list(userid_t userid, uint32_t usertm, uint32_t* p_count, gf_get_plant_list_out_item** pplist);

		int add_plant(userid_t userid, uint32_t role_regtime, uint32_t plant_tm, uint32_t plant_id, uint32_t status, uint32_t status_tm);

		int del_plant(userid_t userid,uint32_t usertm, uint32_t field_id);

        int set_plant_status(userid_t userid , uint32_t role_regtime, gf_set_plant_status_in* p_in);
		int add_effect_to_all_plants(userid_t userid , uint32_t role_regtime, gf_add_effect_to_all_plants_in* p_in);
		int clear_role_info(userid_t userid,uint32_t role_regtime);
	private:

};
#endif   /* ----- #ifndef __GF_CGF_PLANT_H__  ----- */
