/** 
 * ========================================================================
 * @file pet.cpp
 * @brief 
 * @version 1.0
 * @date 2012-01-18
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "pet.hpp"
#include "player.hpp"
#include "proto.hpp"
#include "db_pet.hpp"
#include "cli_proto.hpp"


using namespace std;
using namespace taomee;

int init_player_pet(player_t * p)
{
    p->pet.clear();
    p->fight_pet = NULL;
    p->assist_pet.clear();

    return 0;
}


int final_player_pet(player_t * p)
{
    vector_for_each(p->pet, it)
    {
        c_pet * p_pet = *it;
        p_pet->uninit();
        delete p_pet;
    }

    p->pet.clear();

    p->fight_pet = NULL;
    p->assist_pet.clear();

    return 0;
}

int is_pet_bag_full(player_t * p_player)
{
    return (MAX_BAG_PET_NUM >= p_player->pet.size());
}

bool is_fight_pet(player_t * p, uint32_t pet_no)
{
    if (NULL == p->fight_pet)
    {
        return false;
    }

    return (p->fight_pet->m_no == pet_no);
}


bool is_assist_pet(player_t * p, uint32_t pet_no)
{
    vector_for_each(p->assist_pet, it)
    {
        c_pet * p_pet = *it;
        if (p_pet->m_no == pet_no)
        {
            return true;
        }
    }

    return false;
}


int add_player_pet(player_t * p, c_pet * p_pet)
{
    if (NULL == p_pet)
    {
        return -1;
    }

    vector_for_each(p->pet, it)
    {
        if (p_pet->m_no == (*it)->m_no)
        {
            return 0;
        }
        else if (p_pet->m_no > (*it)->m_no)
        {
            p->pet.insert(it, p_pet);
            return 0;
        }
    }
    p->pet.push_back(p_pet);

    return 0;

}


int del_player_pet(player_t * p, c_pet * p_pet)
{
    vector_for_each(p->pet, it)
    {
        if (p_pet == *it)
        {
            p->pet.erase(it);
            return 0;
        }
    }
    return 0;
}


c_pet * find_player_pet(player_t * p, uint32_t pet_no)
{
    vector_for_each(p->pet, it)
    {
        c_pet * p_pet = *it;
        if (pet_no == p_pet->m_no)
        {
            return p_pet;
        }
    }

    return NULL;
}


int set_fight_pet(player_t * p, c_pet * p_pet)
{
    p->fight_pet = p_pet;
	p->init_player_attr();
    return 0;
}


int calc_assist_attr(player_t * p)
{
    if (NULL == p->fight_pet)
    {
        return -1;
    }

    c_pet * p_fight_pet = p->fight_pet;

    for (uint32_t i = 0; i < PET_ASSIST_ATTR_NUM; i++)
    {
        p_fight_pet->m_assist_value[i] = 0;
    }

    std::vector<uint32_t> attr_vec;
    // 四个辅助属性
    attr_vec.push_back(OBJ_ATTR_MAGIC);
    attr_vec.push_back(OBJ_ATTR_PHYSIQUE);
    attr_vec.push_back(OBJ_ATTR_AGILITY);
    attr_vec.push_back(OBJ_ATTR_LUCK);

    vector_for_each(p->assist_pet, it)
    {
        c_pet * p_pet = *it;
        for (uint32_t i = 0; i < PET_ASSIST_ATTR_NUM; i++)
        {
            p_fight_pet->m_assist_value[i] += p_pet->m_attr.id2value(attr_vec[i]);
        }
    }

    for (uint32_t i = 0; i < PET_ASSIST_ATTR_NUM; i++)
    {
        p_fight_pet->m_assist_value[i] *= 0.05;
        *p_fight_pet->m_assist_attr.id2p(attr_vec[i]) = p_fight_pet->m_assist_value[i];
    }


    p_fight_pet->calc_attr();

    return 0;
}



int add_assist_pet(player_t * p, c_pet * p_pet)
{

    if (NULL == p_pet)
    {
        return -1;
    }

    vector_for_each(p->assist_pet, it)
    {
        if (p_pet->m_no == (*it)->m_no)
        {
            return 0;
        }
        else if (p_pet->m_no > (*it)->m_no)
        {
            p->assist_pet.insert(it, p_pet);
            return 0;
        }
    }

    p->assist_pet.push_back(p_pet);

    calc_assist_attr(p);

    return 0;
}


int del_assist_pet(player_t * p, c_pet * p_pet)
{
    if (NULL == p_pet)
    {
        return -1;
    }

    vector_for_each(p->assist_pet, it)
    {
        if (p_pet->m_no == (*it)->m_no)
        {
            p->assist_pet.erase(it);
            return 0;
        }
    }

    calc_assist_attr(p);

    return 0;

}

void print_pet_log(player_t * p)
{
    if (p->fight_pet)
    {
        INFO_TLOG("user: %u, fight pet: %u", p->id, p->fight_pet->m_no);
    }
    else
    {
        INFO_TLOG("user: %u, fight pet: %u", p->id, 0);
    }

    vector_for_each(p->assist_pet, it)
    {
        INFO_TLOG("user: %u, assist pet: %u", p->id, (*it)->m_no);
    }
}


//////////////////////////////////////////////////////////////////////
// 精灵的成员函数
//

/////////////////////// c_pet_gift /////////////////////////


int c_pet_gift::init(db_pet_info_t * p_info)
{

    c_pet_data * p_pet_data = pet_data_mgr::instance().get_object(p_info->pet_id);
    if (NULL == p_pet_data)
    {
        return -1;
    }

    c_pet_gift_data * p_gift_data = p_pet_data->get_gift_data(p_info->level);
    if (NULL == p_gift_data)
    {
        return -1;
    }

    m_max_count = p_gift_data->m_gift_count;

    for (uint32_t i = 0; i < p_info->gift.size() && i < m_max_count; i++)
    {
        db_pet_gift_info_t * p_gift_info = &p_info->gift[i];
        pet_gift_t gift;
        gift.gift_no = p_gift_info->gift_no;
        gift.gift_id = p_gift_info->gift_id;
        gift.gift_level = p_gift_info->gift_level;

        // XXX

        m_data.push_back(gift);

    }
    return 0;
}


int c_pet_gift::export_info(std::vector<db_pet_gift_info_t> & vec)
{
    for (uint32_t i = 0; i < get_count(); i++)
    {
        db_pet_gift_info_t info;
        pet_gift_t * p_gift = &m_data[i];
        info.gift_no = p_gift->gift_no;;
        info.gift_id = p_gift->gift_id;
        info.gift_level = p_gift->gift_level;
        vec.push_back(info);
    }
    return 0;
}


pet_gift_t * c_pet_gift::get_gift(uint32_t gift_no)
{
    for (uint32_t i = 0; i < get_count(); i++)
    {
        pet_gift_t * p_gift = &(m_data[i]);
        if (p_gift->gift_no == gift_no)
        {
            return p_gift;
        }

    }

    return NULL;
}



// XXX 未完成
int c_pet_gift::calc_attr(c_pet_attr * p_attr)
{
    c_pet_attr attr;
    for (uint32_t i = 0; i < get_count(); i++)
    {
        attr.reset();
        // const pet_gift_t * p_gift = &(m_data[i]);
        // p_gift->p_info->calc_attr(p_gift->gift_level, &attr);
        p_attr->accumulate(attr);
    }

    return 0;

}

int c_pet_gift::update(db_update_pet_gift_t * p_info)
{
    uint32_t index = 0;
    for (uint32_t i = 0; i < get_count(); i++)
    {
        pet_gift_t * p_gift = &m_data[i];
        if (p_gift->gift_no == p_info->gift_no)
        {
            p_gift->gift_id = p_info->gift_id;
            p_gift->gift_level = p_info->gift_level;
            return 0;
        }
        else if (p_gift->gift_no < p_info->gift_no)
        {
            index++;
        }
    }
    
    // 新的天赋
    if (is_full())
    {
        return -1;
    }

    pet_gift_t gift;
    gift.gift_no = p_info->gift_no;
    gift.gift_id = p_info->gift_id;
    gift.gift_level = p_info->gift_level;
    m_data.insert(m_data.begin() + index, gift);

    return 0;
}

///////////////////////// c_pet //////////////////////////////////
//



c_pet::c_pet(player_t * p)
{

    set_player(p);

}


int c_pet::init(db_pet_info_t * p_info)
{
    if (NULL == p_info)
    {
        return -1;
    }


    m_id = p_info->pet_id;
    m_no = p_info->pet_no;
    m_level = p_info->level;
    m_exp = p_info->exp;
    m_quality = p_info->quality;
    m_status = p_info->status;
    m_iq = p_info->iq;



    m_pet_data = pet_data_mgr::instance().get_object(m_id);

    if (NULL == m_pet_data)
    {
        return -1;
    }

    c_pet_attr * p_base_attr = m_pet_data->get_base_attr(m_level);
    if (NULL == p_base_attr)
    {
        return -1;
    }

    INFO_TLOG("user: %u, pet: %u, base magic: %u", m_player->id, m_id, p_base_attr->id2value(OBJ_ATTR_MAGIC));
    INFO_TLOG("user: %u, pet: %u, base physique: %u", m_player->id, m_id, p_base_attr->id2value(OBJ_ATTR_PHYSIQUE));
    INFO_TLOG("user: %u, pet: %u, base agility: %u", m_player->id, m_id, p_base_attr->id2value(OBJ_ATTR_AGILITY));
    INFO_TLOG("user: %u, pet: %u, base luck: %u", m_player->id, m_id, p_base_attr->id2value(OBJ_ATTR_LUCK));

    m_assist_attr.reset();

    m_base_attr.copy(*p_base_attr);

    m_exp2level_calc = m_pet_data->get_exp2level_calc();

    // 技能
    m_skill.m_skill_1 = m_pet_data->get_skill_data()->m_skill_1;
    m_skill.m_skill_2 = m_pet_data->get_skill_data()->m_skill_2;
    m_skill.m_skill_3 = m_pet_data->get_skill_data()->m_skill_3;
    m_skill.m_uni_skill = m_pet_data->get_skill_data()->m_uni_skill;

    // 随机成长
    m_rand_calc = m_pet_data->get_rand_calc(m_level);
    for (uint32_t i = 0; i < PET_RAND_ATTR_NUM; i++)
    {
        m_rand_value[i] = p_info->rand_attr[i];
    }

    m_rand_calc->calc_attr(&m_rand_attr, m_rand_value);


    // 精灵培养
    m_train_calc = m_pet_data->get_train_calc(m_level);
    for (uint32_t i = 0; i < PET_TRAIN_ATTR_NUM; i++)
    {
        m_train_value[i] = p_info->train_attr[i];
        m_try_train_value[i] = p_info->try_train_attr[i];
    }
    m_train_calc->calc_attr(&m_train_attr, m_train_value);

    // 天赋
    m_gift.init(p_info);



    calc_attr();

    INFO_TLOG("user: %u, pet: %u, total magic: %u", m_player->id, m_id, m_attr.id2value(OBJ_ATTR_MAGIC));
    INFO_TLOG("user: %u, pet: %u, total physique: %u", m_player->id, m_id, m_attr.id2value(OBJ_ATTR_PHYSIQUE));
    INFO_TLOG("user: %u, pet: %u, total agility: %u", m_player->id, m_id, m_attr.id2value(OBJ_ATTR_AGILITY));
    INFO_TLOG("user: %u, pet: %u, total luck: %u", m_player->id, m_id, m_attr.id2value(OBJ_ATTR_LUCK));

    return 0;
}


int c_pet::uninit()
{
    return 0;
}



int c_pet::export_info(simple_pet_info_t * p_info)
{
    p_info->pet_id = m_id;
    p_info->pet_no = m_no;
    p_info->level = m_level;
    p_info->exp = m_exp;
    p_info->quality = m_quality;
    p_info->status = m_status;

    return 0;

}



int c_pet::export_info(btl_pet_info_t * p_info)
{
    p_info->pet_no = m_no;
    p_info->pet_id = m_id;
    p_info->level = m_level;
    p_info->exp = m_exp;
    p_info->status = m_status;
    p_info->quality = m_quality;
	p_info->width = m_pet_data->width;
	p_info->height = m_pet_data->height;
    // p_info->quality = m_quality;
    // for (uint32_t i = 0; i < PET_RAND_ATTR_NUM; i++)
    // {
        // p_info->rand_attr[i] = m_rand_value[i];
    // }

    return 0;
}

int c_pet::export_info(detail_pet_info_t * p_info)
{
    p_info->pet_no = m_no;
    p_info->pet_id = m_id;
    p_info->level = m_level;
    p_info->exp = m_exp;
    p_info->max_exp = m_exp2level_calc->get_level_exp(m_level);
    p_info->quality = m_quality;
    p_info->iq = m_iq;
    p_info->status = m_status;

    p_info->skill_1 = m_skill.m_skill_1;
    p_info->skill_2 = m_skill.m_skill_2;
    p_info->skill_3 = m_skill.m_skill_3;
    p_info->uni_skill = m_skill.m_uni_skill;

    p_info->attr_value[0] = m_attr.id2value(OBJ_ATTR_MAGIC);
    p_info->attr_value[1] = m_attr.id2value(OBJ_ATTR_PHYSIQUE);
    p_info->attr_value[2] = m_attr.id2value(OBJ_ATTR_AGILITY);
    p_info->attr_value[3] = m_attr.id2value(OBJ_ATTR_LUCK);

    m_rand_calc->get_max_value_list(m_iq, p_info->rand_attr_max);

    p_info->train_attr[0] = m_train_attr.id2value(OBJ_ATTR_MAGIC);
    p_info->train_attr[1] = m_train_attr.id2value(OBJ_ATTR_PHYSIQUE);
    p_info->train_attr[2] = m_train_attr.id2value(OBJ_ATTR_AGILITY);
    p_info->train_attr[3] = m_train_attr.id2value(OBJ_ATTR_LUCK);

    for (uint32_t i = 0; i < PET_TRAIN_ATTR_NUM; i++)
    {
        p_info->try_train_attr[i] = m_try_train_value[i];
    }

    p_info->max_gift_count = m_gift.get_max_count();

    m_gift.export_info(p_info->gift);

    return 0;
}

int c_pet::calc_attr()
{
    
    m_attr.reset();


    // 累计各种属性
    m_attr.accumulate(m_base_attr);
    m_attr.accumulate(m_rand_attr);
    m_attr.accumulate(m_train_attr);
    m_attr.accumulate(m_assist_attr);
    // 计算出最终值
    //m_attr.calc();

    if (is_assist_pet(m_player, m_no))
    {
        // 更新对出战精灵的辅助经验加成
        calc_assist_attr(m_player);
    }
    return 0;
}

int c_pet::calc_gift_attr()
{

    m_gift_attr.reset();

    // TODO 属性的计算

    return 0;

}

int c_pet::calc_badge_attr()
{

    m_badge_attr.reset();

    // TODO 属性的计算

    return 0;

}


// 确认培养数据
int c_pet::confirm_train_grow()
{
    for (uint32_t i = 0; i < PET_TRAIN_ATTR_NUM; i++)
    {
        m_train_value[i] = m_try_train_value[i];
        m_try_train_value[i] = 0;
    }
    m_train_calc->calc_attr(&m_train_attr, m_train_value);
    calc_attr();

    // XXX 通知其他模块更新数据


    return 0;
}



// 取消培养
int c_pet::cancel_train_grow()
{
    for (uint32_t i = 0; i < PET_TRAIN_ATTR_NUM; i++)
    {
        m_try_train_value[i] = 0;
    }

    return 0;
}


// 获得经验
int c_pet::gain_exp(uint32_t add_exp)
{
    if (m_exp >= m_exp2level_calc->get_max_exp())
    {
        // exp满了，不再获得
        return 0;
    }

    if (m_level % 30)
    {
        // 等级处在30/60/90...的，也不获得exp
        return 0;
    }

    uint32_t new_exp = m_exp + add_exp;
    uint32_t new_level = m_exp2level_calc->calc_exp2level(new_exp);
    if (new_level != m_level)
    {
        level_up(new_level);
    }

    m_exp = new_exp;

    return 0;
}


// 等级提升
int c_pet::level_up(uint32_t new_level)
{

    // 更新基础属性
    c_pet_attr * p_base_attr = m_pet_data->get_base_attr(new_level);
    if (NULL == p_base_attr)
    {
        // log
        
    }
    else
    {
        m_base_attr.copy(*p_base_attr);
    }

    // 更新随机培养计算器
    c_pet_rand_calc * p_rand_calc = m_pet_data->get_rand_calc(new_level);
    if (NULL == p_rand_calc)
    {
        // log
    }
    else
    {
        m_rand_calc = p_rand_calc;
    }

    // 更新精灵培养计算器
    c_pet_train_calc * p_train_calc = m_pet_data->get_train_calc(new_level);
    if (NULL == p_train_calc)
    {
        // log
    }
    else
    {
        m_train_calc = p_train_calc;
    }
    
    m_level = new_level;
    calc_attr();
    get_player()->init_player_attr();
    return 0;
}

uint32_t c_pet::get_attr_value(uint16_t attr_type)
{
    return m_attr.id2value(attr_type);
}









///////////////////////////////////////////////////////////////////////
// 协议处理函数
///////////////////////////////////////////////////////////////////////

int cli_proto_get_bag_pet(DEFAULT_ARG)
{
    cli_proto_get_bag_pet_in* p_in = P_IN;
    player_t* pet_owner = get_player(p_in->userid);
    if (!pet_owner || pet_owner->role_tm != p_in->role_tm) {
        p->clear_waitcmd();
        return 0;
    }



    if (!pet_owner->check_module(MODULE_PET))
    {
        p->clear_waitcmd();
        return 0;
    }

    cli_proto_get_bag_pet_out * p_out = P_OUT;


    vector_for_each(pet_owner->pet, it)
    {
        simple_pet_info_t info;

        c_pet * p_pet = *it;

        if (p_pet->m_status == PET_STATUS_BAG && pet_owner != p) {
        //不是拉取自己的精灵信息时不用还回背包中的精灵
        } else {
            p_pet->export_info(&info);
            p_out->pet_info.push_back(info);
        }   
    }

    if (NULL != p->fight_pet)
    {
        for (uint32_t i = 0; i < PET_ASSIST_ATTR_NUM; i++)
        {
            p_out->assist_attr[i] = p->fight_pet->m_assist_value[i];
        }
        
    }
    return send_to_player(p, p_out, p->waitcmd, 1);

}


int cli_proto_set_fight_pet(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_PET))
    {
        p->clear_waitcmd();
        return 0;
    }

    cli_proto_set_fight_pet_in * p_in = P_IN;

    uint32_t des_pet_no = p_in->pet_no;
    uint32_t src_pet_no = p->fight_pet ? p->fight_pet->m_no : 0;

    c_pet * p_pet = find_player_pet(p, des_pet_no);
    if (NULL == p_pet)
    {
        ERROR_TLOG("user: %u, pet no %u not found", p->id, des_pet_no);
        send_error_to_player(p, ONLINE_ERR_PET_NOT_FOUND);
        return 0;

    }

    if (p->fight_pet == p_pet)
    {
        // 已经是出战的精灵了
        cli_proto_set_fight_pet_out * p_out = P_OUT;
        p_out->user_id = p->id;
        p_out->pet_id = p_pet->m_id;
        p_out->pet_no = p_pet->m_no;
        p_out->pet_level = p_pet->m_level;

        send_to_player(p, p_out, p->waitcmd, 1);
        return 0;
    }

    if (is_assist_pet(p, des_pet_no))
    {
        return db_set_assist_pet(p, src_pet_no, des_pet_no);
    }
    else
    {
        return db_set_fight_pet(p, des_pet_no, src_pet_no);

    }


}


int pack_assist_pet_info(player_t * p, Cmessage * c_out)
{
    cli_proto_set_assist_pet_out * p_out = P_OUT;
    p_out->init();
    if (p->assist_pet.size() > 0)
    {
        p_out->pet_no_1 = p->assist_pet[0]->m_no;
    }

    if (p->assist_pet.size() > 1)
    {
        p_out->pet_no_2 = p->assist_pet[1]->m_no;
    }

    if (NULL != p->fight_pet)
    {
        for (uint32_t i = 0; i < array_elem_num(p_out->assist_attr); i++)
        {
            p_out->assist_attr[i] = p->fight_pet->m_assist_value[i];
        }

    }


    return 0;
}

int cli_proto_set_assist_pet(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_PET))
    {
        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }

    cli_proto_set_assist_pet_in * p_in = P_IN;


    uint32_t src_pet_no = p_in->src_pet_no;
    uint32_t des_pet_no = p_in->des_pet_no;

    if (src_pet_no == des_pet_no)
    {
        pack_assist_pet_info(p, c_out);

        return send_to_player(p, c_out, p->waitcmd, 1);
    }
    
    if (src_pet_no && !is_assist_pet(p, src_pet_no))
    {
        // 原先的精灵不是辅助的
        // 报文错误
        ERROR_TLOG("user: %u, unknown assist pet_no: %u", p->id, src_pet_no);
        return send_error_to_player(p, ONLINE_ERR_PET_NOT_FOUND);
    }

    if (0 == des_pet_no)
    {
        // 目标精灵编号不能为0
        //
        ERROR_TLOG("user: %u, new assist pet_no: %u", p->id, des_pet_no);
        return send_error_to_player(p, ONLINE_ERR_PET_NO);
    }

    if (is_assist_pet(p, des_pet_no))
    {
        // 目标精灵已经是辅助精灵
        pack_assist_pet_info(p, c_out);

        return send_to_player(p, c_out, p->waitcmd, 1);

    }


    return db_set_assist_pet(p, des_pet_no, src_pet_no);
}




int cli_proto_get_pet_detail(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_PET))
    {
        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }

    cli_proto_get_pet_detail_in * p_in = P_IN;

    player_t* pet_owner = get_player(p_in->userid);
    if (!pet_owner || pet_owner->role_tm != p_in->role_tm) {
        p->clear_waitcmd();
        return 0;
    }

    uint32_t pet_no = p_in->pet_no;

    c_pet * p_pet = find_player_pet(pet_owner, pet_no);
    if (NULL == p_pet)
    {
        ERROR_TLOG("user: %u, pet no %u not found", pet_owner->id, pet_no);
        send_error_to_player(p, ONLINE_ERR_PET_NOT_FOUND);
        return 0;

    }

    cli_proto_get_pet_detail_out * p_out = P_OUT;

    p_pet->export_info(&p_out->pet_info);


    return send_to_player(p, p_out, p->waitcmd, 1);
}




int cli_proto_upgrade_pet_iq(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_PET))
    {

        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }


    cli_proto_upgrade_pet_iq_in * p_in = P_IN;

    uint32_t pet_no = p_in->pet_no;

    c_pet * p_pet = find_player_pet(p, pet_no);
    if (NULL == p_pet)
    {
        ERROR_TLOG("user: %u, pet no %u not found", p->id, pet_no);
        send_error_to_player(p, ONLINE_ERR_PET_NOT_FOUND);
        return 0;
    }

    uint32_t max_iq = pet_iq_data_mgr::instance().get_max_key();
    if (p_pet->m_iq >= max_iq)
    {
        send_error_to_player(p, ONLINE_ERR_PET_IQ_MAX_REACH);
        return 0;
    }


    pet_iq_data_t * p_data = pet_iq_data_mgr::instance().get_object(p_pet->m_iq);

    // 检查消耗品
    uint32_t item_count = p->bag->get_item_count(p_data->item_id);
    if (!p->bag->check_del_item(p_data->item_id, p_data->item_num))
    {
        ERROR_TLOG("user: %u, item %u, %u < require %u, pet %u, iq %u", p->id, p_data->item_id, item_count, p_data->item_num, p_pet->m_id, p_pet->m_iq);
        send_error_to_player(p, ONLINE_ERR_PET_LACK_ITEM);
        return 0;
    }

    uint32_t new_iq = p_pet->m_iq;
    uint32_t r = ranged_random(0, 100);
    if (r < p_data->prob)
    {
        new_iq++;
    }
    return db_set_pet_iq(p, pet_no, new_iq, p_data);
}


int cli_proto_try_pet_train(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_PET))
    {

        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }

    cli_proto_try_pet_train_in * p_in = P_IN;

    uint32_t pet_no = p_in->pet_no;
    uint32_t train_id = p_in->train_id;

    c_pet * p_pet = find_player_pet(p, pet_no);
    if (NULL == p_pet)
    {
        ERROR_TLOG("user: %u, pet no %u not found", p->id, pet_no);
        send_error_to_player(p, ONLINE_ERR_PET_NOT_FOUND);
        return 0;
    }

    pet_train_consume_data_t * p_consume_data = p_pet->m_train_calc->get_consume_data(train_id);
    if (NULL == p_consume_data)
    {
        ERROR_TLOG("user: %u, pet %u, train %u not found", p->id, p_pet->m_id, train_id);
        send_error_to_player(p, ONLINE_ERR_PET_TRAIN);
        return 0;
    }

    // 检查消耗物品够不
    if (p->get_player_attr_value(OBJ_ATTR_GOLD) < p_consume_data->gold)
    {
        ERROR_TLOG("user: %u, gold %u < %u, pet %u, train %u", p->id, p->get_player_attr_value(OBJ_ATTR_GOLD), p_consume_data->gold, p_pet->m_id, train_id);
        send_error_to_player(p, ONLINE_ERR_PET_LACK_ITEM);
        return 0;
    }

    uint32_t item_count = p->bag->get_item_count(p_consume_data->item_id);
    if (!p->bag->check_del_item(p_consume_data->item_id, p_consume_data->item_num))
    {
        ERROR_TLOG("user: %u, item %u, %u < require %u, pet %u, train %u", p->id, p_consume_data->item_id, item_count, p_consume_data->item_num, p_pet->m_id, train_id);
        send_error_to_player(p, ONLINE_ERR_PET_LACK_ITEM);
        return 0;
    }

    // 等db正常返回之后，再更新到pet的内部变量m_train_value
    uint32_t train_value[PET_TRAIN_ATTR_NUM] = {0};
    if (0 != p_pet->m_train_calc->calc(train_id, train_value, &p_pet->m_train_attr))
    {
        ERROR_TLOG("user: %u, pet %u, try train %u failed", p->id, p_pet->m_id, train_id);
        send_error_to_player(p, ONLINE_ERR_PET_TRAIN);
        return 0;
    }



    return db_try_pet_train(p, pet_no, PET_TRAIN_ATTR_NUM, train_value, p_consume_data);
}


int cli_proto_set_pet_train(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_PET))
    {

        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }

    cli_proto_set_pet_train_in * p_in = P_IN;

    uint32_t pet_no = p_in->pet_no;
    uint32_t set_flag = p_in->set_flag;

    c_pet * p_pet = find_player_pet(p, pet_no);
    if (NULL == p_pet)
    {
        ERROR_TLOG("user: %u, pet no %u not found", p->id, pet_no);
        send_error_to_player(p, ONLINE_ERR_PET_NOT_FOUND);
        return 0;
    }


    return db_set_pet_train(p, pet_no, set_flag);

}


int cli_proto_rand_pet_gift(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_PET))
    {

        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }

    cli_proto_rand_pet_gift_in * p_in = P_IN;

    uint32_t locked_count = p_in->gift_no.size();
    uint32_t pet_no = p_in->pet_no;

    c_pet * p_pet = find_player_pet(p, pet_no);
    if (NULL == p_pet)
    {
        ERROR_TLOG("user: %u, pet no %u not found", p->id, pet_no);
        send_error_to_player(p, ONLINE_ERR_PET_NOT_FOUND);
        return 0;

    }


    db_extra_info_t extra_info;
    db_extra_info_t * p_extra_info = NULL;
    pet_gift_consume_data_t * p_consume_data = NULL;


    if (p->extra_info->id2value(EXTRA_INFO_FREE_RAND_GIFT) > 0)
    {
        extra_info.info_id = EXTRA_INFO_FREE_RAND_GIFT;
        extra_info.info_value = p->extra_info->id2value(EXTRA_INFO_FREE_RAND_GIFT) - 1;
        p_extra_info = &extra_info;
    }
    else
    {
        p_consume_data = pet_gift_rand_data_mgr::instance().get_object(locked_count);
        if (NULL == p_consume_data)
        {
            ERROR_TLOG("user: %u, pet %u, locked count %u not found", p->id, p_pet->m_id, locked_count);
            send_error_to_player(p, ONLINE_ERR_XML);
            return 0;
        }


        TRACE_TLOG("user: %u, pet %u, gift(%u locked) rand need item %u * %u", p->id, p_pet->m_id, locked_count, p_consume_data->item_id, p_consume_data->item_num);

        // 检查物品消耗量
        uint32_t item_count = p->bag->get_item_count(p_consume_data->item_id);
        if (!p->bag->check_del_item(p_consume_data->item_id, p_consume_data->item_num))
        {
            ERROR_TLOG("user: %u, item %u, %u < require %u, pet %u, gift locked count %u", p->id, p_consume_data->item_id, item_count, p_consume_data->item_num, p_pet->m_id, locked_count);
            send_error_to_player(p, ONLINE_ERR_PET_LACK_ITEM);
            return 0;

        }
    }


    /// 处理天赋随机
    std::vector<uint32_t> to_rand_gift;
    std::vector<uint32_t> fixed_gift;

    for (uint32_t i = 0; i < p_pet->m_gift.get_max_count(); i++)
    {
        to_rand_gift.push_back(i);
    }

    vector_for_each(p_in->gift_no, it)
    {
        uint32_t gift_no = *it;
        if (gift_no >= p_pet->m_gift.get_max_count())
        {
            send_error_to_player(p, ONLINE_ERR_PET_GIFT);
            return 0;

        }
        pet_gift_t * p_gift = p_pet->m_gift.get_gift(gift_no);
        if (NULL == p_gift)
        {
            ERROR_TLOG("user: %u, pet %u, gift no %u not found", p->id, p_pet->m_id, gift_no);
            send_error_to_player(p, ONLINE_ERR_PET_GIFT);
            return 0;
        }

        vector_for_each(to_rand_gift, iter)
        {
            uint32_t to_rand_no = *iter;
            if (to_rand_no == gift_no)
            {
                to_rand_gift.erase(iter);
                break;
            }
        }

        fixed_gift.push_back(p_gift->gift_id);

    }

    std::vector<db_update_pet_gift_t> req;
    vector_for_each(to_rand_gift, it)
    {
        uint32_t gift_no = *it;

        db_update_pet_gift_t gift;
        gift.gift_no = gift_no;

        if (0 != pet_gift_info_mgr::instance().get_rand_gift(&gift.gift_id, &gift.gift_level, fixed_gift))
        {
            ERROR_TLOG("user: %u, pet %u, rand gift failed", p->id, p_pet->m_id);
            send_error_to_player(p, ONLINE_ERR_PET_GIFT);
            return 0;
        }

        fixed_gift.push_back(gift.gift_id);
        req.push_back(gift);
    }

    /// 天赋随机处理结束

    return db_update_pet_gift(p, pet_no, p_consume_data, p_extra_info, req);

}


int cli_proto_upgrade_pet_gift(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_PET))
    {

        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }

    cli_proto_upgrade_pet_gift_in * p_in = P_IN;

    uint32_t pet_no = p_in->pet_no;
    uint32_t gift_no = p_in->gift_no;

    c_pet * p_pet = find_player_pet(p, pet_no);
    if (NULL == p_pet)
    {
        ERROR_TLOG("user: %u, pet no %u not found", p->id, pet_no);
        send_error_to_player(p, ONLINE_ERR_PET_NOT_FOUND);
        return 0;

    }

    pet_gift_t * p_gift = p_pet->m_gift.get_gift(gift_no);
    if (NULL == p_gift)
    {
        ERROR_TLOG("user: %u, pet %u, gift no %u not found", p->id, p_pet->m_id, gift_no);
        send_error_to_player(p, ONLINE_ERR_PET_GIFT);
        return 0;


    }

    pet_gift_consume_data_t * p_consume_data = pet_gift_upgrade_data_mgr::instance().get_object(p_gift->gift_level);
    if (NULL == p_consume_data)
    {
        ERROR_TLOG("user: %u, pet %u, gift level %u not found", p->id, p_pet->m_id, p_gift->gift_level);
        send_error_to_player(p, ONLINE_ERR_XML);
        return 0;
    }


    TRACE_TLOG("user: %u, pet %u, gift %u upgrade need item %u * %u", p->id, p_pet->m_id, p_gift->gift_id, p_consume_data->item_id, p_consume_data->item_num);

    // 检查物品消耗量
    uint32_t item_count = p->bag->get_item_count(p_consume_data->item_id);
    if (!p->bag->check_del_item(p_consume_data->item_id, p_consume_data->item_num))
    {
        ERROR_TLOG("user: %u, item %u, %u < require %u, pet %u, gift level %u", p->id, p_consume_data->item_id, item_count, p_consume_data->item_num, p_pet->m_id, p_gift->gift_level);
        send_error_to_player(p, ONLINE_ERR_PET_LACK_ITEM);
        return 0;

    }

    std::vector<db_update_pet_gift_t> req;
    db_update_pet_gift_t gift;
    gift.gift_no = p_gift->gift_no;
    gift.gift_id = p_gift->gift_id;
    gift.gift_level = p_gift->gift_level + 1;
    req.push_back(gift);

    return db_update_pet_gift(p, pet_no, p_consume_data, NULL, req);
}



int cli_proto_pet_merge(DEFAULT_ARG)
{
    if (!p->check_module(MODULE_PET))
    {

        send_error_to_player(p, ONLINE_ERR_MODULE);
        return 0;
    }

    cli_proto_pet_merge_in * p_in = P_IN;


    uint32_t total_prob = 0;
    uint32_t base_pet_no = p_in->base_pet_no;

    // if (is_fight_pet(p, base_pet_no))
    // {
        // // 主精灵
    // }
    
    c_pet * p_pet = find_player_pet(p, base_pet_no);
    if (NULL == p_pet)
    {
        ERROR_TLOG("user: %u, pet %u not found", p->id, base_pet_no);
        send_error_to_player(p, ONLINE_ERR_PET_NOT_FOUND);
        return 0;
    }

    if (p_pet->m_level >= p_pet->m_exp2level_calc->get_max_level())
    {
        ERROR_TLOG("user: %u, pet %u level %u max", p->id, base_pet_no, p_pet->m_level);
        send_error_to_player(p, ONLINE_ERR_PET_MERGE);
        return 0;
    }

    // 等级非30的倍数，不给幻化
    if (p_pet->m_level % 30)
    {
        ERROR_TLOG("user: %u, pet %u level %u must be N * 30", p->id, base_pet_no, p_pet->m_level);
        send_error_to_player(p, ONLINE_ERR_PET_MERGE);
        return 0;
    }

    pet_merge_prob_data_t * p_prob_info = pet_merge_prob_data_mgr::instance().get_object(p_pet->m_level);
    if (NULL == p_prob_info)
    {
        ERROR_TLOG("user: %u, pet merge prob data not found, level: %u", p->id, p_pet->m_level);
        send_error_to_player(p, ONLINE_ERR_XML);
        return 0;
    }

    total_prob += p_prob_info->base_prob;

    uint32_t assist_prob = 0;

    // 辅助精灵
    vector_for_each(p_in->assist_pet_no, it)
    {
        uint32_t del_pet_no = *it;

        c_pet * p_del_pet = find_player_pet(p, del_pet_no);
        if (NULL == p_del_pet)
        {
            ERROR_TLOG("user: %u, pet %u not found", p->id, del_pet_no);
            send_error_to_player(p, ONLINE_ERR_PET_NOT_FOUND);
            return 0;
        }

        if (is_fight_pet(p, del_pet_no))
        {
            // 主精灵不能作为参与幻化的精灵
            ERROR_TLOG("user: %u, fight pet %u can not be merged", p->id, del_pet_no);
            send_error_to_player(p, ONLINE_ERR_PET_MERGE);
            return 0;
        }

        uint32_t del_pet_level = p_del_pet->m_level;
        pet_merge_prob_data_t * p_prob_info = pet_merge_prob_data_mgr::instance().get_object(del_pet_level);
        if (NULL == p_prob_info)
        {
            ERROR_TLOG("user: %u, pet merge prob data not found, level: %u", p->id, del_pet_level);
            send_error_to_player(p, ONLINE_ERR_XML);
            return 0;
        }

        assist_prob += p_prob_info->assist_prob;
    }

    total_prob += assist_prob;


    std::vector<db_del_item_request_t> del_items;
    std::vector<db_add_item_request_t> add_items;


    // 魔石加成
    vector_for_each(p_in->consume_items, it)
    {
        simple_item_info_t * p_item_info = &(*it);
        uint32_t item_id = p_item_info->item_id;
        uint32_t item_count = p_item_info->item_count;
        if (!p->bag->check_del_item(item_id, item_count))
        {
            ERROR_TLOG("user: %u, lack item %u * %u", p->id, item_id, item_count);
            send_error_to_player(p, ONLINE_ERR_PET_LACK_ITEM);
            return 0;
        }

        item_ptr p_item = p->bag->get_first_item(item_id);
        item_effect_t * p_effect = p_item->get_item_data()->get_item_effect();
        if (p_effect->effect_id == EFFECT_ID_ADD_PET_MERGE_SUCCESS_RATE)
        {
            total_prob += p_effect->effect_value[0] * 10;
            // 扣掉魔石
            if (!parse_db_del_item_request(p, item_id, item_count, del_items))
            {
                send_error_to_player(p, ONLINE_ERR_PET_LACK_ITEM);
                return -1;
            }
        }
    }

    INFO_TLOG("user: %u, pet merge, base pet no: %u, total rate: %u", p->id, base_pet_no, total_prob);

    uint32_t new_pet_level = p_pet->m_level;
    uint32_t new_pet_exp = p_pet->m_exp;

    uint32_t r = ranged_random(0, 1000);
    if (r < total_prob)
    {
        // 幻化成功

        // 升级
        new_pet_level++;
        // exp置为新级别的0点
        new_pet_exp = p_pet->m_exp2level_calc->get_level_exp(new_pet_level);
    }
    else
    {
        // 幻化失败
        pet_merge_compensation_data_t * p_compensation_info = pet_merge_compensation_data_mgr::instance().get_object(assist_prob);
        if (NULL == p_compensation_info)
        {
            ERROR_TLOG("user: %u, pet merge compensation data not found, level: %u", p->id, assist_prob);
            send_error_to_player(p, ONLINE_ERR_XML);
            return 0;
        }

        r = ranged_random(0, 100);
        uint32_t rate = 0;
        vector_for_each(p_compensation_info->items, it)
        {
            pet_merge_compensation_item_t * p_item_data = &(*it);
            rate += p_item_data->rate;
            if (r <= rate)
            {
                if (!parse_db_add_item_request(p, p_item_data->item_id, p_item_data->item_num, p_item_data->expire_time, add_items))
                {
                    send_error_to_player(p, ONLINE_ERR_BAG_FULL);
                    return 0;
                }

                break;
            }

        }
    }


    // 扣掉幻石
    if (!parse_db_del_item_request(p, HUAN_SHI_ID, 1, del_items))
    {
        send_error_to_player(p, ONLINE_ERR_PET_LACK_ITEM);
        return -1;
    }


    return db_pet_merge(p, base_pet_no, new_pet_level, new_pet_exp, p_in->assist_pet_no, del_items, add_items);
}
