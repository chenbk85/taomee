/** 
 * ========================================================================
 * @file metric.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include <math.h>
#include "metric.h"
#include "node.h"
#include "rrd.h"
#include "db.h"
#include "alarm.h"
#include "proto.h"


using namespace taomee;


int dispatch_metric(c_metric_arg * p_arg)
{

    c_metric * p_metric = p_arg->m_metric;

    if (p_arg->m_fail)
    {
        send_metric_to_alarm(p_arg);
        return 0;
    }

    uint32_t type = p_metric->m_metric_type;

    if (type & METRIC_TYPE_ALARM)
    {
        send_metric_to_alarm(p_arg);
    }
    else
    {
        if (p_arg->m_relative)
        {
            send_metric_to_alarm(p_arg);
        }
    }
    
    if (type & METRIC_TYPE_RRD)
    {
        send_metric_to_rrd(p_arg);
    }

    if (type & METRIC_TYPE_MYSQL)
    {
        send_metric_to_db(p_arg);
    }


    return 0;
}


int pack_metric_to_rrd(void * buf, c_metric_arg * p_arg, int & index)
{
    c_metric * p_metric = p_arg->m_metric;
    // pack_h(buf, p_metric->m_metric_id, index);
    pack(buf, p_metric->m_metric_name, MAX_METRIC_NAME_LEN, index);

    uint32_t arg_len = strlen(p_arg->m_arg) + 1;
    pack_h(buf, arg_len, index);
    pack(buf, p_arg->m_arg, arg_len, index);


    const char * p_value_str = p_arg->m_value.get_string(p_metric->m_metric_fmt);
    uint32_t value_str_len = strlen(p_value_str) + 1;
    pack_h(buf, value_str_len, index);
    pack(buf, p_value_str, value_str_len, index);

    pack_h(buf, p_metric->m_slope, index);
    pack_h(buf, p_metric->m_collect_interval, index);

    return 0;

}


int pack_metric_to_alarm(void * buf, c_metric_arg * p_arg, int & index)
{
    c_metric * p_metric = p_arg->m_metric;
    pack_h(buf, p_metric->m_metric_id, index);
    pack(buf, p_metric->m_metric_name, MAX_METRIC_NAME_LEN, index);
    pack(buf, p_arg->m_arg, MAX_METRIC_ARG_LEN, index);
    pack_h(buf, p_metric->m_metric_class, index);
    pack(buf, p_metric->m_metric_unit, MAX_METRIC_UNIT_LEN, index);
    pack(buf, p_metric->m_metric_fmt, MAX_METRIC_FMT_LEN, index);
    pack_h(buf, p_metric->m_collect_time, index);
    pack(buf, p_arg->m_fail, index);
    pack_h(buf, p_metric->m_value_type, index);

    p_arg->m_value.pack(buf, index);

    return 0;

}

int pack_metric_to_db(void * buf, c_metric_arg * p_arg, int & index)
{
    
    c_metric * p_metric = p_arg->m_metric;

    pack_h(buf, p_metric->m_metric_id, index);
    pack32(buf, MAX_METRIC_NAME_LEN, index);
    pack(buf, p_metric->m_metric_name, MAX_METRIC_NAME_LEN, index);

    // arg id
    pack32(buf, 0, index);
    uint32_t arg_len = strlen(p_arg->m_arg) + 1;
    pack_h(buf, arg_len, index);
    pack(buf, p_arg->m_arg, arg_len, index);


    pack_h(buf, p_metric->m_value_type, index);

    const char * p_value_str = p_arg->m_value.get_string(p_metric->m_metric_fmt);
    uint32_t value_str_len = strlen(p_value_str) + 1;
    pack_h(buf, value_str_len, index);
    pack(buf, p_value_str, value_str_len, index);

    return 0;
}


c_metric::c_metric()
{

    m_arg_map = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free_metric_arg);
}


c_metric::~c_metric()
{
    g_hash_table_destroy(m_arg_map);

}


int c_metric::set_value_type(uint32_t value_type)
{
    if (m_value_type == value_type)
    {
        return 0;
    }

    m_value_type = value_type;

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, m_arg_map);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        c_metric_arg * p_arg = reinterpret_cast< c_metric_arg * >(value);
        if (NULL == p_arg)
        {
            continue;
        }
        p_arg->m_value.set_type(value_type);
    }
    return 0;
}
