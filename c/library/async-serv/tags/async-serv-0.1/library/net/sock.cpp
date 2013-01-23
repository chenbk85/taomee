/**
 * @file sock.cpp
 * @brief 描述符类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-04
 */

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

#include "sock.h"

sock::~sock()
{
    close();
}

int sock::set_options(int level, int option, void* optval, int optlen)
{
    return ::setsockopt(m_sock_fd_, level, option, optval, optlen);
}

int sock::get_options(int level, int option, void* optval, int* optlen)
{
    return ::getsockopt(m_sock_fd_, level, option, optval, (socklen_t*)optlen);
}

int sock::set_nonblocking()
{
    int flags = ::fcntl(m_sock_fd_, F_GETFL);
    return ::fcntl(m_sock_fd_, F_SETFL, flags | O_NONBLOCK);
}

int sock::open(int type, int protocol_family, int protocol)
{
    m_sock_fd_ = ::socket(type, protocol_family, protocol);
    if (m_sock_fd_ < 0) {
        m_sock_fd_ = INVALID_SOCK_HANDLER;
        return -1;
    }

    return 0;
}

void sock::close()
{
    if (m_sock_fd_ >= 0) {
        ::close(m_sock_fd_);
        m_sock_fd_ = INVALID_SOCK_HANDLER;
    }
}
