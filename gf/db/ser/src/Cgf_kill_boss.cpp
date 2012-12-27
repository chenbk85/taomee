#include <algorithm>
#include "db_error.h"
#include "Cgf_kill_boss.h"


Cgf_kill_boss::Cgf_kill_boss(mysql_interface * db)
	:CtableRoute( db,"GF" ,"t_gf_first_killboss","userid")
{

}

int Cgf_kill_boss::get_kill_boss_list(userid_t userid, uint32_t role_regtime, gf_get_kill_boss_list_out_element** pData, uint32_t* count)
{
	GEN_SQLSTR(sqlstr, "select stage_id, boss_id, kill_time, pass_cnt from %s where userid = %u and role_regtime = %u",
			get_table_name(userid), userid,  role_regtime);
	STD_QUERY_WHILE_BEGIN(sqlstr, pData, count);
		INT_CPY_NEXT_FIELD( (*pData+i)->stage_id);
		INT_CPY_NEXT_FIELD( (*pData+i)->boss_id);
		INT_CPY_NEXT_FIELD( (*pData+i)->kill_time);
		INT_CPY_NEXT_FIELD( (*pData+i)->pass_cnt);
	STD_QUERY_WHILE_END();
}

int Cgf_kill_boss::replace_kill_boss(userid_t userid, uint32_t role_regtime, uint32_t stage_id, uint32_t boss_id, uint32_t kill_time, uint32_t pass_cnt)
{
	GEN_SQLSTR(sqlstr, "replace into %s (userid, role_regtime, stage_id, boss_id, kill_time, pass_cnt) values(%u, %u, %u, %u, %u, %u)", 
				      get_table_name(userid), 
					  userid,  
					  role_regtime,
					  stage_id,
					  boss_id,
					  kill_time,
					  pass_cnt
					  );
	return exec_update_sql(sqlstr, SUCC);
}

int Cgf_kill_boss::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}


