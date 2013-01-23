#ifndef ASYNC_SERVER_REACTOR_H
#define ASYNC_SERVER_REACTOR_H

#include <stdint.h>
#include <sys/epoll.h>
#include <vector>

class c_handler
{
public:
    virtual bool handle_input() = 0;
    virtual bool handle_output() = 0;
    virtual void handle_error() = 0;
    virtual void handle_fini() = 0;
    virtual void shut() {};
    virtual void send_pkg(const void *buf, uint32_t len) {};
    virtual ~c_handler() {};
};

class c_reactor
{
public:
    c_reactor();
    bool init(uint32_t size);
    void fini();
    void handle_event();

    inline bool add_handler(int fd, c_handler *handler)
    {
        if ((uint32_t)fd >= m_handlers.size())
            return false;

        m_handlers[fd] = handler;
        return true;
    }

    inline bool del_handler(int fd)
    {
        if ((uint32_t)fd >= m_handlers.size())
            return false;

        m_handlers[fd] = NULL;
        return true;
    }

    inline c_handler *get_handler(int fd)
    {
        if ((uint32_t)fd >= m_handlers.size())
            return NULL;

        return m_handlers[fd];
    }

    inline bool handle_ctl(int fd, int op, uint32_t mask)
    {
        if ((uint32_t)fd >= m_handlers.size())
            return false;

        epoll_event e;
        e.data.fd = fd;
        e.events = mask;
        if (epoll_ctl(m_fd, op, fd, &e) == -1)
            return false;

        return true;
    }

    inline int make_id()
    {
        return ++m_id != 0 ? m_id : ++m_id;
    }

	inline uint32_t max_handler()
	{
		return m_handlers.size();
	}

public:
    int m_fd;
    int m_id;
    std::vector<c_handler *> m_handlers;  
};

extern c_reactor g_reactor;

#endif
