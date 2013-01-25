/** 
 * ========================================================================
 * @file alarm.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

extern "C"
{
#include <libtaomee/inet/inet_utils.h>
}


#include "alarm.h"
#include "metric.h"
#include "node.h"
#include "proto.h"
#include "arg.h"



c_server g_alarm_server;
c_server * g_alarm = &g_alarm_server;

int init_connect_to_alarm()
{
    g_alarm->set_auto_reconnect(CONNECT_TO_SERVER_INTERVAL);
    g_alarm->connect(config_get_strval("alarm_ip", NULL), config_get_intval("alarm_port", 0));
    return 0;
}



int send_to_alarm(c_node * p_node, uint16_t cmd, Cmessage * p_out)
{
    alarm_proto_t head;
    uint32_t id = p_node ? p_node->m_node_id : 0;
    uint32_t seq = p_node ? p_node->m_node_ip : 0;

    init_alarm_proto_header(&head, sizeof(head), seq, cmd, 0, id);

    if (p_node)
    {
        DEBUG_LOG("send to alarm, node[%u: %s]", 
                p_node->m_node_id,
                long2ip(p_node->m_node_ip));

    }

    return g_alarm->send(&head, p_out);
}



int dispatch_alarm(int fd, const char * buf, uint32_t len)
{
    const alarm_proto_t * pkg = reinterpret_cast<const alarm_proto_t *>(buf);

    uint16_t cmd = pkg->cmd;
    uint32_t uid = pkg->id;
    uint32_t seq = pkg->seq;
    uint32_t node_ip = seq;


    TRACE_LOG("dispatch alarm[%u] sender=%u, fd=%u, seq=%u, len=%u",
            cmd, uid, fd, seq, len);

    c_node * p_node = find_node_by_ip(node_ip);
    if (NULL == p_node || p_node->m_node_id != uid)
    {
        ERROR_LOG("miss node, ip: %s, id: %u, pkg->id: %u",
                long2ip(node_ip), 
                p_node ? p_node->m_node_id : 0,
                uid);
        return -1;
    }
    const cmd_proto_t * p_cmd = find_alarm_cmd(cmd);
    if (NULL == p_cmd)
    {
        ERROR_LOG("cmdid not existed: %u", cmd);
        return 0;
    }


    uint32_t body_len = len - sizeof(alarm_proto_t);

    bool read_ret = p_cmd->p_out->read_from_buf_ex(pkg->body, body_len);
    if (!read_ret) 
    {
        ERROR_LOG("read_from_buf_ex error cmd=%u, u=%u", cmd, uid);
        return -1;
    }

    int cmd_ret = p_cmd->func(p_node, p_cmd->p_in, p_cmd->p_out);
    if (ALARM_SUCC == cmd_ret)
    {

    }

    return 0;
}


int alarm_node_up(c_node * p_node)
{
    alarm_p_node_up_in in;
    in.node_ip = p_node->m_node_ip;

    return send_to_alarm(p_node, alarm_p_node_up_cmd, &in);
}


int alarm_node_down(c_node * p_node)
{
    return send_to_alarm(p_node, alarm_p_node_down_cmd, NULL);
}

int send_metric_to_alarm(c_metric_arg * p_arg)
{
    int index = sizeof(alarm_proto_t);

    c_metric * p_metric = p_arg->m_metric;
    c_node * p_node = p_metric->m_node;

    pack_node_to_alarm(g_pkg_buf, p_node, index);

    pack_metric_to_alarm(g_pkg_buf, p_arg, index);


    init_alarm_proto_header(g_pkg_buf, index, 0, alarm_p_metric_data_cmd, 0, p_node->m_node_id);

    // DEBUG_LOG("send metric to alarm, node[%u: %s], metric[%u: %s](%s)", 
            // p_node->m_node_id,
            // long2ip(p_node->m_node_ip),
            // p_metric->m_metric_id,
            // p_metric->m_metric_name,
            // p_arg->m_arg);

    return g_alarm->send(g_pkg_buf, index);

}

int alarm_p_get_relative_metric_cb(DEFAULT_ARG)
{
    c_node * p_node = reinterpret_cast< c_node * >(p_param);
    alarm_p_get_relative_metric_out * p_out = P_OUT;


    // uint32_t node_id = p_out->node_id;
    uint32_t metric_id = p_out->metric_id;
    char * p_metric_arg = p_out->metric_arg;

    DEBUG_LOG("node[%u: %s] get relative metric[%u] arg[%s]", 
            p_node->m_node_id,
            long2ip(p_node->m_node_ip),
            metric_id, p_metric_arg);

    c_metric * p_metric = p_node->m_metric->get_object(metric_id);
    if (NULL == p_metric)
    {
        return 0;
    }

    c_metric_arg * p_arg = find_metric_arg(p_metric->m_arg_map, p_metric_arg);
    if (NULL == p_arg)
    {
        return 0;
    }
    p_arg->m_relative = true;
    send_metric_to_alarm(p_arg);



    return 0;
}
