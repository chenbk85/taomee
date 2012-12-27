#include <algorithm>
#include "db_error.h"
#include "Cgf_home.h"

using namespace std;
Cgf_home::Cgf_home(mysql_interface * db )
	    :CtableRoute( db,"GF" ,"t_gf_home","userid")
{

}

int Cgf_home::get_player_home_data(userid_t userid, uint32_t role_regtime, gf_get_home_data_out* out)
{
	GEN_SQLSTR(sqlstr, "select home_lv, home_exp, last_randseed_tm from %s\
		  	where userid = %u and role_regtime = %u", 
				get_table_name(userid),
				userid, 
				role_regtime
				);

	STD_QUERY_ONE_BEGIN(sqlstr, SUCC);
		INT_CPY_NEXT_FIELD( out->home_level);
		INT_CPY_NEXT_FIELD( out->home_exp);
		INT_CPY_NEXT_FIELD( out->last_randseed_tm);
	STD_QUERY_ONE_END();
}

int Cgf_home::set_player_home_data(userid_t userid, uint32_t role_regtime, gf_set_home_data_in * in)
{
	GEN_SQLSTR(sqlstr, "update %s set home_lv=%u , home_exp=%u where userid=%u  and role_regtime=%u", 
			get_table_name(userid), in->home_level, in->home_exp, userid, role_regtime); 
	return exec_update_sql(sqlstr, SUCC);
}

int Cgf_home::set_player_home_update_tm(userid_t userid, uint32_t role_regtime, uint32_t last_randseed_tm)
{
	GEN_SQLSTR(sqlstr, "update %s set last_randseed_tm=%u where userid=%u and role_regtime=%u",
			get_table_name(userid), last_randseed_tm, userid, role_regtime);
	return exec_update_sql(sqlstr, SUCC);
}
int Cgf_home::insert_player_home_data(userid_t userid, uint32_t role_regtime)
{
	GEN_SQLSTR(sqlstr, "replace into %s set userid=%u , role_regtime=%u, home_lv=1, home_exp=0",
			this->get_table_name(userid), userid, role_regtime);
	return exec_update_sql(sqlstr, SUCC);
}
int Cgf_home::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

//int Cgf_home::replace_player_home_exp_data(userid_t userid, uint32_t role_regtime, int exp)
//{
//	GEN_SQLSTR(sqlstr, "replace into %s set home_exp = %u , userid = %u, role_regtime = %u",
//			get_table_name(userid),
//			exp,
//			userid,
//			role_regtime
//			);
//	return exec_update_sql(sqlstr, SUCC);
//}
//
//int Cgf_home::replace_player_home_level_data(userid_t userid, uint32_t role_regtime, int level)
//{
//	GEN_SQLSTR(sqlstr, "replace into %s set home_lv = %u , userid = %u , role_regtime = %u", 
//				get_table_name(userid),
//				level,
//				userid,
//				role_regtime
//			);	
//	return exec_update_sql(sqlstr, SUCC);
//}
//
//int Cgf_home::replace_player_home_exp_level_data(userid_t userid, uint32_t role_regtime, int exp, int level)
//{
//	GEN_SQLSTR(sqlstr, "replace into %s set home_lv = %u , home_exp = %u,  userid = %u , role_regtime = %u",
//			get_table_name(userid),
//			level,
//			exp,
//			userid,
//			role_regtime
//			);
//	return exec_update_sql(sqlstr, SUCC);
//}

//int set_player_home_data(userid_t userid, uint32_t role_regtime, gf_set_home_data_in * in)
//{
//	return 0;
//}
//
//int set_player_home_update_tm(userid_t userid, uint32_t role_regtime, uint32_t last_update_tm)
//{
//	return 0;
//}
//

