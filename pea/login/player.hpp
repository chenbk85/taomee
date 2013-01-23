/** 
 * ========================================================================
 * @file player.hpp
 * @brief 
 * @version 1.0
 * @date 2012-04-24
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_PLAYER_H_2012_04_24
#define H_PLAYER_H_2012_04_24

#include "pea_common.hpp"


class c_player
{
    public:

        char     session[SESSION_LEN];
        uint32_t ret;

    public:

        ///////////// 玩家属性 //////////////////////

        uint32_t 		server_id;
        uint32_t        role_tm;
        char        	nick[MAX_NICK_SIZE];

    public:

        /////////////////////// 装备 //////////////////////////

        item_ptr m_head;
        item_ptr m_eye;
        item_ptr m_glass;
        item_ptr m_body;
        item_ptr m_tail;
        item_ptr m_suit;


        bool init_player_equip();

        bool final_player_equip();

        item_ptr get_equip_by_pos(uint32_t body_index);

    public:

        ////////////////// 精灵 ////////////////////////////

        uint32_t pet_id;
        uint32_t pet_level;
};




#endif
