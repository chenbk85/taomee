/** 
 * ========================================================================
 * @file pet.cpp
 * @brief 
 * @version 1.0
 * @date 2012-05-02
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "pet.hpp"
#include "player.hpp"


c_pet::c_pet(player_t * p)
{
    set_player(p);

}


int c_pet::init(btl_pet_info_t * p_info)
{
    m_id = p_info->pet_id;
    m_no = p_info->pet_no;

    m_level = p_info->level;
    m_exp = p_info->exp;

    c_pet_data * p_data = pet_data_mgr::instance().get_object(m_id);
    if (NULL == p_data)
    {
        return -1;
    }
    m_exp2level_calc = p_data->get_exp2level_calc();


    return 0;
}
