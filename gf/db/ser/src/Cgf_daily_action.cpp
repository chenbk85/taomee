#include <algorithm>
#include "db_error.h"
#include "Cgf_daily_action.h"

using namespace std;


//加入
Cgf_daction::Cgf_daction(mysql_interface * db ) 
	:Ctable( db, "GF_OTHER", "t_gf_daily_action")
{ 

}

int Cgf_daction::get_action_type_list(userid_t userid,uint32_t role_regtime, uint32_t* p_count, 
	gf_get_daily_action_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select typeid, count from %s \
		where userid=%u and (role_regtime=%u or role_regtime=0);",
		this->get_table_name(), userid, role_regtime);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->type );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->count );
	STD_QUERY_WHILE_END();
}

int Cgf_daction::get_action_count(uint32_t userid, uint32_t role_regtime,
    uint32_t type, uint32_t *count)
{
	GEN_SQLSTR(this->sqlstr,"select count from %s \
		where userid=%u and role_regtime=%u and typeid=%u",
		this->get_table_name(), userid, role_regtime, type);
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_DAILY_ACTION_TYPE_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *count );
	STD_QUERY_ONE_END();
}

int Cgf_daction::insert_action(uint32_t userid, uint32_t role_regtime, uint32_t type)
{
    GEN_SQLSTR(this->sqlstr, "insert into %s (userid, role_regtime, typeid, flag, count, tm) \
        values (%u, %u, %u, %u, %u, unix_timestamp())",
        this->get_table_name(), userid, role_regtime, type, 0, 1);
    return this->exec_insert_sql (this->sqlstr, GF_DAILY_ACTION_TYPE_EXISTED_ERR );
}

int Cgf_daction::increase_action_count(uint32_t userid, uint32_t role_regtime,
    uint32_t type, uint32_t limit)
{
    uint32_t count = 0;
    int ret = this->get_action_count(userid, role_regtime, type, &count);
    if (ret != SUCC && ret != GF_DAILY_ACTION_TYPE_NOFIND_ERR){
        return  ret;
    } else if ( ret == GF_DAILY_ACTION_TYPE_NOFIND_ERR) {
        return this->insert_action(userid, role_regtime, type);
    }else {
        if (limit != 0 && count >= limit) {
            return GF_DAILY_ACTION_TYPE_MAX_ERR;
        }
        GEN_SQLSTR(this->sqlstr,"update %s set %s=%s+%u \
			where userid=%u and role_regtime=%u and typeid=%u; ",
			this->get_table_name(), "count", "count", 1, userid, role_regtime, type);
		return this->exec_update_sql (this->sqlstr, GF_DAILY_ACTION_TYPE_NOFIND_ERR);
    }
}

