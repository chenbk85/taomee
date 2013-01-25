/** 
 * ========================================================================
 * @file arg.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-08-06
 * Modify $Date: 2012-10-25 11:26:03 +0800 (四, 25 10月 2012) $
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

        c_metric_arg(c_metric * p_metric, uint32_t metric_type) : m_value(metric_type)
        {
            m_metric = p_metric;
            memset(m_arg, 0, sizeof(m_arg));
            m_fail = 0;
            m_collect_time = 0;
        }




    public:

        char m_arg[MAX_METRIC_ARG_LEN];
        c_value m_value;

        uint8_t m_fail;
        uint32_t m_collect_time;

    public:

        c_metric * m_metric;


};


c_metric_arg * add_metric_arg(c_metric * p_metric, const char * arg);

c_metric_arg * alloc_metric_arg(const char * p_arg, c_metric * p_metric);


void free_metric_arg(void * p);


c_metric_arg * find_metric_arg(GHashTable * p_table, const char * p_arg);

int do_metric_collect(c_metric_arg * p_arg);

#endif
