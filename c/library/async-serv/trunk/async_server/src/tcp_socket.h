#ifndef ASYNC_SERVER_TCP_SOCKET_H
#define ASYNC_SERVER_TCP_SOCKET_H

#include <netinet/in.h>
#include <list>
#include "mem_pool.h"
#include "reactor.h"

class c_tcp_socket : public c_handler
{
public:
    c_tcp_socket();
    virtual ~c_tcp_socket();

public:
    void start(int fd, sockaddr_in &ip);

public:
    virtual bool handle_input();
    virtual bool handle_output();
    virtual void handle_error();
    virtual void handle_fini();
    virtual void shut();
    virtual void send_pkg(const void * buf, uint32_t len);

private:
    int m_fd;
    int m_id;
    sockaddr_in m_ip;

    bool m_is_closed;
    uint32_t m_recv_pos;
    uint8_t *m_recv_buf;
    uint32_t m_recv_buf_size;

    uint32_t m_send_pos;
    std::list<buffer_t *> m_send_list;
    uint32_t m_send_list_size;
};

#endif
