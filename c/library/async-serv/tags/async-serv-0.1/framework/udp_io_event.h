/**
 * @file udp_io_event.h
 * @brief udp io事件类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-31
 */

#ifndef _H_UDP_IO_EVENT_H_
#define _H_UDP_IO_EVENT_H_

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include <connection.hpp>
#include <sock_dgram.h>

#include "async_serv.h"

class udp_io_event : boost::noncopyable {
public:
    typedef boost::function<int (void*, int, fdsession_t*)> udp_proc_pkg_cb_t;
    typedef boost::function<void (fdsession_t*)> udp_close_conn_cb_t;
    typedef boost::function<int (fdsession_t*)> udp_conn_comple_cb_t;

    udp_io_event() { }
    ~udp_io_event() { }

    int udp_on_new_conn(connection<sock_dgram>* p_conn);
    int udp_on_message(connection<sock_dgram>* p_conn, void* data, int len);
    int udp_on_write_comple(connection<sock_dgram>* p_conn);
    int udp_on_close_conn(connection<sock_dgram>* p_conn);

    void set_udp_proc_callback(const udp_proc_pkg_cb_t& cb) {
        m_udp_proc_pkg_cb_ = cb;
    }

    void set_udp_close_conn_callback(const udp_close_conn_cb_t& cb) {
        m_udp_close_cb_ = cb;
    }

    void set_udp_conn_comple_callback(const udp_conn_comple_cb_t& cb) {
        m_udp_conn_comple_cb_ = cb;
    }

private:
    udp_proc_pkg_cb_t m_udp_proc_pkg_cb_;
    udp_close_conn_cb_t m_udp_close_cb_;
    udp_conn_comple_cb_t m_udp_conn_comple_cb_;
};

#endif
