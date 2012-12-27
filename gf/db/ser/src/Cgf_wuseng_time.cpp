/*
 * =====================================================================================
 *
 *       Filename:  Cgf_wusheng_time.cpp
 *
 *    Description: 真假武圣击杀信息 排行榜  
 *
 *        Version:  1.0
 *        Created:  10/11/2011 10:46:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus@taomee.com
 *        Company:  Taomee.sh
 *
 * =====================================================================================
 */
#include "Cgf_wuseng_time.h"
#include "db_error.h"
#include "proto.h"

Cgf_wuseng_info::Cgf_wuseng_info(mysql_interface *db)
		   : Ctable(db, "GF_OTHER", "t_gf_wuseng_info") 
{

}


int Cgf_wuseng_info::list_wuseng_pass_info(gf_list_true_wusheng_info_out_item ** out, uint32_t * count)
{
	GEN_SQLSTR(sqlstr, "select userid, role_regtime, role_type, nick_name, pass_time from %s \
			order by pass_time, userid limit 10" ,
				   	this->get_table_name());
			STD_QUERY_WHILE_BEGIN(this->sqlstr, out, count);
		   		INT_CPY_NEXT_FIELD( (*out+i)->uid);
				INT_CPY_NEXT_FIELD( (*out+i)->role_tm);
				INT_CPY_NEXT_FIELD( (*out+i)->role_type);
				BIN_CPY_NEXT_FIELD( (*out+i)->nick_name, sizeof((*out+i)->nick_name) );
				INT_CPY_NEXT_FIELD( (*out+i)->pass_time);
			STD_QUERY_WHILE_END();
}

int Cgf_wuseng_info::set_wuseng_pass_info(gf_set_true_wusheng_info_in *in)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, in->nick_name, NICK_LEN);
	
	GEN_SQLSTR(sqlstr, "replace into %s (userid, role_regtime, role_type, nick_name, pass_time) \
		   	values (%u, %u, %u, '%s', %u);",
			  this->get_table_name(), in->uid, in->role_tm, in->role_type, nick_mysql, in->pass_time);
	return exec_update_list_sql(this->sqlstr, GF_WUSENG_INFO_ERR);
}

int Cgf_wuseng_info::get_user_pass_info(uint32_t uid, uint32_t role_time, uint32_t * pass_time)
{
	GEN_SQLSTR(sqlstr, "select pass_time from %s where userid=%u and role_regtime=%u",
				   	this->get_table_name(), uid, role_time);
		STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
			INT_CPY_NEXT_FIELD(*pass_time);
		STD_QUERY_ONE_END();

}


int Cgf_wuseng_info::get_quicker_pass(uint32_t pass_time, uint32_t * count)
{
	GEN_SQLSTR(sqlstr, "select count(*) from %s where pass_time <= %u", 
			this->get_table_name(), pass_time);
		STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
			INT_CPY_NEXT_FIELD(*count);
		STD_QUERY_ONE_END();
}

/*
int Cgf_wuseng_info::get_all_pass_count(uint32_t * count)
{
	GEN_SQLSTR(sqlstr, "select count(*) from %s;", this->get_table_name()); 
		STD_QUERY_ONE_BEGIN(this->sqlstr, GF_WUSENG_INFO_ERR);
			INT_CPY_NEXT_FIELD(*count);
		STD_QUERY_ONE_END();
} 

int Cgf_wuseng_info::get_slowest_pass(uint32_t * uid, uint32_t * role_time)
{
	GEN_SQLSTR(sqlstr, "select userid, role_regtime from %s order by pass_time, userid desc limit 1;", 
			this->get_table_name());
		STD_QUERY_ONE_BEGIN(this->sqlstr, GF_WUSENG_INFO_ERR);
			INT_CPY_NEXT_FIELD(*uid);
			INT_CPY_NEXT_FIELD(*role_time);
		STD_QUERY_ONE_END();
}

int Cgf_wuseng_info::del_slowest_pass(uint32_t uid, uint32_t role_time)
{
	GEN_SQLSTR(sqlstr, "delete from %s where userid=%u and role_regtime=%u;", 
			  this->get_table_name(), uid, role_time);
	return exec_update_list_sql(this->sqlstr, GF_WUSENG_INFO_ERR);

} */

