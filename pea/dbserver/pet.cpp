#include "pet.hpp"

using namespace std;



#define PET_CPY_NEXT_FIELD(pet)\
    INT_CPY_NEXT_FIELD(pet.pet_no);\
    INT_CPY_NEXT_FIELD(pet.pet_id);\
    INT_CPY_NEXT_FIELD(pet.level);\
    INT_CPY_NEXT_FIELD(pet.exp);\
    INT_CPY_NEXT_FIELD(pet.quality);\
    INT_CPY_NEXT_FIELD(pet.status);\
    INT_CPY_NEXT_FIELD(pet.iq);\
    INT_CPY_NEXT_FIELD(pet.rand_attr[0]);\
    INT_CPY_NEXT_FIELD(pet.rand_attr[1]);\
    INT_CPY_NEXT_FIELD(pet.rand_attr[2]);\
    INT_CPY_NEXT_FIELD(pet.rand_attr[3]);\
    INT_CPY_NEXT_FIELD(pet.train_attr[0]);\
    INT_CPY_NEXT_FIELD(pet.train_attr[1]);\
    INT_CPY_NEXT_FIELD(pet.train_attr[2]);\
    INT_CPY_NEXT_FIELD(pet.train_attr[3]);\
    INT_CPY_NEXT_FIELD(pet.try_train_attr[0]);\
    INT_CPY_NEXT_FIELD(pet.try_train_attr[1]);\
    INT_CPY_NEXT_FIELD(pet.try_train_attr[2]);\
    INT_CPY_NEXT_FIELD(pet.try_train_attr[3]);\



pea_pet::pea_pet(mysql_interface * db)
    : CtableRoute100x10(db, "pea", "pea_pet", "pet_no")
{

}

int pea_pet::query_pet(db_user_id_t * db_user_id, std::vector<db_pet_info_t> & vec)
{

    return do_get_all_pet(db_user_id, vec);
    // db_proto_get_pet_in * p_in = P_IN;
    // db_proto_get_pet_out * p_out = P_OUT;

    // p_out->init();


    // db_pet_info_t fight_pet;
    // int ret = get_fight_pet(&p_in->db_user_id, &fight_pet);
    // if (DB_SUCC != ret)
    // {
        // return ret;
    // }

    // if (0 != fight_pet.pet_id)
    // {
        // p_out->db_pet_info.push_back(fight_pet);
    // }

    // ret = get_assist_pet(&p_in->db_user_id, p_out->db_pet_info);
    // if (DB_SUCC != ret)
    // {
        // return ret;
    // }

    // ret = get_bag_pet(&p_in->db_user_id, p_out->db_pet_info);
    // if (DB_SUCC != ret)
    // {
        // return ret;
    // }

    // DEBUG_LOG("pet num: %zu", p_out->db_pet_info.size());

    // return 0;
}


int pea_pet::query_fight_pet(Cmessage * c_in, Cmessage * c_out)
{
    db_proto_get_fight_pet_in * p_in = P_IN;
    db_proto_get_fight_pet_out * p_out = P_OUT;


    return get_fight_pet(&p_in->db_user_id, &p_out->fight_pet);
}




int pea_pet::get_fight_pet(db_user_id_t * db_user_id, db_pet_info_t * p_pet)
{
    vector<db_pet_info_t> pet_vec;
    int ret = do_get_status_pet(db_user_id, PET_STATUS_FIGHT, pet_vec);
    if (DB_SUCC != ret)
    {
        return ret;
    }

    if (pet_vec.size() > 1)
    {
        // 有超过一只出战精灵
        // 数据出现错误，需要修复
        // 保留第一只，多余的精灵放入背包

        for (uint32_t i = 1; i < pet_vec.size(); i++)
        {
            db_pet_info_t * p = &(pet_vec[i]);
            do_set_pet_status(db_user_id, p->pet_no, PET_STATUS_BAG);
        }
    }

    if (1 <= pet_vec.size())
    {
        static byte_array_t ba;
        pet_vec[0].write_to_buf_ex(ba);
        p_pet->read_from_buf_ex(ba);
    }

    return 0;
}

int pea_pet::get_assist_pet(db_user_id_t * db_user_id, vector<db_pet_info_t> & pet_vec)
{
    uint32_t old_size = pet_vec.size();

    int ret = do_get_status_pet(db_user_id, PET_STATUS_ASSIST, pet_vec);
    if (DB_SUCC != ret)
    {
        return ret;
    }

    if (pet_vec.size() > MAX_ASSIST_PET_NUM + old_size)
    {
        // 辅助精灵数量超过限制
        // 数据错误，需要修复
        // 保留前n只，其余放入背包
        for (uint32_t i = MAX_ASSIST_PET_NUM; i < pet_vec.size(); i++)
        {
            db_pet_info_t * p = &(pet_vec[i]);
            do_set_pet_status(db_user_id, p->pet_no, PET_STATUS_BAG);
        }

        pet_vec.erase(pet_vec.begin() + MAX_ASSIST_PET_NUM, pet_vec.end());

    }

    return 0;
}

int pea_pet::get_bag_pet(db_user_id_t * db_user_id, vector<db_pet_info_t> & pet_vec)
{
    int ret = do_get_status_pet(db_user_id, PET_STATUS_BAG, pet_vec);
    if (DB_SUCC != ret)
    {
        return ret;
    }

    // if (pet_vec.size() > MAX_BAG_PET_NUM)
    // {
        // // 精灵数量超过限制
        // // 数据错误，需要修复
        // // 保留前n只，其余放入小屋
        // for (uint32_t i = MAX_BAG_PET_NUM; i < pet_vec.size(); i++)
        // {
            // db_pet_info_t * p = &(pet_vec[i]);
            // do_set_pet_status(db_user_id, p->pet_no, p->pet_id, PET_STATUS_HOME);
        // }
        // pet_vec.erase(pet_vec.begin() + MAX_BAG_PET_NUM, pet_vec.end());

    // }

    return 0;
}


int pea_pet::set_fight_pet(db_user_id_t * db_user_id, uint32_t des_pet_no, uint32_t src_pet_no)
{
    int ret = SUCC;

    if (0 != src_pet_no)
    {
        ret = do_set_pet_status(db_user_id, src_pet_no, PET_STATUS_BAG);
        if (SUCC != ret)
        {
            return ret;
        }

    }

    vector<db_pet_info_t> vec;

    ret = do_get_status_pet(db_user_id, PET_STATUS_FIGHT, vec);
    if (SUCC != ret)
    {
        return ret;
    }

    if (!vec.empty())
    {
        for (uint32_t i = 0; i < vec.size(); i++)
        {
            do_set_pet_status(db_user_id, vec[i].pet_no, PET_STATUS_BAG);
        }
    }

    if (0 != des_pet_no)
    {
        ret = do_set_pet_status(db_user_id, des_pet_no, PET_STATUS_FIGHT);
        if (SUCC != ret)
        {
            return ret;
        }
    }

    return ret;

}

int pea_pet::set_assist_pet(db_user_id_t * db_user_id, uint32_t des_pet_no, uint32_t src_pet_no)
{
    int ret = SUCC;

    if (0 != src_pet_no)
    {
        ret = do_set_pet_status(db_user_id, src_pet_no, PET_STATUS_BAG);
        if (SUCC != ret)
        {
            return ret;
        }

    }


    if (0 != des_pet_no)
    {
        ret = do_set_pet_status(db_user_id, des_pet_no, PET_STATUS_ASSIST);
        if (SUCC != ret)
        {
            return ret;
        }
    }

    return ret;

}



int pea_pet::set_pet_iq(db_user_id_t * db_user_id, uint32_t pet_no, uint32_t iq)
{
    GEN_SQLSTR(sqlstr, "UPDATE %s SET iq = %u WHERE user_id = %u AND role_tm = %u AND server_id = %u AND pet_no = %u",
            get_table_name(db_user_id->user_id),
            iq, 
            db_user_id->user_id,
            db_user_id->role_tm, 
            db_user_id->server_id,
            pet_no);

    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_pet::try_pet_train(db_user_id_t * db_user_id, uint32_t pet_no, uint32_t * attr_array)
{
    GEN_SQLSTR(sqlstr, "UPDATE %s SET try_train_attr_1 = %u, try_train_attr_2 = %u, try_train_attr_3 = %u, try_train_attr_4 = %u WHERE user_id = %u AND role_tm = %u AND server_id = %u AND pet_no = %u",
            get_table_name(db_user_id->user_id),
            attr_array[0],
            attr_array[1],
            attr_array[2],
            attr_array[3],
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            pet_no);

    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_pet::save_pet_train(db_user_id_t * db_user_id, uint32_t pet_no)
{
    GEN_SQLSTR(sqlstr, "UPDATE %s SET train_attr_1 = try_train_attr_1, train_attr_2 = try_train_attr_2, train_attr_3 = try_train_attr_3, train_attr_4 = try_train_attr_4, try_train_attr_1 = 0, try_train_attr_2 = 0, try_train_attr_3 = 0, try_train_attr_4 = 0 WHERE user_id = %u AND role_tm = %u AND server_id = %u AND pet_no = %u",
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            pet_no);

    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}


int pea_pet::cancel_pet_train(db_user_id_t * db_user_id, uint32_t pet_no)
{
    GEN_SQLSTR(sqlstr, "UPDATE %s SET try_train_attr_1 = 0, try_train_attr_2 = 0, try_train_attr_3 = 0, try_train_attr_4 = 0 WHERE user_id = %u AND role_tm = %u AND server_id = %u AND pet_no = %u",
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            pet_no);

    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);

}

int pea_pet::get_pet_train_attr(db_user_id_t * db_user_id, uint32_t pet_no, uint32_t * train_attr, uint32_t * try_train_attr)
{
    GEN_SQLSTR(sqlstr, "SELECT train_attr_1, train_attr_2, train_attr_3, train_attr_4, try_train_attr_1, try_train_attr_2, try_train_attr_3, try_train_attr_4 FROM %s WHERE user_id = %u AND role_tm = %u AND server_id = %u AND pet_no = %u",
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            pet_no);

    STD_QUERY_ONE_BEGIN(sqlstr, DB_ERR_NOT_EXIST)
    {
        INT_CPY_NEXT_FIELD(train_attr[0]);
        INT_CPY_NEXT_FIELD(train_attr[1]);
        INT_CPY_NEXT_FIELD(train_attr[2]);
        INT_CPY_NEXT_FIELD(train_attr[3]);
        INT_CPY_NEXT_FIELD(try_train_attr[0]);
        INT_CPY_NEXT_FIELD(try_train_attr[1]);
        INT_CPY_NEXT_FIELD(try_train_attr[2]);
        INT_CPY_NEXT_FIELD(try_train_attr[3]);
    }
    STD_QUERY_ONE_END()
}



int pea_pet::do_get_all_pet(db_user_id_t * db_user_id, vector<db_pet_info_t>& pet_vec)
{
    // 按照获得的先后顺序逆序排列
    GEN_SQLSTR(sqlstr, "SELECT pet_no, pet_id, level, exp, quality, status, iq, rand_attr_1, rand_attr_2, rand_attr_3, rand_attr_4, train_attr_1, train_attr_2, train_attr_3, train_attr_4, try_train_attr_1, try_train_attr_2, try_train_attr_3, try_train_attr_4 FROM %s WHERE user_id = %u AND role_tm = %u AND server_id = %u ORDER BY pet_no DESC LIMIT %d;",
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            MAX_BAG_PET_NUM);

    STD_QUERY_WHILE_BEGIN_NEW(sqlstr, pet_vec)
    {
        PET_CPY_NEXT_FIELD(item);
    }
    STD_QUERY_WHILE_END_NEW()


}

int pea_pet::do_get_status_pet(db_user_id_t * db_user_id, uint32_t status, vector<db_pet_info_t>& pet_vec)
{
    // 按照获得的先后顺序逆序排列
    GEN_SQLSTR(sqlstr, "SELECT pet_no, pet_id, level, exp, quality, status, iq, rand_attr_1, rand_attr_2, rand_attr_3, rand_attr_4, train_attr_1, train_attr_2, train_attr_3, train_attr_4, try_train_attr_1, try_train_attr_2, try_train_attr_3, try_train_attr_4 FROM %s WHERE user_id = %u AND role_tm = %u AND server_id = %u AND status = %u ORDER BY pet_no DESC LIMIT %d;",
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            status,
            MAX_BAG_PET_NUM);

    STD_QUERY_WHILE_BEGIN_NEW(sqlstr, pet_vec)
    {
        PET_CPY_NEXT_FIELD(item);
    }
    STD_QUERY_WHILE_END_NEW()

}

int pea_pet::do_set_pet_status(db_user_id_t * db_user_id, uint32_t pet_no, uint32_t status)
{
    GEN_SQLSTR(sqlstr, "UPDATE %s SET status = %u WHERE user_id = %u AND role_tm = %u AND server_id = %u AND pet_no = %u",
            get_table_name(db_user_id->user_id),
            status,
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            pet_no);

    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_pet::do_del_pet(db_user_id_t * db_user_id, uint32_t pet_no)
{
    GEN_SQLSTR(sqlstr, "DELETE FROM %s WHERE user_id = %u AND role_tm = %u AND server_id = %u AND pet_no = %u",
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            pet_no);

    return exec_delete_sql(sqlstr, DB_SUCC);
}

int pea_pet::do_add_pet(db_user_id_t * db_user_id, uint32_t pet_id, const char * name, uint32_t status)
{
    GEN_SQLSTR(sqlstr, "INSERT INTO %s (user_id, role_tm, server_id, pet_id, pet_name, status) VALUES(%u, %u, %u, %u, \'%s\', %u)",
            get_table_name(db_user_id->user_id),
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            pet_id, 
            name,
            status);

    return exec_insert_sql(sqlstr, SUCC);
}


int pea_pet::do_set_pet_level_exp(db_user_id_t * db_user_id, uint32_t pet_no, uint32_t pet_level, uint32_t pet_exp)
{
    GEN_SQLSTR(sqlstr, "UPDATE %s SET level = %u, exp = %u WHERE user_id = %u AND role_tm = %u AND server_id = %u AND pet_no = %u",
            get_table_name(db_user_id->user_id),
            pet_level,
            pet_exp,
            db_user_id->user_id,
            db_user_id->role_tm,
            db_user_id->server_id,
            pet_no);

    return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}
