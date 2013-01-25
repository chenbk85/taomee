/** 
 * ========================================================================
 * @file node.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: 2012-11-15 18:29:06 +0800 (四, 15 11月 2012) $
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
#include "proto.h"

class c_node
{
    public:

        c_node()
        {
            m_node_id = 0;
            m_node_ip = 0;
            memset(m_node_ip_str, 0, sizeof(m_node_ip_str));
            m_node_fd = -1;
        }

    public:


        uint32_t m_node_id;
        // 内网ip
        uint32_t m_node_ip;
        char m_node_ip_str[INET_ADDRSTRLEN];
        int m_node_fd;



};


#define node_mgr c_single_container(uint32_t, c_node)


int init_nodes();

int fini_nodes();

c_node * alloc_node(uint32_t node_id, int fd, uint32_t node_ip);

void dealloc_node(c_node * p_node);

c_node * find_node_by_fd(int fd);


c_node * find_node_by_ip(uint32_t node_ip);

c_node * find_node(uint32_t node_id);

int send_cli_to_itl_node(c_node * p_node, uint16_t cmd, Cmessage * p_out);


int get_link_nodes_num();


int node_down(c_node * p_node);

int notify_config_change(c_node * p_node);

int notify_mysql_instance_change(c_node * p_node);
#endif
