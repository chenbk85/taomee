/** 
 * ========================================================================
 * @file node.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "node.h"
#include "proto.h"

using namespace taomee;

GHashTable * g_fd_node;
GHashTable * g_ip_node;


int free_node(c_node * p_node, void * param);

int init_nodes()
{
    g_fd_node = g_hash_table_new(g_int_hash, g_int_equal);
    g_ip_node = g_hash_table_new(g_int_hash, g_int_equal);
    return 0;
}


int fini_nodes()
{
    g_hash_table_destroy(g_fd_node);
    g_hash_table_destroy(g_ip_node);
    node_mgr.uninit(free_node, NULL);
    return 0;
}


c_node * alloc_node(uint32_t node_id, int fd, uint32_t node_ip)
{
    c_node * p_node = new (std::nothrow) c_node;
    if (NULL == p_node)
    {
        ERROR_LOG("malloc node failed, %s", strerror(errno));
        return NULL;
    }

    p_node->m_node_id = node_id;
    p_node->m_node_ip = node_ip;
    p_node->m_node_fd = fd;


    // 注意，fd是临时分配的空间，m_node_fd才是一直可用的
    g_hash_table_insert(g_fd_node, &(p_node->m_node_fd), p_node);

    g_hash_table_insert(g_ip_node, &(p_node->m_node_ip), p_node);
    node_mgr.insert_object(node_id, p_node);

    return p_node;
}


int free_node(c_node * p_node, void * param)
{
    delete p_node;
    return 0;
}

void dealloc_node(c_node * p_node)
{
    g_hash_table_remove(g_fd_node, &(p_node->m_node_fd));
    g_hash_table_remove(g_ip_node, &(p_node->m_node_ip));
    node_mgr.remove_object(p_node->m_node_id, free_node, NULL);
}


c_node * find_node_by_fd(int fd)
{
    c_node * p_node = reinterpret_cast<c_node *>(g_hash_table_lookup(g_fd_node, &fd));
    return p_node;
}



c_node * find_node_by_ip(uint32_t node_ip)
{
    c_node * p_node = reinterpret_cast<c_node *>(g_hash_table_lookup(g_ip_node, &node_ip));
    return p_node;
}

c_node * find_node(uint32_t node_id)
{
    return node_mgr.get_object(node_id);
}

int send_cli_to_itl_node(c_node * p_node, uint16_t cmd, Cmessage * p_out)
{
    control_proto_t head;
    init_control_proto_header(&head, sizeof(head), 0, cmd, 0, p_node->m_node_id);

    if (-1 == net_send_cli_msg(p_node->m_node_fd, &head, p_out))
    {
        ERROR_LOG("failed to send cli to itl node(id: %u), fd: %d", p_node->m_node_id, p_node->m_node_fd);
        return -1;
    }
    
    DEBUG_LOG("send to node[%u: %s], fd: %d", 
            p_node->m_node_id, 
            p_node->m_node_ip_str, 
            p_node->m_node_fd);
    return 0;
}






int node_down(c_node * p_node)
{
    INFO_LOG("NODE down, node id: %u", p_node->m_node_id);
    dealloc_node(p_node);

    return 0;
}



int notify_config_change(c_node * p_node)
{
    node_p_notify_config_change_in in;
    in.config_type = ITL_TYPE_SERVER;
    send_cli_to_itl_node(p_node, node_p_notify_config_change_cmd, &in);
    INFO_LOG("notify collect config update to node[%u: %s]", p_node->m_node_id, p_node->m_node_ip_str);

    return 0;
}


int notify_mysql_instance_change(c_node * p_node)
{
    send_cli_to_itl_node(p_node, node_p_notify_mysql_instance_change_cmd, NULL);
    INFO_LOG("notify mysql instance update to node[%u: %s]", p_node->m_node_id, p_node->m_node_ip_str);

    return 0;

}
