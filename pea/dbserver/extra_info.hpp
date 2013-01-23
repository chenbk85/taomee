#ifndef H_PEA_EXTRA_INFO_H_2012_05_17
#define H_PEA_EXTRA_INFO_H_2012_05_17



#include "CtableRoute100x10.h"
#include "proto.hpp"
#include "benchapi.h"
#include "pea_common.hpp"






class pea_extra_info : public CtableRoute100x10
{

    public:

        pea_extra_info(mysql_interface * db);

        // info_vec提供info_id，并带回对应的info_value
        // 数据库中没有对应info_id的，保持传入的info_value不变
        int query_extra_info(db_user_id_t * db_user_id, std::vector<db_extra_info_t> & info_vec);


        int update_extra_info(db_user_id_t * db_user_id, std::vector<db_extra_info_t> & info_vec);


    private:


        int do_update_extra_info(db_user_id_t * db_user_id, db_extra_info_t * p_info);


};



#endif
