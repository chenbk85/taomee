/** 
 * ========================================================================
 * @file pet.hpp
 * @brief 
 * @version 1.0
 * @date 2012-05-02
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_PET_H_2012_05_02
#define H_PET_H_2012_05_02

#include "pea_common.hpp"
#include "proto.hpp"

typedef class Player player_t;

class c_pet
{

    public:

        c_pet(player_t * p);

        int init(btl_pet_info_t * p_info);

	private:

		// 精灵归属的玩家
		player_t * m_player;

	public:

		inline void set_player(player_t * p_player)
		{
			m_player = p_player;
		}

		inline player_t * get_player()
		{
			return m_player;
		}

    public:


        // id
        uint32_t m_id;
        // 编号
        uint32_t m_no;

        // 等级
        uint32_t m_level;
        // 经验
        uint32_t m_exp;
        // 经验等级表
        base_exp2level_calculator * m_exp2level_calc;
};


#endif
