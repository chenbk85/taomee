/**
 * @file sock_stream.cpp
 * @brief sock连接类，封装基本socket操作
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-05
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#include "sock_stream.h"

ssize_t sock_stream::write(void* buf, size_t len, int flags)
{
    int fd = get_fd();
    if (fd < 0)
        return -1;

    return ::send(fd, buf, len, flags);
}

ssize_t sock_stream::read(void* buf, size_t len, int flags)
{
    int fd = get_fd();
    if (fd < 0)
        return -1;

    return ::recv(fd, buf, len , flags);
}

ssize_t sock_stream::writev(struct iovec* iov, int iov_cnt, int flags)
{
    int fd = get_fd();
    if (fd < 0)
        return -1;

    struct msghdr send_msg;
    ::memset(&send_msg, 0, sizeof(send_msg));

    send_msg.msg_iov = iov;
    send_msg.msg_iovlen = iov_cnt;

    return ::sendmsg(fd, &send_msg, flags);
}

ssize_t sock_stream::readv(struct iovec* iov, int iov_cnt, int flags)
{
    int fd = get_fd();
    if (fd < 0)
        return -1;

    struct msghdr recv_msg;
    ::memset(&recv_msg, 0, sizeof(recv_msg));

    recv_msg.msg_iov = iov;
    recv_msg.msg_iovlen = iov_cnt;

    return ::recvmsg(fd, &recv_msg, flags);
}

int sock_stream::close_writer()
{
    int fd = get_fd();
    if (fd < 0)
        return -1;

    return ::shutdown(fd, SHUT_WR);
}

int sock_stream::close_reader()
{
    int fd = get_fd();
    if (fd < 0) {
        return -1;
    }

    return ::shutdown(fd, SHUT_RD);
}
