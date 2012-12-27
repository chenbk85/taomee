/*
 * =====================================================================================
 *
 *       Filename:  Cgf_hunter_top.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2011 01:37:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Saga (), saga@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include <algorithm>
#include "db_error.h"
#include "Cgf_tmp_info.h"
#include "proto.h"

Cgf_tmp_info::Cgf_tmp_info(mysql_interface *db)
		:Ctable(db, "GF_OTHER", "t_gf_tmp_info")
{
}


int Cgf_tmp_info::insert(gf_insert_tmp_info_in* p_in)
{
    uint32_t id_value = 0;
	get_id_value(p_in->id, p_in->type, &id_value);
	if (id_value) {
		GEN_SQLSTR(sqlstr, "update %s set value=value+%u where id=%u and type=%u", this->get_table_name(), p_in->value, p_in->id, p_in->type);
	} else {
		GEN_SQLSTR(sqlstr, "insert into %s (type, id, value, tm) values(%u, %u, %u, unix_timestamp(now()))", this->get_table_name(), p_in->type, p_in->id, p_in->value);
	}
    
    return this->exec_update_sql(this->sqlstr, GF_HUNTER_TOP_ERR);
}

int Cgf_tmp_info::get_id_value(uint32_t id_in, uint32_t type, uint32_t* db_num)
{
    GEN_SQLSTR(this->sqlstr, "select value from %s where id=%u and type=%u",
        this->get_table_name(), id_in, type);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, GF_SUMMON_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*db_num );
    STD_QUERY_ONE_END();
}



