/** 
 * ========================================================================
 * @file metric.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-03
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "metric.h"

using namespace taomee;

c_metric::c_metric()
{
    m_metric_id = 0;
    m_metric_type = 0;
    m_so = NULL;
    m_index = 0;
    m_metric_info = NULL;


    m_arg_map = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free_metric_arg);
}

c_metric::~c_metric()
{
    g_hash_table_destroy(m_arg_map);
}




int pack_metric_data_to_itl_head(void * pkg, c_metric_arg * p_arg, int & index)
{


    c_metric * p_metric = p_arg->m_metric;

    // metric id
    pack_h(pkg, p_metric->get_id(), index);
    // name
    pack_string(pkg, p_metric->get_name(), MAX_METRIC_NAME_LEN, index);
    // arg
    pack_string(pkg, p_arg->m_arg, MAX_METRIC_ARG_LEN, index);
    // type
    pack_h(pkg, p_metric->get_type(), index);
    // class
    pack_h(pkg, p_metric->get_class(), index);
    // unit
    pack(pkg, p_metric->get_unit(), MAX_METRIC_UNIT_LEN, index);
    // fmt
    pack(pkg, p_metric->get_fmt(), MAX_METRIC_FMT_LEN, index);
    // slope
    pack_h(pkg, p_metric->get_slope(), index);
    // collect time
    pack_h(pkg, p_arg->m_collect_time, index);
    // fail flag
    pack(pkg, p_arg->m_fail, index);
    // value type
    pack_h(pkg, p_metric->get_value_type(), index);
    // value
    p_arg->m_value.pack(pkg, index);

    return 0;
}



int attach_so(c_metric * p_metric)
{
    container_for_each(so_file_mgr, it)
    {
        metric_so_t * p_so = it->second;
        int index = 0;
        const metric_info_t * p_metric_info = NULL;
        while (NULL != (p_metric_info = p_so->get_metric_info(index)))
        {
            if (0 == strncmp(p_metric->get_name(), p_metric_info->name, MAX_METRIC_NAME_LEN))
            {


                p_metric->m_so = p_so;
                p_metric->m_index = index;
                p_metric->m_metric_info = p_metric_info;

                DEBUG_LOG("\tmetric[%u: %s] attach so: %s value type: %u, slope: %u", 
                        p_metric->get_id(),
                        p_metric->get_name(),
                        p_so->file.m_full, 
                        p_metric->get_value_type(),
                        p_metric->get_slope());

                return 0;
            }

            index++;
        }

    }

    return -1;

}
