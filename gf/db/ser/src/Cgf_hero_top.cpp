/*
 * =====================================================================================
 *
 *       Filename:  Cgf_top_hero.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/07/2010 01:37:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include <algorithm>
#include "db_error.h"
#include "Cgf_hero_top.h"

#define MAX_TOP 1000

Cgf_hero_top::Cgf_hero_top(mysql_interface *db)
		:Ctable(db, "GF_OTHER", "t_gf_top_hero")
{
}

int Cgf_hero_top::set_user_hero_top_info(uint32_t userid, uint32_t role_tm, uint32_t lv, uint32_t exp)
{
	GEN_SQLSTR(sqlstr, "replace into %s set userid=%u, role_tm=%u, user_lv=%u, user_exp=%u",
			this->get_table_name(), userid, role_tm,  lv, exp);
	return this->exec_update_sql(this->sqlstr, GF_HERO_TOP_ERR);
}

int Cgf_hero_top::get_user_hero_top_info(uint32_t user_exp, uint32_t *count)
{
	GEN_SQLSTR(sqlstr, "select count(*) from %s where user_exp>%u", this->get_table_name(), user_exp);
	STD_QUERY_ONE_BEGIN(sqlstr, GF_HERO_TOP_ERR);
			INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();
}




