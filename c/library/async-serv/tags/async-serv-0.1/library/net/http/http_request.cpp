/**
 * @file http_request.cpp
 * @brief http请求类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-08
 */
#include <string.h>
#include <ctype.h>

#include "http_request.h"

bool http_request::set_method(const char* start, const char* end)
{
    if (!::strncmp(start, "GET", 3) && (end - start) == 3) {
        m_method_ = rGet;
    } else if (!::strncmp(start, "POST", 4) && (end - start) == 4) {
        m_method_ = rPost;
    } else if (!::strncmp(start, "HEAD", 4) && (end - start) == 4) {
        m_method_ = rHead;
    } else if (!::strncmp(start, "PUT", 3) && (end - start) == 3) {
        m_method_ = rPut;
    } else if (!::strncmp(start, "DELETE", 6) && (end - start) == 6) {
        m_method_ = rDelete;
    } else {
        m_method_ = rUnknown;
    }

    return m_method_ != rUnknown;
}

std::string http_request::get_method_string()
{
    if (m_method_ == rDelete) {
        return "DELETE";
    } else if (m_method_ == rGet) {
        return "GET";
    } else if (m_method_ == rPost) {
        return "POST";
    } else if (m_method_ == rHead) {
        return "HEAD";
    } else if (m_method_ == rPut) {
        return "PUT";
    } else {
        return "UNKOWN";
    }
}

void http_request::add_request_head(const char* start, const char* end)
{
    while(::isblank(*start) && start != end) {
        start++;
    }

    const char* colon = start;
    while (*colon != ':' && colon != end) {
        colon++;
    }
    if (colon == end)
        return;
    const char* filed_end = colon;
    while (::isblank(*filed_end)) {
        filed_end--;
    }

    std::string field(start, filed_end);

    ++colon;
    while (::isblank(*colon) && colon != end) {
        colon++;
    }
    if (colon == end)
        return;
    while (::isblank(*end)) {
        end--;
    }

    std::string result(colon, end);

    m_request_head_.insert(
            std::pair<std::string, std::string>(field, result));

}

std::string http_request::get_request_head(const std::string& field) const
{
    std::string result;
    std::map<std::string, std::string>::const_iterator it;
    if ((it = m_request_head_.find(field)) != m_request_head_.end())
        result = it->second;

    return result;
}
