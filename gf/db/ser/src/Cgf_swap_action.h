#ifndef  __GF_CGF_SWAP_ACTION_H__
#define  __GF_CGF_SWAP_ACTION_H__ 

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"


class Cgf_swap_action:public CtableRoute{
	public:
		Cgf_swap_action(mysql_interface * db ); 

        int get_action_type_list(userid_t userid,uint32_t role_regtime, uint32_t* p_count,
            gf_get_swap_action_out_item** pp_list);
		//added by cws 0608
	int add_action_type_list(userid_t userid, uint32_t role_regtime, gf_add_swap_in* p_in);
        int get_action_count(uint32_t userid, uint32_t role_regtime, uint32_t id, uint32_t *count);
        int insert_action(uint32_t userid, uint32_t role_regtime, uint32_t id, uint32_t type, uint32_t add_times);
		int clear_action(uint32_t userid, uint32_t role_regtime, uint32_t type);
        int increase_action_count(uint32_t userid, uint32_t role_regtime, uint32_t id, uint32_t type, uint32_t add_times);
	int delete_action(uint32_t userid, uint32_t role_regtime, gf_clear_swap_action_in* p_in);

		int clear_role_info(userid_t userid,uint32_t role_regtime);
	private:

};
#endif   /* ----- #ifndef CDD_DAILY_ACTION_INCL  ----- */


