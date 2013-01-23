/**
 * @file epoll_reactor_impl.cpp
 * @brief 基于epoll的响应器实现
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-10
 */
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "log.h"
#include "epoll_reactor_impl.h"
#include "../event_handler.h"

#define MAX_ACTIVE_EVENTS (1024*10)

epoll_reactor_impl::epoll_reactor_impl() : m_ep_fd_(-1),
                                           m_p_event_vec_(NULL)
{

}

epoll_reactor_impl::~epoll_reactor_impl()
{
    reactor_impl_fini();
}

int epoll_reactor_impl::reactor_impl_init()
{
    if ((m_ep_fd_ = ::epoll_create(MAX_ACTIVE_EVENTS)) < 0) {
        m_ep_fd_ = -1;
        CRIT_LOG("epoll_create failed, err: %s", ::strerror(errno));
        return -1;
    }

    m_p_event_vec_ = (struct epoll_event*) ::calloc(MAX_ACTIVE_EVENTS, sizeof(epoll_event));
    if (m_p_event_vec_ == NULL) {
        ERROR_LOG("epoll_event malloc failed");
        ::close(m_ep_fd_);
        m_ep_fd_ = -1;
        return -1;
    }

    return 0;
}

int epoll_reactor_impl::reactor_impl_fini() {
    if (m_ep_fd_ >= 0) {
        ::close(m_ep_fd_);
        m_ep_fd_ = -1;
    }

    if (m_p_event_vec_) {
        ::free(m_p_event_vec_);
        m_p_event_vec_ = NULL;
    }

    return 0;
}

int epoll_reactor_impl::register_event(event_handler* h, int t)
{
    int fd = h->get_fd();
    if (fd < 0) {
        ERROR_LOG("event_handler::get_fd failed");
        return -1;
    }

    struct epoll_event ev;
    ::memset(&ev, 0, sizeof(ev));

    ev.data.ptr = h;
    ev.events |= EPOLLET | EPOLLRDHUP;

    if (t & READ_MASK)      ev.events |= EPOLLIN;
    if (t & WRITE_MASK)     ev.events |= EPOLLOUT;
    if (t & ERROR_MASK)     ev.events |= EPOLLPRI;
    if (t & ONESHOT_MASK)   ev.events |= EPOLLONESHOT;

    if (::epoll_ctl(m_ep_fd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
        ERROR_LOG("epoll_ctl failed, err: %s", ::strerror(errno));
        return -1;
    }

    return 0;
}

int epoll_reactor_impl::update_event(event_handler* h, int t)
{
    int fd = h->get_fd();
    if (fd < 0) {
        ERROR_LOG("event_handler::get_fd failed");
        return -1;
    }

    struct epoll_event ev;
    ::memset(&ev, 0, sizeof(ev));

    ev.data.ptr = h;
    ev.events |= EPOLLET | EPOLLRDHUP;

    if (t & READ_MASK)      ev.events |= EPOLLIN;
    if (t & WRITE_MASK)     ev.events |= EPOLLOUT;
    if (t & ERROR_MASK)     ev.events |= EPOLLPRI;
    if (t & ONESHOT_MASK)   ev.events |= EPOLLONESHOT;

    if (::epoll_ctl(m_ep_fd_, EPOLL_CTL_MOD, fd, &ev) < 0) {
        ERROR_LOG("epoll_ctl failed, err: %s", ::strerror(errno));
        return -1;
    }

    return 0;
}

int epoll_reactor_impl::remove_event(event_handler* h)
{
    int fd = h->get_fd();
    if (fd < 0) {
        ERROR_LOG("event_handler::get_fd failed");
        return -1;
    }

    ///kernel version should be at least 2.6.9
    return ::epoll_ctl(m_ep_fd_, EPOLL_CTL_DEL, fd, NULL);
}

int epoll_reactor_impl::reactor_impl_wait(int timeout)
{
    int nevents = ::epoll_wait(m_ep_fd_, m_p_event_vec_, MAX_ACTIVE_EVENTS, timeout);
    if (nevents < 0) {
        return -1;
    } else if (nevents == 0) {
        return 0;
    }

    event_handler* h = NULL;
    for (int i = 0; i < nevents; i++) {
        h = (event_handler*)(m_p_event_vec_[i].data.ptr);

        if (m_p_event_vec_[i].events & (EPOLLHUP | EPOLLERR)) {
            h->handle_error();
            m_p_event_vec_[i].data.ptr = h = NULL;
            continue;
        }

        if (m_p_event_vec_[i].events & (EPOLLIN | EPOLLPRI)) {
            if (h->handle_read() < 0) {
                m_p_event_vec_[i].data.ptr = h = NULL;
                continue;
            }
        }

        if (m_p_event_vec_[i].events & EPOLLRDHUP) {
            if (h->handle_read() < 0) {
                m_p_event_vec_[i].data.ptr = h = NULL;
                continue;
            }
        }

        if (m_p_event_vec_[i].events & EPOLLOUT) {
            if (h->handle_write() < 0) {
                m_p_event_vec_[i].data.ptr = h = NULL;
                continue;
            }
        }
    }

    return 0;
}
