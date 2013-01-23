/** 
 * ========================================================================
 * @file pea_pet.hpp
 * @brief 
 * @version 1.0
 * @date 2012-04-10
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_PEA_PET_H_2012_04_10
#define H_PEA_PET_H_2012_04_10


#include <libtaomee++/proto/proto_base.h>
#include "CtableRoute100x10.h"
#include "proto.hpp"
#include "benchapi.h"
#include "pea_common.hpp"



class pea_pet : public CtableRoute100x10
{
    public:

        pea_pet(mysql_interface * db);

        int query_pet(db_user_id_t * db_user_id, std::vector<db_pet_info_t> & vec);

        int query_fight_pet(Cmessage * c_in, Cmessage * c_out);

        int set_fight_pet(db_user_id_t * db_user_id, uint32_t des_pet_no, uint32_t src_pet_no);

        int set_assist_pet(db_user_id_t * db_user_id, uint32_t des_pet_no, uint32_t src_pet_no);


        int do_add_pet(db_user_id_t * db_user_id, uint32_t pet_id, const char * name, uint32_t status);

        
        int set_pet_iq(db_user_id_t * db_user_id, uint32_t pet_no, uint32_t iq);

        int try_pet_train(db_user_id_t * db_user_id, uint32_t pet_no, uint32_t * attr_array);

        int save_pet_train(db_user_id_t * db_user_id, uint32_t pet_no);
        int cancel_pet_train(db_user_id_t * db_user_id, uint32_t pet_no);
        int get_pet_train_attr(db_user_id_t * db_user_id, uint32_t pet_no, uint32_t * train_attr, uint32_t * try_train_attr);

        int get_fight_pet(db_user_id_t * db_user_id, db_pet_info_t * p_pet);

        int get_assist_pet(db_user_id_t * db_user_id, vector<db_pet_info_t> & pet_vec);

        int get_bag_pet(db_user_id_t * db_user_id, vector<db_pet_info_t> & pet_vec);

        int do_get_all_pet(db_user_id_t * db_user_id, vector<db_pet_info_t>& pet_vec);

        int do_get_status_pet(db_user_id_t * db_user_id, uint32_t status, vector<db_pet_info_t>& pet_vec);

        int do_set_pet_status(db_user_id_t * db_user_id, uint32_t pet_no, uint32_t status);

        int do_del_pet(db_user_id_t * db_user_id, uint32_t pet_no);


        int do_set_pet_level_exp(db_user_id_t * db_user_id, uint32_t pet_no, uint32_t pet_level, uint32_t pet_exp);
};

#endif
