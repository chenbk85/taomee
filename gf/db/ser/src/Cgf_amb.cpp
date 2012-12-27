#include "db_error.h"
#include "Cgf_amb.h"
Cgf_amb::Cgf_amb(mysql_interface * db) : CtableRoute100(db, "GF_OTHER", "t_gf_amb", "childuid")
{
}
int Cgf_amb::set_amb_info(uint32_t userid, uint32_t parentuid)
{
	GEN_SQLSTR(this->sqlstr,"insert into %s (childuid,parentuid,tm) values (%u,%u,%u)",
		this->get_table_name(userid), userid, parentuid, (uint32_t)time(NULL));
	return this->exec_insert_sql (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cgf_amb::get_amb_info(uint32_t userid, gf_get_amb_info_out* p_out)
{
	sprintf( this->sqlstr, "select parentuid from %s where childuid=%u", 
		this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->parentuid);
	STD_QUERY_ONE_END();

}

int Cgf_amb::del_amb_info(uint32_t userid)
{
	sprintf( this->sqlstr, "delete from %s where childuid=%u", 
		this->get_table_name(userid), userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR );

}


