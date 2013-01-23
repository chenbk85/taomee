/**
 * @file http_response.h
 * @brief http响应类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-08
 */

#ifndef _H_HTTP_RESPONSE_H_
#define _H_HTTP_RESPONSE_H_

#include <string>
#include <map>

class http_response {
public:
    enum http_status_code {
        kUnknown,
        k200Ok = 200,
        k404NotFound = 404,
        k400BadRequest = 400,
        k403Forbidden = 403,
        k500InternalError = 500,
        k503SevUnavailable = 503
    };

    http_response()
        : m_status_code_(kUnknown),
          m_conn_close_(false) { }
    ~http_response() { }

    void set_status_code(http_status_code code) {
        m_status_code_ = code;
    }

    void set_status_str(const std::string& str) {
        m_status_str_ = str;
    }

    void set_close_conn(bool on) {
        m_conn_close_ = on;
    }

    void add_header(const std::string& fieid, const std::string& result) {
        m_resp_head_[fieid] = result;
    }

    bool get_conn_close() const {
        return m_conn_close_;
    }

    void set_body(const std::string& body) {
        m_resp_body = body;
    }

    void append_resp_buffer(char** buffer, int* len);

    void reset() {
        m_status_code_ = kUnknown;
        m_status_str_.clear();
        m_resp_head_.clear();
        m_resp_body.clear();
        m_conn_close_ = false;
    }
private:
    http_status_code m_status_code_;
    std::string m_status_str_;
    std::map<std::string, std::string> m_resp_head_;
    std::string m_resp_body;
    bool m_conn_close_;
};
#endif
