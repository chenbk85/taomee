#include <algorithm>
#include "db_error.h"
#include "Cgf_clothes_warehouse.h"

using namespace std;

Cgf_clothes_warehouse::Cgf_clothes_warehouse(mysql_interface * db )
	:CtableRoute( db,"GF" ,"t_gf_clothes_warehouse","userid")
{

}

int Cgf_clothes_warehouse::get_clothes_warehouse_item_list(userid_t userid, uint32_t role_regtime, gf_get_warehouse_clothes_item_list_out_item**  pData, uint32_t* count)

{
	GEN_SQLSTR(this->sqlstr,"select id, attireid, get_time, attire_rank, duration, end_time, attire_lv from %s where user_id=%u", this->get_table_name(userid),userid);
        
        STD_QUERY_WHILE_BEGIN(this->sqlstr, pData, count);
        		INT_CPY_NEXT_FIELD( (*pData+i)->id );
				INT_CPY_NEXT_FIELD( (*pData+i)->attireid );
                INT_CPY_NEXT_FIELD( (*pData+i)->get_time );
				INT_CPY_NEXT_FIELD( (*pData+i)->attire_rank );
				INT_CPY_NEXT_FIELD( (*pData+i)->duration );
				INT_CPY_NEXT_FIELD( (*pData+i)->end_time);
				INT_CPY_NEXT_FIELD( (*pData+i)->attire_lv);
        STD_QUERY_WHILE_END();
}


int  Cgf_clothes_warehouse::add_clothes_warehouse_item(userid_t userid, uint32_t role_regtime, uint32_t attireid, uint32_t get_time ,uint32_t attire_rank, uint32_t duration,  uint32_t end_time, uint32_t attire_lv,  uint32_t *last_insert_id)
{
	*last_insert_id = 0; 
	GEN_SQLSTR(this->sqlstr, "insert into %s  (user_id, role_regtime, mail_time, attireid, get_time, attire_rank, duration, end_time, attire_lv) values(%u, %u, %u, %u, %u ,%u, %u, %u, %u )",
	get_table_name(userid),
	userid,  role_regtime, (uint32_t)time(NULL), attireid, get_time, attire_rank, duration, end_time, attire_lv);
	
	return exec_insert_sql_get_auto_increment_id(sqlstr, SUCC, last_insert_id);
}


int Cgf_clothes_warehouse::delete_clothes_warehouse_item(userid_t userid, uint32_t role_regtime, uint32_t item_id, uint32_t id)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where user_id = %u  and attireid = %u and id = %u",
			get_table_name(userid),
			userid,
			item_id,
			id	
		   );
	return exec_update_list_sql(sqlstr,SUCC);
}

int Cgf_clothes_warehouse::get_clothes_warehouse_item_by_id(userid_t userid, uint32_t role_regtime, uint32_t id, uint32_t *attireid, uint32_t *get_time,  uint32_t *attire_rank, uint32_t *duration, uint32_t *end_time, uint32_t* attire_lv)
{
	GEN_SQLSTR(sqlstr, "select attireid, get_time, attire_rank, duration, end_time, attire_lv from %s where id = %u and user_id = %u limit 1 ",   get_table_name(userid),  id,  userid);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_ATTIREID_NOFIND_ERR);
                INT_CPY_NEXT_FIELD(  *attireid);
                INT_CPY_NEXT_FIELD(  *get_time);
                INT_CPY_NEXT_FIELD(  *attire_rank);
                INT_CPY_NEXT_FIELD(  *duration);
				INT_CPY_NEXT_FIELD(  *end_time);
				INT_CPY_NEXT_FIELD(  *attire_lv);
    STD_QUERY_ONE_END();
}

