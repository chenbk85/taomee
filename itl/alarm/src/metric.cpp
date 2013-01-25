/** 
 * ========================================================================
 * @file metric.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-13
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "metric.h"
#include "node.h"
#include "metric_alarm.h"
#include "itl_common.h"


c_metric::c_metric(c_node * p_node)
{
    m_node = p_node;
    m_metric_id = 0;
    m_value_type = 0;

    m_alarm = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, dealloc_metric_alarm);

}

c_metric::~c_metric()
{

    if (NULL != m_alarm)
    {
        g_hash_table_destroy(m_alarm);
        m_alarm = NULL;
    }
}





int c_metric::set_value_type(uint32_t value_type)
{
    m_value_type = value_type;

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, m_alarm);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        c_metric_alarm * p_alarm = reinterpret_cast< c_metric_alarm * >(value);
        if (NULL == p_alarm)
        {
            continue;
        }

        p_alarm->m_value.set_type(value_type);
    }
    return 0;
}
