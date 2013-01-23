#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "log.h"
#include "mem_pool.h"
#include "plugin.h"
#include "global.h"
#include "tcp_socket.h"
#include "shmq.h"

#define MAX_SEND_LIST_SIZE  1024

c_tcp_socket::c_tcp_socket()
{
    m_fd = -1;
    m_recv_pos = 0;
    m_recv_buf = NULL;
    m_recv_buf_size = 0;

    m_send_pos = 0;

    m_is_closed = false;
    m_send_list_size = 0;
}

c_tcp_socket::~c_tcp_socket()
{
}

void c_tcp_socket::start(int fd, sockaddr_in &ip)
{
    m_fd = fd;
    m_ip = ip;
    m_id = g_reactor.make_id();

    set_noblock(m_fd);

    m_recv_buf_size = 4096;
    m_recv_buf = (uint8_t *)malloc(m_recv_buf_size);
    if (!m_recv_buf) {
        handle_fini();
        return;
    }

    if (!g_reactor.add_handler(m_fd, this)) {
        handle_fini();
        return;
    }

    if (!g_reactor.handle_ctl(m_fd, EPOLL_CTL_ADD, EPOLLIN)) {
        handle_fini();
        return;
    }

    if (g_plugin.check_open_cli) {
        if (g_plugin.check_open_cli(m_ip.sin_addr.s_addr, m_ip.sin_port) == -1) {
            handle_fini();
            return;
        }
    }

    shm_block_t sb;
    sb.fd = m_fd;
    sb.id = m_id;
    sb.len = 4;
    sb.type = LOGIN_BLOCK;
    for (uint32_t i = 0; i < g_shm_queue_mgr.channel_num; ++i) // notify all.
        recv_push(i, &sb, (uint8_t *)(&m_ip.sin_addr.s_addr), true);

    return;
}

bool c_tcp_socket::handle_input()
{
    int n, len;
    uint32_t pos = 0;

    n = recv(m_fd, m_recv_buf + m_recv_pos, m_recv_buf_size - m_recv_pos, 0);
    if (n > 0) {
        if (m_is_closed) {
            m_recv_pos = 0;
            return true;
        }
        m_recv_pos += n;
        goto READ;
    } else {
        if (n == 0)
            return false;

        if (errno == EAGAIN || errno == EINTR) {
            return true;
        } else {
            return false;
        }
    }

READ:
    len = g_plugin.get_pkg_len_cli((char*)m_recv_buf + pos, m_recv_pos - pos);
    if ((uint32_t)len > g_max_pkg_len) { // len error
        ERROR_LOG("get_pkg_len_cli len=%d len < 0 || len > %d", len, g_max_pkg_len);
        return false;
    }

    if (len == 0) {
        if (pos != 0)
            goto MOVE;

        if (m_recv_pos == m_recv_buf_size) { // big protocol error
            ERROR_LOG("unsupported big protocol head, recvlen=%d", m_recv_buf_size);
            return false;
        }
    } else { // get valid len
        if (pos == 0 && (uint32_t)len > m_recv_buf_size) { // realloc recvbuf
            uint8_t *tmp = (uint8_t *)realloc(m_recv_buf, len);
            if (tmp == NULL) { // memory empty
                ERROR_LOG("memory empty realloc");
                return false;
            }

            m_recv_buf = tmp;
            m_recv_buf_size = len;
        } else if (len + pos <= m_recv_pos) { // recv a complete pack
            int channel = -1;
            if (g_plugin.select_channel)
                channel = g_plugin.select_channel(m_fd, (char *)m_recv_buf + pos, len, m_ip.sin_addr.s_addr,
                                                  g_shm_queue_mgr.channel_num);

            if (channel == -1) {
                static int random_channel = 0;
                channel = ++random_channel % g_shm_queue_mgr.channel_num;
                shm_block_t sb;
                sb.fd = m_fd;
                sb.id = m_id;
                sb.len = len;
                sb.type = PROTO_BLOCK;
                if (recv_push(channel, &sb, m_recv_buf + pos, false) == 0) {
                    if (g_plugin.shmq_pushed) {
                        if (g_plugin.shmq_pushed(m_fd, (const char *)(m_recv_buf + pos), len, 1) == -1) {
                            return false;
                        }
                    }
                } else {
                    if (g_plugin.shmq_pushed) {
                        if (g_plugin.shmq_pushed(m_fd, (const char *)(m_recv_buf + pos), len, 0) == -1) {
                            return false;
                        }
                    }
                }
            } else {
                if (channel < 0 || (uint32_t)channel >= g_shm_queue_mgr.channel_num) {
                    ERROR_LOG("select_channel return invalid channel:%u", channel);
                } else {
                    shm_block_t sb;
                    sb.fd = m_fd;
                    sb.id = m_id;
                    sb.len = len;
                    sb.type = PROTO_BLOCK;
                    if (recv_push(channel, &sb, m_recv_buf + pos, false) == 0) {
                        if (g_plugin.shmq_pushed) {
                            if (g_plugin.shmq_pushed(m_fd, (const char *)(m_recv_buf + pos), len, 1) == -1) {
                                return false;
                            }
                        }
                    } else {
                        if (g_plugin.shmq_pushed) {
                            if (g_plugin.shmq_pushed(m_fd, (const char *)(m_recv_buf + pos), len, 0) == -1) {
                                return false;
                            }
                        }
                    }
                }
            }

            pos += len;
            if (pos < m_recv_pos)
                goto READ;
        }
    }

MOVE:
    if (pos != 0) {
        memcpy(m_recv_buf, m_recv_buf + pos, m_recv_pos - pos);
        m_recv_pos -= pos;
    }

    return true;
}

bool c_tcp_socket::handle_output()
{
    int n;

    while (!m_send_list.empty()) {
        buffer_t *buffer = m_send_list.front();
        n = send(m_fd, buffer->buf + m_send_pos, buffer->dat_len - m_send_pos, 0);
        if (n > 0) {
            m_send_pos += n;
            if (m_send_pos == buffer->dat_len) {
                del_buffer(buffer);
                m_send_list.pop_front();
                --m_send_list_size;
                m_send_pos = 0;
                continue;
            }
        } else {
            if (0 == n)
                return false;

            if (errno == EAGAIN || errno == EINTR) {
                return true;
            } else {
                return false;
            }
        }
    }

    if (m_is_closed)
        return false;

    if (!g_reactor.handle_ctl(m_fd, EPOLL_CTL_MOD, EPOLLIN))
        return false;

    return true;
}

void c_tcp_socket::handle_error()
{
    if (!m_is_closed) {
        struct shm_block_t sb;
        sb.fd = m_fd;
        sb.id = m_id;
        sb.len = 0;
        sb.type = CLOSE_BLOCK;
        for (uint32_t i = 0; i < g_shm_queue_mgr.channel_num; ++i) // notify all.
            recv_push(i, &sb, NULL, true);
    }

    handle_fini();
}

void c_tcp_socket::handle_fini()
{
    if (m_fd != -1) {
        g_reactor.del_handler(m_fd);
        close(m_fd);
        m_fd = -1;
    }

    while (!m_send_list.empty()) {
        buffer_t *buffer = m_send_list.front();
        del_buffer(buffer);
        m_send_list.pop_front();
        --m_send_list_size;
    }

    if (m_recv_buf)
        free(m_recv_buf);

    delete this;
}

void c_tcp_socket::shut()
{
    struct shm_block_t sb;
    sb.fd = m_fd;
    sb.id = m_id;
    sb.len = 0;
    sb.type = CLOSE_BLOCK;
    for (uint32_t i = 0; i < g_shm_queue_mgr.channel_num; ++i) // notify all.
        recv_push(i, &sb, NULL, true);

    if (!m_send_list.empty()) {
        m_is_closed = true;
    } else {
        handle_fini();
    }
}

void c_tcp_socket::send_pkg(const void * buf, uint32_t len)
{
    if (!m_send_list.empty()) {
        if (m_send_list_size >= MAX_SEND_LIST_SIZE) {
            ERROR_LOG("client send buffer is full");
            return;
        }

        uint32_t p = 0;
        struct buffer_t *buffer;
        buffer = m_send_list.back();

        while (true) {
            uint32_t size = BUF_LEN - buffer->dat_len;
            if (size >= len - p)
                size = len - p;

            memcpy(buffer->buf + buffer->dat_len, (const uint8_t *)buf + p, size);
            buffer->dat_len += size;
            p += size;
            if (p < len) {
                buffer = get_buffer();
                if (!buffer) {
                    handle_error();
                    return;
                }
                buffer->dat_len = 0;
                m_send_list.push_back(buffer);
                ++m_send_list_size;
            } else {
                return;
            }
        }
    } else {
        uint32_t p = 0;
        int n = send(m_fd, buf, len, 0);
        if (n > 0) {
            if ((uint32_t)n == len) {
                return;
            } else {
                p = n;
            }
        } else {
            if (errno != EAGAIN && errno != EINTR) {
                handle_error();
                return;
            }
        }

        while (true) {
            struct buffer_t *buffer;
            buffer = get_buffer();
            if (!buffer) {
                handle_error();
                return;
            }

            buffer->dat_len = 0;
            m_send_list.push_back(buffer);
            ++m_send_list_size;
            uint32_t size = BUF_LEN > len - p ? len - p : BUF_LEN;
            memcpy(buffer->buf, (const uint8_t *)buf + p, size);
            buffer->dat_len = size;
            p += size;
            if (p == len) {
                if (!g_reactor.handle_ctl(m_fd, EPOLL_CTL_MOD, EPOLLIN|EPOLLOUT) == -1)
                    handle_error();

                return;
            }
        }
    }
}
