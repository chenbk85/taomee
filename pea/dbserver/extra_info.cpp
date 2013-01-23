#include "extra_info.hpp"


using namespace std;


pea_extra_info::pea_extra_info(mysql_interface * db)
    : CtableRoute100x10(db, "pea", "pea_extra_info", "")
{

}



int pea_extra_info::query_extra_info(db_user_id_t * db_user_id, std::vector<db_extra_info_t> & info_vec)
{
    if (info_vec.empty())
    {
        return 0;
    }

    GEN_SQLSTR(sqlstr, "SELECT info_id, info_value FROM %s WHERE user_id = %u AND role_tm = %u AND server_id = %u AND info_id IN (",
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id);
    
    for (uint32_t i = 0; i < info_vec.size(); i++)
    {
        db_extra_info_t * p_info = &(info_vec[i]);
        GEN_SQLSTR(sqlstr + strlen(sqlstr), "%u, ", p_info->info_id);
    }

    GEN_SQLSTR(sqlstr + strlen(sqlstr) - 2, ")");

    std::vector<db_extra_info_t> existed_info_vec;

    STD_QUERY_WHILE_BEGIN_NEW(sqlstr, existed_info_vec)
    {
        INT_CPY_NEXT_FIELD(item.info_id);
        INT_CPY_NEXT_FIELD(item.info_value);

        for (uint32_t i = 0; i < info_vec.size(); i++)
        {
            if (info_vec[i].info_id == item.info_id)
            {
                info_vec[i].info_value = item.info_value;
                break;
            }
        }
    }
    STD_QUERY_WHILE_END_NEW()
}


int pea_extra_info::update_extra_info(db_user_id_t * db_user_id, std::vector<db_extra_info_t> & info_vec)
{

    
    for (uint32_t i = 0; i < info_vec.size(); i++)
    {
        db_extra_info_t * p_info = &(info_vec[i]);

        int ret = do_update_extra_info(db_user_id, p_info);
        if (DB_SUCC != ret)
        {
            return ret;
        }


    }

    return SUCC;

}


int pea_extra_info::do_update_extra_info(db_user_id_t * db_user_id, db_extra_info_t * p_info)
{
    GEN_SQLSTR(sqlstr, "INSERT INTO %s (user_id, role_tm, server_id, info_id, info_value) VALUES(%u, %u, %u, %u, %u) ON DUPLICATE KEY UPDATE info_value = %u",
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            p_info->info_id,
            p_info->info_value,
            p_info->info_value);

    return exec_insert_sql(sqlstr, DB_SUCC);

}
