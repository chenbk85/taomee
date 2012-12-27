#include <algorithm>
#include "db_error.h"
#include "Cgf_swap_action.h"

using namespace std;


//加入
Cgf_swap_action::Cgf_swap_action(mysql_interface * db ) 
	:CtableRoute( db, "GF", "t_gf_swap_action", "userid")
{ 

}

int Cgf_swap_action::get_action_type_list(userid_t userid,uint32_t role_regtime, uint32_t* p_count, 
	gf_get_swap_action_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select id, type, count, tm from %s \
		where userid=%u and (role_regtime=%u or role_regtime=0);",
		this->get_table_name(userid), userid, role_regtime);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->type );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->cnt );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->tm );
	STD_QUERY_WHILE_END();
}
//added by cws0608
int Cgf_swap_action::add_action_type_list(userid_t userid, uint32_t role_regtime, gf_add_swap_in* p_in)
{
	
	GEN_SQLSTR(this->sqlstr,"insert into %s (userid,role_regtime,id,type,tm,count) values (%u,%u,%u,%u,%u,%u)",
		this->get_table_name(userid), userid, role_regtime, p_in->id, p_in->type, 
		p_in->tm, p_in->cnt);
	return this->exec_insert_sql  (this->sqlstr, GF_SWAPID_EXISTED_ERR);	
}

int Cgf_swap_action::get_action_count(uint32_t userid, uint32_t role_regtime,
    uint32_t id, uint32_t *count)
{
	GEN_SQLSTR(this->sqlstr,"select count from %s \
		where userid=%u and role_regtime=%u and id=%u",
		this->get_table_name(userid), userid, role_regtime, id);
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_DAILY_ACTION_TYPE_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *count );
	STD_QUERY_ONE_END();
}

int Cgf_swap_action::insert_action(uint32_t userid, uint32_t role_regtime, uint32_t id, uint32_t type, uint32_t add_times)
{
    GEN_SQLSTR(this->sqlstr, "insert into %s (userid, role_regtime, id, type, count, tm) \
        values (%u, %u, %u, %u, %u, unix_timestamp())",
        this->get_table_name(userid), userid, role_regtime, id, type, add_times);
    return this->exec_insert_sql (this->sqlstr, GF_DAILY_ACTION_TYPE_EXISTED_ERR );
}

int Cgf_swap_action::clear_action(uint32_t userid, uint32_t role_regtime, uint32_t type)
{
    GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and type=%u ",
        this->get_table_name(userid), userid, type);
    return this->exec_insert_sql (this->sqlstr, GF_DAILY_ACTION_TYPE_EXISTED_ERR );
}


int Cgf_swap_action::increase_action_count(uint32_t userid, uint32_t role_regtime,
    uint32_t id, uint32_t type, uint32_t add_times)
{
    uint32_t count = 0;
    int ret = this->get_action_count(userid, role_regtime, id, &count);
    if (ret != SUCC && ret != GF_DAILY_ACTION_TYPE_NOFIND_ERR){
        return  ret;
    } else if ( ret == GF_DAILY_ACTION_TYPE_NOFIND_ERR) {
        return this->insert_action(userid, role_regtime, id, type, add_times);
    }else {
        GEN_SQLSTR(this->sqlstr,"update %s set %s=%s+%u, tm=unix_timestamp() \
			where userid=%u and role_regtime=%u and id=%u; ",
			this->get_table_name(userid), "count", "count", add_times, userid, role_regtime, id);
		return this->exec_update_sql (this->sqlstr, GF_DAILY_ACTION_TYPE_NOFIND_ERR);
    }
}

int Cgf_swap_action::delete_action(uint32_t userid, uint32_t role_regtime, gf_clear_swap_action_in* p_in)
{
	if(p_in->id != 0){
		GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and (role_regtime=%u or role_regtime=0) and id=%u ",
       	 this->get_table_name(userid), userid, role_regtime, p_in->id);
   		 return this->exec_insert_sql (this->sqlstr, GF_DAILY_ACTION_TYPE_EXISTED_ERR );
		}
	else if(p_in->id == 0){
		GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and (role_regtime=%u or role_regtime=0)",
       	 this->get_table_name(userid), userid, role_regtime);
   		 return this->exec_insert_sql (this->sqlstr, GF_DAILY_ACTION_TYPE_EXISTED_ERR );
		}
}

int Cgf_swap_action::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}



