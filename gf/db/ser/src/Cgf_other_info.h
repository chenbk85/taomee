#ifndef _CGF_OTHER_INFO_H_
#define _CGF_OTHER_INFO_H_

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"


class Cgf_other_info: public CtableRoute
{
public:
	Cgf_other_info(mysql_interface * db);
	int get_other_info_list(userid_t userid, uint32_t role_regtime, gf_get_other_info_list_out_item** pData, uint32_t* count);
	int replace_other_info(userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t value);
	int clear_role_info(userid_t userid,uint32_t role_regtime);
};








#endif
