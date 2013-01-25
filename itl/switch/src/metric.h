/** 
 * ========================================================================
 * @file metric.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-03
 * Modify $Date: 2012-08-09 16:45:54 +0800 (Thu, 09 Aug 2012) $
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
// typedef singleton_default< c_object_container< uint32_t, metric_so_t > > so_file_mgr;

class c_metric
{
public:
    c_metric();
    ~c_metric();

    int init(
            uint32_t metric_id,
            uint32_t metric_type,
            uint32_t metric_class,
            const char *p_metric_name,
            const char *p_metric_arg);
    int uninit();

    int do_collect(unsigned int switch_type, const char *switch_ip, const char *switch_community);
    int pack_metric_data(void * pkg, int & pkg_len);

protected:
    int init_arg();

public:
    inline const char * get_metric_name()
    {
        return m_metric_name;
    }

    inline const uint32_t get_metric_id()
    {
        return m_metric_id;
    }

    inline const char * get_metric_arg()
    {
        return m_metric_arg;
    }

    // metric的type，标识head分发的方式，rrd/alarm/db
    inline const uint32_t get_metric_type()
    {
        return m_metric_type;
    }
    

    // value的type，uint/string/short
    inline const uint32_t get_metric_value_type()
    {
        if (NULL == m_metric_info)
        {
            return OA_VALUE_BEGIN;
        }
        else
        {
            return m_metric_info->value_type;
        }
    }

    inline const value_t get_metric_value(const char * arg = "")
    {
        c_metric_arg * p_arg = find_metric_arg(m_arg_map, arg);
        if (NULL == p_arg)
        {
            value_t v;
            v.value_f = 0;
            return v;
        }
        else
        {
            return p_arg->m_value;
        }
    }

    inline const uint32_t get_metric_slope()
    {
        if (NULL == m_metric_info)
        {
            return SLOPE_UNSPECIFIED;
        }
        else
        {
            return m_metric_info->slope;
        }
    }

    inline const char * get_metric_fmt()
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

    inline const char * get_metric_unit()
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

    inline const metric_so_t* get_metric_so()
    {
        return m_so;
    }

    inline void set_metric_so(const metric_so_t *p_metric_so)
    {
        m_so = p_metric_so;
    }

    inline uint32_t get_metric_index()
    {
        return m_index;
    }

    inline void set_metric_index(uint32_t index)
    {
        m_index = index;
    }

    inline const metric_info_t* get_metric_info()
    {
        return m_metric_info;
    }

    inline void set_metric_info(const metric_info_t *p_metric_info)
    {
       m_metric_info = p_metric_info;
    }

    inline int get_arg_map_size()
    {
        return g_hash_table_size(m_arg_map);
    }


private:
    bool m_inited;
    uint32_t m_metric_id;
    uint32_t m_metric_type;//metric类型，1告警，2rrd，4数据库，可组合使用
    uint32_t m_metric_class;//metric的种类，服务器、数据库or交换机
    char m_metric_name[MAX_METRIC_NAME_LEN];
    char m_metric_arg[MAX_METRIC_ARG_LEN];

    const metric_so_t * m_so;           /**<so*/
    uint32_t m_index;                   /**<在so中的index*/
    const metric_info_t * m_metric_info;/**<metric info*/

    GHashTable * m_arg_map;

};


#define metric_mgr c_single_container(uint32_t, c_metric)


#endif
