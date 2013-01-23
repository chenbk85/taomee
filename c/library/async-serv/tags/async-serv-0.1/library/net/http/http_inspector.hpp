/**
 * @file http_inspector.hpp
 * @brief http回调函数返回进程信息
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-09
 */
#ifndef _H_HTTP_INSPECTOR_HPP_
#define _H_HTTP_INSPECTOR_HPP_

#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include <string>
#include <vector>

#include "http_request.h"
#include "http_response.h"
#include "process_info.h"

class http_inspector {
public:
    http_inspector(pid_t* begin, pid_t* end)
        : m_pid_vec_(begin, end) { }
    http_inspector(const std::vector<pid_t> rhs)
        : m_pid_vec_(rhs) { }
    http_inspector() { }

    ~http_inspector() { }

    int operator() (const http_request& request, http_response* response)
    {
        char buf[256] = {0};
        std::string resp_body;
        std::vector<pid_t>::iterator it;
        std::string path_str;
        time_t time_now;

        ///只允许get方法
        http_request::request_method method = request.get_method();
        if (method != http_request::rGet)
            goto err;

        ///只允许请求/
        path_str =request.get_path();
        if (path_str != "/")
            return -1;

        if (request.get_request_head("Connection") == std::string("close")) {
            response->add_header("Connection", "close");
            response->set_close_conn(true);
        }

        snprintf(buf, sizeof(buf), "uid: %d \r\n", process_info::uid());
        resp_body = buf + process_info::username() + "\r\n" +
                    process_info::hostname() + "\r\n" + "\r\n";

        for (it = m_pid_vec_.begin(); it != m_pid_vec_.end(); ++it) {
            resp_body += process_info::get_proc_status(*it) + "\r\n";

            snprintf(buf, sizeof(buf), "opend Files: %d \r\n\r\n", process_info::get_opened_files(*it));
            resp_body += buf;
        }

        response->set_status_code(http_response::k200Ok);
        response->set_status_str("OK");
        response->add_header("Content-Type", "text/plain");
        response->add_header("Server", "async_serv");
        response->add_header("Connection", "keep-alive");

        ::time(&time_now);
        ::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", ::gmtime(&time_now));
        response->add_header("Date", buf);

        response->set_body(resp_body);

        return 0;
err:
        return default_404_response(request, response);
    }

    void set_pid_vec(pid_t* begin, pid_t* end) {
        m_pid_vec_.assign(begin, end);
    }

private:
    int default_404_response(const http_request& request, http_response* response)
    {
        char time_now[128];
        time_t t = ::time(NULL);
        struct tm *tmp;
        tmp = ::gmtime(&t);
        ::strftime(time_now, sizeof(time_now), "%a, %d %b %Y %H:%M:%S GMT", tmp);

        response->set_close_conn(true);
        response->add_header("Connection", "close");
        response->set_status_code(http_response::k404NotFound);
        response->set_status_str("Not Found");
        response->add_header("Content-Type", "text/html");
        response->add_header("Server", "async_serv");
        response->add_header("Date", time_now);

        response->set_body("<html>\r\n<head><title>404 Not Found</title></head>\r\n"
                "<body bgcolor=\"white\">\r\n<center><h1>404 Not Found</h1></center>\r\n"
                "<hr><center>async_serv</center>\r\n</body>\r\n</html>");

        return 0;

    }

    std::vector<pid_t> m_pid_vec_;

};

#endif
