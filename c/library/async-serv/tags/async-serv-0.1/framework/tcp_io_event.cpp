/**
 * @file tcp_io_event.cpp
 * @brief tcp io事件封装类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-31
 */
#include "tcp_io_event.h"

int tcp_io_event::tcp_on_new_conn(connection<sock_stream>* p_conn)
{
    fdsession_t tcp_fdsess;
    tcp_fdsess.id = p_conn->get_connection_id();
    tcp_fdsess.ip_addr = (p_conn->get_remote_addr()).get_ip_addr();
    tcp_fdsess.port = (p_conn->get_remote_addr()).get_port();

    TRACE_LOG("tcp new conn: ip[%s], port[%u], id[%u]",
            tcp_fdsess.ip_addr.c_str(), tcp_fdsess.port, tcp_fdsess.id);
    if (m_tcp_conn_comple_cb_) {
        if (m_tcp_conn_comple_cb_(&tcp_fdsess) < 0)
            return -1;
    }

    return 0;
}

int tcp_io_event::tcp_on_conn_failed(connector<connection<sock_stream>, sock_connector>* p_conn)
{
    if (m_tcp_conn_failed_cb_)
        return m_tcp_conn_failed_cb_(p_conn);

    return 0;
}

int tcp_io_event::tcp_on_message(connection<sock_stream>* p_conn, void* data, int len)
{

    fdsession_t tcp_fdsess;
    tcp_fdsess.id = p_conn->get_connection_id();
    tcp_fdsess.ip_addr = (p_conn->get_remote_addr()).get_ip_addr();
    tcp_fdsess.port = (p_conn->get_remote_addr()).get_port();

    if (!m_tcp_pkg_len_cb_ || !m_tcp_proc_pkg_cb_)
        return 0;

    do {
       int rt = m_tcp_pkg_len_cb_(data, len);
       if (rt < 0) {
           return -1;
       } else if (rt == 0 || rt > len) {
           return len;
       } else {
           ///收到完整包
       }

       int result = m_tcp_proc_pkg_cb_(data, rt, &tcp_fdsess);
       if (result < 0) {
           return -1;
       } else {
           //do nothing
       }

       data = (char*)data + rt;
       len -= rt;

   } while (len > 0);

   return 0;
}

int tcp_io_event::tcp_on_write_complete(connection<sock_stream>* p_conn)
{
    return 0;
}

int tcp_io_event::tcp_on_close_conn(connection<sock_stream>* p_conn)
{
    fdsession_t tcp_fdsess;
    tcp_fdsess.id = p_conn->get_connection_id();
    tcp_fdsess.ip_addr = (p_conn->get_remote_addr()).get_ip_addr();
    tcp_fdsess.port = (p_conn->get_remote_addr()).get_port();

    if (m_tcp_close_cb_)
        m_tcp_close_cb_(&tcp_fdsess);

    return 0;
}
