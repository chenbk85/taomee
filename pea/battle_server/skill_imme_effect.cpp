/** 
 * ========================================================================
 * @file skill_imme_effect.cpp
 * @brief 
 * @version 1.0
 * @date 2012-03-23
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "skill_imme_effect.hpp"

#include "base_effect.hpp"

int process_skill_effect(c_player * p, uint32_t effect_id, c_player * target)
{
    if (NULL == target)
    {
        return -1;
    }


    effect_data_t * p_effect_data = effect_data_mgr::instance().get_object(effect_id);
    if (NULL == p_effect_data)
    {
        return -1;
    }

    c_base_effect effect_obj;
    const timeval * tv = get_now_tv();

    effect_obj.init_base_effect(p_effect_data, *tv);
    effect_obj.process_effect(target, *tv, p);

    return 0;
}
