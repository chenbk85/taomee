/** 
 * ========================================================================
 * @file buff.cpp
 * @brief 
 * @version 1.0
 * @date 2012-03-15
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "buff.hpp"


using namespace taomee;


int init_buff_data(xmlNodePtr root)
{
    xmlnode_for_each(root, node_1)
    {
        if (!is_xmlnode_name(node_1, "buff"))
        {
            continue;
        }

        uint32_t effect_id = 0;
        uint32_t effect_type = 0;

        buff_data_t * p_data = new buff_data_t;
        if (NULL == p_data)
        {
            return -1;
        }

        get_xml_prop_def(p_data->buff_id, node_1, "buff_id", 0);
        get_xml_prop_def(p_data->buff_type, node_1, "buff_type", 1);
        get_xml_prop_def(p_data->end_type, node_1, "buff_end_type", 1);
        get_xml_prop_def(p_data->trigger_times, node_1, "buff_trigger_times", 0);
        get_xml_prop_def(p_data->turn_times, node_1, "buff_turn_times", 0);
        get_xml_prop_def(p_data->category, node_1, "buff_category", 0);
        get_xml_prop_def(p_data->level, node_1, "buff_priority", 0);
        get_xml_prop_def(p_data->icon, node_1, "icon", 0);

        // TODO 读取其他数据

        xmlnode_for_each(node_1, node_2)
        {
            get_xml_prop_def(effect_id, node_2, "effect_id", 0);
            get_xml_prop_def(effect_type, node_2, "effect_type", 0);
            if (0 == effect_id || 0 == effect_type)
            {
                return -1;
            }


            if (is_xmlnode_name(node_2, "trigger"))
            {
                effect_data_t * p_data = new effect_data_t;
                if (NULL == p_data)
                {
                    return -1;
                }

                // TODO parse
                
                if (effect_data_mgr::instance().has_object(p_data->effect_id))
                {
                    return -1;
                }

                effect_data_mgr::instance().insert_object(p_data->effect_id, p_data);
            }
        }
    }

    return 0;
}


c_buff * create_buff(uint32_t buff_id)
{
    buff_data_t * p_buff_data = buff_data_mgr::instance().get_object(buff_id);
    if (NULL == p_buff_data)
    {
        return NULL;
    }

    if (0 == p_buff_data->effect.size())
    {
        return NULL;
    }

    c_buff * p_buff = new c_buff(p_buff_data);
    if (NULL == p_buff)
    {
        return NULL;
    }

    p_buff->set_end_type(p_buff_data->end_type);
    p_buff->set_trigger_times(p_buff_data->trigger_times);

    struct timeval begin_time = *get_now_tv();
    vector_for_each(p_buff_data->effect, it)
    {
        uint32_t effect_id = *it;
        effect_data_t * p_effect_data = effect_data_mgr::instance().get_object(effect_id);
        if (NULL == p_effect_data)
        {
            return NULL;
        }

        c_base_effect * p_effect = new c_base_effect();
        p_effect->init_base_effect(p_effect_data, begin_time);

        p_buff->add_base_effect(p_effect);
    }

    //p_buff->init_base_duration_timer(begin_time, p_buff_data->duration_time);
    p_buff->init_base_duration_timer(begin_time, 0);

    return p_buff;
}


void destroy_buff(c_buff * p_buff)
{
    if (NULL != p_buff)
    {
        delete p_buff;
    }
}
