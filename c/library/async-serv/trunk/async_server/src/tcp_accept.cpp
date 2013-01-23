#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "log.h"
#include "global.h"
#include "tcp_accept.h"
#include "tcp_socket.h"

c_tcp_accept::c_tcp_accept()
{
    m_fd = -1;
}

c_tcp_accept::~c_tcp_accept()
{
}

bool c_tcp_accept::start(const char *ip, uint16_t port)
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    inet_aton(ip, &(server_addr.sin_addr));
    server_addr.sin_port = htons(port);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
        return false;

    m_fd = fd;
    set_noblock(fd);

    int reuse = 1;
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse,  sizeof(reuse)) == -1) {
        close(m_fd);
        BOOT_LOG(10, "1 start listen %s:%u", ip, port);
    }

    if (bind(m_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1) {
        close(m_fd);
        BOOT_LOG(10, "2 start listen %s:%u", ip, port);
    }

    if (listen(m_fd, 1024) == -1) {
        close(m_fd);
        BOOT_LOG(10, "3 start listen %s:%u", ip, port);
    }

    if (!g_reactor.add_handler(m_fd, this)) {
        close(m_fd);
        BOOT_LOG(10, "4 start listen %s:%u", ip, port);
    }

    if (!g_reactor.handle_ctl(m_fd, EPOLL_CTL_ADD, EPOLLIN)) {
        close(m_fd);
        BOOT_LOG(10, "5 start listen %s:%u", ip, port);
    }

    boot_log(0, 0, "start listen %s:%u", ip, port);
    return true;
}

bool c_tcp_accept::handle_input()
{
    sockaddr_in ip;
    socklen_t len;

    int cli_fd;
    while (1) {
        len = sizeof(ip);
        cli_fd = accept(m_fd, (sockaddr *)&ip, &len);

        if (cli_fd >= 0) {
            if ((uint32_t)cli_fd >= g_reactor.max_handler()) {
                close(cli_fd);
                continue;
            }

            c_tcp_socket *ts = new (std::nothrow) c_tcp_socket();
            if (!ts) {
                close(cli_fd);
                continue;
            }

            ts->start(cli_fd, ip);
        } else {
            if (errno == EAGAIN || errno == EINTR || errno == EMFILE || errno == ENFILE) {
                return true;
            } else {
                return false;
            }
        }
    }
}

bool c_tcp_accept::handle_output()
{
    return false;
}

void c_tcp_accept::handle_error()
{
    handle_fini();
}

void c_tcp_accept::handle_fini()
{
    if (m_fd != -1) {
        g_reactor.del_handler(m_fd);
        close(m_fd);
        m_fd = -1;
    }
}
