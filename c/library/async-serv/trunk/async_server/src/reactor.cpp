#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>

#include "reactor.h"

c_reactor g_reactor;

c_reactor::c_reactor()
 : m_handlers(0)
{
}

bool c_reactor::init(uint32_t size)
{
    m_handlers.resize(size);

    for (uint32_t i = 0; i < m_handlers.size(); ++i)
        m_handlers[i] = NULL;

    m_fd = epoll_create(m_handlers.size());
    if (m_fd == -1)
        return false;

    m_id = 0;
    return true;
}

void c_reactor::fini()
{
    for (uint32_t i = 0; i < m_handlers.size(); ++i) {
        if (m_handlers[i]) {
            m_handlers[i]->handle_fini();
            m_handlers[i] = NULL;
        }
    }

    close(m_fd);
}

void c_reactor::handle_event()
{
    struct epoll_event events[128];
    int n = epoll_wait(m_fd, events, 128, 1000);
    for (int i = 0; i < n; ++i) {
        int fd = events[i].data.fd;
        if (!m_handlers[fd])
            continue;

        uint32_t e = events[i].events;
        if (e & (EPOLLERR|EPOLLHUP|EPOLLPRI)) {
            m_handlers[fd]->handle_error();
            continue;
        }

        if (e & EPOLLIN) {
            if (!m_handlers[fd]->handle_input()) {
                m_handlers[fd]->handle_error();
                continue;
            }
        }

        if (e & EPOLLOUT) {
            if (!m_handlers[fd]->handle_output()) {
                m_handlers[fd]->handle_error();
                continue;
            }
        }
    }
}
