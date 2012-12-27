#include <algorithm>
#include "db_error.h"
#include "Cgf_donate.h"

using namespace std;


Cgf_donate::Cgf_donate(mysql_interface *db)
	        :Ctable(db, "GF_OTHER", "t_gf_donate")
{
	
}

int Cgf_donate::get_red_black_count(uint32_t* red,  uint32_t* black)
{
	GEN_SQLSTR(sqlstr, "select * from %s limit 1", get_table_name());
	STD_QUERY_ONE_BEGIN(sqlstr, SUCC);
	        INT_CPY_NEXT_FIELD(*red);
			INT_CPY_NEXT_FIELD(*black);
	STD_QUERY_ONE_END();
}

int Cgf_donate::add_red(uint32_t add )
{
	GEN_SQLSTR(sqlstr, "update %s set red = red + 1", get_table_name());
	return exec_update_sql(sqlstr, SUCC);
}

int Cgf_donate::add_black(uint32_t add )
{
	GEN_SQLSTR(sqlstr, "update %s set black = black + 1", get_table_name());
	return exec_update_sql(sqlstr, SUCC);
}

