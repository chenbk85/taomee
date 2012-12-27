#include <algorithm>
#include "db_error.h"
#include "Cgf_item.h"

using namespace std;


const char* item_count_column = "count";

//¼ÓÈë
Cgf_item::Cgf_item(mysql_interface * db, Citem_change_log * p_log_in) 
	:CtableRoute( db,"GF" ,"t_gf_item","userid")
{ 
	p_log = p_log_in;
}

int Cgf_item::get_user_item_list(userid_t userid,uint32_t role_regtime, uint32_t* p_count, 
	gf_get_user_item_list_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select itemid,count from %s \
		where userid=%u and role_regtime=%u and count>0;",
		this->get_table_name(userid), userid, role_regtime);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->itemid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->count );
	STD_QUERY_WHILE_END();
}

int Cgf_item::clear_role_item(userid_t userid,uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), userid, role_regtime);
	
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_item::set_item_int_value(userid_t userid,uint32_t role_regtime, uint32_t itemid ,const char * field_type , uint32_t  value)
{
	sprintf( this->sqlstr, "update %s set %s =%u where %s=%u and role_regtime=%u and itemid=%u;" ,
		this->get_table_name(userid), field_type, value, this->id_name, userid, role_regtime, itemid );
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR );	
}

inline int Cgf_item::insert_table(userid_t userid, uint32_t role_regtime, uint32_t itemid, uint32_t count)
{
	sprintf( this->sqlstr, "insert into %s (userid,role_regtime,itemid,count) values \
		(%u, %u, %u, %u)" ,
		this->get_table_name(userid), userid, role_regtime, itemid, count);
	return this->exec_insert_sql (this->sqlstr, GF_ITEM_EXISTED_ERR );	
}

int Cgf_item::insert_item(userid_t userid, uint32_t role_regtime, uint32_t itemid, uint32_t count, uint32_t max_item_bag_grid_count)
{
	uint32_t kind_num = 0;
	int ret = get_item_kind_cnt(userid, role_regtime, &kind_num);
	if (ret != SUCC)
	{
		return ret;
	}
	if (kind_num >= max_item_bag_grid_count)
	{
		return GF_ITEM_KIND_MAX_ERR;
	}
	return insert_table(userid, role_regtime, itemid, count);
}

int Cgf_item::del_item(userid_t userid, uint32_t role_regtime, uint32_t itemid)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and itemid=%u ;" ,
		this->get_table_name(userid), userid, role_regtime, itemid);
	return this->exec_update_sql(this->sqlstr, GF_ITEM_NOFIND_ERR );	
}

int Cgf_item::reduce_item_int_value(userid_t userid, uint32_t role_regtime, uint32_t itemid, 
	const char* column_type, uint32_t del_num, bool log_flg)
{
	uint32_t db_num = 0;

	int ret = this->get_item_int_value(userid, role_regtime, itemid, column_type, &db_num);
	if (ret != SUCC)
	{
		return ret;
	}
	if (log_flg) {
		if (del_num != 0xFFFFFFFF) {
			p_log->add(time(NULL), item_change_log_opt_type_del ,userid, itemid, del_num);
		} else {
			p_log->add(time(NULL), item_change_log_opt_type_del ,userid, itemid, db_num);
		}
	}
	if (db_num == del_num)
	{
		ret = this->del_item(userid, role_regtime, itemid);
		if (ret != SUCC)
		{
			return ret;
		}
	}
	else if (db_num > del_num)
	{
		ret = this->set_item_int_value(userid, role_regtime, itemid, 
			column_type, db_num-del_num);
		if (ret != SUCC)
		{
			return ret;
		}
	}
	else if (del_num == 0xFFFFFFFF)
	{
		ret = this->del_item(userid, role_regtime, itemid);
		if (ret != SUCC)
		{
			return ret;
		}
		return GF_ITEM_NOTENOUGH_ERR;
	}
	else
	{
		return GF_ITEM_NOTENOUGH_ERR;
	}

	return SUCC;
}

int Cgf_item::get_item_int_value(userid_t userid, uint32_t role_regtime, uint32_t itemid,
	const char* column_type,uint32_t *p_count)
{
	sprintf( this->sqlstr, "select %s from %s where userid=%u and role_regtime=%u \
		and itemid=%u ;" ,
		column_type, this->get_table_name(userid), userid, role_regtime, itemid);
	STD_QUERY_ONE_BEGIN(sqlstr, GF_ITEM_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*p_count);
	STD_QUERY_ONE_END();
}

int Cgf_item::get_item_kind_cnt(userid_t userid, uint32_t role_regtime, uint32_t *p_count)
{
	sprintf( this->sqlstr, "select count(*) from %s where userid=%u and role_regtime=%u ;" ,
		 this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_ONE_BEGIN(sqlstr, GF_ITEM_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*p_count);
	STD_QUERY_ONE_END();
}

int Cgf_item::get_item_cnt_by_itemid(userid_t userid, uint32_t role_regtime, uint32_t *p_count, uint32_t id)
{
	sprintf( this->sqlstr, "select sum(count) from %s where userid=%u and role_regtime=%u and itemid=%u;" ,
		 this->get_table_name(userid), userid, role_regtime, id);
	STD_QUERY_ONE_BEGIN(sqlstr, GF_ITEM_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*p_count);
	STD_QUERY_ONE_END();
}

int Cgf_item::add_item(userid_t userid, uint32_t role_regtime, uint32_t itemid,uint32_t count,uint32_t max_num, uint32_t max_item_bag_grid_count, 
	item_change_log_opt_type is_vip_opt, bool log_flg)
{
	uint32_t db_num = 0;
	DEBUG_LOG("ADD ITEM %u count:%u  %u", itemid, count, max_num);
	if (count>max_num)
		return VALUE_OUT_OF_RANGE_ERR;

	if (log_flg) {
		p_log->add(time(NULL), is_vip_opt ,userid, itemid, count);
	}

	int ret = this->get_item_int_value(userid, role_regtime, itemid, item_count_column, &db_num);
	if (ret != SUCC && ret != GF_ITEM_NOFIND_ERR)
	{
		return ret;
	}
	else if (ret == GF_ITEM_NOFIND_ERR)
	{
		return this->insert_item(userid, role_regtime, itemid, count,  max_item_bag_grid_count);
	}
	else if (db_num + count > max_num)
	{
		return VALUE_OUT_OF_RANGE_ERR;
	}
	else
	{
		GEN_SQLSTR(this->sqlstr,"update %s set count=count+%u \
			where userid=%u and role_regtime=%u and itemid=%u; ",
			this->get_table_name(userid), count, userid, role_regtime, itemid);
		return this->exec_update_sql (this->sqlstr, GF_ITEM_NOFIND_ERR);		
	}

}




