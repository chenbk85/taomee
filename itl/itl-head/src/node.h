/** 
 * ========================================================================
 * @file node.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: 2012-10-31 11:40:38 +0800 (三, 31 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_NODE_H_2012_07_10
#define H_NODE_H_2012_07_10

extern "C"
{
#include <glib.h>
}

#include <string.h>
#include <errno.h>

#include "itl_common.h"
#include "metric.h"
#include "proto.h"

class c_node
{
    public:

        c_node()
        {
            m_node_id = 0;
            m_node_fd = -1;
        }

    public:


        uint32_t m_node_id;
        char m_server_tag[SERVER_TAG_LEN];
        // 内网ip
        uint32_t m_node_ip;
        int m_node_fd;


        uint32_t m_update_interval;

    public:

        c_object_container< uint32_t, c_metric > * m_metric;
};


#define node_mgr c_single_container(uint32_t, c_node)
// typedef singleton_default< c_object_container< uint32_t, c_node > > node_mgr;


int init_nodes();

int fini_nodes();

c_node * alloc_node(uint32_t node_id, int fd, uint32_t node_ip);

void dealloc_node(c_node * p_node);

c_node * find_node_by_fd(int fd);


c_node * find_node_by_ip(uint32_t node_ip);

c_node * find_node(uint32_t node_id);


int node_down(c_node * p_node);

int pack_node_to_alarm(void * buf, c_node * p_node, int & index);

int pack_node_to_db(void * buf, c_node * p_node, int & index);

#endif
