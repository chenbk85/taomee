#include "Cgf_buff.h"

Cgf_buff::Cgf_buff(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_buf","userid")
{ 

}

int Cgf_buff::add_buff(userid_t userid, uint32_t role_time, uint32_t buff_type, 
	uint32_t duration, uint32_t mutex_type, uint32_t start_tm)
{
	if (duration) {
		sprintf( this->sqlstr, "replace into %s (userid,role_regtime,buff_type,duration, mutex_type, start_tm) values \
			(%u, %u, %u, %u, %u, %u)" ,
			this->get_table_name(userid), userid, role_time, buff_type, duration, mutex_type, start_tm);
	} else {
		sprintf( this->sqlstr, "delete from %s where userid = %u and role_regtime = %u and buff_type=%u" ,
			this->get_table_name(userid), userid, role_time, buff_type);
	}
	return this->exec_insert_sql (this->sqlstr, USER_ID_NOFIND_ERR );
}

int Cgf_buff::del_all_btl_buff(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and buff_type < 900000;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_buff::clear_role_buff(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_buff::daily_del_buff(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and 	buff_type=1003;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_buff::del_buff_when_login(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and \
		buff_type > 900000 and (start_tm + duration) < unix_timestamp();" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_buff::update_buff(userid_t userid, uint32_t role_time, uint32_t buff_type, uint32_t duration)
{
	sprintf( this->sqlstr, "update %s set duration =%u where userid=%u and role_regtime=%u and buff_type=%u;" ,
		this->get_table_name(userid), duration, userid, role_time, buff_type );
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR );	
}

int Cgf_buff::get_buff_list(userid_t userid, userid_t role_regtime, uint32_t* p_count,
		gf_get_db_buff_list_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select buff_type, duration,mutex_type,start_tm from %s \
		where userid=%u and role_regtime=%u ;",
		this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->buff_type );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->duration );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->mutex_type );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->start_tm );
	STD_QUERY_WHILE_END();
}

int Cgf_buff::del_one_buff_on_player(userid_t userid, uint32_t role_time, uint32_t buff_type)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and buff_type=%u;",
			this->get_table_name(userid), userid,  role_time, buff_type);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cgf_buff::del_invitee_buff(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and \
		buff_type = 1400 and (start_tm + duration) < unix_timestamp();" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}
