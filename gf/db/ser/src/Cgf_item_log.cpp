#include "db_error.h"

#include "Cgf_item_log.h"

Cgf_item_log::Cgf_item_log(mysql_interface * db) : CtableRoute100x10(db,"GF" ,"t_gf_item_log","userid")
{
	
}

int Cgf_item_log::insert_use_item_log(userid_t userid, uint32_t role_regtime, uint32_t itemid, uint32_t count)
{
	sprintf( this->sqlstr, "insert into %s (userid,role_regtime,itemid,count,use_tm) values \
		(%u, %u, %u, %u, UNIX_TIMESTAMP());" ,
		this->get_table_name(userid), userid, role_regtime, itemid, count);
	return this->exec_insert_sql (this->sqlstr, GF_ITEM_EXISTED_ERR );	
}
