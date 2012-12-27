#ifndef  __GF_CGF_OTHER_ACTIVE_H_
#define  __GF_CGF_OTHER_ACTIVE_H_
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

enum out_date_t {
	reset_daily = 1,
	reset_weekly = 2,
	reset_month = 3,
	reset_area = 4,
};

class Cgf_other_active:public CtableRoute{
	public:
		Cgf_other_active(mysql_interface * db);

        int get_other_active_list(userid_t userid,uint32_t role_regtime, uint32_t* p_count, 
            gf_get_other_active_list_out_item** pp_list);

		int get_player_other_active_info(userid_t userid, uint32_t usertm, gf_get_other_active_out * out);

        int get_player_single_other_active(userid_t userid, uint32_t usertm,
            gf_get_single_other_active_out *out);

		int set_player_other_active_info(userid_t userid, uint32_t usertm, gf_set_other_active_in * in);

		int clear_player_out_date_active_info(userid_t userid, uint32_t usrtm, uint32_t outdate_type);

		int clear_player_out_tm_active_info(userid_t usrid, uint32_t usrtm);

	private:

};
#endif   /* ----- #ifndef __GF_CGF_RED_BLUE_H__  ----- */
