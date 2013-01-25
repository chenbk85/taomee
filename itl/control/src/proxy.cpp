/** 
 * ========================================================================
 * @file proxy.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-18
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "proxy.h"
#include "db.h"



#define PROXY_TIMEOUT   (60)

struct proxy_info_t
{
    proxy_info_t()
    {
        is_use = false;
        use_time = 0;
        cli_sock = -1;
        svr_sock = -1;
        seq = 0;
        cmd = 0;
        id = 0;
    }
    bool is_use;
    uint32_t use_time;
    int cli_sock;
    int svr_sock;
    uint32_t seq;
    uint16_t cmd;
    uint32_t id;
};

std::vector<proxy_info_t> g_proxy_tbl(65536 * 8);






int make_seq()
{
    uint32_t now = get_now_tv()->tv_sec;
    static uint32_t idx = 0;
    for (uint32_t i = 0; i < g_proxy_tbl.size(); i++) 
    {
        if (idx >= g_proxy_tbl.size())
        {
            idx = 0;
        }

        if (g_proxy_tbl[idx].is_use) 
        {
            if (g_proxy_tbl[idx].use_time + PROXY_TIMEOUT < now)
            {
                // 超时未返回
                return idx++;
            }
            else
            {
                idx++;
            }
        }
        else
        {
            return idx++;
        }

    }

    return -1;
}


// 处理来自node的报文
int dispatch_from_node(c_node * p_node, const char * buf, uint32_t len)
{
    node_proto_t * pkg = ( node_proto_t * )(buf);


    uint32_t seq = pkg->seq;
    proxy_info_t * p_info = &g_proxy_tbl[seq];
    if (!p_info->is_use)
    {
        return -1;
    }

    p_info->is_use = false;

    pkg->seq = p_info->seq;

    DEBUG_LOG("send to cli, fd: %d", p_info->cli_sock);
    net_send_cli(p_info->cli_sock, buf, len);

    return 0;
}



// 处理要发给node的报文
int dispatch_to_node(int fd, const c_node * p_node, const char * buf, uint32_t len)
{
    int seq = make_seq();
    if (-1 == seq)
    {
        ERROR_LOG("too many msg, seq full");
        net_send_cli(fd, buf, len);
        return -1;
    }

    node_proto_t * pkg = ( node_proto_t * )(buf);

    proxy_info_t * p_info = &g_proxy_tbl[seq];
    p_info->is_use = true;
    p_info->use_time = get_now_tv()->tv_sec;
    p_info->cli_sock = fd;
    p_info->svr_sock = p_node->m_node_fd;
    p_info->seq = pkg->seq;
    p_info->cmd = pkg->cmd;
    p_info->id = pkg->id;
    pkg->seq = seq;

    net_send_cli(p_node->m_node_fd, buf, len);

    return 0;


}




int clear_node_proxy(c_node * p_node)
{
    int fd = p_node->m_node_fd;
    vector_for_each(g_proxy_tbl, it)
    {
        proxy_info_t * p_info = &(*it);
        if (p_info->is_use && p_info->svr_sock == fd)
        {
            node_proto_t head;
            init_node_proto_header(&head, sizeof(head), p_info->seq, p_info->cmd, NODE_ERR_RESET, p_info->id);
            net_send_cli(p_info->cli_sock, &head, head.len);
            p_info->is_use = false;
        }
    }

    return 0;
}
