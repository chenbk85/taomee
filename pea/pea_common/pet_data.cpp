/** 
 * ========================================================================
 * @file pet_data.cpp
 * @brief 
 * @version 1.0
 * @date 2012-04-16
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

extern "C"
{
#include <libtaomee/tlog/tlog.h>

}

#include <libtaomee++/random/random.hpp>
#include "pet_data.hpp"
#include "attr_config_data.hpp"

using namespace std;
using namespace taomee;



#define RAND_ATTR_MAX(iq, max)  ((iq) * (max) / 30)

/////////////////////////////// xml 数据解析 ////////////////////////////
/////////////////////////////////////////////////////////////////////////


///////////////////// 精灵的数据 //////////////////////////////

int parse_pet_attr_config_data(xmlNodePtr node, pet_attr_config_t * p_data)
{
    if (NULL == node || NULL == p_data)
    {
        return -1;
    }

    get_xml_prop_def(p_data->hp_, node, "hp", 0);
    get_xml_prop_def(p_data->magic_, node, "magic", 0);
    get_xml_prop_def(p_data->agility_, node, "agility", 0);
    get_xml_prop_def(p_data->luck_, node, "luck", 0);
    get_xml_prop_def(p_data->physique_, node, "physique", 0);
    get_xml_prop_def(p_data->anger_, node, "anger", 0);
    get_xml_prop_def(p_data->atk_, node, "atk", 0);
    get_xml_prop_def(p_data->def_, node, "def", 0);
    get_xml_prop_def(p_data->strength_, node, "strength", 0);
    get_xml_prop_def(p_data->happy_, node, "happy", 0);
    get_xml_prop_def(p_data->soul_, node, "soul", 0);
    get_xml_prop_def(p_data->intensity_, node, "intensity", 0);
    get_xml_prop_def(p_data->crit_, node, "crit", 0);
    get_xml_prop_def(p_data->double_hit_, node, "double_hit", 0);
    get_xml_prop_def(p_data->speed_, node, "speed", 0);
    get_xml_prop_def(p_data->crit_damage_, node, "crit_damage", 0);
    get_xml_prop_def(p_data->tenacity_, node, "tenacity", 0);
    get_xml_prop_def(p_data->gift_count, node, "innate", 0);
    get_xml_prop_def(p_data->badge_count, node, "unreal", 0);


    return 0;
}

int init_pet_config_data(xmlNodePtr root)
{
    xmlnode_for_each(root, node_1)
    {
        if (is_xmlnode_name(node_1, "pet"))
        {
            uint32_t pet_id = 0;
            get_xml_prop_def(pet_id, node_1, "id", 0);

            c_pet_config * p_pet = pet_config_mgr::instance().get_object(pet_id);

            if (NULL == p_pet)
            {
                p_pet = new c_pet_config();
                if (NULL == p_pet)
                {
                    return -1;
                }

                pet_config_mgr::instance().insert_object(pet_id, p_pet);
            }

            get_xml_prop_def(p_pet->width, node_1, "width", 0);
            get_xml_prop_def(p_pet->height, node_1, "height", 0);
            get_xml_prop_def(p_pet->skill_1, node_1, "skill1", 0);
            get_xml_prop_def(p_pet->skill_2, node_1, "skill2", 0);
            get_xml_prop_def(p_pet->skill_3, node_1, "skill3", 0);
            get_xml_prop_def(p_pet->uni_skill, node_1, "uni_skill", 0);

            pet_attr_config_t init_attr;
            pet_attr_config_t step_attr;

            uint32_t level = 0;
            uint32_t sum_exp = 0;
            uint32_t star = 0;

            xmlnode_for_each(node_1, node_2)
            {
                if (is_xmlnode_name(node_2, "initial"))
                {
                    parse_pet_attr_config_data(node_2, &init_attr);
                }
                else if (is_xmlnode_name(node_2, "star"))
                {
                    get_xml_prop_def(star, node_2, "star", 0);

                    parse_pet_attr_config_data(node_2, &step_attr);


                    xmlnode_for_each(node_2, node_3)
                    {
                        if (is_xmlnode_name(node_3, "level"))
                        {
                            get_xml_prop_def(level, node_3, "level", 0);
                            get_xml_prop_def(sum_exp, node_3, "sum_exp", 0);

                            if (1 == star && 1 == level)
                            {
                                // xml中的1级，sum_exp = 0
                                // 直接跳过
                                // xml中的2级，sum_exp才是1级到2级的升级经验
                                continue;
                            }
                            pet_attr_config_t * p_data = new pet_attr_config_t();
                            if (NULL == p_data)
                            {
                                return -1;
                            }


                            p_data->copy(&init_attr);
                            p_data->accumulate(&step_attr);
                            p_data->gift_count = step_attr.gift_count;
                            p_data->badge_count = step_attr.badge_count;

                            // 作为计算下一级时的init_attr
                            init_attr.copy(p_data);

                            p_data->level_ = level - 1 + (star - 1) * 30;
                            p_data->exp_ = sum_exp;

                            if (!p_pet->insert_object(p_data->level_, p_data))
                            {
                                throw XmlParseError(string("pet data = %u has exist", pet_id));
                                return -1;
                            }
                            else
                            {
                                //TRACE_TLOG("[pet attr] id: %u, level: %u", pet_id, p_data->level_);
                            }

                        }
                    }
                }

            }

            // 生成最高等级的数据
            pet_attr_config_t * p_data = new pet_attr_config_t();
            if (NULL == p_data)
            {
                return -1;
            }


            p_data->copy(&init_attr);
            p_data->accumulate(&step_attr);
            p_data->gift_count = step_attr.gift_count;
            p_data->badge_count = step_attr.badge_count;

            p_data->level_ = level + (star - 1) * 30;
            p_data->exp_ = -1;

            if (!p_pet->insert_object(p_data->level_, p_data))
            {
                throw XmlParseError(string("pet data = %u has exist", pet_id));
                return -1;
            }
            else
            {
                TRACE_TLOG("[pet attr] id: %u, level: %u", pet_id, p_data->level_);
            }



        }


    }
    return 0;
}



////////////// iq 成长 ///////////////


int init_pet_iq_config_data(xmlNodePtr root)
{
    xmlNodePtr node_1 = root->xmlChildrenNode;
    while (node_1)
    {
        if (is_xmlnode_name(node_1, "pet_iq"))
        {
            uint32_t iq = 0;
            get_xml_prop_def(iq, node_1, "iq", 0);
            pet_iq_config_t * p_iq_config = new pet_iq_config_t;
            if (NULL == p_iq_config)
            {
                return -1;
            }

            get_xml_prop_def(p_iq_config->cost, node_1, "cost", 0);
            get_xml_prop_def(p_iq_config->prob, node_1, "success_rate", 0);

            /*
               TRACE_TLOG("[pet iq] iq: %u, cost: %u, rate: %u", 
               iq,
               p_iq_config->cost,
               p_iq_config->prob);
               */
            pet_iq_config_mgr::instance().insert_object(iq, p_iq_config);

        }

        node_1 = node_1->next;
    }

    return 0;
}



//////////////////////// 精灵培养 //////////////////////////



int init_pet_train_config_data(xmlNodePtr root)
{
    xmlNodePtr node_1 = root->xmlChildrenNode;
    while (node_1)
    {
        if (is_xmlnode_name(node_1, "pet_culture"))
        {
            uint32_t train_id = 0;
            get_xml_prop_def(train_id, node_1, "culture_id", 0);

            pet_train_config_t * p_train = new pet_train_config_t;
            if (NULL == p_train)
            {
                return -1;
            }

            pet_train_config_mgr::instance().insert_object(train_id, p_train);

            uint32_t attr_count = 0;

            xmlNodePtr node_2 = node_1->xmlChildrenNode;
            while (node_2)
            {
                if (is_xmlnode_name(node_2, "petlv"))
                {
                    uint32_t pet_lv = 0;
                    get_xml_prop_def(pet_lv, node_2, "pet_lv", 0);

                    pet_train_consume_config_t * p_consume = p_train->get_consume_config(pet_lv);
                    if (NULL != p_consume)
                    {
                        get_xml_prop_def(p_consume->gold, node_2, "gold", 0);
                        get_xml_prop_def(p_consume->item_id, node_2, "item_id", 0);
                        get_xml_prop_def(p_consume->item_num, node_2, "item_num", 0);

                    }



                }
                else if (is_xmlnode_name(node_2, "attr"))
                {

                    pet_train_attr_config_t * p_attr = p_train->get_attr_config(attr_count);
                    if (NULL != p_attr)
                    {
                        get_xml_prop_def(p_attr->attr_id, node_2, "attr_id", 0);
                        get_xml_prop_def(p_attr->min_step, node_2, "min_step", 0);
                        get_xml_prop_def(p_attr->max_step, node_2, "max_step", 0);
                        get_xml_prop_def(p_attr->max_rate, node_2, "max_rate", 0);
                        attr_count++;

                    }
                }

                node_2 = node_2->next;
            }
        }

        node_1 = node_1->next;
    }
    return 0;
}




///////////////////////////// 精灵天赋 ////////////////////////////////////



int init_pet_gift_config_data(xmlNodePtr root)
{
    xmlNodePtr node_1 = root->xmlChildrenNode;
    while (node_1)
    {
        if (is_xmlnode_name(node_1, "innate"))
        {
            uint32_t gift_id = 0;
            get_xml_prop_def(gift_id, node_1, "id", 0);

            pet_gift_config_t * p_gift = new pet_gift_config_t;
            if (NULL == p_gift)
            {
                return -1;
            }

            pet_gift_config_mgr::instance().insert_object(gift_id, p_gift);
        }

        node_1 = node_1->next;
    }
    return 0;
}

/////////////// 精灵天赋升级 ////////////////////

int init_pet_gift_upgrade_config_data(xmlNodePtr root)
{
    xmlnode_for_each(root, node_1)
    {
        if (is_xmlnode_name(node_1, "level"))
        {
            uint32_t level= 0;
            get_xml_prop_def(level, node_1, "level", 0);

            pet_gift_upgrade_config_t * p_gift = new pet_gift_upgrade_config_t;
            if (NULL == p_gift)
            {
                return -1;
            }

            get_xml_prop_def(p_gift->item_id, node_1, "item_id", 0);
            get_xml_prop_def(p_gift->item_num, node_1, "item_quantity", 0);

            pet_gift_upgrade_config_mgr::instance().insert_object(level, p_gift);
        }

    }
    return 0;
}




// 随机时，天赋的各个等级的概率
int init_pet_gift_rand_prob_config_data(xmlNodePtr root)
{
    xmlnode_for_each(root, node_1)
    {
        if (is_xmlnode_name(node_1, "rate"))
        {
            uint32_t level = 0;
            get_xml_prop_def(level, node_1, "level", 0);

            uint32_t * p_prob = new uint32_t;
            if (NULL == p_prob)
            {
                return -1;
            }

            get_xml_prop_def(*p_prob, node_1, "probability", 0);

            pet_gift_rand_prob_config_mgr::instance().insert_object(level, p_prob);

        }

    }

    return 0;
}

/////////////// 精灵天赋随机 ////////////////

int init_pet_gift_rand_config_data(xmlNodePtr root)
{
    xmlnode_for_each(root, node_1)
    {
        if (is_xmlnode_name(node_1, "quantity"))
        {
            uint32_t close_num = 0;
            get_xml_prop_def(close_num, node_1, "close_quantity", 0);

            pet_gift_rand_config_t * p_gift = new pet_gift_rand_config_t;
            if (NULL == p_gift)
            {
                return -1;
            }

            get_xml_prop_def(p_gift->item_id, node_1, "item_id", 0);
            get_xml_prop_def(p_gift->item_num, node_1, "item_quantity", 0);

            pet_gift_rand_config_mgr::instance().insert_object(close_num, p_gift);
        }
    }

    return 0;
}


//////////////// 精灵天赋消耗 //////////////

int init_pet_gift_consume_config_data(xmlNodePtr root)
{
    xmlnode_for_each(root, node_1)
    {
        if (is_xmlnode_name(node_1, "comprehend"))
        {
            if (0 != init_pet_gift_rand_config_data(node_1))
            {
                return -1;
            }
        }
        else if (is_xmlnode_name(node_1, "upgrade"))
        {
            if (0 != init_pet_gift_upgrade_config_data(node_1))
            {
                return -1;
            }
        }
        else if (is_xmlnode_name(node_1, "probability"))
        {
            if (0 != init_pet_gift_rand_prob_config_data(node_1))
            {
                return -1;
            }
        }

    }

    return 0;
}


//////////// 精灵幻化 //////////////


/////// 幻化成功率 /////////

int init_pet_merge_prob_config_data(xmlNodePtr root)
{
    xmlNodePtr node_1 = root->xmlChildrenNode;
    while (node_1)
    {
        if (is_xmlnode_name(node_1, "rate"))
        {
            uint32_t lv_low = 0;
            uint32_t lv_high = 0;
            float success_rate = 0;
            get_xml_prop_def(lv_low, node_1, "lv_low", 0);
            get_xml_prop_def(lv_high, node_1, "lv_high", 0);
            get_xml_prop_def(success_rate, node_1, "success_rate", 0);

            // xml中带小数，转成千分比
            uint32_t prob = success_rate * 10;


            for (uint32_t i = lv_low; i <= lv_high; i++)
            {
                pet_merge_prob_config_t * p_data = new pet_merge_prob_config_t;
                if (NULL == p_data)
                {
                    return -1;
                }

                p_data->assist_prob = prob;
                pet_merge_prob_config_mgr::instance().insert_object(i, p_data);
            }
        }

        node_1 = node_1->next;
    }

    return 0;
}

/////// 幻化失败补偿 /////////

int init_pet_merge_compensation_config_data(xmlNodePtr root)
{
    xmlNodePtr node_1 = root->xmlChildrenNode;
    while (node_1)
    {
        if (is_xmlnode_name(node_1, "itemgroup"))
        {
            float rate_low = 0;
            float rate_high = 0;
            get_xml_prop_def(rate_low, node_1, "rate_low", 0);
            get_xml_prop_def(rate_high, node_1, "rate_high", 0);

            // xml中带小数的百分比，转成千分比
            uint32_t prob_low = rate_low * 10;
            uint32_t prob_high = rate_high * 10;

            for (uint32_t i = prob_low; i <= prob_high; i++)
            {
                pet_merge_compensation_config_t * p_data = new pet_merge_compensation_config_t;
                if (NULL == p_data)
                {
                    return -1;
                }

                xmlNodePtr node_2 = node_1->xmlChildrenNode;
                while (node_2)
                {
                    if (is_xmlnode_name(node_2, "item"))
                    {
                        pet_merge_compensation_config_item_t item;
                        get_xml_prop_def(item.item_id, node_2, "item_id", 0);
                        get_xml_prop_def(item.item_num, node_2, "num", 0);
                        get_xml_prop_def(item.expire_time, node_2, "expire_time", 0);
                        get_xml_prop_def(item.rate, node_2, "rate", 0);

                        p_data->items.push_back(item);
                    }

                    node_2 = node_2->next;
                }

                pet_merge_compensation_config_mgr::instance().insert_object(i, p_data);

            }

        }

        node_1 = node_1->next;
    }

    // 检查概率为0补偿是否配置了
    // 没有的话，给加一个空的数据
    if (!pet_merge_compensation_config_mgr::instance().has_object(0))
    {
        pet_merge_compensation_config_t * p_data = new pet_merge_compensation_config_t;
        if (NULL == p_data)
        {
            return -1;
        }

        pet_merge_compensation_config_mgr::instance().insert_object(0, p_data);
    }

    return 0;
}


int init_pet_merge_config_data(xmlNodePtr root)
{
    xmlNodePtr node_1 = root->xmlChildrenNode;
    while (node_1)
    {
        if (is_xmlnode_name(node_1, "rebates"))
        {
            if (0 != init_pet_merge_compensation_config_data(node_1))
            {
                return -1;
            }
        }
        else if (is_xmlnode_name(node_1, "assist"))
        {
            if (0 != init_pet_merge_prob_config_data(node_1))
            {
                return -1;
            }
        }

        node_1 = node_1->next;
    }
    // 解析基础成功率
    node_1 = root->xmlChildrenNode;
    while (node_1)
    {
        if (is_xmlnode_name(node_1, "success"))
        {
            xmlNodePtr node_2 = node_1->xmlChildrenNode;
            while (node_2)
            {
                if (is_xmlnode_name(node_2, "star"))
                {
                    uint32_t level = 0;
                    float base_rate = 0;
                    get_xml_prop_def(level, node_2, "level", 0);
                    get_xml_prop_def(base_rate, node_2, "base_rate", 0);

                    uint32_t base_prob = base_rate * 10;


                    pet_merge_prob_config_t * p_data = pet_merge_prob_config_mgr::instance().get_object(level);
                    if (NULL == p_data)
                    {
                        return -1;
                    }

                    p_data->base_prob = base_prob;
                }
                node_2 = node_2->next;
            }
        }

        node_1 = node_1->next;
    }
    return 0;
}

/////////////////////// 以上是解析xml部分 /////////////////////////




////////////////// iq 成长 ///////////////////


int init_pet_iq_data()
{
    container_for_each(pet_iq_config_mgr::instance(), it)
    {
        uint32_t level = it->first;
        pet_iq_config_t * p_config = it->second;
        pet_iq_data_t * p_data = new pet_iq_data_t;
        if (NULL == p_data)
        {
            return -1;
        }

        p_data->item_id = CONG_MING_DOU_ID;
        p_data->item_num = p_config->cost;
        p_data->prob = p_config->prob;

        //KTRACE_TLOG(level, "cost: %u * %u, rate: %u", p_data->item_id, p_data->item_num, p_data->prob);

        pet_iq_data_mgr::instance().insert_object(level, p_data);
    }

    return 0;
}


///////////////////////////// 随机成长 ///////////////////////////////

int c_pet_rand_calc::init(c_pet_data * p_pet, uint32_t level, c_pet_attr * p_attr, c_pet_attr * p_base_attr)
{
    if (NULL == p_pet || NULL == p_attr || NULL == p_base_attr)
    {
        return -1;
    }

    m_pet = p_pet;
    m_level = level;

    attr_data_t * p_attr_data = NULL;


    // 四个属性

    p_attr_data = m_data + 0;
    p_attr_data->attr_id = OBJ_ATTR_MAGIC;
    p_attr_data->max = calc_max(p_attr_data->attr_id, p_attr, p_base_attr);

    p_attr_data = m_data + 1;
    p_attr_data->attr_id = OBJ_ATTR_PHYSIQUE;
    p_attr_data->max = calc_max(p_attr_data->attr_id, p_attr, p_base_attr);

    p_attr_data = m_data + 2;
    p_attr_data->attr_id = OBJ_ATTR_AGILITY;
    p_attr_data->max = calc_max(p_attr_data->attr_id, p_attr, p_base_attr);

    p_attr_data = m_data + 3;
    p_attr_data->attr_id = OBJ_ATTR_LUCK;
    p_attr_data->max = calc_max(p_attr_data->attr_id, p_attr, p_base_attr);


    // step动态计算，不固定

    return 0;
}


int c_pet_rand_calc::calc_attr(obj_attr * p_attr, uint32_t * p_value)
{
    if (NULL == p_attr || NULL == p_value)
    {
        return -1;
    }

    for (uint32_t i = 0; i < PET_RAND_ATTR_NUM; i++)
    {
        uint32_t * p_v = p_attr->id2p(m_data[i].attr_id);
        if (NULL == p_v)
        {
            continue;
        }
        *p_v = p_value[i];
    }

    return 0;

}



int c_pet_rand_calc::calc_new_attr(uint32_t iq, obj_attr * p_des, obj_attr * p_src)
{
    int size = PET_RAND_ATTR_NUM;

    std::vector<attr_data_t *> data_vec;
    data_vec.clear();
    attr_data_t * p_data = NULL;
    uint32_t attr_id = 0;
    uint32_t max = 0;
    uint32_t * p_src_value = NULL;
    uint32_t * p_des_value = NULL;


    // 筛选出未达到上限的属性值
    for (int i = 0; i < size; i++)
    {
        p_data = &(m_data[i]);
        attr_id = p_data->attr_id;
        p_src_value = p_src->id2p(attr_id);
        p_des_value = p_des->id2p(attr_id);

        // 计算出max
        max = RAND_ATTR_MAX(iq, p_data->max);

        // 计算出step
        p_data->step = ranged_random(1, 3);

        if (!p_src_value && !p_des_value)
        {
            continue;
        }

        if (*p_src_value >= max)
        {
            continue;
        }
        else
        {
            data_vec.push_back(p_data);
        }
    }

    size = data_vec.size();
    if (0 == size)
    {
        // 都达到极限了
        return 0;
    }

    int r = ranged_random(0, size - 1);
    p_data = data_vec[r];
    attr_id = p_data->attr_id;
    p_src_value = p_src->id2p(attr_id);
    p_des_value = p_des->id2p(attr_id);
    max = RAND_ATTR_MAX(iq, p_data->max);

    if (*p_src_value + p_data->step > max)
    {
        *p_des_value = max;
    }
    else
    {
        *p_des_value = *p_src_value + p_data->step;
    }

    return 0;
}


uint32_t c_pet_rand_calc::calc_max(uint32_t attr_id, c_pet_attr * p_attr, c_pet_attr * p_base_attr)
{
    uint32_t high_v = p_attr->id2value(attr_id);
    uint32_t low_v = p_base_attr->id2value(attr_id);
    return (high_v > low_v ? high_v - low_v : 0);
}


uint32_t c_pet_rand_calc::get_max_value(uint32_t iq, uint32_t attr_id)
{
    for (uint32_t i = 0; i < PET_RAND_ATTR_NUM; i++)
    {
        attr_data_t * p_data = m_data + i;
        if (p_data->attr_id == attr_id)
        {
            return RAND_ATTR_MAX(iq, p_data->max);
        }
    }

    return 0;
}



int c_pet_rand_calc::get_max_value_list(uint32_t iq, uint32_t * p_des)
{
    for (uint32_t i = 0; i < PET_RAND_ATTR_NUM; i++)
    {
        attr_data_t * p_data = m_data + i;
        p_des[i] = RAND_ATTR_MAX(iq, p_data->max);
    }

    return 0;
}



////////////////////////////// 精灵培养 ///////////////////////////////

int c_pet_train_calc::init(c_pet_data * p_pet, uint32_t level, c_pet_attr * p_attr)
{
    if (NULL == p_pet || NULL == p_attr)
    {
        return -1;
    }

    m_pet = p_pet;
    m_level = level;


    container_for_each(pet_train_config_mgr::instance(), it)
    {

        uint32_t train_id = it->first;
        pet_train_config_t * p_train_config = it->second;

        train_data_t * p_train_data = new train_data_t;
        if (NULL == p_train_data)
        {
            return -1;
        }

        m_data.insert_object(train_id, p_train_data);

        pet_train_consume_config_t * p_consume_config = p_train_config->get_consume_config(level);
        if (NULL == p_consume_config)
        {
            return -1;
        }


        p_train_data->consume.gold = p_consume_config->gold;
        p_train_data->consume.item_id = p_consume_config->item_id;
        p_train_data->consume.item_num = p_consume_config->item_num;

        for (uint32_t i = 0; i < PET_TRAIN_ATTR_NUM; i++)
        {
            attr_data_t * p_attr_data = p_train_data->attr + i;
            pet_train_attr_config_t * p_attr_config = p_train_config->get_attr_config(i);
            if (NULL == p_attr_config)
            {
                continue;
            }

            p_attr_data->attr_id = p_attr_config->attr_id;
            p_attr_data->min_step = p_attr_config->min_step;
            p_attr_data->max_step = p_attr_config->max_step;
            p_attr_data->max_value = p_attr->id2value(p_attr_data->attr_id) * p_attr_config->max_rate;

        }

    }

    return 0;
}





int c_pet_train_calc::calc_attr(obj_attr * p_attr, uint32_t * p_src_value)
{
    train_data_t * p_train_data = m_data.begin()->second;
    for (uint32_t i = 0; i < PET_TRAIN_ATTR_NUM; i++)
    {
        uint32_t attr_id = p_train_data->attr[i].attr_id;
        uint32_t * p_value = p_attr->id2p(attr_id);
        if (NULL == p_value)
        {
            continue;
        }

        *p_value = p_src_value[i];
    }

    return 0;

}

int c_pet_train_calc::calc(uint32_t train_id, uint32_t * p_des, obj_attr * p_src)
{
    train_data_t * p_train_data = m_data.get_object(train_id);
    if (NULL == p_train_data)
    {
        return -1;

    }

    for (uint32_t i = 0; i < PET_TRAIN_ATTR_NUM; i++)
    {
        attr_data_t * p_attr_data = p_train_data->attr + i;
        uint32_t attr_id = p_attr_data->attr_id;

        int32_t value = ranged_random(p_attr_data->min_step, p_attr_data->max_step);
        int32_t new_value = p_src->id2value(attr_id) + value;
        if (new_value < 0)
        {
            p_des[i] = 0;
        }
        else if ((uint32_t)new_value > p_attr_data->max_value)
        {
            p_des[i] = p_attr_data->max_value;
        }
        else
        {
            p_des[i] = new_value;
        }

    }

    return 0;
}


pet_train_consume_data_t * c_pet_train_calc::get_consume_data(uint32_t train_id)
{
    train_data_t * p_train_data = m_data.get_object(train_id);
    if (NULL == p_train_data)
    {
        return NULL;

    }

    return &(p_train_data->consume);
}


////////////////////////////// 精灵天赋 //////////////////////////////

int init_pet_gift_consume_data()
{
    container_for_each(pet_gift_upgrade_config_mgr::instance(), it)
    {
        uint32_t level = it->first;
        pet_gift_upgrade_config_t * p_config = it->second;
        pet_gift_consume_data_t * p_data = new pet_gift_consume_data_t;
        if (NULL == p_data)
        {
            return -1;
        }
        p_data->item_id = p_config->item_id;
        p_data->item_num = p_config->item_num;

        //KTRACE_TLOG(level, "item id: %u, num: %u", p_data->item_id, p_data->item_num);

        pet_gift_upgrade_data_mgr::instance().insert_object(level, p_data);
    }


    container_for_each(pet_gift_rand_config_mgr::instance(), it)
    {
        uint32_t close_num = it->first;
        pet_gift_rand_config_t * p_config = it->second;
        pet_gift_consume_data_t * p_data = new pet_gift_consume_data_t;
        if (NULL == p_data)
        {
            return -1;
        }
        p_data->item_id = p_config->item_id;
        p_data->item_num = p_config->item_num;

        pet_gift_rand_data_mgr::instance().insert_object(close_num, p_data);

    }

    container_for_each(pet_gift_rand_prob_config_mgr::instance(), it)
    {
        uint32_t level = it->first;
        uint32_t * p_prob_config = it->second;

        pet_gift_rand_prob_calc::instance().insert(level, *p_prob_config);

    }
    return 0;
}

int c_pet_gift_info_mgr::get_rand_gift(uint32_t * p_des_gift_id, uint32_t * p_des_gift_level, std::vector<uint32_t> & exclude_gift)
{

    std::vector<uint32_t> avail_gift;
    avail_gift.clear();

    container_for_each(*this, it)
    {
        uint32_t gift_id = it->first;

        uint8_t flag = 0;
        for (uint32_t i = 0; i < exclude_gift.size(); i++)
        {
            if (gift_id == exclude_gift[i])
            {
                flag = 1;
            }
        }

        if (!flag)
        {
            avail_gift.push_back(gift_id);
        }

    }

    if (avail_gift.empty())
    {
        return 0;
    }

    int index = ranged_random(0, avail_gift.size() - 1);
    *p_des_gift_id = avail_gift[index];
    const uint32_t * p_level = pet_gift_rand_prob_calc::instance().calc();

    if (NULL == p_level)
    {
        *p_des_gift_level = 1;
    }
    else
    {
        *p_des_gift_level = *p_level;
    }

    return 0;
}





int init_pet_gift_info()
{
    container_for_each(pet_gift_config_mgr::instance(), it)
    {

        uint32_t gift_id = it->first;
        c_pet_gift_info * p_gift_info = new c_pet_gift_info;
        if (NULL == p_gift_info)
        {
            return -1;
        }

        pet_gift_info_mgr::instance().insert_object(gift_id, p_gift_info);

    }

    return 0;
}

//////////////// 精灵幻化 //////////////////


int init_pet_merge_data()
{
    container_for_each(pet_merge_compensation_config_mgr::instance(), it)
    {
        uint32_t rate = it->first;
        pet_merge_compensation_config_t * p_config = it->second;
        pet_merge_compensation_data_t * p_data = new pet_merge_compensation_data_t;
        if (NULL == p_data)
        {
            return -1;
        }
        for (uint32_t i = 0; i < p_config->items.size(); i++)
        {
            pet_merge_compensation_config_item_t * p_config_item = &p_config->items[i];
            pet_merge_compensation_item_t item;

            item.item_id = p_config_item->item_id;
            item.item_num = p_config_item->item_num;
            item.expire_time = p_config_item->expire_time;
            item.rate = p_config_item->rate;

            p_data->items.push_back(item);
        }


        pet_merge_compensation_data_mgr::instance().insert_object(rate, p_data);
    }


    container_for_each(pet_merge_prob_config_mgr::instance(), it)
    {
        uint32_t rate = it->first;
        pet_merge_prob_config_t * p_config = it->second;
        pet_merge_prob_data_t * p_data = new pet_merge_prob_data_t;
        if (NULL == p_data)
        {
            return -1;
        }
        p_data->base_prob = p_config->base_prob;
        p_data->assist_prob = p_config->assist_prob;

        pet_merge_prob_data_mgr::instance().insert_object(rate, p_data);

    }
    return 0;
}



/////////////////////////////// 精灵数据 //////////////////////////////


int c_pet_data::init_skill_data()
{
    c_pet_config * p_data = pet_config_mgr::instance().get_object(m_id);
    if (NULL == p_data)
    {
        return -1;
    }

    height = p_data->height;
    width  = p_data->width;
    m_skill_id_data.m_skill_1 = p_data->skill_1;
    m_skill_id_data.m_skill_2 = p_data->skill_2;
    m_skill_id_data.m_skill_3 = p_data->skill_3;
    m_skill_id_data.m_uni_skill = p_data->uni_skill;

    return 0;
}


int c_pet_data::init_base_attr_data()
{
    c_pet_config * p_config_data = pet_config_mgr::instance().get_object(m_id);
    if (NULL == p_config_data)
    {
        return -1;
    }

    container_for_each(*p_config_data, it)
    {
        pet_attr_config_t * p_data = it->second;

        c_pet_attr * p_attr = new c_pet_attr(*p_data);
        if (NULL == p_attr)
        {
            return -1;
        }

        p_attr->m_gift_count = p_data->gift_count;
        p_attr->m_badge_count = p_data->badge_count;

        m_base_attr_map.insert_object(p_attr->id2value(OBJ_ATTR_LEVEL), p_attr);

    }

    return 0;
}


int c_pet_data::init_rand_calc()
{

    c_pet_attr * p_base_attr = NULL;
    map_for_each(m_base_attr_map, it)
    {

        c_pet_attr * p_attr = it->second;
        if (1 == p_attr->id2value(OBJ_ATTR_LEVEL))
        {
            p_base_attr = it->second;
        }

        c_pet_rand_calc * p_calc = new c_pet_rand_calc();
        if (NULL == p_calc)
        {
            return -1;
        }

        if (0 != p_calc->init(this, p_attr->id2value(OBJ_ATTR_LEVEL), p_attr, p_base_attr))
        {
            return -1;
        }

        m_rand_calc_map.insert_object(p_attr->id2value(OBJ_ATTR_LEVEL), p_calc);

    }

    return 0;
}


int c_pet_data::init_train_calc()
{
    map_for_each(m_base_attr_map, it)
    {

        c_pet_attr * p_attr = it->second;

        c_pet_train_calc * p_calc = new c_pet_train_calc();
        if (NULL == p_calc)
        {
            return -1;
        }

        if (0 != p_calc->init(this, p_attr->id2value(OBJ_ATTR_LEVEL), p_attr))
        {
            return -1;
        }

        m_train_calc_map.insert_object(p_attr->id2value(OBJ_ATTR_LEVEL), p_calc);

    }


    return 0;
}

int c_pet_data::init_gift_data()
{
    c_pet_config * p_config_data = pet_config_mgr::instance().get_object(m_id);
    if (NULL == p_config_data)
    {
        return -1;
    }

    map_for_each(m_base_attr_map, it)
    {

        c_pet_attr * p_attr = it->second;
        uint32_t level = p_attr->id2value(OBJ_ATTR_LEVEL);

        pet_attr_config_t * p_attr_config = p_config_data->get_object(level);
        if (NULL == p_attr_config)
        {
            return -1;
        }


        c_pet_gift_data * p_gift = new c_pet_gift_data;
        if (NULL == p_gift)
        {
            return -1;
        }

        p_gift->m_gift_count = p_attr_config->gift_count;


        m_gift_data_map.insert_object(level, p_gift);

    }


    return 0;

}

int c_pet_data::init()
{
    if (true != m_exp2level_calc.init(m_id))
    {
        return -1;
    }

    if (0 != init_skill_data())
    {
        return -1;
    }

    if (0 != init_base_attr_data())
    {
        return -1;
    }

    if (0 != init_rand_calc())
    {
        return -1;
    }

    if (0 != init_train_calc())
    {
        return -1;
    }

    if (0 != init_gift_data())
    {
        return -1;
    }

    return 0;

}



c_pet_data::c_pet_data(uint32_t id)
{
    m_id = id;

}


int init_pet_data()
{
    if (0 != init_pet_iq_data())
    {
        return -1;
    }

    if (0 != init_pet_gift_consume_data())
    {
        return -1;
    }

    if (0 != init_pet_gift_info())
    {
        return -1;
    }

    if (0 != init_pet_merge_data())
    {
        return -1;
    }

    container_for_each(pet_config_mgr::instance(), it)
    {
        uint32_t pet_id = it->first;

        c_pet_data * p_pet = new c_pet_data(pet_id);
        if (NULL == p_pet)
        {
            return -1;
        }

        if (0 != p_pet->init())
        {
            delete p_pet;
        }
        else
        {
            pet_data_mgr::instance().insert_object(pet_id, p_pet);
            //TRACE_TLOG("[pet data] id: %u", pet_id);
        }


    }

    return 0;
}
