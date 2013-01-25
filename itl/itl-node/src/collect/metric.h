/** 
 * ========================================================================
 * @file metric.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-03
 * Modify $Date: 2012-10-25 11:26:03 +0800 (四, 25 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_METRIC_H_2012_07_03
#define H_METRIC_H_2012_07_03

#include <stdint.h>

extern "C"
{
#include <glib.h>
}
#include "arg.h"
#include "itl_common.h"


#define so_file_mgr c_single_container(uint32_t, metric_so_t)




class c_metric
{


    public:


        c_metric();
        ~c_metric();



        inline const char * get_name()
        {
            return m_metric_name;
        }

        inline const uint32_t get_id()
        {
            return m_metric_id;
        }

        // metric的type，标识head分发的方式，rrd/alarm/db
        inline const uint32_t get_type()
        {
            return m_metric_type;
        }
        
        inline const uint32_t get_class()
        {
            return m_metric_class;
        }

        // value的type，uint/string/short
        inline const uint32_t get_value_type()
        {
            if (NULL == m_metric_info)
            {
                return ITL_VALUE_BEGIN;
            }
            else
            {
                return m_metric_info->value_type;
            }
        }

        inline const c_value * get_value(const char * arg = "")
        {
            c_metric_arg * p_arg = find_metric_arg(m_arg_map, arg);
            if (NULL == p_arg)
            {
                return NULL;
            }
            else
            {
                return &p_arg->m_value;
            }
        }

        inline const uint32_t get_slope()
        {
            if (NULL == m_metric_info)
            {
                return SLOPE_CURR;
            }
            else
            {
                return m_metric_info->slope;
            }
        }

        inline const char * get_fmt()
        {
            if (NULL == m_metric_info)
            {
                return "";
            }
            else
            {
                return m_metric_info->fmt;
            }
        }

        inline const char * get_unit()
        {
            if (NULL == m_metric_info)
            {
                return "";
            }
            else
            {
                return m_metric_info->unit;
            }
        }


    public:

        uint32_t m_metric_id;
        uint32_t m_metric_type;
        // metric的种类，服务器、数据库，或者其他
        uint32_t m_metric_class;
        char m_metric_name[MAX_METRIC_NAME_LEN];

    public:

        // so
        const metric_so_t * m_so;
        // 在so中的index
        uint32_t m_index;
        // metric info
        const metric_info_t * m_metric_info;

    public:

        GHashTable * m_arg_map;

};


#define metric_mgr c_single_container(uint32_t, c_metric)



int pack_metric_data_to_itl_head(void * pkg, c_metric_arg * p_arg, int & index);

// 通过metric_name将metric和so关联起来
int attach_so(c_metric * p_metric);

#endif
