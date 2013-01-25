/** 
 * ========================================================================
 * @file arg.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-08-06
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <string.h>
#include <errno.h>


#include "arg.h"
#include "metric.h"
#include "itl_timer.h"


c_metric_arg * add_metric_arg(c_metric * p_metric, const char * arg)
{
    c_metric_arg * p_arg = find_metric_arg(p_metric->m_arg_map, arg);
    if (NULL == p_arg)
    {
        p_arg = alloc_metric_arg(arg, p_metric);
        if (NULL == p_arg)
        {
            return NULL;
        }
        p_arg->m_value.reset();
        g_hash_table_insert(p_metric->m_arg_map, p_arg->m_arg, p_arg);
    }

    return p_arg;
}


c_metric_arg * alloc_metric_arg(const char * p_arg, c_metric * p_metric)
{
    c_metric_arg * p = new (std::nothrow)c_metric_arg(p_metric, p_metric->get_value_type());
    if (NULL == p)
    {
        ERROR_LOG("malloc node failed, %s", strerror(errno));
        return NULL;

    }

    STRNCPY(p->m_arg, p_arg, MAX_METRIC_ARG_LEN);

    return p;
}

void free_metric_arg(void * p)
{
    c_metric_arg * p_arg = reinterpret_cast< c_metric_arg * >(p);
    delete p_arg;


}


c_metric_arg * find_metric_arg(GHashTable * p_table, const char * p_arg)
{
    return reinterpret_cast< c_metric_arg * >(g_hash_table_lookup(p_table, p_arg));
}


int do_metric_collect(c_metric_arg * p_arg)
{
    c_metric * p_metric = p_arg->m_metric;

    if (NULL == p_metric->m_so)
    {
        return -1;
    }

    int ret = p_metric->m_so->metric_handler(p_metric->m_index, p_arg->m_arg, &p_arg->m_value);
    if (0 == ret)
    {
        p_arg->m_fail = 0;
    }
    else if (-2 == ret)
    {
        return -1;
    }
    else
    {
        p_arg->m_fail = 1;
    }

    p_arg->m_collect_time = get_now_tv()->tv_sec;
    return 0;
}
