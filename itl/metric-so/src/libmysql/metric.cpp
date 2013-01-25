/** 
 * ========================================================================
 * @file metric.cpp
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-09-10
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include <glib.h>
#include "instance.h"
#include "metric.h"

c_metric * alloc_metric(c_instance * p_instance, const char * metric_name, uint32_t type)
{
    c_metric * p_metric = new (std::nothrow) c_metric(type);
    if (NULL == p_metric)
    {
        ERROR_LOG("malloc metric failed");
        return NULL;
    }

    STRNCPY(p_metric->m_metric_name, metric_name, MAX_METRIC_NAME_LEN);
    p_metric->m_instance = p_instance;

    g_hash_table_insert(p_instance->m_metric_map, p_metric->m_metric_name, p_metric);
    return p_metric;
}


c_metric * find_metric(c_instance * p_instance, const char * metric_name)
{
    char name[MAX_METRIC_NAME_LEN];
    STRNCPY(name, metric_name, MAX_METRIC_NAME_LEN);
    lower_case(name);
    return reinterpret_cast< c_metric * >(g_hash_table_lookup(p_instance->m_metric_map, name));
}


void free_metric(void * p)
{
    c_metric * p_metric = reinterpret_cast< c_metric * >(p);
    delete p_metric;
}

