#ifndef  GF_CGF_ITEM_H
#define    GF_CGF_ITEM_H

#include "Citem_change_log.h"

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

#define ITEM_KIND_MAX 50
#define DEFAULT_MAX_ITEM_CNT 50000
#define DEFAULT_MAX_ITEM_GRID_CNT 50000
extern const char* item_count_column ;

class Cgf_item:public CtableRoute{
	public:
		Cgf_item(mysql_interface * db,  Citem_change_log * p_log_in); 

		int get_user_item_list(userid_t userid, uint32_t role_regtime, 
				uint32_t* p_count, gf_get_user_item_list_out_item** pp_list);
		
		int get_item_kind_cnt(userid_t userid, uint32_t role_regtime, uint32_t *p_count);
        int get_item_cnt_by_itemid(userid_t userid, uint32_t role_regtime, uint32_t *p_count, uint32_t id);
		
		int clear_role_item(userid_t userid, uint32_t role_regtime);	

		int get_item_int_value(userid_t userid, uint32_t role_regtime, uint32_t itemid,
				const char* column_type,uint32_t *p_count);

		int reduce_item_int_value(userid_t userid, uint32_t role_regtime, uint32_t itemid, 
			const char* column_type, uint32_t del_num, bool log_flg = true);

		int add_item(userid_t userid, uint32_t role_regtime, uint32_t itemid,uint32_t count,
			uint32_t max_num, uint32_t max_item_bag_grid_count, item_change_log_opt_type is_vip_opt, bool log_flg=true);

	private:
		int set_item_int_value(userid_t userid, uint32_t role_regtime, uint32_t itemid, const char * field_type , uint32_t	value);
		
		inline int insert_table(userid_t userid, uint32_t role_regtime, uint32_t itemid, uint32_t count);

		int insert_item(userid_t userid, uint32_t role_regtime, uint32_t itemid, uint32_t count, uint32_t max_item_bag_grid_count);

		int del_item(userid_t userid, uint32_t role_regtime, uint32_t itemid);

		Citem_change_log * p_log;
};
#endif   /* ----- #ifndef CDD_ATTIRE_INCL  ----- */




