#ifndef ASYNC_SERVER_SHM_SEVENT_H
#define ASYNC_SERVER_SHM_SEVENT_H

#include <stdint.h>

#include "reactor.h"

class c_sevent : public c_handler
{
public:
    c_sevent();
    virtual ~c_sevent();

public:
    bool start(uint32_t channel);

public:
    virtual bool handle_input();
    virtual bool handle_output();
    virtual void handle_error();
    virtual void handle_fini();

private:
    int m_fd;
    uint32_t m_channel;
};

#endif
