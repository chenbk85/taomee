#ifndef ASYNC_SERVER_SHM_REVENT_H
#define ASYNC_SERVER_SHM_REVENT_H

#include <stdint.h>

#include "reactor.h"

class c_revent : public c_handler
{
public:
    c_revent();
    virtual ~c_revent();

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
