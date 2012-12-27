#ifndef  __GF_CGF_MONTHLY_ACTION_H__
#define  __GF_CGF_MONTHLY_ACTION_H__ 

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"


class Cgf_maction:public Ctable{
	public:
		Cgf_maction(mysql_interface * db ); 

        int get_action_type_list(userid_t userid,uint32_t role_regtime, uint32_t* p_count,
            gf_get_daily_action_out_item** pp_list);
        int get_action_count(uint32_t userid, uint32_t role_regtime, uint32_t type, uint32_t *count);
        int insert_action(uint32_t userid, uint32_t role_regtime, uint32_t type);
        int increase_action_count(uint32_t userid, uint32_t role_regtime, uint32_t type, uint32_t limit);
	private:

};
#endif   /* ----- #ifndef CDD_DAILY_ACTION_INCL  ----- */


