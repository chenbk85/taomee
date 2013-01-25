/** 
 * ========================================================================
 * @file control.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-16
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "control.h"
#include "itl_timer.h"
#include "node.h"
#include "db.h"
#include "proto.h"
#include "proxy.h"


c_server g_control_server;
c_server * g_control = &g_control_server;

timer_head_t g_control_event;

int connect_to_control_timely(void * owner, void * data)
{
    ADD_TIMER_EVENT(&g_control_event, connect_to_control_timely, NULL, get_now_tv()->tv_sec + GET_NODE_CONFIG_INTERVAL);

    if (0 == g_node_ip)
    {
        return 0;
    }

    if (g_control->is_connected())
    {
        return 0;
    }


    db_get_node_control_config(g_node_ip_str);

    return 0;
}



int init_connect_to_control()
{
    g_control->set_auto_reconnect(CONNECT_TO_SERVER_INTERVAL);
    g_control->set_on_connected_func(on_connected_to_control);
    connect_to_control_timely(0, 0);
    return 0;

}



int on_connected_to_control()
{
    DEBUG_LOG("reg NODE to CONTROL, node id: %u", g_node_id);

    control_p_node_register_in in;
    in.node_version = CONTROL_VERSION;
    in.node_id = g_node_id;
    in.node_ip = g_node_ip;

    uint16_t cmd = control_p_node_register_cmd;
    control_proto_t head;
    init_control_proto_header(&head, sizeof(control_proto_t), 0, cmd, 0, g_node_id);

    return g_control->send(&head, &in);
}



int dispatch_control(int fd, const char * buf, uint32_t len)
{
    g_node->send(buf, len);
    return 0;
}


int control_p_node_register_cb(DEFAULT_ARG)
{
    return 0;
}
