#ifndef  __GF_CGF_STRENGTHEN_MATERIAL_H__
#define  __GF_CGF_STRENGTHEN_MATERIAL_H__

#include "Citem_change_log.h"

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"


class Cgf_StrengthenMaterial:public CtableRoute{
	public:
		Cgf_StrengthenMaterial(mysql_interface * db, Citem_change_log * p_log_in);
        int get_user_material_list(userid_t userid,uint32_t role_regtime, uint32_t* p_count,
            gf_get_strengthen_material_list_out_item** pp_list);
        int get_material_cnt_by_id(userid_t userid, uint32_t role_regtime, uint32_t *p_count,uint32_t id);
        int reduce_material_count(userid_t userid, uint32_t role_regtime,uint32_t material_id, uint32_t del_num);
        int add_material(userid_t userid,uint32_t role_regtime,uint32_t material_id,uint32_t cnt
			,uint32_t max_num, uint32_t max_item_bag_grid_count, item_change_log_opt_type is_vip_opt = item_change_log_opt_type_add, bool log_flg=true);

		int clear_role_info(userid_t userid,uint32_t role_regtime);
	private:
        int insert_material(userid_t userid, uint32_t role_regtime, uint32_t material_id, uint32_t cnt);
        int del_material(userid_t userid, uint32_t role_regtime, uint32_t material_id);

		Citem_change_log * p_log;
};
#endif   /* ----- #ifndef __GF_CGF_STRENGTHEN_MATERIAL_H__  ----- */




