/**
 * @file http_serv.cpp
 * @brief
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-08
 */

#include <time.h>

#include <boost/bind.hpp>
#include <algorithm>

#include "log.h"
#include "http_serv.h"

int http_serv::start()
{
    if (m_http_acceptor_.start() < 0)
        return -1;
    if (m_http_acceptor_.enable_accepting() < 0)
        return -1;

    m_http_acceptor_.set_new_conn_callback(
            boost::bind(&http_serv::on_new_conn, this, _1));
    m_http_acceptor_.set_on_message_callback(
            boost::bind(&http_serv::on_message, this, _1, _2, _3));
    m_http_acceptor_.set_on_close_callback(
            boost::bind(&http_serv::on_close_conn, this, _1));

    return 0;
}

int http_serv::on_new_conn(connection<sock_stream>* p_conn)
{
    TRACE_LOG("new http connection, conn_id[%d], ip[%s], port[%d]",
               p_conn->get_connection_id(),
               (p_conn->get_remote_addr()).get_ip_addr().c_str(),
               (p_conn->get_remote_addr()).get_port());

    http_sess_t sess;
    sess.status = expectRequestLine;
    m_http_session_[p_conn->get_connection_id()] = sess;

    return 0;
}

int http_serv::on_message(connection<sock_stream>* p_conn, void* buf, int len)
{
    std::map<uint32_t, http_sess_t>::iterator it;
    it = m_http_session_.find(p_conn->get_connection_id());
    if (it == m_http_session_.end()) {
        TRACE_LOG("cannot find http session");
        return -1;
    }

    char* send_buf = NULL;
    int send_len = 0;

    char* start = (char*)buf;
    char* end = start + len;
    const char* clrf = "\r\n";

    while (true) {
        if (start >= end)
            return 0;

        char* found = std::search(start, end, clrf, clrf + 2);
        ///没找到完整的一行，则返回原数据，再接收
        if (found == end)
            return end - start;

        ///请求包头已到空行
        if (found == start) {
            it->second.status = gotAll;
            break;
        }

        if (it->second.status == expectRequestLine) {
            if (parse_request_line(start, found, it->second.request) < 0)
                goto err;
            it->second.status = expectHeaders;
            start = found + 2;
        } else if (it->second.status == expectHeaders) {
            it->second.request.add_request_head(start, found);
            start = found + 2;
        } else if (it->second.status == expectBody) {
            ///对于post这种有body的http请求暂时不处理
        }
    }

    ///收到完整请求，发送响应报文
    if (m_http_cb_) {
        if (m_http_cb_(it->second.request, &(it->second.response)) < 0)
            goto err;
    } else {
        if (default_http_callback(it->second.request, &(it->second.response)) < 0)
            goto err;
    }

    it->second.response.append_resp_buffer(&send_buf, &send_len);
    if (!send_buf || send_len <= 0)
        goto err;

    p_conn->send_data(send_buf, send_len);

    ::free(send_buf);
    send_buf = NULL;
    send_len = 0;

    if(it->second.response.get_conn_close())
        p_conn->shutdown();

    it->second.request.reset();
    it->second.response.reset();
    it->second.status = expectRequestLine;

    return 0;
err:
    p_conn->shutdown();
    return 0;
}

int http_serv::on_close_conn(connection<sock_stream>* p_conn)
{
    std::map<uint32_t, http_sess_t>::iterator it;
    it = m_http_session_.find(p_conn->get_connection_id());
    if (it == m_http_session_.end()) {
        TRACE_LOG("cannot find http session[%u]", p_conn->get_connection_id());
        return -1;
    }

    m_http_session_.erase(it);
    return 0;
}

int http_serv::parse_request_line(const char* start, const char* end, http_request& request)
{
    const char* method_start = start;
    while(!::isblank(*start) && start != end) {
        start++;
    }

    if (start != end && request.set_method(method_start, start)) {
        start++;
        const char* space = std::find(start, end, ' ');
        if (space != end) {
            request.set_request_path(start, space);
            start = space + 1;
            if (end-start == 8 && std::equal(start, end, "HTTP/1.1"))
                return 0;
        }
    }

    return -1;
}

int http_serv::default_http_callback(const http_request& request, http_response* response)
{
    char time_now[128];
    time_t t = ::time(NULL);
    struct tm *tmp;
    tmp = ::gmtime(&t);
    ::strftime(time_now, sizeof(time_now), "%a, %d %b %Y %H:%M:%S GMT", tmp);

    response->set_close_conn(true);
    response->add_header("Connection", "close");
    response->set_status_code(http_response::k400BadRequest);
    response->set_status_str("Bad Request");
    response->add_header("Content-Type", "text/html");
    response->add_header("Server", "async_serv");
    response->add_header("Date", time_now);

    response->set_body("<html>\r\n<head><title>400 Bad Request</title></head>\r\n"
            "<body bgcolor=\"white\">\r\n<center><h1>400 Bad Request</h1></center>\r\n"
            "<hr><center>async_serv</center>\r\n</body>\r\n</html>");

    return 0;
}

