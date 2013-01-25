/** 
 * ========================================================================
 * @file metric_group.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "proto.h"
#include "switch_macro.h"
#include "metric_group.h"

using namespace taomee;

static char g_pkg_buf[PKG_BUF_SIZE];

c_metric_group::c_metric_group(): m_inited(false), m_collect_interval(0), m_p_switch(NULL)
{
}

c_metric_group::~c_metric_group()
{
    if (m_inited)
    {
        uninit();
    }
}

int c_metric_group::init(uint32_t collect_interval, c_switch *p_switch)
{
    NEED_NOT_INIT(m_inited, "c_metric_group");
    assert(NULL != p_switch);

    m_collect_interval = SWITCH_MAX(collect_interval, SWITCH_MIN_COLLECT_INTERVAL);
    m_p_switch = p_switch;
    m_metric.clear();
    m_inited = true;

    return 0;
}

int c_metric_group::uninit()
{
    NEED_INIT(m_inited, "c_metric_group");
    if (NULL != m_p_switch)
    {
        m_p_switch->uninit();
        m_p_switch = NULL;
    }
    vector_for_each(m_metric, metric_it)
    {
        c_metric *p_metric = *metric_it;
        if (NULL != p_metric)
        {
            p_metric->uninit();
            p_metric = NULL;
        }
    }

    return 0;
}


int c_metric_group::push_metric(c_metric *p_metric)
{
    NEED_INIT(m_inited, "c_metric_group");
    assert(NULL != p_metric);
    m_metric.push_back(p_metric);
    return 0;
}


int c_metric_group::do_group_collect_and_send()
{
    NEED_INIT(m_inited, "c_metric_group");
    // 只有连接上了head才进行采集
    if (!m_p_switch->is_connected_to_head())
    {
        if (0 != m_p_switch->connect_to_head())
        {
            ERROR_LOG("switch[%s] reconnect to head[%s] failed.",
                    m_p_switch->get_switch_ip_str(), g_itl_head_ip);
            return 0;
        }
    }

    int index = sizeof(head_proto_t);
    pack_h(g_pkg_buf, m_collect_interval, index);

    // 记下metric_count的位置
    int metric_count_index = index;
    // 先打上，后面再改
    uint32_t metric_count = 0;
    pack_h(g_pkg_buf, metric_count, index);

    unsigned int switch_type = m_p_switch->get_switch_type();
    const char *switch_ip = m_p_switch->get_switch_ip_str();
    const char *switch_community = m_p_switch->get_switch_community();

    vector_for_each(m_metric, metric_it)
    {
        c_metric * p_metric = *metric_it;
        if (0 != p_metric->do_collect(switch_type, switch_ip, switch_community))
        {
            ERROR_LOG("switch[%s] collect metric[%s] failed", switch_ip, p_metric->get_metric_name());
            continue;
        }

        DEBUG_LOG("collect DONE: switch type[%u] ip[%s], metric id[%u] name[%s] arg[%s]", 
                switch_type, switch_ip,
                p_metric->get_metric_id(), 
                p_metric->get_metric_name(),
                p_metric->get_metric_arg());

        metric_count += p_metric->get_arg_map_size();

        p_metric->pack_metric_data(g_pkg_buf, index);
    }

    pack_h(g_pkg_buf, metric_count, metric_count_index);

    init_head_proto_header(g_pkg_buf, (uint32_t)index, 0, head_p_metric_data_cmd, m_p_switch->get_switch_id());
    if (-1 == m_p_switch->send_to_head(g_pkg_buf, index))
    {
        ERROR_LOG("send switch[%s] metric data to itl head failed", switch_ip);
        return -1;
    }

    return 0;
}


int init_switch_metric_group(Cmessage * c_out)
{
    db_p_get_node_switch_config_out * p_out = P_OUT;

    DEBUG_LOG("=== INIT SWITCH METRIC GROUP CONFIG ===");

    int vector_id = 0;
    uint32_t work_id = get_work_id();
    uint32_t work_num = get_work_num();
    vector_for_each(p_out->switch_confs, switch_it)
    {
        switch_node_conf_t *p_switch_conf = &(*switch_it);
        //if (p_switch_conf->switch_id % work_num != work_id)
        if (vector_id % work_num != work_id)
        {
            ++vector_id;
            continue;
        }
        ++vector_id;
        DEBUG_LOG("###switch[%u:%s] --> work_id[%u]###",
                p_switch_conf->switch_id, p_switch_conf->switch_ip, work_id);

        c_switch *p_switch = new c_switch;
        if (NULL == p_switch)
        {
            ERROR_LOG("new c_switch failed, %s", strerror(errno));
            return -1;
        }
        if (p_switch->init(
                p_switch_conf->switch_id,
                p_switch_conf->switch_type,
                p_switch_conf->switch_tag,
                p_switch_conf->switch_ip,
                p_switch_conf->switch_community) != 0)
        {
            ERROR_LOG("c_switch init failed.");
            return -1;
        }
        uint32_t switch_id = p_switch->get_switch_id();
        switch_mgr.insert_object(switch_id, p_switch);

        vector_for_each(p_switch_conf->group_info, group_it)
        {
            group_config_t * p_group_config = &(*group_it);

            // group id随意定的
            uint32_t group_id = metric_group_mgr.size();
            uint32_t collect_interval = p_group_config->collect_interval;
            if (0 == collect_interval)
            {
                collect_interval = DEFAULT_COLLECT_INTERVAL;
            }

            c_metric_group * p_group = new c_metric_group;
            if (NULL == p_group)
            {
                ERROR_LOG("new c_metric_group failed, %s", strerror(errno));
                return -1;
            }
            p_group->init(collect_interval, p_switch);
            p_switch->increase_reference_count();//增加c_switch计数

            DEBUG_LOG("group id: %u, collect interval: %u", group_id, collect_interval);

            vector_for_each(p_group_config->metric_config, metric_it)
            {
                metric_config_t * p_metric_config = &(*metric_it);

                c_metric * p_metric = new c_metric;
                if (NULL == p_metric)
                {
                    ERROR_LOG("new metric failed, %s", strerror(errno));
                    return -1;
                }

                p_metric->init(p_metric_config->metric_id, 
                        p_metric_config->metric_type,
                        ITL_TYPE_SWITCH,
                        p_metric_config->metric_name,
                        p_metric_config->metric_arg);

                DEBUG_LOG(">>>metric id: %u, name: %s, type: %u, arg: %s", 
                        p_metric->get_metric_id(),
                        p_metric->get_metric_name(),
                        p_metric->get_metric_type(),
                        p_metric->get_metric_arg());

                uint32_t metric_id = metric_mgr.size();
                metric_mgr.insert_object(metric_id, p_metric);

                p_group->push_metric(p_metric);
            }

            metric_group_mgr.insert_object(group_id, p_group);
        }
    }

    DEBUG_LOG("=== SWITCH METRIC GROUP CONFIG END ===");

    return 0;
}

int fini_metric_group()
{
    metric_group_mgr.uninit();
    metric_mgr.uninit();
    switch_mgr.uninit();

    return 0;
}
