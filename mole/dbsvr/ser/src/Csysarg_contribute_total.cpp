/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_contribute_total.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/10/2010 03:30:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include "Csysarg_contribute_total.h"

Csysarg_contribute_total::Csysarg_contribute_total(mysql_interface * db):Ctable(db, "SYSARG_DB","t_sysarg_contribute_total")
{
}

int Csysarg_contribute_total::get_contribute_total(sysarg_contribute_get_user_count_out *p_out)
{
	sprintf(this->sqlstr, "select sum_user, sum_xiaomee, sum_attire from %s", this->get_table_name());

	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(p_out->user_cnt);
		INT_CPY_NEXT_FIELD(p_out->xiaomee_cnt);
		INT_CPY_NEXT_FIELD(p_out->attire_cnt);
	STD_QUERY_ONE_END();
}

int Csysarg_contribute_total::update(uint32_t add_user, uint32_t add_xiaomee, uint32_t add_attire)
{
	sprintf(this->sqlstr, "update %s set sum_user = sum_user + %u, sum_xiaomee = sum_xiaomee + %u, \
			sum_attire = sum_attire + %u", this->get_table_name(), add_user, add_xiaomee, add_attire);
	
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}
