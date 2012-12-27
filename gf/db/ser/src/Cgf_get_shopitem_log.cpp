#include "db_error.h"

#include "Cgf_get_shopitem_log.h"

Cgf_get_shopitem_log::Cgf_get_shopitem_log(mysql_interface * db) : CtableRoute100x10(db,"GF" ,"t_gf_get_shopitem_log","userid")
{
	
}

int Cgf_get_shopitem_log::insert_item_log(userid_t userid, uint32_t role_regtime, uint32_t itemid, uint32_t count)
{
	sprintf( this->sqlstr, "insert into %s (userid,role_regtime,itemid,count,use_tm) values \
		(%u, %u, %u, %u, UNIX_TIMESTAMP());" ,
		this->get_table_name(userid), userid, role_regtime, itemid, count);
	return this->exec_insert_sql (this->sqlstr, GF_ITEM_EXISTED_ERR );	
}
