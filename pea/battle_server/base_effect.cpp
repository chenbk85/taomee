/** 
 * ========================================================================
 * @file base_effect.cpp
 * @brief 
 * @version 1.0
 * @date 2012-03-14
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "base_effect.hpp"
#include "player.hpp"



c_base_effect::c_base_effect()
{
    m_effect_id = 0;
    m_effect_type = 0;
    m_trigger_type = 0;
    m_probability = 0;
    m_effect_data = NULL;
}

c_base_effect::~c_base_effect()
{
    m_effect_data = NULL;
}


int c_base_effect::init_base_effect(const effect_data_t * p_data, struct timeval begin_time)
{
    if (NULL == p_data)
    {
        return -1;
    }
    m_effect_data = p_data;
    m_effect_id = p_data->effect_id;
    m_effect_type = p_data->effect_type;
    m_trigger_type = p_data->trigger_type;
    m_probability = p_data->trigger_rate;

    init_base_trigger_timer(begin_time, p_data->trigger_time);
    return 0;
}


// 检查是否到了触发时间
bool c_base_effect::to_next_time(struct timeval cur_time)
{
	if (m_trigger_type == EFFECT_TRIGGER_NORMAL) {
		return base_trigger_timer::to_next_time(cur_time);
	}
    return true;
}

// 重置timer
void c_base_effect::reset_timer(struct timeval cur_time)
{
    if (m_trigger_type == EFFECT_TRIGGER_NORMAL) 
    {
        base_trigger_timer::reset_timer(cur_time);
    }
}


int c_base_effect::process_effect(c_player * p_owner, struct timeval cur_time, c_player * p_creator)
{
    // 没到触发时间的跳过
    if (!to_next_time(cur_time))
    {
        return 0;
    }

    TRACE_TLOG("process effect, time: %zu, effect_type: %u, owner: %u", time(NULL), m_effect_type, p_owner->id);

    switch (m_effect_type)
    {
        case EFFECT_ADD_DEFENCE_PERCENT:
            break;
        case EFFECT_REDUCE_DEFENCE_PERCENT:
            break;
        default:
            break;

    }
    return 0;
}


int init_effect_data(xmlNodePtr root)
{
    xmlnode_for_each(root, node_1)
    {
        uint32_t effect_type = 0;
        get_xml_prop_def(effect_type, node_1, "effect_type", 0);

        xmlnode_for_each(node_1, node_2)
        {
            if (is_xmlnode_name(node_2, "trigger"))
            {
                effect_data_t * p_data = new effect_data_t;
                if (NULL == p_data)
                {
                    return -1;
                }
				p_data->effect_type = effect_type;
				get_xml_prop(p_data->effect_id, node_2, "effect_id");
				get_xml_prop(p_data->trigger_type, node_2, "trigger_type");
				get_xml_prop(p_data->trigger_rate, node_2, "trigger_rate");
				get_xml_prop(p_data->trigger_value, node_2, "trigger_value");
                // TODO parse



                if (effect_data_mgr::instance().has_object(p_data->effect_type))
                {
                    return -1;
                }

                effect_data_mgr::instance().insert_object(p_data->effect_type, p_data);
            }
        }
    }

    return 0;
}


/////////////////// c_base_effect_mgr /////////////////////


int c_base_effect_mgr::get_base_effect_count()
{
    return (int)m_data.size();
}

int c_base_effect_mgr::add_base_effect(c_base_effect * p_effect)
{
    if (NULL == p_effect)
    {
        return -1;
    }

    uint32_t effect_id = p_effect->get_base_effect_id();
    if (is_effect_exist(effect_id))
    {
        return -1;
    }

    m_data.push_back(p_effect);
    return 0;
}

int c_base_effect_mgr::del_base_effect(uint32_t effect_id)
{
    vector_for_each(m_data, it)
    {
        c_base_effect * p_effect = *it;
        if (p_effect->get_base_effect_id() == effect_id)
        {
            m_data.erase(it);
            return 0;
        }
    }

    return -1;
}

c_base_effect * c_base_effect_mgr::get_base_effect_by_id(uint32_t effect_id)
{
    vector_for_each(m_data, it)
    {
        c_base_effect * p_effect = *it;
        if (p_effect->get_base_effect_id() == effect_id)
        {
            return p_effect;
        }
    }

    return NULL;
}

bool c_base_effect_mgr::is_effect_exist(uint32_t effect_id)
{
    return (NULL != get_base_effect_by_id(effect_id));
}

// 遍历所有effect
void c_base_effect_mgr::effect_routing(c_player * p_owner, struct timeval time, c_player * p_creator)
{
    vector_for_each(m_data, it)
    {
        c_base_effect * p_effect = *it;
        if (NULL == p_effect)
        {
            continue;
        }

        uint32_t type = p_effect->get_base_effect_type();
        if (EFFECT_TRIGGER_NORMAL == type)
        {
            p_effect->process_effect(p_owner, time, p_creator);
        }
    }
}

// 遍历特定type的effect
void c_base_effect_mgr::special_effect_routing(c_player * p_owner, struct timeval time, c_player * p_creator, uint32_t trigger_type)
{
    vector_for_each(m_data, it)
    {
        c_base_effect * p_effect = *it;
        if (NULL == p_effect)
        {
            continue;
        }

        uint32_t type = p_effect->get_base_effect_trigger_type();
        if (type == trigger_type)
        {
            p_effect->process_effect(p_owner, time, p_creator);
        }
    }
}

bool c_base_effect_mgr::is_effect_type_exist(uint32_t effect_type)
{
    return (NULL != get_base_effect_by_type(effect_type));
}

c_base_effect * c_base_effect_mgr:: get_base_effect_by_type(uint32_t effect_type)
{
    vector_for_each(m_data, it)
    {
        c_base_effect * p_effect = *it;
        if (NULL == p_effect)
        {
            continue;
        }

        if (p_effect->get_base_effect_type() == effect_type)
        {
            return p_effect;
        }
    }

    return NULL;
}

