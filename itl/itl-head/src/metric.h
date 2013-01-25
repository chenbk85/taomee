/** 
 * ========================================================================
 * @file metric.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: 2012-10-11 11:58:14 +0800 (四, 11 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_METRIC_H_2012_07_12
#define H_METRIC_H_2012_07_12


extern "C"
{
#include <glib.h>
}

#include "itl_common.h"
#include "arg.h"

class c_node;


class c_metric
{
    public:

        c_metric();

        ~c_metric();

        int set_value_type(uint32_t value_type);

        uint32_t m_metric_id;
        char m_metric_name[MAX_METRIC_NAME_LEN];
        char m_metric_unit[MAX_METRIC_UNIT_LEN];
        char m_metric_fmt[MAX_METRIC_FMT_LEN];
        uint32_t m_metric_type;
        uint32_t m_metric_class;
        uint32_t m_slope;
        uint32_t m_collect_time;
        uint32_t m_collect_interval;
        uint32_t m_value_type;

    public:

        c_node * m_node;

    public:

        GHashTable * m_arg_map;


};


int dispatch_metric(c_metric_arg * p_arg);

int pack_metric_to_rrd(void * buf, c_metric_arg * p_arg, int & index);

int pack_metric_to_alarm(void * buf, c_metric_arg * p_arg, int & index);


int pack_metric_to_db(void * buf, c_metric_arg * p_arg, int & index);

#endif
