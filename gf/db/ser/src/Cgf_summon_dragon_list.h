#ifndef _CGF_SUMMON_DRAGON_LIST_H_
#define _CGF_SUMMON_DRAGON_LIST_H_

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_summon_dragon_list: public Ctable
{
public:
	Cgf_summon_dragon_list(mysql_interface * db );
	int get_summon_dragon_list(uint32_t top_limit, gf_get_summon_dragon_list_out_item** pdata, uint32_t* count);
	int add_summon_dragon(gf_add_summon_dragon_in* p_in);
};





#endif


