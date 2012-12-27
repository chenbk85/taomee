#ifndef __CGF_TITLE_H__
#define __CGF_TITLE_H__

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_title: public CtableRoute
{
public:	
	Cgf_title( mysql_interface * db );
	int get_achieve_title(userid_t userid, uint32_t role_regtime, gf_get_achieve_title_out_item** pdata, uint32_t* cnt);
	int add_achieve_title(userid_t userid, uint32_t role_regtime, uint8_t type, uint32_t subid, uint32_t gettime);

	int clear_role_info(userid_t userid,uint32_t role_regtime);
};


#endif
