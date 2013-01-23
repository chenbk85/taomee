/**
 * @file tcp_io_event.h
 * @brief tcp io事件封装类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-31
 */

#ifndef _H_TCP_IO_EVENT_H_
#define _H_TCP_IO_EVENT_H_

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "connection.hpp"
#include "sock_stream.h"
#include "connector.hpp"
#include "sock_connector.h"

#include "async_serv.h"

class tcp_io_event : boost::noncopyable {
public:
    typedef boost::function<int (void*, int , fdsession_t*)> tcp_proc_pkg_cb_t;
    typedef boost::function<int (void*, int)> tcp_pkg_len_cb_t;
    typedef boost::function<int (fdsession_t*)> tcp_conn_comple_cb_t;
    typedef boost::function<int (void*)> tcp_conn_failed_cb_t;
    typedef boost::function<void (fdsession_t*)> tcp_close_cb_t;

    tcp_io_event() { }
    ~tcp_io_event() { }

    int tcp_on_new_conn(connection<sock_stream>* p_conn);
    int tcp_on_conn_failed(connector<connection<sock_stream>, sock_connector>* p_conn);
    int tcp_on_message(connection<sock_stream>* p_conn, void* data, int len);
    int tcp_on_write_complete(connection<sock_stream>* p_conn);
    int tcp_on_close_conn(connection<sock_stream>* p_conn);

    void set_tcp_proc_callback(const tcp_proc_pkg_cb_t& cb) {
        m_tcp_proc_pkg_cb_ = cb;
    }

    void set_tcp_pkg_len_callback(const tcp_pkg_len_cb_t& cb) {
        m_tcp_pkg_len_cb_ = cb;
    }

    void set_tcp_conn_comple_callback(const tcp_conn_comple_cb_t& cb) {
        m_tcp_conn_comple_cb_ = cb;
    }

    void set_tcp_conn_failed_callback(const tcp_conn_failed_cb_t& cb) {
        m_tcp_conn_failed_cb_ = cb;
    }

    void set_tcp_close_callback(const tcp_close_cb_t cb) {
        m_tcp_close_cb_ = cb;
    }

private:
    tcp_proc_pkg_cb_t m_tcp_proc_pkg_cb_;
    tcp_pkg_len_cb_t m_tcp_pkg_len_cb_;
    tcp_conn_comple_cb_t m_tcp_conn_comple_cb_;
    tcp_close_cb_t m_tcp_close_cb_;
    tcp_conn_failed_cb_t m_tcp_conn_failed_cb_;
};

#endif

