/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file net_io_notifier.cpp
 * @brief 网络IO框架通知机制
 * @author baron baron@taomee.com
 * @version 1.0
 * @date 2010-12-16
 */

#include "net_io_notifier.h"

#include <string.h>
#include <new>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>

int net_io_notifier_create(i_net_io_notifier **pp_net_io_notifier)
{
    if (!pp_net_io_notifier) {
        return -1;
    }

    i_net_io_notifier *p_net_io_notifier = new (std::nothrow) net_io_notifier();

    if (!p_net_io_notifier) {
        return -1;
    }

    *pp_net_io_notifier = p_net_io_notifier;
    return 0;
}

net_io_notifier::net_io_notifier() : m_init_(0)
{
}

net_io_notifier::~net_io_notifier()
{
    uninit();
}

int net_io_notifier::init()
{
    if (m_init_) {
        return -1;
    }

    int pipefd[2];

    if (pipe(pipefd)) {
        return -1;
    }

    int flags = fcntl(pipefd[0], F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(pipefd[0], F_SETFL, flags);
    flags = fcntl(pipefd[1], F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(pipefd[1], F_SETFL, flags);

    int *p_mutex = reinterpret_cast<int *>(mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0));

    if (MAP_FAILED == p_mutex) {
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    m_rfd_ = pipefd[0];
    m_wfd_ = pipefd[1];
    m_p_mutex_ = p_mutex;
    *m_p_mutex_ = 0;
    m_init_ = 1;
    return 0;
}

int net_io_notifier::popup()
{
    if (!m_init_) {
        return -1;
    }

    if (__sync_lock_test_and_set(m_p_mutex_, 1)) {
        return 0;
    }

    for (;;) {
        int ret = write(m_wfd_, "w", 1);

        if (ret < 0 && EINTR == errno) {
            continue;
        }

        break;
    }

    return 0;
}

int net_io_notifier::got()
{
    if (!m_init_) {
        return -1;
    }

    __sync_lock_release(m_p_mutex_);

    for (;;) {
        char x;
        int ret = read(m_rfd_, &x, sizeof(char));

        if (ret < 0 && EINTR == errno) {
            continue;
        }

        break;
    }

    return 0;
}

int net_io_notifier::uninit()
{
    if (!m_init_) {
        return -1;
    }

    if (m_rfd_ >= 0) {
        close(m_rfd_);
    }

    if (m_wfd_ >= 0) {
        close(m_wfd_);
    }

    munmap(m_p_mutex_, sizeof(int));
    return 0;
}

int net_io_notifier::release()
{
    delete this;
    return 0;
}
