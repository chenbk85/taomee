/** 
 * ========================================================================
 * @file server.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-15
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



extern "C"
{
#include <libtaomee/inet/inet_utils.h>
}

#include "server.h"
#include "async_server.h"



c_server::c_server()
{
    m_fd = -1;
    memset(m_ip, 0, sizeof(m_ip));
    m_port = 0;
    m_auto_reconnect_interval = 0;
    m_reconnect_timer = NULL;
}

c_server::c_server(uint32_t auto_reconnect_interval)
{
    c_server();
    set_auto_reconnect(auto_reconnect_interval);
}

c_server::c_server(const char * ip, uint32_t port, uint32_t auto_reconnect_interval)
{
    c_server();

    connect(ip, port);
    set_auto_reconnect(auto_reconnect_interval);
}



c_server::~c_server()
{
    disconnect();
}



int c_server::connect(const char * ip, uint32_t port)
{

    if (!is_legal_ip(ip) || !is_legal_port(port))
    {
        return -1;
    }

    strncpy(m_ip, ip, sizeof(m_ip));
    m_port = port;
    return connect();
}



int c_server::connect()
{
    if (!is_legal_ip(m_ip) || !is_legal_port(m_port))
    {
        return -1;
    }

    m_fd = net_connect_ser(m_ip, m_port, 0);
    if (is_connected())
    {
        DEBUG_LOG("net_connect_ser: %s:%u, fd: %d", m_ip, m_port, m_fd);
        if (NULL != m_on_connected_func)
        {
            m_on_connected_func();
        }
        return 0;
    }
    else
    {
        return -1;
    }
}


int c_server::disconnect()
{
    if (is_connected())
    {
        net_close_ser(m_fd);
        m_fd = -1;
    }
    return 0;
}


int do_connect_timely(void * owner, void * data)
{
    c_server * p_server = reinterpret_cast< c_server * >(owner);
    if (NULL == p_server)
    {
        return 0;
    }

    return p_server->connect_timely();
}


int c_server::connect_timely()
{
    if (!is_connected())
    {
        if (0 != connect())
        {
            m_reconnect_timer = ADD_TIMER_EVENT(this, do_connect_timely, NULL, get_now_tv()->tv_sec + m_auto_reconnect_interval);
        }
    }

    return 0;
}


int c_server::on_server_down()
{
    m_fd = -1;
    if (m_auto_reconnect_interval)
    {
        connect_timely();
    }
    return 0;
}



int c_server::set_auto_reconnect(uint32_t interval)
{
    if (0 == interval)
    {
        m_auto_reconnect_interval = 0;
        if (m_reconnect_timer)
        {
            REMOVE_TIMER(m_reconnect_timer);
        }
    }
    else
    {
        m_auto_reconnect_interval = interval;
        if (!is_connected())
        {
            connect_timely();
        }
    }

    return 0;
}

int c_server::send(const void * buf, uint32_t len)
{
    if (!is_connected())
    {
        if (0 != connect())
        {
            return -1;
        }
    }

    return net_send_ser(m_fd, buf, len);
}

int c_server::send(const void * head, Cmessage * p_out)
{
    if (!is_connected())
    {
        if (0 != connect())
        {
            return -1;
        }
    }

    return net_send_ser_msg(m_fd, head, p_out);
}


int c_server::dispatch(dispatch_func_t dispatch_func, const char * buf, uint32_t len)
{
    if (NULL == dispatch_func)
    {
        return -1;
    }

    return dispatch_func(m_fd, buf, len);
}

