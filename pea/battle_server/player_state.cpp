/** 
 * ========================================================================
 * @file player_state.cpp
 * @brief 
 * @version 1.0
 * @date 2012-03-19
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



#include "player_state.hpp"
#include "player.hpp"
#include "cli_proto.hpp"

/////////////////////////////////////////////////////
// state
//

c_player_state_mrg::c_player_state_mrg(c_player* p)
{
	owner = p;
}

c_player_state_mrg::~c_player_state_mrg()
{
	final_player_status();
}

int c_player_state_mrg::init_player_status()
{
	player_buff_mrg = new c_player_buff_mrg(owner);
	merge_attr = new obj_attr(*(owner->attr));
    return 0;
}

int c_player_state_mrg::final_player_status()
{
	if (player_buff_mrg) {
		delete player_buff_mrg;
		player_buff_mrg = 0;
	}
	if (merge_attr) {
		delete merge_attr;
		merge_attr = 0;
	}
    return 0;
}

uint32_t c_player_state_mrg::calc_merge_attr() 
{
	merge_attr->copy(*(owner->attr));
	merge_attr->accumulate(*(player_buff_mrg->attr_buff_trim));
	merge_attr->decrease(*(player_buff_mrg->attr_debuff_trim));
	//add other attr
	//... ...

	//calc atk etc.
	merge_attr->calc();
	return 0;
}

void c_player_state_mrg::inc_player_hp(uint32_t value) {
	if(merge_attr->id2value(OBJ_ATTR_HP) + value > merge_attr->id2value(OBJ_ATTR_MAX_HP_LIMIT)) {
		merge_attr->set_attr(OBJ_ATTR_HP, merge_attr->id2value(OBJ_ATTR_MAX_HP_LIMIT)); 
	} else {
		merge_attr->add_attr(OBJ_ATTR_HP, value);	
	}
}

void c_player_state_mrg::dec_player_hp(uint32_t value) {
	if(merge_attr->id2value(OBJ_ATTR_HP) <= value) {
		merge_attr->set_attr(OBJ_ATTR_HP, 0);
		owner->set_dead(true);
		if(owner->btl) {
			owner->btl->del_player_from_queue(owner->id);
		}
	} else {
		merge_attr->add_attr(OBJ_ATTR_HP, -value);	
	}
}


/////////////////////////////////////////////////////
// buff
//

c_player_buff_mrg::c_player_buff_mrg(c_player *p)
{
	owner = p;
	init_player_buff();
}

c_player_buff_mrg::~c_player_buff_mrg()
{
	final_player_buff();
}

int c_player_buff_mrg::init_player_buff()
{
	damage_trim_ = 1.0;
	
    m_buff.clear();
	attr_buff_trim = new obj_attr();
	attr_debuff_trim = new obj_attr();
    return 0;
}


int c_player_buff_mrg::final_player_buff()
{
    vector_for_each(m_buff, it)
    {
        c_buff * p_buff = *it;
        if (NULL != p_buff)
        {
            destroy_buff(p_buff);
        }
    }

    m_buff.clear();

	if (attr_buff_trim) {
		delete attr_buff_trim;
		attr_buff_trim = 0;
	}
	if (attr_debuff_trim) {
		delete attr_debuff_trim;
		attr_debuff_trim = 0;
	}
    return 0;
}

void
c_player_buff_mrg::noti_buf_event_to_map(uint16_t buf_id, uint8_t buf_type, uint8_t flag)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, owner->id, idx);
	pack(pkgbuf, buf_id, idx);
	pack(pkgbuf, flag, idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_players_buff_event_noti_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	
	owner->btl->broadcast_to_all(pkgbuf, idx);
	TRACE_LOG("buff change [%u %u %u %u]", owner->id, buf_id, buf_type, flag);
}

void c_player_buff_mrg::notify_add_buff_to_player(c_buff * p_buff)
{
    if (NULL == p_buff)
    {
        return;
    }

	noti_buf_event_to_map(p_buff->get_buff_id(), p_buff->get_buff_type(), 1);
}



void c_player_buff_mrg::notify_del_buff_to_player(c_buff * p_buff)
{
    if (NULL == p_buff)
    {
        return;
    }

}


bool c_player_buff_mrg::is_effect_exist(uint32_t effect_type)
{
    bool ret;
    vector_for_each(m_buff, it)
    {
        c_buff * p_buff = *it;
        ret = p_buff->is_effect_type_exist(effect_type);
        if (ret)
        {
            return ret;
        }
    }

    return false;
}

int c_player_buff_mrg::add_player_buff(uint32_t buff_id, uint32_t creator_id, uint32_t duration_time, bool notify)
{
    // 存在免疫一切buff的buff
    if (is_effect_exist(EFFECT_IMMUNITY_BUFF))
    {
        return -1;
    }


    TRACE_TLOG("user: %u, add buff, id: %u, cur time: %zu", owner->id, buff_id, time(NULL));

    c_buff * p_buff = get_player_buff(buff_id);
    if (NULL != p_buff)
    {
        // 重置
        p_buff->reset();
    }

    p_buff = create_buff(buff_id);
    if (NULL == p_buff)
    {
        return -1;
    }


    // 效果免疫一切debuff
    if (is_effect_exist(EFFECT_IMMUNITY_DEBUFF) && p_buff->get_buff_type() == BUFF_TYPE_DEBUFF)
    {
        destroy_buff(p_buff);
        return -1;

    }


    if (duration_time)
    {
        p_buff->set_duration_time(duration_time);
    }

    p_buff->set_creator_id(creator_id);

    // TODO 处理和已有buff的叠加关系
    if (0 != p_buff->get_buff_category())
    {
        vector_for_each(m_buff, it)
        {
            c_buff * p_player_buff = *it;
            if (NULL == p_player_buff)
            {
                continue;
            }

            if (p_buff->get_buff_category() == p_player_buff->get_buff_category())
            {
                if (p_buff->get_buff_level() == p_player_buff->get_buff_level())
                {
                    // 重置已有的buff
                    TRACE_TLOG("user: %u, buff replace, old buff: %u, new buff: %u", owner->id, p_player_buff->get_buff_id(), p_buff->get_buff_id());
                    del_player_buff(p_buff, true);
                }
                else if (p_buff->get_buff_level() > p_player_buff->get_buff_level())
                {
                    // 覆盖已有的buff
                    TRACE_TLOG("user: %u, buff reset, old buff: %u, new buff: %u", owner->id, p_player_buff->get_buff_id(), p_buff->get_buff_id());
                    p_player_buff->reset();
                    destroy_buff(p_buff);
                    return 0;
                }
                else
                {
                    // 不添加该buff
                    TRACE_TLOG("user: %u, buff NOT replace, old buff: %u, new buff: %u", owner->id, p_player_buff->get_buff_id(), p_buff->get_buff_id());
                    destroy_buff(p_buff);
                    return 0;
                }
            }
        }
    }

    TRACE_TLOG("user: %u, get buff, id: %u, cur time: %zu", owner->id, buff_id, time(NULL));

    return add_player_buff(p_buff, notify);

}


int c_player_buff_mrg::add_player_buff(c_buff * p_buff, bool notify)
{
    if (NULL == p_buff)
    {
        return -1;
    }

    m_buff[p_buff->get_buff_id()] = p_buff;

    calc_player_buff_additional_attribute();

    if (notify)
    {
        notify_add_buff_to_player(p_buff);

    }

    // 其他处理
    return 0;
}

int c_player_buff_mrg::del_player_buff(c_buff * p_buff, bool notify)
{
    if (NULL != p_buff)
    {
        if (notify)
        {
            notify_del_buff_to_player(p_buff);
        }
        vector_while_each(m_buff, it)
        {
            if (p_buff == *it)
            {
                it = m_buff.erase(it);
            }
            else
            {
                it++;
            }
        }

        destroy_buff(p_buff);
    }

    return 0;
}


int c_player_buff_mrg::del_player_buff(uint32_t buff_id, bool notify)
{
    c_buff * p_buff = get_player_buff(buff_id);
    if (NULL == p_buff)
    {
        return -1;
    }
    return del_player_buff(p_buff, notify);
}


int c_player_buff_mrg::del_player_all_buff()
{
    return final_player_buff();
}


int c_player_buff_mrg::del_player_all_buff(uint32_t buff_type, bool notify)
{
    vector_while_each(m_buff, it)
    {
        c_buff * p_buff = *it;
        if (NULL != p_buff)
        {
            if (p_buff->get_buff_type() == buff_type)
            {
                if (notify)
                {
                    notify_del_buff_to_player(p_buff);
                }
                destroy_buff(p_buff);
                it = m_buff.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    return 0;
}


int c_player_buff_mrg::del_random_player_buff(uint32_t buff_type, uint32_t max_count)
{
    return 0;
}

int c_player_buff_mrg::reset_player_buff(uint32_t buff_id)
{
    c_buff * p_buff = get_player_buff(buff_id);
    if (NULL == p_buff)
    {
        return -1;
    }

    p_buff->reset();
    return 0;
}


bool c_player_buff_mrg::is_player_buff_exist(uint32_t buff_id)
{
    if (NULL != get_player_buff(buff_id))
    {
        return true;
    }
    else
    {
        return false;
    }
}


c_buff * c_player_buff_mrg::get_player_buff(uint32_t buff_id)
{
    vector_for_each(m_buff, it)
    {
        c_buff * p_buff = *it;
        if (p_buff->get_buff_id() == buff_id)
        {
            return p_buff;
        }
    }

    return NULL;
}


void c_player_buff_mrg::special_buff_routing(struct timeval cur_time, uint32_t type)
{
    vector_while_each(m_buff, it)
    {
        c_buff * p_buff = *it;
        if (NULL == p_buff)
        {
            it++;
            continue;
        }

        if (p_buff->is_buff_end(cur_time))
        {
            TRACE_TLOG("user: %u, buff %u end, time: %zu", owner->id, p_buff->get_buff_id(), get_now_tv()->tv_sec);

            notify_del_buff_to_player(p_buff);
            it = m_buff.erase(it);
            destroy_buff(p_buff);

            calc_player_buff_additional_attribute();
            continue;
        }

        uint32_t creator_id = p_buff->get_creator_id();
        c_player * p_creator = NULL;
        if (0 != creator_id)
        {
            p_creator = owner->btl->get_player_by_id(creator_id);
        }

        p_buff->effect_routing(owner, cur_time, p_creator);
        it++;
    }
}

void c_player_buff_mrg::player_passive_buff_trigger(struct timeval cur_time, uint32_t trigger_type)
{
    if (! (trigger_type != EFFECT_TRIGGER_BE_ATK || trigger_type != EFFECT_TRIGGER_DAMAGE))
    {
        return;
    }


    vector_for_each(m_buff, it)
    {
        c_buff * p_buff = *it;
        if (NULL == p_buff)
        {
            continue;
        }

        p_buff->special_effect_routing(owner, cur_time, owner, trigger_type);
        if (p_buff->get_end_type() & BUFF_END_TIMES)
        {
            p_buff->dec_buff_times();
        }

    }

}

uint32_t c_player_buff_mrg::get_player_base_additional_attr_by_effect(c_base_effect* p_base_effect, uint32_t type)
{
	if (!p_base_effect) {
		return 0;
	}
	const effect_data_t* p_data = p_base_effect->get_effect_data();
	uint32_t value = 0;
	if (p_base_effect->get_base_effect_id() == type) {
	if (p_data->trigger_value != 0) {
			value += p_data->trigger_value;
		} else {
			value += owner->player_state_mrg->get_attr((type - OBJ_ATTR_HP) / 2) * p_data->trigger_percent / 100;
		}
	}
	return value;
}

uint32_t c_player_buff_mrg::get_base_attr_by_type(c_buff* p_buff, uint32_t type)
{
	if(p_buff == NULL){
		return 0;
	}
	uint32_t value = 0;

	std::vector<c_base_effect *>::iterator pItr = p_buff->m_data.begin();
	for(; pItr != p_buff->m_data.end(); ++pItr)
	{
		value += get_player_base_additional_attr_by_effect(*pItr, type);
	}
	return value;
}

uint32_t c_player_buff_mrg::get_addition_damage_by_buff(c_buff* p_buff)
{
	if(p_buff == NULL){
		return 0;
	}
	float value = 0;

	std::vector<c_base_effect *>::iterator pItr = p_buff->m_data.begin();
	for(; pItr != p_buff->m_data.end(); ++pItr)
	{
		value += get_addition_damage_by_effect(*pItr);
	}
	return value;
}
float c_player_buff_mrg::get_addition_damage_by_effect(c_base_effect* p_base_effect)
{
	if (!p_base_effect) {
		return 0;
	}
	const effect_data_t* p_data = p_base_effect->get_effect_data();
	float value = 0;
	if (p_base_effect->get_base_effect_id() == EFFECT_ADD_DAMAGE) {
		value += 1 * p_data->trigger_percent/100.0;
	}
	return value;
}

// player additional attribute
int c_player_buff_mrg::calc_player_buff_additional_attribute()
{
	//init
	attr_buff_trim->reset();
	attr_debuff_trim->reset();

	//renew
	std::vector<c_buff*>::iterator pItr = m_buff.begin();
	for(; pItr != m_buff.end(); ++pItr ) {

		//for base attr
		for (uint32_t i = 0; i < OBJ_ATTR_MAX; i++) {
			attr_buff_trim->add_attr(i, get_base_attr_by_type(*pItr, OBJ_ATTR_HP + i * 2));
			attr_debuff_trim->add_attr(i, get_base_attr_by_type(*pItr, OBJ_ATTR_HP + i * 2 + 1));
		}
		//for other

		damage_trim_ += get_addition_damage_by_buff(*pItr);
		
	}
	return 0;
}



