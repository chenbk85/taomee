#include <algorithm>
#include "Cgf_reward.h"
#include "db_error.h"

using namespace std;
//加入
Cgf_reward::Cgf_reward(mysql_interface * db ) 
	:Ctable( db, "GF_OTHER", "t_gf_random_reward")
{ 

}

int Cgf_reward::insert_reward_player(userid_t userid, uint32_t usertm, uint32_t reward_id)
{
    uint32_t id = 0;
	GEN_SQLSTR(this->sqlstr,"insert into %s (id, userid, role_regtime, reward_id, reward_tm, reward_flag) \
        values (null, %u, %u, %u, unix_timestamp(now()), 0);",
		this->get_table_name(),userid, usertm, reward_id);
	
	return exec_insert_sql_get_auto_increment_id(this->sqlstr, GF_ITEM_EXISTED_ERR, &id);
}


int Cgf_reward::get_reward_player_list(gf_get_reward_player_out_item ** plist, uint32_t *pcount)
{
	GEN_SQLSTR(sqlstr, "select id, userid, role_regtime, reward_id, reward_tm, reward_flag from %s \
		where reward_tm > unix_timestamp(now())-86400*7+14*3600 order by reward_tm limit 100;", get_table_name());
	STD_QUERY_WHILE_BEGIN(this->sqlstr, plist, pcount);
		INT_CPY_NEXT_FIELD( (*plist+i)->id );
		INT_CPY_NEXT_FIELD( (*plist+i)->userid );
		INT_CPY_NEXT_FIELD( (*plist+i)->roletm );
		INT_CPY_NEXT_FIELD( (*plist+i)->reward_id );
		INT_CPY_NEXT_FIELD( (*plist+i)->reward_tm );
		INT_CPY_NEXT_FIELD( (*plist+i)->reward_flag );
	STD_QUERY_WHILE_END();
}

int Cgf_reward::set_reward_flag(uint32_t userid, uint32_t role_regtime, uint32_t key)
{
	GEN_SQLSTR(sqlstr, "update %s set reward_flag=1 where userid=%u and role_regtime=%u and id=%u",
							get_table_name(), userid, role_regtime, key);
	return exec_update_sql(sqlstr, GF_ATTIREID_NOFIND_ERR);
}

