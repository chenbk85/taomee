#include "pet_gift.hpp"

using namespace std;


pea_pet_gift::pea_pet_gift(mysql_interface * db)
    : CtableRoute100x10(db, "pea", "pea_pet_gift", "")
{

}



int pea_pet_gift::update_pet_gift(db_user_id_t * db_user_id, uint32_t pet_no, db_update_pet_gift_t * p_info)
{
    GEN_SQLSTR(sqlstr, "INSERT INTO %s (user_id, role_tm, server_id, pet_no, gift_no, gift_id, gift_level) VALUES(%u, %u, %u, %u, %u, %u, %u) ON DUPLICATE KEY UPDATE gift_id = %u, gift_level = %u", 
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            pet_no,
            p_info->gift_no,
            p_info->gift_id,
            p_info->gift_level,
            p_info->gift_id,
            p_info->gift_level);

    return exec_insert_sql(sqlstr, DB_ERR_EXIST);
}

int pea_pet_gift::query_pet_gift(db_user_id_t * db_user_id, uint32_t pet_no, std::vector<db_pet_gift_info_t> & gift_vec)
{

    GEN_SQLSTR(sqlstr, "SELECT gift_no, gift_id, gift_level FROM %s WHERE user_id = %u AND role_tm = %u AND server_id = %u AND pet_no = %u ORDER BY pet_no",
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            pet_no);

    STD_QUERY_WHILE_BEGIN_NEW(sqlstr, gift_vec)
    {
        INT_CPY_NEXT_FIELD(item.gift_no);
        INT_CPY_NEXT_FIELD(item.gift_id);
        INT_CPY_NEXT_FIELD(item.gift_level);

    }
    STD_QUERY_WHILE_END_NEW()
}
