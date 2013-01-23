/**
 * @file http_response.cpp
 * @brief http响应类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-08
 */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "http_response.h"

void http_response::append_resp_buffer(char** buffer, int* len)
{
    char buf[64] = {0};
    std::string buffer_str;

    ::snprintf(buf, sizeof(buf), "HTTP/1.1 %d ", m_status_code_);
    buffer_str = buf + m_status_str_ + "\r\n";

    ::snprintf(buf, sizeof(buf), "Content-Length: %d\r\n", m_resp_body.size());
    buffer_str += buf;

    std::map<std::string, std::string>::iterator it;
    for (it = m_resp_head_.begin(); it != m_resp_head_.end(); ++it) {
        buffer_str += it->first + ": " + it->second + "\r\n";
    }

    buffer_str += "\r\n" + m_resp_body;

    char* mem = (char*) ::calloc(buffer_str.size(), sizeof(char));
    if (!mem) {
        return;
    }

    ::memcpy(mem, buffer_str.c_str(), buffer_str.size());

    *buffer = mem;
    *len = buffer_str.size();
}
