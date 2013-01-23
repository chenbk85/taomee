/**
 * @file udp_io_event.cpp
 * @brief udp io事件类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-31
 */

#include "log.h"
#include "inet_address.h"

#include "udp_io_event.h"


int udp_io_event::udp_on_new_conn(connection<sock_dgram>* p_conn)
{
    fdsession_t udp_fdsess;
    udp_fdsess.id = p_conn->get_connection_id();
    //udp_fdsess.ip_addr = "NULL";
    udp_fdsess.port = 0;
    if (m_udp_conn_comple_cb_)
        m_udp_conn_comple_cb_(&udp_fdsess);

    return 0;
}

int udp_io_event::udp_on_message(connection<sock_dgram>* p_conn, void* data, int len)
{
    fdsession_t udp_fdsess;
    udp_fdsess.id = p_conn->get_connection_id();
    udp_fdsess.ip_addr = (p_conn->get_remote_addr()).get_ip_addr();
    udp_fdsess.port = (p_conn->get_remote_addr()).get_port();

    TRACE_LOG("udp on message: ip[%s], port[%u], data_len[%d]",
            udp_fdsess.ip_addr.c_str(), udp_fdsess.port, len);

    if (m_udp_proc_pkg_cb_)
        m_udp_proc_pkg_cb_(data, len, &udp_fdsess);

    return 0;
}

int udp_io_event::udp_on_write_comple(connection<sock_dgram>* p_conn)
{
    return 0;
}

int udp_io_event::udp_on_close_conn(connection<sock_dgram>* p_conn)
{
    fdsession_t udp_fdsess;
    udp_fdsess.id = p_conn->get_connection_id();
    //udp_fdsess.ip_addr = "NULL";
    udp_fdsess.port = 0;

    if (m_udp_close_cb_)
        m_udp_close_cb_(&udp_fdsess);

    return 0;
}
