/**
 * @file sock_dgram.cpp
 * @brief udp socket类封装
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-09
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.h"
#include "sock_dgram.h"

int sock_dgram::open_listening()
{
#ifdef _IPV6
    if (get_fd() < 0 && open(AF_INET6, SOCK_DGRAM, 0) < 0) {
        ERROR_LOG("sock::open failed, err: %s", ::strerror(errno));
        return -1;
    }
#else
    if (get_fd() < 0 && open(AF_INET, SOCK_DGRAM, 0) < 0) {
        ERROR_LOG("sock::open failed, err: %s", ::strerror(errno));
        return -1;
    }
#endif

    int fd = get_fd();
    if (fd < 0) {
        ERROR_LOG("PANIC: get_fd failed again!");
        return -1;
    }

    if (set_nonblocking() < 0) {
        ERROR_LOG("sock::set_nonblocking failed");
        close();
        return -1;
    }

    if (::bind(fd,
               (struct sockaddr*)(m_local_addr_.get_sockaddr()),
                m_local_addr_.get_sockaddr_len()) < 0) {
        ERROR_LOG("bind falied, err: %s", ::strerror(errno));
        close();
        return -1;
    }

    return 0;
}

int sock_dgram::open_connecting()
{
#ifdef _IPV6
    if (get_fd() < 0 && open(AF_INET6, SOCK_DGRAM, 0) < 0) {
        ERROR_LOG("sock::open failed, err: %s", ::strerror(errno));
        return -1;
    }
#else
    if (get_fd() < 0 && open(AF_INET, SOCK_DGRAM, 0) < 0) {
        ERROR_LOG("sock::open failed, err: %s", ::strerror(errno));
        return -1;
    }
#endif
    if (set_nonblocking() < 0) {
        ERROR_LOG("sock::set_nonblocking failed");
        close();
        return -1;
    }

    return 0;
}


ssize_t sock_dgram::write(const inet_address& remote_addr, void* buf, size_t len, int flags)
{
    int fd = get_fd();
    if (fd < 0) {
        return -1;
    }

    return ::sendto(fd,
                    buf,
                    len,
                    flags,
                    (struct sockaddr*)((const_cast<inet_address&>(remote_addr)).get_sockaddr()),
                    remote_addr.get_sockaddr_len());
}

ssize_t sock_dgram::read(inet_address& remote_addr, void* buf, size_t len, int flags)
{
    int fd = get_fd();
    if (fd < 0) {
        return -1;
    }

#ifdef _IPV6
    struct sockaddr_in6 tmp_addr;
    socklen_t tmp_len = sizeof(tmp_addr);
    int rt = ::recvfrom(fd, buf, len, flags, (struct sockaddr*)&tmp_addr, &tmp_len);
    if (rt < 0) {
        return -1;
    } else {
        char ip_addr[INET6_ADDRSTRLEN] = {0};
        ::inet_ntop(AF_INET6,
                    &tmp_addr.sin6_addr,
                    ip_addr,
                    sizeof(ip_addr));
        remote_addr.set_ip_addr(ip_addr);
        remote_addr.set_port(ntohs(tmp_addr.sin6_port));

        return rt;
    }
#else
    struct sockaddr_in tmp_addr;
    socklen_t tmp_len = sizeof(tmp_addr);
    int rt = ::recvfrom(fd, buf, len, flags, (struct sockaddr*)&tmp_addr, &tmp_len);
    if (rt < 0) {
        return -1;
    } else {
        char ip_addr[INET_ADDRSTRLEN] = {0};
        ::inet_ntop(AF_INET,
                    &tmp_addr.sin_addr,
                    ip_addr,
                    sizeof(ip_addr));
        remote_addr.set_ip_addr(ip_addr);
        remote_addr.set_port(ntohs(tmp_addr.sin_port));

        return rt;
    }
#endif
}

ssize_t sock_dgram::writev(const inet_address& remote_addr, struct iovec* iov, int iov_cnt, int flags)
{
    int fd = get_fd();
    if (fd < 0) {
        return -1;
    }

    struct msghdr send_msg;
    ::memset(&send_msg, 0, sizeof(send_msg));

    send_msg.msg_name = (const_cast<inet_address&>(remote_addr)).get_sockaddr();
    send_msg.msg_namelen = remote_addr.get_sockaddr_len();

    send_msg.msg_iov = iov;
    send_msg.msg_iovlen = iov_cnt;

    return ::sendmsg(fd, &send_msg, flags);
}

ssize_t sock_dgram::readv(inet_address& remote_addr, struct iovec* iov, int iov_cnt, int flags)
{
    int fd = get_fd();
    if (fd < 0) {
        return -1;
    }

    msghdr recv_msg;
    ::memset(&recv_msg, 0, sizeof(recv_msg));
#ifdef _IPV6
    struct sockaddr_in6 tmp_addr;
    socklen_t tmp_len = sizeof(sockaddr_in6);

    recv_msg.msg_name = &tmp_addr;
    recv_msg.msg_namelen = tmp_len;

    recv_msg.msg_iov = iov;
    recv_msg.msg_iovlen = iov_cnt;

    int rt = ::recvmsg(fd, &recv_msg, flags);
    if (rt < 0) {
        return -1;
    } else {
        char ip_addr[INET6_ADDRSTRLEN] = {0};
        ::inet_ntop(AF_INET6,
                  &tmp_addr.sin6_addr,
                  ip_addr,
                  sizeof(ip_addr));
        remote_addr.set_ip_addr(ip_addr);
        remote_addr.set_port(ntohs(tmp_addr.sin6_port));

        return rt;
    }
#else
    struct sockaddr_in tmp_addr;
    socklen_t tmp_len = sizeof(sockaddr_in);

    recv_msg.msg_name = &tmp_addr;
    recv_msg.msg_namelen = tmp_len;

    recv_msg.msg_iov = iov;
    recv_msg.msg_iovlen = iov_cnt;

    int rt = ::recvmsg(fd, &recv_msg, flags);
    if (rt < 0) {
        return -1;
    } else {
        char ip_addr[INET_ADDRSTRLEN] = {0};
        ::inet_ntop(AF_INET,
                  &tmp_addr.sin_addr,
                  ip_addr,
                  sizeof(ip_addr));
        remote_addr.set_ip_addr(ip_addr);
        remote_addr.set_port(ntohs(tmp_addr.sin_port));

        return rt;
    }
#endif
}
