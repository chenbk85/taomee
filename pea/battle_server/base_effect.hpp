/** 
 * ========================================================================
 * @file base_effect.hpp
 * @brief 
 * @version 1.0
 * @date 2012-03-14
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



#ifndef H_BASE_EFFECT_H_2012_03_14
#define H_BASE_EFFECT_H_2012_03_14

#include <libtaomee++/utils/obj_registry.hpp>
#include "pea_common.hpp"
#include "singleton.hpp"


typedef class Player c_player;


// 各种效果的id
enum effect_id_type
{
    // 免于各种buff
    EFFECT_IMMUNITY_BUFF = 1,
    // 免于各种debuff
    EFFECT_IMMUNITY_DEBUFF = 2,
    // 增加防御%
    EFFECT_ADD_DEFENCE_PERCENT = 3,
    // 减少防御%
    EFFECT_REDUCE_DEFENCE_PERCENT = 4,
    // 增加攻击
    EFFECT_ADD_ATK = 5,
    // 减少攻击
    EFFECT_REDUCE_ATK = 6,
    // 增加伤害
    EFFECT_ADD_DAMAGE = 7,
    // 减少伤害
    EFFECT_REDUCE_DAMAGE = 8,

};


// 效果触发方式
enum effect_trigger_type
{
    // 默认触发
    EFFECT_TRIGGER_NORMAL = 1,
    // 按回合触发，每回合出手前触发
    EFFECT_TRIGGER_BEFORE_TURN = 2,
    // 按回合触发，每回合出手后触发
    EFFECT_TRIGGER_AFTER_TURN = 3,
    // 攻击触发
    EFFECT_TRIGGER_ATK = 4,
    // 被攻击触发
    EFFECT_TRIGGER_BE_ATK = 5,
    // 受伤时触发
    EFFECT_TRIGGER_DAMAGE = 6,
};


struct effect_data_t
{
    effect_data_t()
    {
        memset(this, 0, sizeof(effect_data_t));
    }

    // 效果id，见enum effect_id_type
    uint32_t effect_id;
    // 效果类型
    uint32_t effect_type;
    // 触发类型，见enum effect_trigger_type
    uint32_t trigger_type;
    // 触发时间间隔
    uint32_t trigger_time;
    // 触发概率
    uint32_t trigger_rate;
    // 触发参数1
    uint32_t trigger_value;
    // 触发参数2
    uint32_t trigger_percent;
};


class c_base_effect : public base_trigger_timer
{
    public:

        c_base_effect();

        virtual ~c_base_effect();


        int init_base_effect(const effect_data_t * p_data, struct timeval begin_time);

        // 处理效果
        int process_effect(c_player * p_owner, struct timeval cur_time, c_player * p_creator);

        // 检查是否到了触发时间
        bool to_next_time(struct timeval cur_time);

        // 重置timer
        void reset_timer(struct timeval cur_time);

        inline uint32_t get_base_effect_id()
        {
            return m_effect_id;
        }

        inline uint32_t get_base_effect_type()
        {
            return m_effect_type;
        }

        inline uint32_t get_base_effect_trigger_type()
        {
            return m_trigger_type;
        }

        inline const effect_data_t * get_effect_data()
        {
            return m_effect_data;
        }


    private:

        // 效果id
        uint32_t m_effect_id;
        // 效果类型
        uint32_t m_effect_type;
        // 效果触发类型
        uint32_t m_trigger_type;
        // 触发概率
        uint32_t m_probability;
        // 配置数据
        const effect_data_t * m_effect_data;
};





typedef c_object_container<uint32_t, effect_data_t> c_effect_data_mgr;
typedef singleton_default<c_effect_data_mgr> effect_data_mgr;



// 载入效果配置
int init_effect_data(xmlNodePtr root);



class c_base_effect_mgr
{
    public:

        virtual int get_base_effect_count();

        virtual int add_base_effect(c_base_effect * p_effect);

        virtual int del_base_effect(uint32_t effect_id);

        virtual c_base_effect * get_base_effect_by_id(uint32_t effect_id);

        virtual bool is_effect_exist(uint32_t effect_id);

        // 遍历所有effect
        virtual void effect_routing(c_player * p_owner, struct timeval time, c_player * p_creator);

        // 遍历特定type的effect
        virtual void special_effect_routing(c_player * p_owner, struct timeval time, c_player * p_creator, uint32_t trigger_type);

        virtual bool is_effect_type_exist(uint32_t effect_type);

        virtual c_base_effect * get_base_effect_by_type(uint32_t effect_type);


    public:

        std::vector<c_base_effect *> m_data;

};



#endif
