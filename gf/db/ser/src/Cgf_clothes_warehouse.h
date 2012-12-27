#ifndef CGF_CLOTHES_WAREHOUSE_H
#define CGF_CLOTHES_WAREHOUSE_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_clothes_warehouse: public CtableRoute
{
public:
        Cgf_clothes_warehouse(mysql_interface * db );
		int  get_clothes_warehouse_item_list(userid_t userid, uint32_t role_regtime, gf_get_warehouse_clothes_item_list_out_item**  pData, uint32_t* count);
		int  add_clothes_warehouse_item(userid_t userid, uint32_t role_regtime,  uint32_t attireid, uint32_t get_time, uint32_t attire_rank, uint32_t duration, uint32_t end_time, uint32_t attire_lv,  uint32_t *last_insert_id);
		int  delete_clothes_warehouse_item(userid_t userid, uint32_t role_regtime, uint32_t item_id,  uint32_t id);
		int  get_clothes_warehouse_item_by_id(userid_t userid, uint32_t role_regtime, uint32_t id, uint32_t *attireid, uint32_t *get_time,  uint32_t *attire_rank, uint32_t *duration, uint32_t *end_time, uint32_t* attire_lv);	
};


#endif
