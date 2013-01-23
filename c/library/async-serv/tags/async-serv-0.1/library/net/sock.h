/**
 * @file sock.h
 * @brief 描述符类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-04
 */

#ifndef _H_SOCK_H_
#define _H_SOCK_H_

#include <boost/noncopyable.hpp>

#include "ipc_traits.hpp"

#define INVALID_SOCK_HANDLER -1

class sock : boost::noncopyable {
public:
    typedef sock_tag ipc_category;
    /**
     * @brief 设置socket fd相应的属性，封装setsockopt
     * @return -1failed, 0success
     */
    int set_options(int level, int option, void* optval, int optlen);

    /**
     * @brief 获得socket fd相应的属性，封装getsockopt
     * @return -1failed, 0success
     */
    int get_options(int level, int option, void* optval, int* optlen);

    /**
     * @brief 设置socket fd的非阻塞模式
     * @return -1failed, 0success
     */
    int set_nonblocking();

    /**
     * @brief 创建一个socket套接字fd
     * @param type              套接字类型
     * @param protocol_family   套接字协议域
     * @param protocol          具体协议，通常为0
     * @return -1failed, 0success
     */
    int open(int type, int protocol_family, int protocol);

    /**
     * @brief 关闭套接字
     * @return NULL
     */
    void close();

    /**
     * @brief 获得套接字fd
     * @return 套接字fd
     */
    const int get_fd() const {return m_sock_fd_; }

    /**
     * @brief 设置套接字fd
     * @param fd    套接字fd
     * @return NULL
     */
    void set_fd(int fd) { m_sock_fd_ = fd; }

protected:
    sock():m_sock_fd_(INVALID_SOCK_HANDLER) { }
    ~sock();

private:
    int m_sock_fd_;
};

#endif
