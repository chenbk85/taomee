/** 
 * ========================================================================
 * @file arg.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-08-06
 * Modify $Date: 2012-10-31 17:59:21 +0800 (三, 31 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_ARG_H_2012_08_06
#define H_ARG_H_2012_08_06


extern "C"
{
#include <glib.h>
}
#include "itl_common.h"

class c_metric;

class c_metric_arg
{
    public:

        c_metric_arg(uint32_t value_type) : m_value(value_type)
        {
            memset(m_arg, 0, sizeof(m_arg));
            m_fail = 0;
            m_relative = false;
        }
    public:

        char m_arg[MAX_METRIC_ARG_LEN];
        c_value m_value;
        uint8_t m_fail;

    public:

        c_metric * m_metric;

    public:

        // 表示在告警配置中，它作为其他metric的阀值参数
        // 需要将这个metric推送给alarm服务
        bool m_relative;

};



c_metric_arg * alloc_metric_arg(const char * p_arg, c_metric * p_metric);


void free_metric_arg(void * p);


c_metric_arg * find_metric_arg(GHashTable * p_table, char * p_arg);

#endif
