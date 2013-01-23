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
#include "tcp_linker.h"
#include "global.h"

c_tcp_linker::c_tcp_linker()
{
    m_fd = -1;
    m_recv_pos = 0;
    m_recv_buf = NULL;
    m_recv_buf_size = 0;

    m_send_pos = 0;

    m_is_closed = false;
}

c_tcp_linker::~c_tcp_linker()
{
}

bool c_tcp_linker::start(int fd, sockaddr_in &ip)
{
    m_fd = fd;
    m_ip = ip;


    set_noblock(m_fd);

    m_recv_buf_size = 4096;
    m_recv_buf = (uint8_t *)malloc(m_recv_buf_size);
    if (!m_recv_buf) {
        handle_fini();
        return false;
    }

    if (!g_reactor.add_handler(m_fd, this)) {
        handle_fini();
        return false;
    }

    if (!g_reactor.handle_ctl(m_fd, EPOLL_CTL_ADD, EPOLLIN)) {
        handle_fini();
        return false;
    }

    return true;
}

bool c_tcp_linker::handle_input()
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
    len = g_plugin.get_pkg_len_ser((char*)m_recv_buf + pos, m_recv_pos - pos);
    if ((uint32_t)len > g_max_pkg_len) { // len error
        ERROR_LOG("get_pkg_len_ser len=%d len < 0 || len > %d", len, g_max_pkg_len);
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
            g_plugin.proc_pkg_ser(m_fd, (char*)m_recv_buf + pos, len);
            if (!g_reactor.get_handler(m_fd) || m_is_closed)
                return true;

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

bool c_tcp_linker::handle_output()
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

void c_tcp_linker::handle_error()
{
    if (!m_is_closed) {
        if (g_plugin.link_down_ser)
            g_plugin.link_down_ser(m_fd);
    }

    handle_fini();
}

void c_tcp_linker::handle_fini()
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
    }

    if (m_recv_buf)
        free(m_recv_buf);

    delete this;
}

void c_tcp_linker::shut()
{
    if (!m_send_list.empty()) {
        m_is_closed = true;
    } else {
        handle_fini();
    }
}

void c_tcp_linker::send_pkg(const void * buf, uint32_t len)
{
    if (!m_send_list.empty()) {
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
