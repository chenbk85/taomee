/** 
 * ========================================================================
 * @file head.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-31
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "head.h"
#include "proto.h"
#include "itl_common.h"



char g_pkg_buf[1024];

using namespace taomee;

int alarm_get_relative_metric(c_metric_alarm * p_alarm, uint32_t metric_id)
{
    c_node * p_node = p_alarm->m_node;
    DEBUG_LOG("get relative metric[%u] arg[%s]",
            metric_id,
            p_alarm->m_arg);
    int index = sizeof(alarm_proto_t);
    pack_h(g_pkg_buf, p_node->m_node_id, index);
    pack_h(g_pkg_buf, metric_id, index);
    pack(g_pkg_buf, p_alarm->m_arg, MAX_METRIC_ARG_LEN, index);

    init_alarm_proto_header(g_pkg_buf, index, p_node->m_node_ip, alarm_p_get_relative_metric_cmd, 0, p_node->m_node_id);





    return net_send_cli(p_node->m_head_fd, g_pkg_buf, index);
}
