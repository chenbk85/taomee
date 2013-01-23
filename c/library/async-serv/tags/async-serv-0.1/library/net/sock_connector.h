/**
 * @file sock_connector.h
 * @brief socket连接封装类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-08
 */

#ifndef _H_SOCK_CONNECTOR_H_
#define _H_SOCK_CONNECTOR_H_

#include "sock.h"
#include "inet_address.h"
#include "config.h"

class sock;
class inet_address;
class sock_stream;

class sock_connector : public sock {
public:
    typedef sock_connector_tag ipc_category;

    sock_connector(const inet_address& serv_addr)
        : m_serv_addr_(serv_addr) { }
    sock_connector() { }

    ~sock_connector() { }

    /**
     * @brief socket连接函数
     * @return -1failed, 0连接成功 1正在连接，2暂时无法连接，-1连接出错
     */
    int open_connecting();

    /**
     * @brief 设置对端连接结构体
     * @return NULL
     */
    void set_serv_addr(const inet_address& serv_addr) {
        m_serv_addr_ = serv_addr;
    }

    /**
     * @brief 获得连接对端地址
     */
    const inet_address& get_serv_addr() { return m_serv_addr_; }

private:
    inet_address m_serv_addr_;
};

#endif
