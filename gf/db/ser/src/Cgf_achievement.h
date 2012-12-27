#ifndef _CGF_ACHIEVEMENT_H_
#define _CGF_ACHIEVEMENT_H_

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_achievement: public CtableRoute
{
public:	
	Cgf_achievement( mysql_interface * db );
	int get_achievement_data_list( userid_t userid, uint32_t role_regtime, gf_get_achievement_data_list_out_element** pdata, uint32_t* count);
	int replace_achievement_data(userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t get_time);
	int clear_role_achievement(userid_t userid, uint32_t role_regtime);
};


#endif
