#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <netinet/in.h>

#include "log.h"
#include "shmq.h"
#include "global.h"
#include "plugin.h"
#include "shm_revent.h"

c_revent::c_revent()
{
    m_fd = -1;
}

c_revent::~c_revent()
{
}

bool c_revent::start(uint32_t channel)
{//将对应管道读端fd加入到epoll事件队列
    m_channel = channel;
    m_fd = g_shm_queue_mgr.recv_queue[channel].pipe[0];

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

bool c_revent::handle_input()
{
    pruge_pipe(m_fd);

    int count = 1024;
    while (count > 0) {
        struct shm_block_t sb;
        uint8_t *buf;
        if (recv_pull(m_channel, &sb, &buf) != 0)
            break;

        --count;
        g_shm_queue_mgr.recv_queue[m_channel].head->idle = 0;

        if (sb.type == PROTO_BLOCK) {
            g_link_flags[sb.fd] = sb.id;
           // if (g_link_flags[sb.fd] != 0)
                g_plugin.proc_pkg_cli(sb.fd, (char *)buf, sb.len);
        } else if (sb.type == CLOSE_BLOCK) {
            if (g_link_flags[sb.fd] != 0) {
                if (g_plugin.link_down_cli)
                    g_plugin.link_down_cli(sb.fd);

                g_link_flags[sb.fd] = 0;
            }
        } else if (sb.type == LOGIN_BLOCK) {
            g_link_flags[sb.fd] = sb.id;
            if (g_plugin.link_up_cli)
                g_plugin.link_up_cli(sb.fd, *(uint32_t *)buf);
        }
    }

    g_shm_queue_mgr.recv_queue[m_channel].head->idle = 1;
    return true;
}

bool c_revent::handle_output()
{
    return false;
}

void c_revent::handle_error()
{
    handle_fini();
}

void c_revent::handle_fini()
{
    if (m_fd) {
        g_reactor.del_handler(m_fd);
        close(m_fd);
        m_fd = -1;
    }

    delete this;
}
