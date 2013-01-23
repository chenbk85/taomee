/** 
 * ========================================================================
 * @file db_pet.hpp
 * @brief 
 * @version 1.0
 * @date 2012-04-11
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_DB_PET_H_2012_04_11
#define H_DB_PET_H_2012_04_11


#include "pea_common.hpp"
#include "dbproxy.hpp"
#include "pet.hpp"

class player_t;


int db_get_pet(player_t * p);

int db_set_fight_pet(player_t * p, uint32_t des_pet_no, uint32_t src_pet_no);

int db_set_assist_pet(player_t * p, uint32_t des_pet_no, uint32_t src_pet_no);

int db_try_pet_train(player_t * p, uint32_t pet_no, uint32_t value_count, uint32_t * train_value, pet_train_consume_data_t * p_consume_data);

int db_set_pet_train(player_t * p, uint32_t pet_no, uint32_t set_flag);

int db_update_pet_gift(player_t * p, uint32_t pet_no, pet_gift_consume_data_t * p_consume_data, db_extra_info_t * p_extra_info, std::vector<db_update_pet_gift_t> & update_gift);

int db_set_pet_iq(player_t * p, uint32_t pet_no, uint32_t iq, pet_iq_data_t * p_data);


int db_pet_merge(
        player_t * p,
        uint32_t base_pet_no,
        uint32_t base_pet_level,
        uint32_t base_pet_exp,
        std::vector<uint32_t> & del_pet_no,
        std::vector<db_del_item_request_t> & del_items,
        std::vector<db_add_item_request_t> & add_items);
#endif
