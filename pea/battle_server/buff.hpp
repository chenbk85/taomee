/** 
 * ========================================================================
 * @file buff.hpp
 * @brief 
 * @version 1.0
 * @date 2012-03-15
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_BUFF_H_2012_03_15
#define H_BUFF_H_2012_03_15

#include <libtaomee++/utils/obj_registry.hpp>
#include "pea_common.hpp"
#include "singleton.hpp"
#include "base_effect.hpp"
#include <vector>

#define MAX_EFFECT_NUM_PER_BUFF     5


enum buff_type
{
    // 正常buff
    BUFF_TYPE_NORMAL = 1,
    // debuff
    BUFF_TYPE_DEBUFF = 2,
};


enum buff_end_type
{
    // 按次数结束
    BUFF_END_TIMES = 1,
    // 按触发回合数结束
    BUFF_END_TURN = 1<<1,
    // 按次数或者回合任意一条件结束
    BUFF_END_TIMES_OR_TURN = 1<<2,
};


enum buff_delete_reason
{
    BUFF_DELETE_DEAD = 1,
    BUFF_DELETE_STAGE_RESTART = 1<<1,
};


struct buff_data_t
{
    buff_data_t()
    {
        buff_id = 0;
        buff_type = 0;
        end_type = 0;
        trigger_times = 0;
        icon = 0;
        category = 0;
        level = 0;
        effect.clear();
    }

    uint32_t buff_id;
    // buff类型，见enum buff_type
    uint32_t buff_type;
    // buff结束类型，见enum buff_end_type
    uint32_t end_type;
    // buff触发次数限制
    uint32_t trigger_times;
    // buff持续回合数
    uint32_t turn_times;
    // buff图标索引
    uint32_t icon;
    // buff种类，默认0无种类，添加时无限制，其他的高等级覆盖低等级
    uint32_t category;
    // buff等级,加入同个category时，level高的替换level低的
    uint32_t level;

    // buff的效果
    std::vector<uint32_t> effect;
};


class c_buff : public c_base_effect_mgr, public base_duration_timer, public taomee::MemPool
{
    public:

        c_buff(const buff_data_t * p_data) : 
            m_buff_id(p_data->buff_id), m_category(p_data->category), m_level(p_data->level)
        {

        }

        inline uint32_t get_buff_id()
        {
            return m_buff_id;
        }

        inline uint32_t get_buff_category()
        {
            return m_category;
        }

        inline uint32_t get_buff_level()
        {
            return m_level;
        }

        inline uint32_t get_creator_id()
        {
            return m_creator_id;
        }

        inline void set_creator_id(uint32_t id)
        {
            m_creator_id = id;
        }

        inline uint32_t get_end_type()
        {
            return m_end_type;
        }

        inline void set_end_type(uint32_t end_type)
        {
            m_end_type = end_type;
        }

        inline bool is_buff_end(struct timeval cur_time)
        {
           // if (m_end_type & BUFF_END_TIME)
            {
          //      return is_timer_finish(cur_time);
            }

            if (m_end_type & BUFF_END_TIMES)
            {
                return (m_trigger_times <= 0);
            }

            return false;
        }


        inline int32_t dec_buff_times(uint32_t times = 1)
        {
            m_trigger_times -= times;
            return m_trigger_times;
        }


        inline void set_trigger_times(int32_t times)
        {
            m_trigger_times = times;
        }

        inline int32_t get_trigger_times()
        {
            return m_trigger_times;
        }

        inline void reset()
        {
            struct timeval begin_time = *get_now_tv();
            reset_timer(begin_time);
        }

        inline uint32_t get_buff_type()
        {
            return m_buff_type;
        }

        inline void set_buff_type(uint32_t buff_type)
        {
            m_buff_type = buff_type;
        }

    public:

        uint32_t m_buff_id;
        uint32_t m_category;
        uint32_t m_level;
        uint32_t m_end_type;
        uint32_t m_creator_id;
        int32_t m_trigger_times;
        uint32_t m_buff_type;
        uint32_t m_icon;
};




typedef c_object_container<uint32_t, buff_data_t> c_buff_data_mgr;
typedef singleton_default<c_buff_data_mgr> buff_data_mgr;



// 载入buff配置
int init_buff_data(xmlNodePtr root);



c_buff * create_buff(uint32_t buff_id);

void destroy_buff(c_buff * p_buff);

#endif
