#ifndef GF_CGF_WAREHOUSE_H
#define GF_CGF_WAREHOUSE_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

#define WAREHOUSE_SPACE_MAX 12

class Cgf_warehouse: public CtableRoute
{
public:
	 Cgf_warehouse(mysql_interface * db );
	 int  get_warehouse_item_list(userid_t userid, uint32_t role_regtime, gf_get_warehouse_item_list_out_item**  pData, uint32_t* count);
	 int  add_warehouse_item(userid_t userid, uint32_t role_regtime, uint32_t item_id,  uint32_t item_count);
	 int  delete_warehouse_item(userid_t userid, uint32_t role_regtime, uint32_t item_id,  uint32_t item_count);
	 int  get_warehouse_item_count(userid_t userid, uint32_t item_id, uint32_t* count);
     int get_used_space_cnt(userid_t userid, uint32_t* cnt);
     int insert_item2warehouse(userid_t userid, uint32_t item_id, uint32_t count);
     int add_item2warehouse(userid_t userid, uint32_t item_id, uint32_t count, uint32_t limit);
	 int  get_warehouse_used_count(userid_t userid, uint32_t* count);
};

#endif
