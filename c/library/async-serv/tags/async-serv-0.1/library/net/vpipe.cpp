/**
 * @file vpipe.cpp
 * @brief 类管道类型的fd封装类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-19
 */

#include "unistd.h"
#include "fcntl.h"

#include "vpipe.h"
#include "log.h"

vpipe::~vpipe()
{
    close();
}

int vpipe::set_nonblocking()
{
    if (m_vpipe_fd_[0] < 0 || m_vpipe_fd_[1] < 0) {
        return -1;
    }

    int flags = ::fcntl(m_vpipe_fd_[0], F_GETFL);
    ::fcntl(m_vpipe_fd_[0], F_SETFL, flags | O_NONBLOCK);

    flags = ::fcntl(m_vpipe_fd_[1], F_GETFL);
    ::fcntl(m_vpipe_fd_[1], F_SETFL, flags | O_NONBLOCK);

    return 0;
}

void vpipe::close()
{
    for (int i = 0; i < 2; i++) {
        if (m_vpipe_fd_[i] >= 0) {
            ::close(m_vpipe_fd_[i]);
            m_vpipe_fd_[i] = INVALID_VPIPE_HANDLER;
        }
    }
}

void vpipe::close_fd1()
{
    if (m_vpipe_fd_[0] >= 0) {
        ::close(m_vpipe_fd_[0]);
        m_vpipe_fd_[0] = INVALID_VPIPE_HANDLER;
    }
}

void vpipe::close_fd2()
{
    if (m_vpipe_fd_[1] >= 0) {
        ::close(m_vpipe_fd_[1]);
        m_vpipe_fd_[1] = INVALID_VPIPE_HANDLER;
    }
}
