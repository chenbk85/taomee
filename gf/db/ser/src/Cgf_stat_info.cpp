/*
 * =====================================================================================
 *
 *       Filename:  Cgf_stat_info.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/27/2012 02:16:34 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#include "Cgf_stat_info.h"
#include "db_error.h"



Cgf_stat_info::Cgf_stat_info(mysql_interface * db )
	:CtableRoute( db, "GF" ,"t_gf_stat_info","userid")
{ }

int Cgf_stat_info::get_stat_val(userid_t userid, uint32_t role_regtime, uint32_t stat_id, uint32_t * val)
{
	GEN_SQLSTR( this->sqlstr, "select stat_cnt from %s where userid=%u and role_regtime=%u and stat_id=%u ;" ,
		 this->get_table_name(userid), userid, role_regtime, stat_id);

	STD_QUERY_ONE_BEGIN(sqlstr, GF_STAT_NO_DATA);
		INT_CPY_NEXT_FIELD (*val);
	STD_QUERY_ONE_END();
}

int Cgf_stat_info::insert_stat_val(userid_t userid, uint32_t role_regtime, uint32_t stat_id, uint32_t cnt)
{
	GEN_SQLSTR( this->sqlstr, "insert into %s (userid,role_regtime,stat_id,stat_cnt) values \
		(%u, %u, %u, %u)" ,
		this->get_table_name(userid), userid, role_regtime, stat_id, cnt);
	return exec_update_sql(sqlstr, SUCC);
}

int Cgf_stat_info::add_player_stat_val(userid_t userid, uint32_t role_regtime, uint32_t stat_id, uint32_t add_cnt)
{
	uint32_t db_num = 0;

	int ret = this->get_stat_val(userid, role_regtime, stat_id, &db_num);
	if (ret == GF_STAT_NO_DATA)
	{
		return this->insert_stat_val(userid, role_regtime, stat_id, add_cnt);
	} else if (ret == 0) {
		GEN_SQLSTR(this->sqlstr,"update %s set stat_cnt = stat_cnt + %u \
			where userid=%u and role_regtime=%u and stat_id=%u; ",
			this->get_table_name(userid), add_cnt, userid, role_regtime, stat_id);
		return this->exec_update_sql (this->sqlstr, GF_STAT_NO_DATA);		
	}

	return 0;
}

int Cgf_stat_info::list_player_stat_val(userid_t userid, uint32_t role_regtime, gf_get_stat_info_out_item **items, uint32_t * p_count)
{
	GEN_SQLSTR(sqlstr, "select stat_id, stat_cnt from %s where userid = %u and role_regtime = %u",
			            get_table_name(userid), userid,  role_regtime);
	STD_QUERY_WHILE_BEGIN(sqlstr, items, p_count);
		INT_CPY_NEXT_FIELD( (*items + i)->stat_id);
		INT_CPY_NEXT_FIELD( (*items + i)->stat_cnt);
	STD_QUERY_WHILE_END();
}

