#ifndef _CGF_BUY_ITEM_LIMIT_H_
#define _CGF_BUY_ITEM_LIMIT_H_

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
using namespace std;


class Cgf_buy_item_limit: public CtableRoute
{
public:
	Cgf_buy_item_limit( mysql_interface * db );
	int get_buy_item_limit_list( userid_t userid, uint32_t role_regtime,  gf_buy_item_limit_list_out_element** pData, uint32_t *count );

	int replace_buy_item_limit_data(userid_t userid, uint32_t role_regtime,  uint32_t item_id, uint32_t limit_type, uint32_t item_count, uint32_t last_time);
	int clear_role_info(userid_t userid,uint32_t role_regtime);
};


#endif
