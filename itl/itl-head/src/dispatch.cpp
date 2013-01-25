/** 
 * ========================================================================
 * @file dispatch.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



#include "dispatch.h"
#include "proto.h"
#include "node.h"
#include "itl_head.h"




int get_version();

int dispatch(int fd, const char * buf, uint32_t len)
{
    c_node * p_node = find_node_by_fd(fd);
    if (NULL != p_node)
    {
        return dispatch_node(p_node, buf, len);
    }


    const head_proto_t * pkg = reinterpret_cast<const head_proto_t *>(buf);


    uint16_t cmd = pkg->cmd;
    uint32_t seq = pkg->seq;
    uint32_t node_id = pkg->id;
    uint32_t body_len = len - sizeof(head_proto_t);

    TRACE_LOG("dispatch[%u] sender=%u, fd=%u, seq=%u, len=%u",
			cmd, node_id, fd, seq, len);

    // if (head_p_get_version_cmd == cmd)
    // {
        // return get_version();
    // }
    // else if (head_p_node_register_cmd == cmd)
    // {
        // return head_p_node_register(fd, pkg->body, body_len);
    // }
    // else if (head_p_get_node_data_cmd == cmd)
    // {
        // return head_p_get_node_data(fd, node_id);
    // }
    // else
    // {
        // p_node = find_node(node_id);
        // if (NULL == p_node)
        // {
            // ERROR_LOG("unregistered node, fd: %d, node id: %u", fd, node_id);
            // net_close_cli(fd);
            // return -1;
        // }
    // }


    const cmd_proto_t * p_cmd = find_head_cmd(cmd);
    if (NULL == p_cmd)
    {
        ERROR_LOG("cmdid not existed: %u", cmd);
        return 0;
    }


    bool read_ret = p_cmd->p_in->read_from_buf_ex(pkg->body, body_len);
    if (!read_ret) 
    {
        ERROR_LOG("read_from_buf_ex error cmd=%u, u=%u", cmd, node_id);
        return -1;
    }



    int cmd_ret = p_cmd->func(&fd, p_cmd->p_in, p_cmd->p_out);


    return cmd_ret;
}


int dispatch_node(c_node * p_node, const char * buf, uint32_t len)
{
    const head_proto_t * pkg = reinterpret_cast<const head_proto_t *>(buf);


    uint16_t cmd = pkg->cmd;
    uint32_t seq = pkg->seq;
    uint32_t body_len = len - sizeof(head_proto_t);

    TRACE_LOG("dispatch[%u] sender=%u, fd=%u, seq=%u, len=%u",
			cmd, p_node->m_node_id, p_node->m_node_fd, seq, len);

    const cmd_proto_t * p_cmd = find_head_cmd(cmd);
    if (NULL == p_cmd)
    {
        ERROR_LOG("cmdid not existed: %u", cmd);
        return 0;
    }


    bool read_ret = p_cmd->p_in->read_from_buf_ex(pkg->body, body_len);
    if (!read_ret) 
    {
        ERROR_LOG("read_from_buf_ex error cmd=%u, u=[%u: %s]", cmd, p_node->m_node_id, long2ip(p_node->m_node_ip));
        return -1;
    }



    int cmd_ret = p_cmd->func(p_node, p_cmd->p_in, p_cmd->p_out);


    return cmd_ret;
}


int get_version()
{
    return 0;
    
}
