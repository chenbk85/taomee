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
#include "alarm.h"
#include "db.h"

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
    c_node * p_node = reinterpret_cast< c_node * >(g_slice_alloc0(sizeof(c_node)));
    if (NULL == p_node)
    {
        ERROR_LOG("malloc node failed, %s", strerror(errno));
        return NULL;
    }

    p_node->m_node_id = node_id;
    p_node->m_node_ip = node_ip;
    p_node->m_node_fd = fd;

    p_node->m_metric = new (typeof(*(p_node->m_metric)));

    // 注意，fd是临时分配的空间，m_node_fd才是一直可用的
    g_hash_table_insert(g_fd_node, &(p_node->m_node_fd), p_node);

    g_hash_table_insert(g_ip_node, &(p_node->m_node_ip), p_node);
    node_mgr.insert_object(node_id, p_node);

    return p_node;
}


int free_node(c_node * p_node, void * param)
{
    p_node->m_metric->uninit();
    delete p_node->m_metric;

    g_slice_free1(sizeof(c_node), p_node);
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



int node_down(c_node * p_node)
{
    INFO_LOG("NODE down, node id: %u", p_node->m_node_id);
    db_store_host_status(p_node, STATUS_NODE_DOWN);
    alarm_node_down(p_node);
    dealloc_node(p_node);

    return 0;
}



int pack_node_to_alarm(void * buf, c_node * p_node, int & index)
{
    pack_h(buf, p_node->m_node_id, index);
    pack_h(buf, p_node->m_node_ip, index);
    return 0;
}


int pack_node_to_db(void * buf, c_node * p_node, int & index)
{

    pack_h(buf, p_node->m_node_id, index);
    pack32(buf, SERVER_TAG_LEN, index);
    pack(buf, p_node->m_server_tag, SERVER_TAG_LEN, index);

    return 0;
}

