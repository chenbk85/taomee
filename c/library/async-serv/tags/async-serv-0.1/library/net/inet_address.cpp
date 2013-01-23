/**
 * @file inet_address.cpp
 * @brief
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-01
 */
#include <arpa/inet.h>
#include <string.h>

#include "inet_address.h"

inet_address::inet_address(const u_short port)
    : m_ip_addr_("NULL"),
      m_port_(port)
{
    ::memset(&m_sockaddr_in_, 0, sizeof(m_sockaddr_in_));
}

inet_address::inet_address(const std::string& ip_addr, const u_short port)
    : m_ip_addr_(ip_addr),
      m_port_(port)
{
    ::memset(&m_sockaddr_in_, 0, sizeof(m_sockaddr_in_));
}

inet_address::inet_address()
    : m_ip_addr_("NULL"),
      m_port_(0)
{
    ::memset(&m_sockaddr_in_, 0, sizeof(m_sockaddr_in_));
}

#ifdef _IPV6
struct sockaddr_in6* inet_address::get_sockaddr()
{
    ::memset(&m_sockaddr_in_, 0, sizeof(m_sockaddr_in_));

    if (m_ip_addr_ == std::string("NULL")) {
        if (::inet_pton(AF_INET6,
                        "::",
                        &(m_sockaddr_in_.sin6_addr.s6_addr)) != 1) {
            return NULL;
        }
    } else {
        if (::inet_pton(AF_INET6,
                        m_ip_addr_.c_str(),
                        &(m_sockaddr_in_.sin6_addr.s6_addr)) != 1) {
            return NULL;
        }
    }

    m_sockaddr_in_.sin6_family = AF_INET6;
    m_sockaddr_in_.sin6_port = ::htons(m_port_);

    return &m_sockaddr_in_;
}
#else
struct sockaddr_in* inet_address::get_sockaddr()
{
    ::memset(&m_sockaddr_in_, 0, sizeof(m_sockaddr_in_));

    if (m_ip_addr_ == std::string("NULL")) {
        m_sockaddr_in_.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (::inet_pton(AF_INET,
                        m_ip_addr_.c_str(),
                        &(m_sockaddr_in_.sin_addr.s_addr)) != 1) {
            return NULL;
        }
    }

    m_sockaddr_in_.sin_family = AF_INET;
    m_sockaddr_in_.sin_port = htons(m_port_);

    return &m_sockaddr_in_;
}
#endif
