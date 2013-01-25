/** 
 * ========================================================================
 * @file metric.h
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-09-07
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_METRIC_H_2012_09_07
#define H_METRIC_H_2012_09_07

extern "C"
{
#include <libtaomee/utils.h>
}
#include "itl_common.h"

// 收集的metric信息
static metric_info_t g_metrics_info[] = 
{
#define METRIC(name, type, unit, fmt, slope) \
    {#name, type, unit, fmt, slope},
#include "metric_define.h"
#undef METRIC
};

const uint32_t METRIC_NUM = array_elem_num(g_metrics_info);

class c_instance;

class c_metric
{
    public:

        c_metric(uint32_t type) : m_value(type), m_last_value(type)
        {
            m_instance = NULL;
            memset(m_metric_name, 0, MAX_METRIC_NAME_LEN);
            m_type = type;
            m_slope = 0;
            m_new = false;
            m_initial = true;

        }

    public:

        c_instance * m_instance;

        char m_metric_name[MAX_METRIC_NAME_LEN];
        uint32_t m_type;
        uint32_t m_slope;

    public:

        // 表示数据是否是最新的
        bool m_new;
        // false表示数据经过初始化，true表示数据还没有赋值过
        bool m_initial;
        c_value m_value;
        c_value m_last_value;
        // 上一次采集的时间
        uint32_t m_last_collect_time;
};


c_metric * alloc_metric(c_instance * p_instance, const char * metric_name, uint32_t type);

c_metric * find_metric(c_instance * p_instance, const char * metric_name);

void free_metric(void * p);

#endif
