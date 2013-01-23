/**
 * @file select_reactor_impl.cpp
 * @brief 基于select的反应器实现
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-11
 */
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#include "log.h"
#include "select_reactor_impl.h"
#include "../event_handler.h"

select_reactor_impl::select_reactor_impl()
    : m_p_event_vec_(NULL),
      m_max_fd_(-1)
{
    ::memset(&m_fd_struct_, 0, sizeof(m_fd_struct_));
}

select_reactor_impl::~select_reactor_impl()
{
    reactor_impl_fini();
}

int select_reactor_impl::reactor_impl_init()
{
    m_p_event_vec_ = (void**) ::calloc(FD_SETSIZE, sizeof(void*));
    if (m_p_event_vec_ == NULL) {
        CRIT_LOG("m_p_event_vec_ alloc failed!");
        return -1;
    }

    return 0;
}

int select_reactor_impl::reactor_impl_fini()
{
    if (m_p_event_vec_) {
        ::free(m_p_event_vec_);
        m_p_event_vec_ = NULL;
    }

    ::memset(&m_fd_struct_, 0, sizeof(m_fd_struct_));

    m_max_fd_ = -1;

    return 0;
}

int select_reactor_impl::register_event(event_handler* h, int t)
{
    int fd = h->get_fd();
    if (fd < 0 || fd > FD_SETSIZE) {
        ERROR_LOG("event_handler::get_fd illegal");
        return -1;
    }

    if (m_p_event_vec_[fd]) {
        ERROR_LOG("select_reactor_impl::event_handler already register");
        return -1;
    }

    if (fd > m_max_fd_)
        m_max_fd_ = fd;

    if (t & READ_MASK)  FD_SET(fd, &m_fd_struct_.rd_set);
    if (t & WRITE_MASK) FD_SET(fd, &m_fd_struct_.wr_set);
    if (t & ERROR_MASK) FD_SET(fd, &m_fd_struct_.err_set);

    ///long对齐，利用指针的最后1位存储该事件是否是ONESHOT事件
    if (t & ONESHOT_MASK) {
        m_p_event_vec_[fd] = (void*)((long)h | 1);
    } else {
        m_p_event_vec_[fd] = h;
    }

    return 0;
}

int select_reactor_impl::update_event(event_handler* h, int t)
{
    int fd = h->get_fd();
    if (fd < 0 || fd > FD_SETSIZE) {
        ERROR_LOG("event_handler::get_fd illegal");
        return -1;
    }

    if (!m_p_event_vec_[fd]) {
        ERROR_LOG("select_reactor_impl::event_handler is not registered");
        return -1;
    }

    if (t & READ_MASK)  FD_SET(fd, &m_fd_struct_.rd_set);
    if (t & WRITE_MASK) FD_SET(fd, &m_fd_struct_.wr_set);
    if (t & ERROR_MASK) FD_SET(fd, &m_fd_struct_.err_set);

    if(t & ONESHOT_MASK) {
        m_p_event_vec_[fd] = (void*)((long)m_p_event_vec_[fd] | 1);
    } else {
        m_p_event_vec_[fd] = h;
    }

    return 0;
}

int select_reactor_impl::remove_event(event_handler* h)
{
    int fd = h->get_fd();
    if (fd < 0 || fd > FD_SETSIZE) {
        ERROR_LOG("event_handler::get_fd illegal");
        return -1;
    }

    m_p_event_vec_[fd] = NULL;

    FD_CLR(fd, &m_fd_struct_.rd_set);
    FD_CLR(fd, &m_fd_struct_.wr_set);
    FD_CLR(fd, &m_fd_struct_.err_set);

    if (fd == m_max_fd_) {
        for (int i = m_max_fd_; i >= 0; i--) {
            if (m_p_event_vec_[i]) {
                m_max_fd_ = i;
                break;
            }
        }
    }

    return 0;
}

int select_reactor_impl::reactor_impl_wait(int timeout)
{
    ::memcpy(&m_fd_struct_.old_rd_set, &m_fd_struct_.rd_set, sizeof(fd_set));
    ::memcpy(&m_fd_struct_.old_wr_set, &m_fd_struct_.wr_set, sizeof(fd_set));
    ::memcpy(&m_fd_struct_.old_err_set, &m_fd_struct_.err_set, sizeof(fd_set));

    struct timeval* tv = NULL;
    struct timeval tmp_timeval;

    if (timeout == -1) {
        tv = &tmp_timeval;
        tv->tv_usec = 0;
        tv->tv_sec = 0;
    } else if (timeout == 0) {
        tv = NULL;
    } else {
        tv = &tmp_timeval;
        tv->tv_usec = timeout % 1000 * 1000;
        tv->tv_sec = timeout / 1000;
    }

    int rv = 0;
    rv = ::select(m_max_fd_ + 1,
                  &m_fd_struct_.old_rd_set,
                  &m_fd_struct_.old_wr_set,
                  &m_fd_struct_.old_err_set,
                  tv);

    if (rv < 0) {
        return -1;
    } else if (rv == 0) {
        return 0;
    }

    event_handler* h = NULL;
    for (int i = 0; i <= m_max_fd_; i++) {
        if (m_p_event_vec_[i]) {
            h = (event_handler*)(((long)m_p_event_vec_[i]) & (~1));

            if (FD_ISSET(i, &m_fd_struct_.old_rd_set)
                || FD_ISSET(i, &m_fd_struct_.old_err_set)) {
                if (h->handle_read() < 0) {
                    //remove_event(h);
                    continue;
                }
            }

            if (FD_ISSET(i, &m_fd_struct_.old_wr_set)) {
                if (h->handle_write() < 0) {
                    //remove_event(h);
                    continue;
                }
            }

            ///如果是oneshot事件，则在本次事件响应结束后从fd_set中删除此fd相关事件
            if (((long)m_p_event_vec_[i]) & 1) {
                update_event(h, NULL_MASK);
            }
        }
    }

    return 0;
}
