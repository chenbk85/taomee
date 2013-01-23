/**
 * @file inet_address.h
 * @brief
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-01
 */
#ifndef _H_INET_ADDRESS_H_
#define _H_INET_ADDRESS_H_

#include <string>
#include <netinet/in.h>

class inet_address {
public:
    inet_address();
    inet_address(const u_short port);
    inet_address(const std::string& ip_addr, const u_short port);

    ~inet_address() { }

    /**
     * @brief 设置端口号
     * @param port  端口号
     * @return NULL
     */
    void set_port(const u_short port) { m_port_ = port; }

    /**
     * @brief 设置ip地址
     * @param ip_addr   ip地址
     * @return NULL
     */
    void set_ip_addr(const std::string& ip_addr) { m_ip_addr_ = ip_addr; }

    /**
     * @brief 获得ip地址
     * @return ip地址
     */
    const std::string& get_ip_addr() const { return m_ip_addr_; }

    /**
     * @brief 获得端口号
     * @return 端口号
     */
    const u_short get_port() const { return m_port_; }

#ifdef _IPV6
    /**
     * @brief 获得sockaddr_in6结构体
     * @return sockaddr_in6结构体
     */
    struct sockaddr_in6* get_sockaddr();
#else
    /**
     * @brief 获得sockaddr_in结构体
     * @return sockaddr_in结构体
     */
    struct sockaddr_in* get_sockaddr();
#endif

    /**
     * @brief 获得sockaddr_in6或sockaddr_in结构体的大小
     * @return 结构体的大小
     */
    const int get_sockaddr_len() const { return sizeof(m_sockaddr_in_); }
private:
    std::string m_ip_addr_;
    u_short m_port_;

#ifdef _IPV6
    struct sockaddr_in6 m_sockaddr_in_;
#else
    struct sockaddr_in m_sockaddr_in_;
#endif
};

#endif
