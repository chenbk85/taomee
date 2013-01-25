/** 
 * ========================================================================
 * @file itl_head.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-04
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <string.h>
#include <errno.h>


#include "proto.h"
#include "node.h"
#include "itl_head.h"
#include "alarm.h"
#include "arg.h"
#include "db.h"

using namespace taomee;


const uint32_t g_head_id = get_work_id();

// 打印某个node id的metric log
const uint32_t g_metric_log = config_get_intval("metric_log", 0);
// 允许连上来的node版本，0表示任意
const uint32_t g_allow_node_version = config_get_intval("node_version", 0);



int head_p_node_register(DEFAULT_ARG)
{
    int fd = *(reinterpret_cast< int * >(p_param));
    head_p_node_register_in * p_in = P_IN;

#ifdef CHECK_NODE_VERSION
    uint32_t node_version = p_in->node_version;
    if (0 != g_allow_node_version)
    {
        if (g_allow_node_version != node_version)
        {
            // 关掉连接
            net_close_cli(fd);
            return 0;
        }
    }
#endif

    uint32_t node_id = p_in->node_id;

    uint32_t node_ip = p_in->node_ip;


    c_node * p_node = find_node(node_id);
    if (NULL != p_node)
    {
        dealloc_node(p_node);
    }


    p_node = alloc_node(node_id, fd, node_ip);
    if (NULL == p_node)
    {
        net_close_cli(fd);
        return -1;
    }

    STRNCPY(p_node->m_server_tag, p_in->server_tag, SERVER_TAG_LEN);

    INFO_LOG("new incoming node, id: %u, server tag: %s, ip: %s", 
            p_node->m_node_id, p_node->m_server_tag,
            long2ip(p_node->m_node_ip));


    db_store_host_status(p_node, STATUS_HOST_UP);
    alarm_node_up(p_node);
   


    // DEBUG_LOG("new NODE, id: %u, server tag: %s, ip: %s, port: %u, command fd: %d", p->m_node_id, p->m_server_tag, p->m_command_ip, p->m_command_port, p->m_command_fd);

    return 0;
}

// int head_p_metric_data(c_node * p, const char * body, uint32_t len)
int head_p_metric_data(DEFAULT_ARG)
{
    c_node * p_node = reinterpret_cast< c_node * >(p_param);
    head_p_metric_data_in * p_in = P_IN;
    uint32_t collect_interval = p_in->collect_interval;

    uint32_t metric_count = p_in->metrics.size();

#ifdef METRIC_LOG
    bool log_flag = true;

#else
    bool log_flag = false;
#endif

    if (log_flag || (0 != g_metric_log && p_node->m_node_id == g_metric_log))
    {
        DEBUG_LOG("recieved metric data from node: %u, %s, collect interval: %u, metric count: %u", 
                p_node->m_node_id, p_node->m_server_tag, collect_interval, metric_count);

    }


    vector_for_each(p_in->metrics, it)
    {
        metric_data_t * p_data = &(*it);
        uint32_t metric_id = p_data->metric_id;

        c_metric * p_metric = p_node->m_metric->get_object(metric_id);
        if (NULL == p_metric)
        {
            p_metric = new c_metric;
            p_metric->m_node = p_node;
            p_metric->m_metric_id = metric_id;
            p_node->m_metric->insert_object(metric_id, p_metric);
        }

        STRNCPY(p_metric->m_metric_name, p_data->metric_name, MAX_METRIC_NAME_LEN);
        p_metric->m_metric_type = p_data->metric_type;
        p_metric->m_metric_class = p_data->metric_class;
        STRNCPY(p_metric->m_metric_unit, p_data->metric_unit, MAX_METRIC_UNIT_LEN);
        STRNCPY(p_metric->m_metric_fmt, p_data->metric_fmt, MAX_METRIC_FMT_LEN);
        p_metric->m_slope = p_data->slope_type;
        p_metric->m_collect_time = p_data->collect_time;
        p_metric->m_collect_interval = collect_interval;
        p_metric->set_value_type(p_data->value_type);

        char arg[MAX_METRIC_ARG_LEN] = {0};
        STRNCPY(arg, p_data->metric_arg, MAX_METRIC_ARG_LEN);
        c_metric_arg * p_arg = find_metric_arg(p_metric->m_arg_map, arg);
        if (NULL == p_arg)
        {
            p_arg = alloc_metric_arg(arg, p_metric);
            if (NULL == p_arg)
            {
                return 0;
            }
        }

        p_arg->m_fail = p_data->fail_flag;

        int index = 0;
        p_arg->m_value.unpack(&p_data->_value_len, index);


        if (log_flag || (0 != g_metric_log && p_node->m_node_id == g_metric_log))
        {
            DEBUG_LOG("\tmetric id: %u, name: %s, arg: %s, type: %u, fail: %u, value type: %u, value: %s %s", 
                    metric_id, p_metric->m_metric_name, p_arg->m_arg, 
                    p_metric->m_metric_type,
                    p_arg->m_fail,
                    p_metric->m_value_type,
                    p_arg->m_value.get_string(p_metric->m_metric_fmt),
                    p_metric->m_metric_unit);

        }
        dispatch_metric(p_arg);
    }

    return 0;
}


int head_p_get_node_data(DEFAULT_ARG)
{
    return 0;
}
