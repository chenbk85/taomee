/**
 * @file net_if.cpp
 * @brief 网络接口函数
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-05
 */

#include <list>
#include <new>
#include <boost/bind.hpp>
#include <vector>

#include "log.h"
#include "connector.hpp"
#include "sock_connector.h"
#include "sock_dgram.h"
#include "inet_address.h"
#include "connections_pool.hpp"
#include "connection.hpp"
#include "timer.h"

#include "tcp_io_event.h"
#include "udp_io_event.h"
#include "dll.h"
#include "net_if.h"

extern dll g_dll_inst;

class net_if {
public:
    net_if();
    ~net_if();

    void* tcp_connect_to_svr(const std::string& ip_addr, u_short port);
    void* create_udp_connect();

    int tcp_net_send(uint32_t conn_id, void* data, const int len);
    int udp_net_send(uint32_t conn_id, const std::string& remote_ip,
            u_short remote_port, void* data, const int len);

    void tcp_close_serv_connection(void* connector_handler);
    void udp_close_serv_connection(void* connector_handler);

    void tcp_close_client_connection(uint32_t conn_id);

    void* register_timer(void on_timer(void*),
                          void* arg,
                          u_int expired_time,
                          timer_type_t type,
                          u_int time_interval = 0);
    void remove_timer(void* timer_handler);

private:
    tcp_io_event m_tcp_io_event_inst_;
    udp_io_event m_udp_io_event_inst_;
    int m_tcp_first_;
    int m_udp_first_;
};

net_if::net_if()
    : m_tcp_first_(0),
      m_udp_first_(0) { }

net_if::~net_if() { }

void* net_if::tcp_connect_to_svr(const std::string& ip_addr, u_short port)
{
    if (!m_tcp_first_) {
        if (g_dll_inst.proc_pkg_from_serv)
            m_tcp_io_event_inst_.set_tcp_proc_callback(g_dll_inst.proc_pkg_from_serv);
        if (g_dll_inst.get_pkg_len)
            m_tcp_io_event_inst_.set_tcp_pkg_len_callback(g_dll_inst.get_pkg_len);
        if (g_dll_inst.on_serv_conn_closed)
            m_tcp_io_event_inst_.set_tcp_close_callback(g_dll_inst.on_serv_conn_closed);
        if (g_dll_inst.on_serv_conn_complete)
            m_tcp_io_event_inst_.set_tcp_conn_comple_callback(g_dll_inst.on_serv_conn_complete);
        if (g_dll_inst.on_serv_conn_failed)
            m_tcp_io_event_inst_.set_tcp_conn_failed_callback(g_dll_inst.on_serv_conn_failed);

        m_tcp_first_ = 1;
    }

    inet_address local_addr(ip_addr, port);
    connector<connection<sock_stream>, sock_connector>* p_connector
        = new (std::nothrow) connector<connection<sock_stream>, sock_connector>(local_addr);
    if (!p_connector) {
        ERROR_LOG("create connector failed");
        return NULL;
    }

    p_connector->set_new_conn_callback(
            boost::bind(&tcp_io_event::tcp_on_new_conn, &m_tcp_io_event_inst_, _1));
    p_connector->set_on_message_callback(
            boost::bind(&tcp_io_event::tcp_on_message, &m_tcp_io_event_inst_, _1, _2, _3));
    //p_connector->set_on_write_comple_callback(
    //        boost::bind(&tcp_io_event::tcp_on_write_complete, &m_tcp_io_event_inst_, _1));
    p_connector->set_on_close_callback(
            boost::bind(&tcp_io_event::tcp_on_close_conn, &m_tcp_io_event_inst_, _1));
    p_connector->set_on_conn_fail_callback(
            boost::bind(&tcp_io_event::tcp_on_conn_failed, &m_tcp_io_event_inst_, _1));

    if (p_connector->start() < 0) {
        ERROR_LOG("connector start failed");
        delete p_connector;
        p_connector = NULL;
        return NULL;
    }

    return p_connector;
}

void* net_if::create_udp_connect()
{
    if (!m_udp_first_) {
        if (g_dll_inst.proc_udp_pkg_from_serv)
            m_udp_io_event_inst_.set_udp_proc_callback(g_dll_inst.proc_udp_pkg_from_serv);
        if (g_dll_inst.on_serv_conn_closed)
            m_udp_io_event_inst_.set_udp_close_conn_callback(g_dll_inst.on_serv_conn_closed);
        if (g_dll_inst.on_serv_conn_complete)
            m_udp_io_event_inst_.set_udp_conn_comple_callback(g_dll_inst.on_serv_conn_complete);

        m_udp_first_ = 1;
    }

    connector<connection<sock_dgram>, sock_dgram>* p_connector
        = new (std::nothrow) connector<connection<sock_dgram>, sock_dgram>;
    if (!p_connector) {
        ERROR_LOG("create connector failed");
        return NULL;
    }

    p_connector->set_new_conn_callback(
            boost::bind(&udp_io_event::udp_on_new_conn, &m_udp_io_event_inst_, _1));
    p_connector->set_on_message_callback(
            boost::bind(&udp_io_event::udp_on_message, &m_udp_io_event_inst_, _1, _2, _3));
    //p_connector->set_on_write_comple_callback(
    //        boost::bind(&udp_io_event::udp_on_write_comple, &m_udp_io_event_inst_, _1));
    p_connector->set_on_close_callback(
            boost::bind(&udp_io_event::udp_on_close_conn, &m_udp_io_event_inst_, _1));

    if (p_connector->start() < 0) {
        ERROR_LOG("connector start failed");
        delete p_connector;
        p_connector = NULL;
        return NULL;
    }

    return p_connector;
}

int net_if::tcp_net_send(uint32_t conn_id, void* data, const int len)
{
    if (!data || len <= 0) {
        ERROR_LOG("input paramter invalid");
        return -1;
    }

    connection<sock_stream>* p_tcp_stream =
        connections_pool<connection<sock_stream> >::instance()->search_connection(conn_id);

    if (!p_tcp_stream) {
        ERROR_LOG("cannot find connection by conn_id[%u]", conn_id);
        return -1;
    }

    if (p_tcp_stream->send_data(data, len) < 0)
        return -1;

    return 0;
}

int net_if::udp_net_send(uint32_t conn_id, const std::string& remote_ip,
            u_short remote_port, void* data, const int len)
{
    if (!data || len <= 0) {
        ERROR_LOG("input paramter invalid");
        return -1;
    }

    connection<sock_dgram>* p_udp_stream =
        connections_pool<connection<sock_dgram> >::instance()->search_connection(conn_id);

    if (!p_udp_stream) {
        ERROR_LOG("cannot find connection by conn_id[%u]", conn_id);
        return -1;
    }

    inet_address serv_addr(remote_ip, remote_port);
    if (p_udp_stream->send_data(serv_addr, data, len) < 0)
        return -1;

    return 0;
}

void net_if::tcp_close_serv_connection(void* connector_handler)
{
    if (!connector_handler) {
        return ;
    }

    connector<connection<sock_stream>, sock_connector>* p_connector =
        (connector<connection<sock_stream>, sock_connector>*)connector_handler;

    connection<sock_stream>* p_tcp_stream = p_connector->get_connection();
    if (p_tcp_stream) {
        p_tcp_stream->recycle_connection();
    }

    delete p_connector;
    p_connector = NULL;
    return;
}

void net_if::udp_close_serv_connection(void* connector_handler)
{
    if (!connector_handler) {
        return ;
    }

    connector<connection<sock_dgram>, sock_dgram>* p_connector =
        (connector<connection<sock_dgram>, sock_dgram>*)connector_handler;

    connection<sock_dgram>* p_udp_stream = p_connector->get_connection();
    if (p_udp_stream) {
        p_udp_stream->recycle_connection();
        return;
    }

    delete p_connector;
    p_connector = NULL;
    return;
}

void net_if::tcp_close_client_connection(uint32_t conn_id)
{
    connection<sock_stream>* p_conn =
        connections_pool<connection<sock_stream> >::instance()->search_connection(conn_id);
    if (p_conn) {
        p_conn->shutdown();
    }
}

void* net_if::register_timer(void on_timer(void*),
                               void* arg,
                               u_int expired_time,
                               timer_type_t type,
                               u_int time_interval)
{
    timer* p_timer = new (std::nothrow) timer(on_timer,
                                              arg,
                                              expired_time,
                                              (int)type,
                                              time_interval);
    if (!p_timer) {
        ERROR_LOG("timer create failed");
        return NULL;
    }

    if (p_timer->register_timer() < 0) {
        ERROR_LOG("register timer failed");
        delete p_timer;
        p_timer = NULL;
    }

    return p_timer;
}

void net_if::remove_timer(void* timer_handler)
{
    timer* p_timer = (timer*) timer_handler;
    if (p_timer) {
        p_timer->remove_timer();
        delete p_timer;
        p_timer = NULL;
    }
}

/***********************************************************************/
static net_if g_net_if_inst;

void* tcp_connect_to_svr(const std::string& ip_addr, u_short port)
{
    return g_net_if_inst.tcp_connect_to_svr(ip_addr, port);
}

void* create_udp_connect()
{
    return g_net_if_inst.create_udp_connect();
}

int tcp_net_send(uint32_t conn_id, void* data, const int len)
{
    return g_net_if_inst.tcp_net_send(conn_id, data, len);
}

int udp_net_send(uint32_t conn_id, const std::string& remote_ip, u_short remote_port,
        void* data, const int len)
{
    return g_net_if_inst.udp_net_send(conn_id, remote_ip, remote_port, data, len);
}

void tcp_close_serv_connection(void* connector_handler)
{
    g_net_if_inst.tcp_close_serv_connection(connector_handler);
}

void udp_close_serv_connection(void* connector_handler)
{
    g_net_if_inst.udp_close_serv_connection(connector_handler);
}

void tcp_close_client_connection(uint32_t conn_id)
{
    g_net_if_inst.tcp_close_client_connection(conn_id);
}

void* register_timer(void on_timer(void*),
                     void* arg,
                     u_int expired_time,
                     timer_type_t type,
                     u_int time_interval)
{
    return g_net_if_inst.register_timer(on_timer, arg, expired_time, type, time_interval);
}

void remove_timer(void* timer_handler)
{
    g_net_if_inst.remove_timer(timer_handler);
}

