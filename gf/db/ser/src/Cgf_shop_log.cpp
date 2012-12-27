#include <time.h> 
#include <algorithm>
#include "db_error.h"
#include "benchapi.h"
#include "Cgf_shop_log.h"



//xhx_tool
//public:
Cgf_shop_log::Cgf_shop_log(mysql_interface * db)
	:CtableRoute(db,"GF" ,"t_gf_shop_log","userid") 
{ 
}

int Cgf_shop_log::get_shop_last_log_list(userid_t userid, uint32_t role_tm, uint32_t* p_count, gf_get_shop_last_log_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr, "select type, itemid, uniqueid, attire_lv, cnt, price, sell_tm from %s \
	where userid=%u and role_regtime=%u and shop_tm=(select max(shop_tm) from %s where userid=%u and role_regtime=%u);",
	this->get_table_name(userid), userid, role_tm, this->get_table_name(userid), userid, role_tm);	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->type );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->itemid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->uniquekey );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attire_lv );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->cnt );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->price );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->sell_tm );
	STD_QUERY_WHILE_END();

}

int Cgf_shop_log::add_shop_log(userid_t userid, uint32_t role_tm, gf_add_shop_log_in* p_in)
{
	GEN_SQLSTR(this->sqlstr,"insert into %s (userid, role_regtime, shop_tm, type, \
		itemid, uniqueid, cnt, price, sell_tm) values (%u,%u,%u,%u,%u,%u,%u,%u,%u)",
		this->get_table_name(userid), userid, role_tm, p_in->shop_start_tm, p_in->type, 
		p_in->itemid, p_in->uniquekey, p_in->cnt, p_in->price, p_in->sell_tm);
	return this->exec_insert_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_shop_log::add_shop_log_indb(userid_t userid, uint32_t role_tm, gf_sell_goods_in* p_in)
{
	GEN_SQLSTR(this->sqlstr,"insert into %s (userid, role_regtime, shop_tm, type, \
		itemid, uniqueid, cnt, price, sell_tm, attire_lv) values (%u,%u,%u,%u,%u,%u,%u,%u,unix_timestamp(now()),%u)",
		this->get_table_name(userid), userid, role_tm, p_in->shop_start_tm, p_in->type, 
		p_in->itemid, p_in->uniqueid, p_in->cnt, p_in->coins, p_in->attire_lv);
	return this->exec_insert_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_shop_log::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}


