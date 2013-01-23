/**
 * @file http_request.h
 * @brief http请求类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-08
 */
#ifndef _H_HTTP_REQUEST_H_
#define _H_HTTP_REQUEST_H_

#include <string>
#include <map>

class http_request {
public:
    enum request_method {
        rUnknown,
        rGet,
        rPost,
        rHead,
        rPut,
        rDelete
    };

    http_request()
        : m_method_(rUnknown) { }
    ~http_request() { }

    bool set_method(const char* start, const char* end);

    request_method get_method() const {
        return m_method_;
    }

    std::string get_method_string();

    void set_request_path(const char* start, const char* end) {
        m_request_path_.assign(start, end);
    }

    const std::string& get_path() const {
        return m_request_path_;
    }

    void add_request_head(const char* start, const char* end);

    std::string get_request_head(const std::string& fieid) const;

    const std::map<std::string, std::string>& get_request_head() {
        return m_request_head_;
    }

    void reset() {
        m_method_ = rUnknown;
        m_request_path_.clear();
        m_request_head_.clear();
    }

private:
    request_method m_method_;
    std::string m_request_path_;
    std::map<std::string, std::string> m_request_head_;
};

#endif
