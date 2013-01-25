/** 
 * ========================================================================
 * @file switch.cpp
 * @brief 
 * @author tonyliu
 * @version 1.0.0
 * @date 2012-08-15
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "itl_util.h"
#include "itl_proto.h"
#include "proto.h"
#include "switch_macro.h"
#include "switch.h"

const char *g_itl_head_ip = NULL;
uint16_t g_itl_head_port = 0;

c_switch::c_switch()
{
    m_switch_id = 0;
    m_switch_type = 0;
    m_switch_ip_num = 0;
    m_collect_fail_count = 0;
    m_last_collect_time = 0;
    ITL_MEMZERO(m_switch_tag);
    ITL_MEMZERO(m_switch_ip_str);
    ITL_MEMZERO(m_switch_community);

    m_switch_fd = -1;
    m_inited = false;
}

c_switch::~c_switch()
{
    if (m_inited)
    {
        uninit();
    }
}

int c_switch::uninit()
{
    NEED_INIT(m_inited, "c_switch");

    if (m_reference_count > 0)
    {
        --m_reference_count;
        return 0;
    }

    if (m_switch_fd > 0)
    {
        close(m_switch_fd);
    }
    m_inited = false;

    return 0;
}

int c_switch::init(
        uint32_t switch_id,
        uint32_t switch_type,
        const char *p_switch_tag,
        const char *p_switch_ip,
        const char *p_switch_community)
{
    NEED_NOT_INIT(m_inited, "c_switch");

    assert(NULL != p_switch_tag && NULL != p_switch_ip && NULL != p_switch_community);

    m_switch_id = switch_id;
    m_switch_type = switch_type;
    m_switch_ip_num = ip2long(p_switch_ip);
    STRNCPY(m_switch_ip_str, p_switch_ip, sizeof(m_switch_ip_str));
    STRNCPY(m_switch_tag, p_switch_tag, sizeof(m_switch_tag));
    STRNCPY(m_switch_community, p_switch_community, sizeof(m_switch_community));
    m_inited = true;
    if (0 != connect_to_head())
    {
        ERROR_LOG("reg ITL_SWITCH[%u:%s] to ITL_HEAD[%s:%u] failed.",
                switch_id, p_switch_ip, g_itl_head_ip, g_itl_head_port);
    }
    else
    {
        DEBUG_LOG("reg ITL_SWITCH to ITL_HEAD, switch id: %u, ip: %s", switch_id, p_switch_ip);
    }

    return 0;
}


int c_switch::send_to_head(const void * buf, uint32_t len)
{
    NEED_INIT(m_inited, "c_switch");

    if (!is_connected_to_head())
    {
        if (0 != connect_to_head())
        {
            ERROR_LOG("reconnect to itl head failed");
            return -1;
        }
    }

    const head_proto_t * pkg = reinterpret_cast<const head_proto_t *>(buf);
    
    DEBUG_LOG("switch[%s] send pkg to itl_head, cmd = %u, fd = %d", m_switch_ip_str, pkg->cmd, m_switch_fd);

    return net_send_ser_ex(m_switch_fd, buf, len);
}


int c_switch::send_to_head(uint16_t cmd, Cmessage * p_out)
{
    NEED_INIT(m_inited, "c_switch");

    if (!is_connected_to_head())
    {
        if (0 != connect_to_head())
        {
            ERROR_LOG("reconnect to itl head failed");
            return -1;
        }
    }

    head_proto_t head;
    init_head_proto_header(&head, sizeof(head_proto_t), 0, cmd, m_switch_id);

    DEBUG_LOG("send ser to itl_head, cmd = %u, fd = %d", cmd, m_switch_fd);

    return net_send_ser_msg(m_switch_fd, &head, p_out);
}


int c_switch::send_register_message()
{
    NEED_INIT(m_inited, "c_switch");

    head_p_node_register_in in;
    in.node_id = m_switch_id;
    STRNCPY(in.server_tag, m_switch_tag, SERVER_TAG_LEN);
    in.node_port = config_get_intval("bind_port", 0);
    in.node_ip = m_switch_ip_num;

    return send_to_head(head_p_node_register_cmd, &in);
}


bool is_switch_head_fd(int fd)
{
    container_for_each(switch_mgr, switch_it)
    {
        c_switch *p_switch = switch_it->second;
        if (fd == p_switch->get_switch_fd())
        {
            return true;
        }
    }

    return false;
}

void close_switch_head_fd(int fd)
{
    container_for_each(switch_mgr, switch_it)
    {
        c_switch *p_switch = switch_it->second;
        if (fd == p_switch->get_switch_fd())
        {
            TRACE_LOG("set switch[%s] fd from %d to -1", p_switch->get_switch_ip_str(), p_switch->get_switch_fd());
            p_switch->set_switch_fd(-1);
            break;
        }
    }
}


int dispatch_switch_head(int fd, const char * buf, uint32_t len)
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

    int cmd_ret = p_cmd->func(fd, p_cmd->p_in, p_cmd->p_out, body_len);
    if (HEAD_SUCC == cmd_ret)
    {

    }

    return 0;
}

