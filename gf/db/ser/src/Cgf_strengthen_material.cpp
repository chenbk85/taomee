#include <algorithm>
#include "db_error.h"
#include "Cgf_strengthen_material.h"

using namespace std;


//¼ÓÈë
Cgf_StrengthenMaterial::Cgf_StrengthenMaterial(mysql_interface * db, Citem_change_log * p_log_in) 
	:CtableRoute( db,"GF" ,"t_gf_material","userid")
{ 
	p_log = p_log_in;
}

int Cgf_StrengthenMaterial::get_user_material_list(userid_t userid,uint32_t role_regtime, uint32_t* p_count, 
	gf_get_strengthen_material_list_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select material_id,count from %s \
		where userid=%u and role_regtime=%u ;",
		this->get_table_name(userid), userid, role_regtime);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->material_id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->cnt );
	STD_QUERY_WHILE_END();
}

int Cgf_StrengthenMaterial::insert_material(userid_t userid, uint32_t role_regtime, uint32_t material_id, uint32_t count)
{
	sprintf( this->sqlstr, "insert into %s (userid,role_regtime,material_id,count) values \
		(%u, %u, %u, %u)" ,
		this->get_table_name(userid), userid, role_regtime, material_id, count);
	return this->exec_insert_sql (this->sqlstr, GF_ITEM_EXISTED_ERR );	
}

int Cgf_StrengthenMaterial::del_material(userid_t userid, uint32_t role_regtime, uint32_t material_id)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and material_id=%u ;" ,
		this->get_table_name(userid), userid, role_regtime, material_id);
	return this->exec_update_sql(this->sqlstr, GF_ITEM_NOFIND_ERR );	
}

int Cgf_StrengthenMaterial::get_material_cnt_by_id(userid_t userid, uint32_t role_regtime, uint32_t *p_count, uint32_t id)
{
	sprintf( this->sqlstr, "select count from %s where userid=%u and role_regtime=%u and material_id=%u;" ,
		 this->get_table_name(userid), userid, role_regtime, id);
	STD_QUERY_ONE_BEGIN(sqlstr, GF_ITEM_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*p_count);
	STD_QUERY_ONE_END();
}

int Cgf_StrengthenMaterial::reduce_material_count(userid_t userid, uint32_t role_regtime, 
    uint32_t material_id, uint32_t del_num)
{
	uint32_t db_num = 0;

	int ret = this->get_material_cnt_by_id(userid, role_regtime, &db_num, material_id);
	if (ret != SUCC){
		return ret;
	}

	if (del_num != 0xFFFFFFFF) {
		p_log->add(time(NULL), item_change_log_opt_type_del ,userid, material_id, del_num);
	} else {
		p_log->add(time(NULL), item_change_log_opt_type_del ,userid, material_id, db_num);
	}
    DEBUG_LOG("reduce material:uid=[%u] db_num=[%u] del_num=[%u]",userid,db_num,del_num);
	if (db_num == del_num || del_num == 0xFFFFFFFF) {
		ret = this->del_material(userid, role_regtime, material_id);
		if (ret != SUCC) {
			return ret;
		}
	} else if (db_num > del_num) {
        GEN_SQLSTR(this->sqlstr,"update %s set count=%u where userid=%u and role_regtime=%u and material_id=%u;",
			this->get_table_name(userid), db_num-del_num, userid, role_regtime, material_id);
		return this->exec_update_sql (this->sqlstr, GF_ITEM_NOFIND_ERR);	
	} else {
		return GF_ITEM_NOTENOUGH_ERR;
	}
	return SUCC;
}

int Cgf_StrengthenMaterial::add_material(userid_t userid,uint32_t role_regtime,uint32_t material_id,uint32_t cnt
	,uint32_t max_num, uint32_t max_item_bag_grid_count, item_change_log_opt_type is_vip_opt, bool log_flg)
{
	uint32_t db_num = 0;
	if (cnt > max_num) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	if (log_flg){
		p_log->add(time(NULL), is_vip_opt ,userid, material_id, cnt);
	}

	int ret = this->get_material_cnt_by_id(userid, role_regtime, &db_num, material_id);
	if (ret != SUCC && ret != GF_ITEM_NOFIND_ERR){
		return ret;
	} else if (ret == GF_ITEM_NOFIND_ERR) {
        return this->insert_material(userid, role_regtime, material_id, cnt);
    } else {
		GEN_SQLSTR(this->sqlstr,"update %s set count=%u where userid=%u and role_regtime=%u and material_id=%u; ",
			this->get_table_name(userid), db_num+cnt, userid, role_regtime, material_id);
		return this->exec_update_sql (this->sqlstr, GF_ITEM_NOFIND_ERR);	
    }
}

int Cgf_StrengthenMaterial::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}


