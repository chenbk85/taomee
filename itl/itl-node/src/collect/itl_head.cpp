/** 
 * ========================================================================
 * @file itl_head.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-06
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <libtaomee/inet/inet_utils.h>

#include "itl_head.h"
#include "itl_timer.h"
#include "../node.h"
#include "../proto.h"
#include "../db.h"
#include "collect.h"
#include "server.h"

c_server g_head_server;
c_server * g_head = &g_head_server;

timer_head_t g_head_event;

int connect_to_head_timely(void * owner, void * data)
{
    ADD_TIMER_EVENT(&g_head_event, connect_to_head_timely, NULL, get_now_tv()->tv_sec + GET_NODE_CONFIG_INTERVAL);

    if (0 == g_node_ip)
    {
        return 0;
    }

    if (g_head->is_connected())
    {
        return 0;
    }

    db_get_node_head_config(g_node_ip_str);

    return 0;
}

int init_connect_to_head()
{
    g_head->set_auto_reconnect(CONNECT_TO_SERVER_INTERVAL);
    g_head->set_on_connected_func(on_connected_to_head);
    connect_to_head_timely(0, 0);
    return 0;
}

int on_connected_to_head()
{
    if (g_auto_update_flag)
    {
        // 强制检查一下更新
        if (check_update())
        {
            restart_node();
        }
    }

    DEBUG_LOG("reg NODE to HEAD, node[%u: %s], server tag: %s", g_node_id, g_node_ip_str, g_server_tag);

    head_p_node_register_in in;
    in.node_version = HEAD_VERSION;
    in.node_id = g_node_id;
    STRNCPY(in.server_tag, g_server_tag, SERVER_TAG_LEN);
    in.node_ip = g_node_ip;

    uint32_t cmd = head_p_node_register_cmd;
    head_proto_t head;
    init_head_proto_header(&head, sizeof(head_proto_t), 0, cmd, 0, g_node_id);

    return g_head->send(&head, &in);
}




int dispatch_head(int fd, const char * buf, uint32_t len)
{
    const head_proto_t * pkg = reinterpret_cast<const head_proto_t *>(buf);

    uint16_t cmd = pkg->cmd;
    uint32_t uid = pkg->id;
    uint32_t seq = pkg->seq;


    TRACE_LOG("dispatch itl head[%u] sender=%u, fd=%u, seq=%u, len=%u",
            cmd, uid, fd, seq, len);

    const cmd_proto_t * p_cmd = find_head_cmd(cmd);
    if (NULL == p_cmd)
    {
        ERROR_LOG("cmdid not existed: %u", cmd);
        return 0;
    }


    uint32_t body_len = len - sizeof(head_proto_t);

    bool read_ret = p_cmd->p_out->read_from_buf_ex(pkg->body, body_len);
    if (!read_ret) 
    {
        ERROR_LOG("read_from_buf_ex error cmd=%u, u=%u", cmd, uid);
        return -1;
    }

    int cmd_ret = p_cmd->func(fd, p_cmd->p_in, p_cmd->p_out);
    if (HEAD_SUCC == cmd_ret)
    {

    }

    return 0;

}


int head_p_metric_data_cb(DEFAULT_ARG)
{
    return 0;
}


int head_p_node_register_cb(DEFAULT_ARG)
{
    return 0;
}
