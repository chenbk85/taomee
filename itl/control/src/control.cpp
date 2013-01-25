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

#include "node.h"
#include "itl_common.h"
#include "proto.h"
#include "control.h"

// 允许连上来的node版本，0表示任意
const uint32_t g_allow_node_version = config_get_intval("node_version", 0);


int control_p_get_version(DEFAULT_ARG)
{
    return 0;
}


int control_p_node_register(DEFAULT_ARG)
{
    return 0;
}

int control_p_node_register(int fd, const char * body, uint32_t body_len)
{
    int index = 0;
    uint32_t node_version = 0;
    taomee::unpack_h(body, node_version, index);


#ifdef CHECK_NODE_VERSION
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




    uint32_t node_id = 0;
    taomee::unpack_h(body, node_id, index);

    uint32_t node_ip = 0;
    taomee::unpack_h(body, node_ip, index);


    if (body_len != (uint32_t)index)
    {
        return -1;
    }

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


    STRNCPY(p_node->m_node_ip_str, long2ip(node_ip), sizeof(p_node->m_node_ip_str));


    INFO_LOG("new incoming node, id: %u, ip: %s", 
            p_node->m_node_id, 
            p_node->m_node_ip_str);

    return 0;
}
