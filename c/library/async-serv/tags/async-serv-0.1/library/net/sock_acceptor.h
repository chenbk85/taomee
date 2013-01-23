/**
 * @file sock_acceptor.h
 * @brief socket被动连接类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-08
 */
#ifndef _H_SOCK_ACCEPTOR_H_
#define _H_SOCK_ACCEPTOR_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sock.h"
#include "inet_address.h"
#include "config.h"

class sock;
class inet_address;
class sock_stream;

class sock_acceptor : public sock {
public:
    typedef sock_acceptor_tag ipc_category;

    sock_acceptor(const inet_address& local_addr)
        : m_local_addr_(local_addr),
          m_idle_fd_(::open("/dev/null", O_RDONLY))
    { }

    sock_acceptor()
        : m_idle_fd_(::open("/dev/null", O_RDONLY))
    { }

    ~sock_acceptor() { ::close(m_idle_fd_); }

    /**
     * @brief 打开并监听被动连接
     * @return -1failed, 1正在监听
     */
    int open_listening();

    /**
     * @brief 三次握手完成并建立新的连接
     * @param fd  新连接的fd
     * @param remote_addr   新连接的对端地址
     * @return -1 accept error, 0accept完成, 1需要继续accept，2accept fd耗尽
     */
    int accept(int& client_fd, inet_address& remote_addr);

    /**
     * @brief 设置本地end point连接结构体
     * @param local_addr    本地连接结构体
     * @return NULL
     */
    void set_local_addr(const inet_address& local_addr) {
        m_local_addr_ = local_addr;
    }

private:
    inet_address m_local_addr_;
    int m_idle_fd_;
};

#endif
