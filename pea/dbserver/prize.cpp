#include "prize.hpp"
#include "pea_common.hpp"

pea_prize::pea_prize(mysql_interface* db)
	:CtableRoute100x10(db, "pea", "pea_prize", "user_id")
{
		
}

int pea_prize::add( db_user_id_t db_user_id,  uint32_t prize_id, int32_t add_time)
{
    GEN_SQLSTR(sqlstr, "insert into %s (user_id, role_tm, server_id, prize_id, add_time)values(%u,%u,%u, %u,%u)", 
			get_table_name(db_user_id.user_id), db_user_id.user_id,
            db_user_id.role_tm,db_user_id.server_id, prize_id, add_time
			);
    return exec_insert_sql(sqlstr, DB_ERR_EXIST );
}

int pea_prize::del(db_user_id_t db_user_id,  uint32_t prize_id)
{
    GEN_SQLSTR(sqlstr, "delete from %s where user_id=%u and role_tm=%u and server_id =%u  and prize_id=%u order by add_time asc limit 1", 
			get_table_name(db_user_id.user_id), db_user_id.user_id,db_user_id.role_tm ,db_user_id.server_id,prize_id);
    return exec_delete_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_prize::get_prize_list(db_user_id_t db_user_id, std::vector<db_prize_t> &prize_list)
{
    GEN_SQLSTR(this->sqlstr, "select prize_id, count(*) from %s where user_id=%u and role_tm=%u and server_id =%u group by prize_id",
            this->get_table_name(db_user_id.user_id),db_user_id.user_id,db_user_id.role_tm,db_user_id.server_id);
	db_prize_t item;
    STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr,prize_list);
        INT_CPY_NEXT_FIELD(item.prize_id);
        INT_CPY_NEXT_FIELD(item.count);
    STD_QUERY_WHILE_END_NEW();
}
