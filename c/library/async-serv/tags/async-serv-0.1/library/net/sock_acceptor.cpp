/**
 * @file sock_acceptor.cpp
 * @brief socket被动连接类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-08
 */
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "log.h"
#include "sock_acceptor.h"
#include "config.h"
#include "sock_stream.h"

#define BACK_LOG 80

int sock_acceptor::open_listening()
{
    int fd = -1;

#ifdef _IPV6
    if (get_fd() < 0 && open(AF_INET6, SOCK_STREAM, 0) < 0) {
        ERROR_LOG("sock::open failed, err: %s", ::strerror(errno));
        return -1;
    }
#else
    if (get_fd() < 0 && open(AF_INET, SOCK_STREAM, 0) < 0) {
        ERROR_LOG("sock::open failed, err: %s", ::strerror(errno));
        return -1;
    }
#endif
    if ((fd = get_fd()) < 0) {
        ERROR_LOG("PANIC: sock::get_fd failed again!");
        close();
        return -1;
    }

    ///设置SO_REUSEADDR
    int reuseaddr = 1;
    if (set_options(SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) < 0) {
        ERROR_LOG("sock_acceptor::set_options failed");
        close();
        return -1;
    }

    ///设置非阻塞
    if (set_nonblocking() < 0) {
        ERROR_LOG("sock_acceptor::set_nonblocking failed");
        close();
        return -1;
    }

    if(::bind(fd,
              (struct sockaddr*)(m_local_addr_.get_sockaddr()),
              m_local_addr_.get_sockaddr_len()) < 0) {
        ERROR_LOG("sock_acceptor::bind failed, err: %s", ::strerror(errno));
        close();
        return -1;
    }

    if (::listen(fd, BACK_LOG) < 0) {
        ERROR_LOG("sock_acceptor::listen failed, err: %s", ::strerror(errno));
        close();
        return -1;
    }

    return IS_LISTENING;
}


int sock_acceptor::accept(int& client_fd, inet_address& remote_addr)
{
    int fd = get_fd();
    if (fd < 0) {
        ERROR_LOG("get invalid fd");
        return ACCEPT_ERR;
    }

#ifdef _IPV6
    struct sockaddr_in6 client_addr;
    socklen_t len = sizeof(client_addr);
#else
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
#endif

    if ((client_fd = ::accept(fd, (sockaddr*)&client_addr, &len)) < 0) {
        switch (errno) {
            case EAGAIN:
                return ACCEPT_COMPL;
            case EINTR:
                return ACCEPT_EINTR;
            case EMFILE:
                ///参考libev方式
                ::close(m_idle_fd_);
                m_idle_fd_ = ::accept(fd, NULL, NULL);
                ::close(m_idle_fd_);
                m_idle_fd_ = ::open("/dev/null", O_RDONLY);
                return ACCEPT_EMFILE;
            default:
                ERROR_LOG("sock_acceptor::accept failed, err: %s", ::strerror(errno));
                close();
                return ACCEPT_ERR;
        }
    } else {
#ifdef _IPV6
        char ip_addr[INET6_ADDRSTRLEN] = {0};
        ::inet_ntop(AF_INET6,
                    &client_addr.sin6_addr,
                    ip_addr,
                    sizeof(ip_addr));
        remote_addr.set_port(ntohs(client_addr.sin6_port));
#else
        char ip_addr[INET_ADDRSTRLEN] = {0};
        ::inet_ntop(AF_INET,
                    &client_addr.sin_addr,
                    ip_addr,
                    sizeof(ip_addr));
        remote_addr.set_port(ntohs(client_addr.sin_port));
#endif
        remote_addr.set_ip_addr(ip_addr);
        return ACCEPT_CONTINUE;
    }
}

