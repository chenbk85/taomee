#include "Cgf_other_active.h"
#include "db_error.h"

using namespace std;
//加入
Cgf_other_active::Cgf_other_active(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_active_info","userid")
{ 

}

int Cgf_other_active::get_other_active_list(userid_t userid,uint32_t role_regtime, uint32_t* p_count, 
	gf_get_other_active_list_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select active_id, info_buff from %s \
		where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), userid, role_regtime);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->active_id );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->info_buf, ACTIVE_BUF_LEN );
	STD_QUERY_WHILE_END();
}


int Cgf_other_active::get_player_other_active_info(userid_t userid, uint32_t usertm, gf_get_other_active_out * out)
{
	GEN_SQLSTR(sqlstr, "select active_id, info_buff from %s where active_id =%u and userid=%u and role_regtime=%u",
			this->get_table_name(userid), out->active_id,  userid, usertm);

	STD_QUERY_ONE_BEGIN(sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(out->active_id);
		BIN_CPY_NEXT_FIELD(out->info_buf, ACTIVE_BUF_LEN);
	STD_QUERY_ONE_END();
	return 0;
}


int Cgf_other_active::get_player_single_other_active(userid_t userid, uint32_t usertm, gf_get_single_other_active_out *out)
{
	GEN_SQLSTR(sqlstr, "select active_id, info_buff from %s where active_id =%u and userid=%u and role_regtime=%u",
			this->get_table_name(userid), out->active_id,  userid, usertm);

	STD_QUERY_ONE_BEGIN(sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(out->active_id);
		BIN_CPY_NEXT_FIELD(out->info_buf, ACTIVE_BUF_LEN);
	STD_QUERY_ONE_END();
	return 0;
}

int Cgf_other_active::set_player_other_active_info(userid_t userid, uint32_t usertm, gf_set_other_active_in * in)
{
    char buf_mysql[mysql_str_len(ACTIVE_BUF_LEN)];
    set_mysql_string(buf_mysql, in->info_buf,  ACTIVE_BUF_LEN);

//	char nick_mysql[mysql_str_len(NICK_LEN)];
//	set_mysql_string(nick_mysql, nick, NICK_LEN);

	GEN_SQLSTR(sqlstr, "replace into %s set active_id=%u, userid=%u, role_regtime=%u, reset_type=%u,  out_tm = %u,  info_buff='%s' ",
			this->get_table_name(userid),  in->active_id, userid, usertm, in->reset_type, in->out_tm,  buf_mysql); 

	return this->exec_update_sql(this->sqlstr, SUCC);
}



int Cgf_other_active::clear_player_out_date_active_info(userid_t userid, uint32_t usrtm, uint32_t reset_type)
{
	GEN_SQLSTR(sqlstr, "delete from %s where userid=%u and reset_type=%u",
			this->get_table_name(userid), userid, reset_type);
	return this->exec_update_sql(this->sqlstr, SUCC);
}

int Cgf_other_active::clear_player_out_tm_active_info(userid_t usrid, uint32_t usrtm)
{
	GEN_SQLSTR(sqlstr, "delete from %s where userid=%u and reset_type=4 and out_tm < unix_timestamp(now())",
			this->get_table_name(usrid), usrid);

	return this->exec_update_sql(this->sqlstr, SUCC);
}




