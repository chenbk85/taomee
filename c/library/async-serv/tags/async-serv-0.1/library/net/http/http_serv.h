/**
 * @file http_serv.h
 * @brief 小型http_serv实现
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-08
 */
#ifndef _H_HTTP_SERV_H_
#define _H_HTTP_SERV_H_

#include <stdint.h>

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <map>

#include "inet_address.h"
#include "acceptor.hpp"
#include "connection.hpp"
#include "sock_acceptor.h"
#include "sock_stream.h"

#include "http_request.h"
#include "http_response.h"

class http_serv : boost::noncopyable {
public:
    enum request_status {
        expectRequestLine,
        expectHeaders,
        expectBody,
        gotAll
    };

    typedef boost::function<int (const http_request&, http_response*)> http_callback_t;

    http_serv(const inet_address& http_addr)
        : m_http_acceptor_(http_addr) { }

    ~http_serv() { }

    int start();

    int on_new_conn(connection<sock_stream>* p_conn);
    int on_message(connection<sock_stream>* p_conn, void* buf, int len);
    int on_close_conn(connection<sock_stream>* p_conn);

    void set_http_callback(const http_callback_t& cb) {
        m_http_cb_ = cb;
    }

private:
    int parse_request_line(const char* start, const char* end, http_request& request);
    int default_http_callback(const http_request& request, http_response* response);

private:
    acceptor<connection<sock_stream>, sock_acceptor> m_http_acceptor_;
    http_callback_t m_http_cb_;

    typedef struct {
        http_request request;
        http_response response;
        request_status status;
    } http_sess_t;

    std::map<uint32_t, http_sess_t> m_http_session_;
};

#endif

