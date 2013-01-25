/** 
 * ========================================================================
 * @file dispatch.cpp
 * @brief 
 * @version 1.0
 * @date 2012-7-11
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "proto.h"
#include "dispatch.h"
#include "node.h"
#include "db.h"
#include "alarm.h"


list_head_t g_awaiting_node;

using namespace taomee;

int get_version();

void cache_pkg(c_node * p_node, const void * buf, uint32_t len);


int dispatch(int fd, const char * buf, uint32_t len)
{
    const alarm_proto_t * pkg = reinterpret_cast<const alarm_proto_t *>(buf);

    uint16_t cmd = pkg->cmd;
    uint32_t seq = pkg->seq;
    uint32_t uid = pkg->id;

    TRACE_LOG("dispatch[%u] sender=%u, fd=%u, seq=%u, len=%u",
            cmd, uid, fd, seq, len);

    uint32_t node_id = uid;
    c_node * p_node = NULL;


    if (alarm_p_get_version_cmd == cmd)
    {
        return get_version();
    }
    else if (alarm_p_change_node_alarm_config_cmd == cmd)
    {
        c_node * p_node = find_node(node_id);
        if (NULL == p_node)
        {
            return 0;
        }

        return alarm_p_change_node_alarm_config(p_node, NULL, NULL);
    }
    else if (alarm_p_node_up_cmd == cmd)
    {
        uint32_t node_ip = 0;
        int index = 0;
        unpack_h(pkg->body, node_ip, index);

        DEBUG_LOG("new NODE: %u, ip: %s", node_id, long2ip(node_ip));

        p_node = find_node(node_id);
        if (NULL != p_node)
        {
            if (!p_node->m_node_running)
            {
                p_node->m_node_running = true;
                // 一般是重启了下itl node，不用发告警
            }

            if (!p_node->m_node_alive)
            {
                // itl_node从挂掉中恢复
                p_node->m_node_alive = true;
                send_node_alarm(p_node);
            }
            
            if (!p_node->m_host_alive)
            {
                // 主机恢复
                p_node->m_host_alive = true;
                send_host_alarm(p_node);
            }

            dealloc_node(p_node);
        }

        p_node = alloc_node(node_id, fd);
        if (NULL == p_node)
        {
            return -1;
        }
        p_node->m_host_alive = true;
        p_node->m_node_alive = true;
        p_node->m_node_running = true;
        p_node->m_node_ip = node_ip;

        p_node->m_waitcmd = cmd;
        return alarm_p_node_up(p_node, NULL, NULL);
    }

    p_node = find_node(node_id);
    if (NULL == p_node)
    {
        DEBUG_LOG("new NODE: %u", node_id);
        // 新的node，要去拉取相关配置
        p_node = alloc_node(node_id, fd);
        if (NULL == p_node)
        {
            net_close_cli(fd);
            return -1;
        }

        
        p_node->m_host_alive = true;
        p_node->m_node_alive = true;
        p_node->m_node_running = true;


        // 记下cmd
        p_node->m_waitcmd = cmd;
        // 缓存报文
        // 等待拉取到告警配置
        cache_pkg(p_node, buf, len);
        return db_get_node_alarm_config(p_node);
    }

    if (!p_node->m_node_running)
    {
        p_node->m_node_running = true;
        // 一般是重启了下itl node，不用发告警
    }

    if (!p_node->m_node_alive)
    {
        // itl_node从挂掉中恢复
        p_node->m_node_alive = true;
        send_node_alarm(p_node);
    }


    if (0 != p_node->m_waitcmd)
    {
        uint32_t cache_queue_len = g_queue_get_length(p_node->m_pkg_queue);
        if (cache_queue_len < 300)
        {
            WARN_LOG("cache pkg, node id: %u, waitcmd: %u, cached len: %u", p_node->m_node_id, p_node->m_waitcmd, cache_queue_len);
            cache_pkg(p_node, buf, len);
            return 0;
        }
        else
        {
            ERROR_LOG("wait for cmd: %u, node id: %u, newcmd; %u, cached len: %u", p_node->m_waitcmd, p_node->m_node_id, cmd, cache_queue_len);
            // 清理掉，防止无线等待
            p_node->clear_waitcmd();
            return 0;
        }
    }


    if (!p_node->m_inited)
    {
        // 没有初始化告警配置的也要去拉
        // 记下cmd
        p_node->m_waitcmd = cmd;
        // 缓存报文
        // 等待拉取到告警配置
        cache_pkg(p_node, buf, len);
        return db_get_node_alarm_config(p_node);
    }




    const cmd_proto_t * p_cmd = find_alarm_cmd(cmd);
    if (NULL == p_cmd)
    {
        ERROR_LOG("cmdid not existed: %u", cmd);
        return 0;
    }

    uint32_t body_len = len - sizeof(alarm_proto_t);

    bool read_ret = p_cmd->p_in->read_from_buf_ex(pkg->body, body_len);
    if (!read_ret)
    {
        ERROR_LOG("read_from_buf_ex error cmd=%u, u=%u", cmd, uid);
        return -1;
    }


    p_node->m_waitcmd = cmd;

    int cmd_ret = p_cmd->func(p_node, p_cmd->p_in, p_cmd->p_out);

    // if (ALARM_SUCC == cmd_ret)
    // {
    // }

    return cmd_ret;
}


void proc_cached_pkg()
{
    list_head_t * cur;
    list_head_t * next;
    list_for_each_safe(cur, next, &g_awaiting_node)
    {
        c_node * p_node = list_entry(cur, c_node, m_waitcmd_hook);
        while (0 == p_node->m_waitcmd)
        {
            cached_pkg_t * pkg = reinterpret_cast< cached_pkg_t * >(g_queue_pop_head(p_node->m_pkg_queue));
            if (NULL != pkg)
            {
                uint32_t pkg_len = pkg->len - sizeof(cached_pkg_t);
                int ret = dispatch(p_node->m_head_fd, pkg->pkg, pkg_len);
                g_slice_free1(pkg->len, pkg);
                if (ret)
                {
                    net_close_cli(p_node->m_head_fd);
                    break;
                }

            }
            if (g_queue_is_empty(p_node->m_pkg_queue))
            {
                list_del_init(&(p_node->m_waitcmd_hook));
                break;
            }
        }
    }

}


void cache_pkg(c_node * p_node, const void * buf, uint32_t len)
{
    DEBUG_LOG("cache pkg, len: %u, node id: %u", len, p_node->m_node_id);
    uint32_t cache_len = sizeof(cached_pkg_t) + len;
    cached_pkg_t * cache = reinterpret_cast< cached_pkg_t * >(g_slice_alloc(cache_len));
    cache->len = cache_len;
    memcpy(cache->pkg, buf, len);
    g_queue_push_tail(p_node->m_pkg_queue, cache);
    if (list_empty(&(p_node->m_waitcmd_hook)))
    {
        list_add_tail(&(p_node->m_waitcmd_hook), &g_awaiting_node);
    }
}


int get_version()
{
    return 0;
}
