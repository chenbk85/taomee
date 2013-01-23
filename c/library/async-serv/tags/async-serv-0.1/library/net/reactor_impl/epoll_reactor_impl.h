/**
 * @file epoll_reactor_impl.h
 * @brief 基于epoll的反应器实现
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-10
 */
#ifndef _H_EPOLL_REACTOR_IMPL_H_
#define _H_EPOLL_REACTOR_IMPL_H_

#include <sys/epoll.h>

#include "../reactor_impl.h"

class epoll_reactor_impl : public reactor_impl {
public:
    epoll_reactor_impl();
    virtual ~epoll_reactor_impl();

    virtual int reactor_impl_init();
    virtual int reactor_impl_fini();

    virtual int register_event(event_handler* h, int t);
    virtual int update_event(event_handler* h, int t);
    virtual int remove_event(event_handler* h);

    virtual int reactor_impl_wait(int timeout);

private:
    int m_ep_fd_;  /**<@epoll描述符fd*/

    struct epoll_event* m_p_event_vec_; /**<@epoll响应事件队列*/
};

#endif

