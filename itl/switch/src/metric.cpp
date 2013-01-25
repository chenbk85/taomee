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

extern "C"
{
#include <libtaomee/timer.h>
}

#include "switch_macro.h"
#include "proto.h"
#include "metric.h"
#include "./switch-so/metrics.h"

using namespace taomee;

c_metric::c_metric()
{
    m_metric_id = 0;
    m_metric_type = 0;
    m_so = NULL;
    m_index = 0;
    m_metric_info = NULL;
    m_inited = false;
}

c_metric::~c_metric()
{
    if (m_inited)
    {
        uninit();
    }
}

int c_metric::uninit()
{
    NEED_INIT(m_inited, "c_metric");

    m_metric_id = 0;
    m_metric_type = 0;
    m_so = NULL;
    m_index = 0;
    m_metric_info = NULL;
    g_hash_table_destroy(m_arg_map);

    m_inited = false;

    return 0;
}


int c_metric::init(
        uint32_t metric_id,
        uint32_t metric_type,
        uint32_t metric_class,
        const char *p_metric_name,
        const char *p_metric_arg)
{
    NEED_NOT_INIT(m_inited, "c_metric");
    m_metric_id = metric_id;
    m_metric_type = metric_type;
    m_metric_class = metric_class;
    STRNCPY(m_metric_arg, p_metric_arg, sizeof(m_metric_arg));
    STRNCPY(m_metric_name, p_metric_name, sizeof(m_metric_name));
    m_so = NULL;
    m_index = 0;
    m_metric_info = NULL;
    m_arg_map = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free_metric_arg);
    init_arg();
    m_inited = true;

    return 0;
}

int c_metric::init_arg()
{
    char arg[MAX_METRIC_ARG_LEN] = {0};
    memcpy(arg, m_metric_arg, MAX_METRIC_ARG_LEN);
    if (arg[0] == 0)
    {
        c_metric_arg * p_arg = alloc_metric_arg(arg, this);
        if (NULL == p_arg)
        {
            return 0;
        }
        p_arg->m_value.value_f = 0;
        g_hash_table_insert(m_arg_map, p_arg->m_arg, p_arg);
        return 0;
    }

    char * p = arg;
    char * token = NULL;
    char * save_ptr = NULL;
    while (NULL != (token = strtok_r(p, ",", &save_ptr)))
    {
        p = NULL;

        c_metric_arg * p_arg = find_metric_arg(m_arg_map, token);
        if (NULL == p_arg)
        {
            p_arg = alloc_metric_arg(token, this);
            if (NULL == p_arg)
            {
                continue;
            }
            p_arg->m_value.value_f = 0;
            g_hash_table_insert(m_arg_map, p_arg->m_arg, p_arg);
        }
    }

    return 0;
}


int c_metric::do_collect(unsigned int switch_type, const char *switch_ip, const char *switch_community)
{
    if (NULL == m_so || NULL == switch_ip || NULL == switch_community)
    {
        ERROR_LOG("m_so[%s] switch_ip[%s] switch_community[%s]",
                NULL == m_so ? "NULL" : "NOT NULL",
                NULL == switch_ip ? "NULL" : switch_ip,
                NULL == switch_community ? "NULL" : switch_community);
        return -1;
    }
    

    switch_base_info_t switch_info = {switch_type, switch_ip, switch_community, 0};
    gpointer key;
    gpointer value;
    GHashTableIter it;
    g_hash_table_iter_init(&it, m_arg_map);
    while (g_hash_table_iter_next(&it, &key, &value))
    {
        c_metric_arg * p_arg = reinterpret_cast< c_metric_arg * >(value);
        switch_info.if_num = atoi(p_arg->m_arg);
        int ret = m_so->metric_handler(m_index, (const char *)&switch_info, p_arg->m_value);
        if (0 != ret)
        {
            p_arg->m_fail = 1;
        }
        else
        {
            p_arg->m_fail = 0;
        }
    }

    return 0;
}


int c_metric::pack_metric_data(void * pkg, int & pkg_len)
{
    uint32_t collect_time = get_now_tv()->tv_sec;

    gpointer key;
    gpointer value;
    GHashTableIter it;
    g_hash_table_iter_init(&it, m_arg_map);
    while (g_hash_table_iter_next(&it, &key, &value))
    {
        c_metric_arg * p_arg = reinterpret_cast<c_metric_arg *>(value);

        // metric id
        pack_h(pkg, m_metric_id, pkg_len);
        // name
        pack_string(pkg, m_metric_name, MAX_METRIC_NAME_LEN, pkg_len);
        // arg
        pack_string(pkg, p_arg->m_arg, MAX_METRIC_ARG_LEN, pkg_len);
        // type
        pack_h(pkg, m_metric_type, pkg_len);
        // class
        pack_h(pkg, m_metric_class, pkg_len);
        // unit
        pack(pkg, get_metric_unit(), MAX_METRIC_UNIT_LEN, pkg_len);
        // fmt
        pack(pkg, get_metric_fmt(), MAX_METRIC_FMT_LEN, pkg_len);
        // slope
        pack_h(pkg, get_metric_slope(), pkg_len);
        // collect time
        pack_h(pkg, collect_time, pkg_len);
        // fail flag
        pack(pkg, p_arg->m_fail, pkg_len);
        // value type
        pack_h(pkg, get_metric_value_type(), pkg_len);
        // value
        pack_metric_value_with_len(pkg, p_arg->m_value, get_metric_value_type(), pkg_len);
    }

    return 0;
}

