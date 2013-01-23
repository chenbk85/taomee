/**
 * @file vpipe_sockpair.cpp
 * @brief 类管道模型socketpair封装
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-22
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#include "log.h"
#include "vpipe_sockpair.h"

int vpipe_sockpair::open()
{
        int fd[2] = {-1, -1};

        if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fd) < 0) {
            ERROR_LOG("socketpair failed, err: %s", ::strerror(errno));
            return -1;
        }

        set_fd(fd);
        set_nonblocking();
    return 0;
}

ssize_t vpipe_sockpair::write(void* buf, size_t len, int flags)
{
    int fd = -1;
    if (m_sockpair_type_ == SOCKPAIR_TYPE_1) {
        fd = get_fd1();
    } else {
        fd = get_fd2();
    }

    if (fd < 0)
        return -1;

    return ::send(fd, buf, len, flags);
}

ssize_t vpipe_sockpair::read(void* buf, size_t len, int flags)
{
    int fd = -1;
    if (m_sockpair_type_ == SOCKPAIR_TYPE_1) {
        fd = get_fd1();
    } else {
        fd = get_fd2();
    }

    if (fd < 0)
        return -1;

    return ::recv(fd, buf, len, flags);
}

ssize_t vpipe_sockpair::writev(struct iovec* iov, int iov_cnt, int flags)
{
    int fd = -1;
    if (m_sockpair_type_ == SOCKPAIR_TYPE_1) {
        fd = get_fd1();
    } else {
        fd = get_fd2();
    }

    if (fd < 0)
        return -1;

    struct msghdr send_msg;
    ::memset(&send_msg, 0, sizeof(send_msg));

    send_msg.msg_iov = iov;
    send_msg.msg_iovlen = iov_cnt;

    return ::sendmsg(fd, &send_msg, flags);
}

ssize_t vpipe_sockpair::readv(struct iovec* iov, int iov_cnt, int flags)
{
    int fd = -1;
    if (m_sockpair_type_ == SOCKPAIR_TYPE_1) {
        fd = get_fd1();
    } else {
        fd = get_fd2();
    }

    if (fd < 0)
        return -1;

    struct msghdr recv_msg;
    ::memset(&recv_msg, 0, sizeof(recv_msg));

    recv_msg.msg_iov = iov;
    recv_msg.msg_iovlen = iov_cnt;

    return ::recvmsg(fd, &recv_msg, flags);
}

int vpipe_sockpair::close_writer()
{
    int fd = -1;
    if (m_sockpair_type_ == SOCKPAIR_TYPE_1) {
        fd = get_fd1();
    } else {
        fd = get_fd2();
    }

    if (fd < 0)
        return -1;

    return ::shutdown(fd, SHUT_WR);
}

int vpipe_sockpair::close_reader()
{
    int fd = -1;
    if (m_sockpair_type_ == SOCKPAIR_TYPE_1) {
        fd = get_fd1();
    } else {
        fd = get_fd2();
    }

    if (fd < 0)
        return -1;

    return ::shutdown(fd, SHUT_RD);
}
