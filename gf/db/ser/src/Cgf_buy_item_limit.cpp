#include <algorithm>
#include "db_error.h"
#include "Cgf_buy_item_limit.h"

using namespace std;


Cgf_buy_item_limit::Cgf_buy_item_limit( mysql_interface * db  )
	:CtableRoute( db,"GF" ,"t_gf_buy_item_limit","userid")
{
	
}


int Cgf_buy_item_limit::get_buy_item_limit_list( userid_t userid, uint32_t role_regtime, gf_buy_item_limit_list_out_element** pData, uint32_t *count )
{
	if(pData == NULL || count == NULL)return 1001;
	GEN_SQLSTR(sqlstr,"select item_id, limit_type, item_count,last_time  from %s where userid=%u and role_regtime = %u ",get_table_name(userid), userid,  role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pData, count);
	        INT_CPY_NEXT_FIELD( (*pData+i)->item_id );
			INT_CPY_NEXT_FIELD( (*pData+i)->limit_type );
			INT_CPY_NEXT_FIELD( (*pData+i)->item_count );
			INT_CPY_NEXT_FIELD( (*pData+i)->last_time );
	STD_QUERY_WHILE_END();
}

int Cgf_buy_item_limit::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_buy_item_limit::replace_buy_item_limit_data(userid_t userid, uint32_t role_regtime,  uint32_t item_id, uint32_t limit_type, uint32_t item_count, uint32_t last_time)
{
	GEN_SQLSTR(sqlstr, "replace into %s set userid = %u, role_regtime = %u, item_id = %u, limit_type = %u, item_count = %u, last_time= %u",  get_table_name(userid), 
		 userid,
 		 role_regtime,
		 item_id, 
		 limit_type,
		 item_count,
		 last_time		 
	);
	return exec_update_list_sql(sqlstr,SUCC);
}
