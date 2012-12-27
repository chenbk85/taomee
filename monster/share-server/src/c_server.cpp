/*
 * =====================================================================================
 *
 *       Filename:  server.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月18日 16时54分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <new>
#include <string.h>
#include "c_server.h"

using namespace std;

int create_server_instance(c_server **pp_instance)
{
    if (NULL == pp_instance)
    {
        return -1;
    }

    c_server* p_instance = new (std::nothrow)c_server();
    if (NULL == p_instance)
    {
        return -1;
    }
    else
    {
        *pp_instance = p_instance;
        return 0;
    }
}

c_server::c_server() : m_inited(0), m_port(0), m_fd(-1), m_bufsz(0), m_timeout(0)
{
    memset(m_ip, 0, sizeof(m_ip));
    memset(m_server_name, 0, sizeof(m_server_name));
}

c_server::~c_server()
{
    uninit();
}

int c_server::init(int bufsz, int timeout, const char *server_name, const char *ip, int port)
{
    if(m_inited)
    {
        ERROR_LOG("c_sever has been inited.");
        return -1;
    }

    if (ip != NULL)
    {
        strcpy(m_ip, ip);
        m_port = port;
        DEBUG_LOG("init connection to server:[%s:%u]", m_ip, m_port);
    }
    else
    {
        DEBUG_LOG("init connection to server:[%s]", server_name);
    }

    m_bufsz = bufsz;
    m_timeout = timeout;
    strcpy(m_server_name, server_name);

    m_inited = 1;
    return 0;
}

int c_server::uninit()
{
    if(!m_inited)
    {
        ERROR_LOG("c_sever has been uninited.");
        return -1;
    }

    if(m_fd != -1)
    {
        DEBUG_LOG("close connection from server.");
        close(m_fd);
        m_fd = -1;
    }

    memset(m_ip, 0, sizeof(m_ip));
    m_port = 0;
    m_bufsz = 0;
    m_timeout = 0;

    m_inited = 0;
    return 0;
}

int c_server::release()
{
    delete this;
    return 0;
}

int c_server::send_data(usr_info_t *p_user, const char *p_data, int data_len, TIMEOUT_CALLBACK timeout_cb)
{
    if (NULL == p_data)
    {
        ERROR_LOG("parameter cann't be NULL.");
        return -1;
    }

    if(-1 == m_fd)
    {
        if (connect_to_server() == -1)
        {
            if (p_user)
            {
                asynsvr_send_warning_msg(m_server_name, p_user->uid, p_user->waitcmd, 0, m_ip);
            }
            else
            {
                asynsvr_send_warning_msg(m_server_name, 0, 0, 0, m_ip);
            }
            return -1;
        }
    }

    int ret = net_send(m_fd, p_data, data_len);
    if (!ret && p_user && timeout_cb)
    {
        //添加定时器
        ADD_TIMER_EVENT(p_user, timeout_cb, NULL, time(NULL) + REQUEST_TIMEOUT);
    }

    return ret;
}
