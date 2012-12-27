#ifndef _CGF_HOME_LOG_H_
#define _CGF_HOME_LOG_H_

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_home_log: public CtableRoute
{
public:
	Cgf_home_log(mysql_interface * db);
public:
	int get_home_log(userid_t userid, uint32_t role_regtime, uint32_t* p_count, gf_get_home_log_out_item** pp_list);
	int add_log(userid_t userid, uint32_t role_regtime, gf_add_home_log_in* p_in);
	int clear_log(userid_t userid, uint32_t role_regtime);
	int clear_role_info(userid_t userid,uint32_t role_regtime);
};


#endif
