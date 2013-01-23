#ifndef ASYNC_SERVER_TCP_ACCEPT_H
#define ASYNC_SERVER_TCP_ACCEPT_H

#include <netinet/in.h>

#include "reactor.h"

class c_tcp_accept : public c_handler
{
public:
    c_tcp_accept();
    virtual ~c_tcp_accept();

public:
    bool start(const char *ip, uint16_t port);

public:
    virtual bool handle_input();
    virtual bool handle_output();
    virtual void handle_error();
    virtual void handle_fini();

private:
    int m_fd;
};

#endif
