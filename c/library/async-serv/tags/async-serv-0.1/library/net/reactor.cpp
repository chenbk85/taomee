/**
 * @file reactor.cpp
 * @brief 反应器类(singleton模式)
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-11
 */
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <new>

#include "log.h"
#include "reactor.h"
#include "reactor_impl.h"
#include "timer_queue.h"


reactor* reactor::m_p_instance_ = NULL;

reactor* reactor::instance()
{
    m_p_instance_ = ((m_p_instance_ == NULL) ?
            new (std::nothrow) reactor : m_p_instance_);
    return m_p_instance_;
}

reactor::reactor()
    : m_reactor_impl_(reactor_impl::new_reactor_impl()),
      m_timer_queue_impl_(new (std::nothrow) timer_queue)
{
    m_wake_fd_[0] = m_wake_fd_[1] = -1;
}

int reactor::reactor_init()
{
    ///若m_reactor_impl_在构造函数中初始化失败
    if (m_reactor_impl_.get() == NULL) {
        reactor_impl* ptr = reactor_impl::new_reactor_impl();
        if (!ptr) {
            ERROR_LOG("reactor_impl::new_reactor_impl() failed");
            return -1;
        } else {
            m_reactor_impl_.reset(ptr);
        }
    }

    ///若m_timer_queue_impl_在构造函数中初始化失败
    if (m_timer_queue_impl_.get() == NULL) {
        timer_queue* ptr = new (std::nothrow) timer_queue;
        if (!ptr) {
            ERROR_LOG("new timer_queue failed");
            return -1;
        } else {
            m_timer_queue_impl_.reset(ptr);
        }
    }

    int rt = m_reactor_impl_->reactor_impl_init()
            || m_timer_queue_impl_->timer_queue_init();

    if (rt < 0) {
        return -1;
    }

    if (::pipe(m_wake_fd_)) {
        ERROR_LOG("pipe create failed, err: %s", ::strerror(errno));
        return -1;
    }

    int flags = ::fcntl(m_wake_fd_[0], F_GETFL);
    ::fcntl(m_wake_fd_[0], F_SETFL, flags | O_NONBLOCK);

    flags = ::fcntl(m_wake_fd_[1], F_GETFL);
    ::fcntl(m_wake_fd_[1], F_SETFL, flags | O_NONBLOCK);

    if (register_event(this, READ_MASK) < 0) {
        return -1;
    }

    return 0;
}

int reactor::reactor_fini()
{
    m_reactor_impl_->reactor_impl_fini();
    m_timer_queue_impl_->timer_queue_fini();

    ::close(m_wake_fd_[0]);
    ::close(m_wake_fd_[1]);

    m_wake_fd_[0] = m_wake_fd_[1] = -1;

    return 0;
}

void reactor::queue_in_loop(const functor_t func) {
    m_pending_func_.push_back(func);
    wakeup();
}

void reactor::wakeup()
{
    char byte;
    ssize_t n = ::write(m_wake_fd_[1], &byte, sizeof(byte));
    if (n != sizeof(byte))
        DEBUG_LOG("reactor::wakeup() writes %d bytes instead of 1, err: %s",
                n, ::strerror(errno));
}

int reactor::handle_read()
{
    ssize_t n = ::read(m_wake_fd_[0], m_buffer_, sizeof(m_buffer_));
    return 0;
}

int reactor::register_event(event_handler* h, int t)
{
    return m_reactor_impl_->register_event(h, t);
}

int reactor::update_event(event_handler* h, int t)
{
    return m_reactor_impl_->update_event(h, t);
}

timer_id_t reactor::register_timer(event_handler* h,
                                   int t,
                                   u_int expired_time,
                                   u_int time_interval)
{
    return m_timer_queue_impl_->register_timer(h, t, expired_time, time_interval);
}

int reactor::remove_event(event_handler* h)
{
    return m_reactor_impl_->remove_event(h);
}

int reactor::remove_timer(timer_id_t* id)
{
    return m_timer_queue_impl_->remove_timer(id);
}

int reactor::reactor_wait(int timeout)
{
    if (m_reactor_impl_->reactor_impl_wait(timeout) < 0) {
        return -1;
    }

    std::vector<functor_t> tmp_pending_func;
    tmp_pending_func.swap(m_pending_func_);

    for (size_t i = 0; i < tmp_pending_func.size(); i++) {
        tmp_pending_func[i]();
    }

    if (m_timer_queue_impl_->timer_wait() < 0) {
        return -1;
    }

    return 0;
}

